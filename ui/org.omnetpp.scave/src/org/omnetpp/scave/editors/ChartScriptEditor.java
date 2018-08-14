package org.omnetpp.scave.editors;

import java.io.IOException;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.util.EContentAdapter;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.DocumentEvent;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IDocumentListener;
import org.eclipse.jface.text.Position;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.console.ConsolePlugin;
import org.eclipse.ui.console.IConsole;
import org.eclipse.ui.console.IConsoleManager;
import org.eclipse.ui.console.IOConsole;
import org.eclipse.ui.console.IOConsoleOutputStream;
import org.eclipse.ui.texteditor.IDocumentProvider;
import org.eclipse.ui.texteditor.MarkerAnnotation;
import org.omnetpp.common.canvas.LargeScrollableCanvas;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.ChartMouseModeAction;
import org.omnetpp.scave.actions.ClosePageAction;
import org.omnetpp.scave.actions.CopyChartToClipboardAction;
import org.omnetpp.scave.actions.EditAction;
import org.omnetpp.scave.actions.ExportToSVGAction;
import org.omnetpp.scave.actions.RefreshChartAction;
import org.omnetpp.scave.actions.SaveTempChartAction;
import org.omnetpp.scave.actions.ToggleShowSourceAction;
import org.omnetpp.scave.actions.ZoomChartAction;
import org.omnetpp.scave.charting.ChartUpdater;
import org.omnetpp.scave.editors.ui.FormEditorPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.MatplotlibChart;
import org.omnetpp.scave.pychart.PlotWidget;
import org.omnetpp.scave.pychart.PythonOutputMonitoringThread.IOutputListener;
import org.omnetpp.scave.python.BackAction;
import org.omnetpp.scave.python.ExportAction;
import org.omnetpp.scave.python.ForwardAction;
import org.omnetpp.scave.python.GotoChartDefinitionAction;
import org.omnetpp.scave.python.HomeAction;
import org.omnetpp.scave.python.InteractAction;
import org.omnetpp.scave.python.KillPythonProcessAction;
import org.omnetpp.scave.python.MatplotlibChartViewer;
import org.omnetpp.scave.python.MatplotlibChartViewer.IPy4JExceptionHandler;
import org.omnetpp.scave.python.MatplotlibChartViewer.IStateChangeListener;
import org.omnetpp.scave.python.NativeChartViewer;
import org.omnetpp.scave.python.PanAction;
import org.omnetpp.scave.python.ToggleAutoUpdateAction;
import org.omnetpp.scave.python.ZoomAction;
import org.python.pydev.editor.PyEdit;
import org.python.pydev.shared_core.callbacks.ICallbackListener;

import py4j.Py4JException;

public class ChartScriptEditor extends PyEdit {
    Chart chart;
    ScaveEditor scaveEditor;

    FormEditorPage formEditor;
    SashForm sashForm;
    Composite sourceEditorContainer;

    ResultsProvider resultsProvider;
    PropertiesProvider propertiesProvider;

    NativeChartViewer nativeChartViewer;
    MatplotlibChartViewer matplotlibChartViewer;

    ChartScriptEditorInput editorInput;

    IOConsole console;
    IOConsoleOutputStream consoleStream;

    ChartScriptDocumentProvider documentProvider;

    IMarker errorMarker;
    MarkerAnnotation errorMarkerAnnotation;

    SaveTempChartAction saveTempChartAction;
    GotoChartDefinitionAction gotoChartDefinitionAction;

    ExportAction exportAction = new ExportAction();

    InteractAction interactAction = new InteractAction();
    PanAction panAction = new PanAction();
    ZoomAction zoomAction = new ZoomAction();

    HomeAction homeAction = new HomeAction();
    BackAction backAction = new BackAction();
    ForwardAction forwardAction = new ForwardAction();

    ZoomChartAction zoomToFitAction = new ZoomChartAction(true, true, 0.0);
    ZoomChartAction zoomInHorizAction = new ZoomChartAction(true, false, 2.0);
    ZoomChartAction zoomOutHorizAction = new ZoomChartAction(true, false, 1 / 2.0);

    ZoomChartAction zoomInVertAction = new ZoomChartAction(false, true, 2.0);
    ZoomChartAction zoomOutVertAction = new ZoomChartAction(false, true, 1 / 2.0);

    KillPythonProcessAction killAction;

    ToggleAutoUpdateAction toggleAutoUpdateAction;
    boolean autoUpdateEnabled = true;

    ToggleShowSourceAction toggleShowSourceAction = new ToggleShowSourceAction();
    boolean showSource = false;

    ChartScriptEditor(ScaveEditor scaveEditor, Chart chart) {


        this.chart = chart;
        this.scaveEditor = scaveEditor;

        this.documentProvider = new ChartScriptDocumentProvider();

        propertiesProvider = new PropertiesProvider(chart);
        resultsProvider = new ResultsProvider(scaveEditor.getResultFileManager());

        console = new IOConsole("Chart script output of " + getChartName(), null);
        IConsoleManager consoleManager = ConsolePlugin.getDefault().getConsoleManager();
        consoleManager.addConsoles(new IConsole[] { console });
        consoleStream = console.newOutputStream();

        onCreatePartControl.registerListener(new ICallbackListener<Composite>() {

            @Override
            public Object call(Composite obj) {

                obj.addDisposeListener(new DisposeListener() {
                    @Override
                    public void widgetDisposed(DisposeEvent e) {
                        ChartScriptEditor.this.dispose();
                    }
                });

                formEditor = new FormEditorPage(obj, SWT.NONE, scaveEditor);
                Composite content = formEditor.getContent();

                saveTempChartAction = new SaveTempChartAction();
                gotoChartDefinitionAction = new GotoChartDefinitionAction();

                formEditor.addToToolbar(saveTempChartAction);
                formEditor.addToToolbar(gotoChartDefinitionAction);

                formEditor.setToolbarActionVisible(saveTempChartAction, chart.isTemporary());
                formEditor.setToolbarActionVisible(gotoChartDefinitionAction, !chart.isTemporary());

                formEditor.addSeparatorToToolbar();

                formEditor.addToToolbar(toggleShowSourceAction);
                formEditor.addToToolbar(new EditAction());

                toggleAutoUpdateAction = new ToggleAutoUpdateAction();
                toggleAutoUpdateAction.setChecked(true);

                if (chart instanceof MatplotlibChart) {
                    formEditor.addSeparatorToToolbar();
                    interactAction.setChecked(true);
                    formEditor.addToToolbar(interactAction);
                    formEditor.addToToolbar(panAction);
                    formEditor.addToToolbar(zoomAction);
                    formEditor.addSeparatorToToolbar();
                    formEditor.addToToolbar(homeAction);
                    formEditor.addToToolbar(backAction);
                    formEditor.addToToolbar(forwardAction);
                    formEditor.addSeparatorToToolbar();
                    formEditor.addSeparatorToToolbar();
                    formEditor.addToToolbar(toggleAutoUpdateAction);
                    formEditor.addToToolbar(new RefreshChartAction());
                    formEditor.addToToolbar(killAction = new KillPythonProcessAction());
                    formEditor.addSeparatorToToolbar();
                    formEditor.addToToolbar(new CopyChartToClipboardAction());
                    formEditor.addToToolbar(exportAction);
                    formEditor.addSeparatorToToolbar();
                    formEditor.addToToolbar(new ClosePageAction());
                }
                else {
                    formEditor.addSeparatorToToolbar();
                    ChartMouseModeAction panAction = new ChartMouseModeAction(ZoomableCanvasMouseSupport.PAN_MODE);
                    formEditor.addToToolbar(panAction);
                    panAction.setChecked(true);
                    formEditor.addToToolbar(new ChartMouseModeAction(ZoomableCanvasMouseSupport.ZOOM_MODE));
                    formEditor.addSeparatorToToolbar();
                    formEditor.addToToolbar(zoomToFitAction);
                    formEditor.addToToolbar(zoomInHorizAction);
                    formEditor.addToToolbar(zoomOutHorizAction);
                    formEditor.addToToolbar(zoomInVertAction);
                    formEditor.addToToolbar(zoomOutVertAction);
                    formEditor.addSeparatorToToolbar();
                    formEditor.addSeparatorToToolbar();
                    formEditor.addToToolbar(toggleAutoUpdateAction);
                    formEditor.addToToolbar(new RefreshChartAction());
                    formEditor.addToToolbar(killAction = new KillPythonProcessAction());
                    formEditor.addSeparatorToToolbar();
                    formEditor.addToToolbar(new CopyChartToClipboardAction());
                    formEditor.addToToolbar(new ExportToSVGAction());
                    formEditor.addSeparatorToToolbar();
                    formEditor.addToToolbar(new ClosePageAction());
                }

                formEditor.setFormTitle("Chart: " + getChartName());
                GridLayout layout = new GridLayout(1, true);
                layout.marginWidth = 0;
                layout.marginHeight = 0;
                content.setLayout(layout);

                sashForm = new SashForm(content, SWT.NONE);
                sashForm.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

                formEditor.getToolbar().layout();
                formEditor.layout();
                content.layout();
                obj.layout();

                return sashForm;
            }
        });

        onAfterCreatePartControl.registerListener(new ICallbackListener<ISourceViewer>() {

            @Override
            public Object call(ISourceViewer obj) {

                Assert.isTrue(sashForm.getChildren().length == 1 && sashForm.getChildren()[0] instanceof Composite);
                sourceEditorContainer = (Composite) (sashForm.getChildren()[0]);

                IOutputListener outputListener = output -> {
                    Display.getDefault().asyncExec(() -> {
                        try {
                            consoleStream.write(output);
                        }
                        catch (IOException e) {
                            // TODO Auto-generated catch block
                            e.printStackTrace();
                        }
                    });
                };

                IStateChangeListener stateChangeListener = new IStateChangeListener() {

                    @Override
                    public void pythonProcessLivenessChanged(boolean alive) {
                        Display.getDefault().syncExec(() -> {
                            updateActions();
                        });
                    }

                    @Override
                    public void activeActionChanged(String action) {
                        updateActiveAction(action);
                    }
                };

                if (chart instanceof MatplotlibChart) {
                    matplotlibChartViewer = new MatplotlibChartViewer(scaveEditor.processPool, sashForm);
                    matplotlibChartViewer.setResultsProvider(resultsProvider);
                    matplotlibChartViewer.setChartPropertiesProvider(propertiesProvider);

                    matplotlibChartViewer.addOutputListener(outputListener);
                    matplotlibChartViewer.addStateChangeListener(stateChangeListener);

                    MenuManager manager = new MenuManager();

                    manager.add(new GotoChartDefinitionAction());
                    manager.add(new Separator());

                    manager.add(toggleShowSourceAction);
                    manager.add(new EditAction());
                    manager.add(new Separator());

                    // TODO: buttons to control RLE and halfres interaction? Or maybe only in a
                    // settings dialog somewhere?

                    MenuManager navigationMenu = new MenuManager("Navigation");
                    navigationMenu.add(interactAction);
                    navigationMenu.add(panAction);
                    navigationMenu.add(zoomAction);

                    navigationMenu.add(new Separator());

                    navigationMenu.add(homeAction);
                    navigationMenu.add(backAction);
                    navigationMenu.add(forwardAction);

                    manager.add(navigationMenu);
                    manager.add(new Separator());

                    manager.add(toggleAutoUpdateAction);
                    manager.add(new RefreshChartAction());
                    manager.add(killAction);
                    manager.add(new Separator());

                    manager.add(new CopyChartToClipboardAction());
                    manager.add(exportAction);

                    PlotWidget plotWidget = matplotlibChartViewer.getPlotWidget();
                    plotWidget.setMenu(manager.createContextMenu(plotWidget));

                }
                else {
                    nativeChartViewer = new NativeChartViewer(chart, scaveEditor.processPool, sashForm);
                    nativeChartViewer.setResultsProvider(resultsProvider);
                    nativeChartViewer.setChartPropertiesProvider(propertiesProvider);

                    nativeChartViewer.addOutputListener(outputListener);
                    nativeChartViewer.addStateChangeListener(stateChangeListener);

                    nativeChartViewer.getChartViewer().addPropertyChangeListener(new IPropertyChangeListener() {
                        public void propertyChange(PropertyChangeEvent event) {
                            if (event.getProperty() == LargeScrollableCanvas.PROP_VIEW_X
                                    || event.getProperty() == LargeScrollableCanvas.PROP_VIEW_Y
                                    || event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_X
                                    || event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_Y) {
                                updateActions();
                            }
                        }
                    });


                    ChartUpdater chartUpdater = new ChartUpdater(chart, ChartScriptEditor.this, nativeChartViewer.getChartViewer(), scaveEditor.getResultFileManager());

                    chart.eAdapters().add(new EContentAdapter() {
                        @Override
                        public void notifyChanged(Notification notification) {
                            super.notifyChanged(notification);
                            chartUpdater.updateChart(notification);
                        }
                    });

                    IMenuManager zoomSubmenuManager = new MenuManager("Zoom", ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_ZOOM), null);
                    zoomSubmenuManager.add(zoomInHorizAction);
                    zoomSubmenuManager.add(zoomOutHorizAction);
                    zoomSubmenuManager.add(zoomInVertAction);
                    zoomSubmenuManager.add(zoomOutVertAction);
                    zoomSubmenuManager.add(new Separator());
                    zoomSubmenuManager.add(zoomToFitAction);


                    MenuManager manager = new MenuManager();

                    manager.add(new GotoChartDefinitionAction());
                    manager.add(toggleShowSourceAction);
                    manager.add(new EditAction());
                    manager.add(new Separator());

                    manager.add(new Separator());
                    manager.add(zoomSubmenuManager);

                    manager.add(new Separator());
                    manager.add(toggleAutoUpdateAction);
                    manager.add(new RefreshChartAction());
                    manager.add(killAction);

                    manager.add(new Separator());
                    manager.add(new CopyChartToClipboardAction());
                    manager.add(new ExportToSVGAction());

                    nativeChartViewer.getChartViewer().setMenu(manager.createContextMenu(nativeChartViewer.getChartViewer()));

                }

                sashForm.layout();

                if (chart.getScript() == null || chart.getScript().isEmpty())
                    showSource = true;

                setShowSource(showSource);

                runChartScript();
                return obj;
            }

        });

        editorInput = new ChartScriptEditorInput(chart);

        getDocumentProvider().getDocument(editorInput).addDocumentListener(new IDocumentListener() {

            int documentChangeCounter = 0;

            @Override
            public void documentChanged(DocumentEvent event) {
                ++documentChangeCounter;

                // System.out.println("doc counter is: " + documentChangeCounter);
                Display.getDefault().timerExec(1000, new Runnable() {
                    int savedChangeCounter = documentChangeCounter;


                    @Override
                    public void run() {
                        // System.out.println("doc counter is: " + documentChangeCounter + " saved counter is: " + savedChangeCounter);

                        if (documentChangeCounter == savedChangeCounter) {
                            if (!getDocument().get().equals(chart.getScript())) {
                                SetCommand cmd = (SetCommand) SetCommand.create(scaveEditor.getEditingDomain(), chart,
                                        chart.eClass().getEStructuralFeature("script"), event.getDocument().get());
                                scaveEditor.getEditingDomain().getCommandStack().execute(cmd);
                            }

                            if (autoUpdateEnabled)
                                runChartScript();
                        }
                    }
                });
            }

            @Override
            public void documentAboutToBeChanged(DocumentEvent event) { }
        });
    }

    public void runChartScript() {
        Display.getDefault().syncExec(() -> {
            console.clearConsole();
            documentProvider.annotationModel.removeAnnotation(errorMarkerAnnotation);

            try {
                if (errorMarker != null)
                    errorMarker.delete();
            }
            catch (CoreException e1) {
                // TODO Auto-generated catch block
                e1.printStackTrace();
            }
            errorMarker = null;

            Runnable afterRun = () -> {
                Display.getDefault().syncExec(() -> {
                    updateActions();
                });
            };

            IPy4JExceptionHandler errorHandler = (e) -> {
                Display.getDefault().syncExec(() -> {
                    annotatePythonException(e);
                    revealErrorAnnotation();
                });
            };

            if (chart instanceof MatplotlibChart)
                matplotlibChartViewer.runPythonScript(chart.getScript(), scaveEditor.getAnfFileDirectory(), afterRun,
                        errorHandler);
            else
                nativeChartViewer.runPythonScript(scaveEditor.getAnfFileDirectory(), afterRun, errorHandler);
        });
    }

    private void annotatePythonException(Py4JException e) {
        String msg = e.getMessage();

        // System.out.println("msg: " + msg);

        String[] parts = msg.split("File \"<string>\", line ", 2);
        //System.out.println(parts[0]);
        String[] parts2 = parts[1].split("(,|\n)", 2);

        int line = Integer.parseInt(parts2[0]);

        // String[] parts3 = parts[1].split("(in <module>)?\n", 2);
        // String msg2 = parts3[1];

        IDocument doc = getDocument();

        int offset = 0;
        int length = doc.getLength();

        try {
            offset = doc.getLineOffset(line - 1);
            length = doc.getLineLength(line - 1);
        }
        catch (BadLocationException exc) {
            // ignore
        }

        String problemMessage = null;

        if (msg.contains("py4j.protocol.Py4JJavaError")) {
            problemMessage = StringUtils.substringAfter(msg, "py4j.protocol.Py4JJavaError: ");
            problemMessage = StringUtils.substringAfterLast(problemMessage, ": ");
            problemMessage = StringUtils.substringBefore(problemMessage, "\n");
        } else
            problemMessage = StringUtils.substringAfterLast(msg.trim(), "\n");

        try {
            if (errorMarker != null)
                errorMarker.delete();

            errorMarker = scaveEditor.getInputFile().createMarker(IMarker.PROBLEM);
            errorMarker.setAttribute(IMarker.SEVERITY, IMarker.SEVERITY_ERROR);
            errorMarker.setAttribute(IMarker.TRANSIENT, true);
            errorMarker.setAttribute(IMarker.MESSAGE, problemMessage);
            errorMarker.setAttribute(IMarker.LINE_NUMBER, line);

            errorMarkerAnnotation = new MarkerAnnotation(errorMarker);

            documentProvider.annotationModel.removeAnnotation(errorMarkerAnnotation);
            documentProvider.annotationModel.addAnnotation(errorMarkerAnnotation, new Position(offset, length));

        }
        catch (CoreException e1) {
        }
    }

    public void updateActiveAction(String action) {
        if (action == null)
            action = "";
        interactAction.setChecked(action.equals(""));
        zoomAction.setChecked(action.equalsIgnoreCase("zoom"));
        panAction.setChecked(action.equalsIgnoreCase("pan"));
    }

    public void updateActions() {
        formEditor.setToolbarActionVisible(saveTempChartAction, chart.isTemporary());
        formEditor.setToolbarActionVisible(gotoChartDefinitionAction, !chart.isTemporary());

        saveTempChartAction.updateEnabled();
        gotoChartDefinitionAction.updateEnabled();

        zoomInHorizAction.updateEnabled();
        zoomOutHorizAction.updateEnabled();
        zoomInVertAction.updateEnabled();
        zoomOutVertAction.updateEnabled();

        killAction.updateEnabled();

        interactAction.updateEnabled();
        panAction.updateEnabled();
        zoomAction.updateEnabled();

        homeAction.updateEnabled();
        backAction.updateEnabled();
        forwardAction.updateEnabled();

        exportAction.updateEnabled();
    }

    public void setShowSource(boolean show) {
        toggleShowSourceAction.setChecked(show);
        sourceEditorContainer.setVisible(show);
        sashForm.layout();
        showSource = show;
    }

    public void revealErrorAnnotation() {
        setShowSource(true);

        Position pos = documentProvider.annotationModel.getPosition(errorMarkerAnnotation);
        getSourceViewer().revealRange(pos.offset, pos.length);
    }

    public boolean isSourceShown() {
        return showSource;
    }

    public void setShowChart(boolean show) {
        if (nativeChartViewer != null)
            nativeChartViewer.setVisible(show);

        if (matplotlibChartViewer != null)
            matplotlibChartViewer.setVisible(show);

        sashForm.layout();
    }

    public boolean isPythonProcessAlive() {
        return (matplotlibChartViewer != null && matplotlibChartViewer.isAlive())
                || (nativeChartViewer != null && nativeChartViewer.isAlive());
    }

    public void killPythonProcess() {
        if (matplotlibChartViewer != null)
            matplotlibChartViewer.killPythonProcess();
        if (nativeChartViewer != null)
            nativeChartViewer.killPythonProcess();

        updateActions();
    }

    public MatplotlibChartViewer getMatplotlibChartViewer() {
        return matplotlibChartViewer;
    }

    public NativeChartViewer getNativeChartViewer() {
        return nativeChartViewer;
    }

    @Override
    public IDocumentProvider getDocumentProvider() {
        return documentProvider;
    }

    public Chart getChart() {
        return chart;
    }

    public String getChartName() {
        return (chart.getName() == null || chart.getName().isEmpty()) ? "<unnamed>" : chart.getName();
    }

    @Override
    public boolean isDirty() {
        return getDocument().get() != chart.getScript();
    }

    @Override
    public boolean isSaveAsAllowed() {
        return false;
    }

    @Override
    public void dispose() {

        if (matplotlibChartViewer != null)
            matplotlibChartViewer.dispose();

        if (nativeChartViewer != null)
            nativeChartViewer.dispose();

        try {
            consoleStream.close();
        }
        catch (IOException e) {
        }

        IConsoleManager consoleManager = ConsolePlugin.getDefault().getConsoleManager();
        consoleManager.removeConsoles(new IConsole[] { console });

        documentProvider.annotationModel.removeAnnotation(errorMarkerAnnotation);

        try {
            if (errorMarker != null)
                errorMarker.delete();
        }
        catch (CoreException e) {
        }
    }

    public void refreshChart() {
        updateActions();

        if (!chart.getScript().equals(getDocument().get())) {
            SetCommand cmd = (SetCommand) SetCommand.create(scaveEditor.getEditingDomain(), chart,
                    chart.eClass().getEStructuralFeature("script"), getDocument().get());
            scaveEditor.getEditingDomain().getCommandStack().execute(cmd);
        }

        runChartScript();
    }

    public void setAutoUpdateEnabled(boolean enabled) {
        toggleAutoUpdateAction.setChecked(enabled);
        autoUpdateEnabled = enabled;
    }

    @Override
    public void saveState(IMemento memento) {
        super.saveState(memento);
        memento.putBoolean("autoupdate", autoUpdateEnabled);
        memento.putBoolean("sourcevisible", isSourceShown());
        int[] weights = sashForm.getWeights();
        memento.putInteger("sashweight_left", weights[0]);
        memento.putInteger("sashweight_right", weights[1]);
    }

    @Override
    public void restoreState(IMemento memento) {
        super.restoreState(memento);

        setAutoUpdateEnabled(memento.getBoolean("autoupdate"));
        setShowSource(memento.getBoolean("sourcevisible"));

        int[] weights = new int[2];
        weights[0] = memento.getInteger("sashweight_left");
        weights[1] = memento.getInteger("sashweight_right");
        sashForm.setWeights(weights);
    }

    public void pageActivated() {
        scaveEditor.setSelection(new StructuredSelection(chart));
    }
}
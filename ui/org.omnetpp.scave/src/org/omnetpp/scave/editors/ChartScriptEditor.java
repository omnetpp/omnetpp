package org.omnetpp.scave.editors;

import java.io.IOException;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.Position;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.graphics.Color;
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
import org.omnetpp.scave.actions.AddVectorOperationAction;
import org.omnetpp.scave.actions.ChartMouseModeAction;
import org.omnetpp.scave.actions.ClosePageAction;
import org.omnetpp.scave.actions.CopyChartToClipboardAction;
import org.omnetpp.scave.actions.EditAction;
import org.omnetpp.scave.actions.ExportToSVGAction;
import org.omnetpp.scave.actions.RefreshChartAction;
import org.omnetpp.scave.actions.SaveTempChartAction;
import org.omnetpp.scave.actions.ToggleShowSourceAction;
import org.omnetpp.scave.actions.ZoomChartAction;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.editors.ui.FormEditorPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.ChartType;
import org.omnetpp.scave.model.commands.ICommand;
import org.omnetpp.scave.model.commands.SetChartScriptCommand;
import org.omnetpp.scave.pychart.PlotWidget;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonOutputMonitoringThread.IOutputListener;
import org.omnetpp.scave.python.BackAction;
import org.omnetpp.scave.python.ChartViewerBase;
import org.omnetpp.scave.python.ExportAction;
import org.omnetpp.scave.python.ForwardAction;
import org.omnetpp.scave.python.GotoChartDefinitionAction;
import org.omnetpp.scave.python.HomeAction;
import org.omnetpp.scave.python.InteractAction;
import org.omnetpp.scave.python.KillPythonProcessAction;
import org.omnetpp.scave.python.MatplotlibChartViewer;
import org.omnetpp.scave.python.MatplotlibChartViewer.IStateChangeListener;
import org.omnetpp.scave.python.NativeChartViewer;
import org.omnetpp.scave.python.PanAction;
import org.omnetpp.scave.python.ToggleAutoUpdateAction;
import org.omnetpp.scave.python.ZoomAction;
import org.python.pydev.editor.PyEdit;
import org.python.pydev.shared_core.callbacks.ICallbackListener;

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

    ChartUpdater chartUpdater;
    ChartScriptEditorInput editorInput;

    IOConsole console;
    IOConsoleOutputStream outputStream;
    IOConsoleOutputStream errorStream;

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
    boolean scriptNotYetExecuted = true;

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
        outputStream = console.newOutputStream();
        errorStream = console.newOutputStream();
        errorStream.setColor(new Color(Display.getCurrent(), 220, 10, 10));

        onCreatePartControl.registerListener(new ICallbackListener<Composite>() {

            @Override
            public Object call(Composite obj) {

                obj.addDisposeListener(new DisposeListener() {
                    @Override
                    public void widgetDisposed(DisposeEvent e) {
                        ChartScriptEditor.this.dispose();
                    }
                });

                formEditor = new ChartPage(obj, SWT.NONE, scaveEditor, ChartScriptEditor.this);
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

                if (chart.getType() == ChartType.MATPLOTLIB) {
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

                IOutputListener outputListener = (output, err) -> {
                    Display.getDefault().asyncExec(() -> {
                        try {
                            if (err)
                                errorStream.write(output);
                            else
                                outputStream.write(output);
                        }
                        catch (IOException e) {
                            ScavePlugin.logError(e);
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


                IMenuManager applySubmenuManager = new MenuManager("Apply...", ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_APPLY), null);
                applySubmenuManager.add(new AddVectorOperationAction("Aggregator", "df = ops.apply(df, ops.vector_aggregator, 'average') # Possible parameter values: 'sum', 'average', 'count', 'maximum', 'minimum'"));
                applySubmenuManager.add(new AddVectorOperationAction("Merger", "df = ops.apply(df, ops.vector_merger)"));
                applySubmenuManager.add(new AddVectorOperationAction("Mean", "df = ops.apply(df, ops.vector_mean)"));
                applySubmenuManager.add(new AddVectorOperationAction("Sum", "df = ops.apply(df, ops.vector_sum)"));
                applySubmenuManager.add(new AddVectorOperationAction("Add constant", "df = ops.apply(df, ops.vector_add, 100) # Feel free to change the constant right here"));
                applySubmenuManager.add(new AddVectorOperationAction("Compare with threshold", "df = ops.apply(df, ops.vector_compare, threshold=9000, less=-1, equal=0, greater=1) # The last three parameters are all optional"));
                applySubmenuManager.add(new AddVectorOperationAction("Crop in time", "df = ops.apply(df, ops.vector_crop, from_time=10, to_time=100) # The time values are in seconds, feel free to change them"));
                applySubmenuManager.add(new AddVectorOperationAction("Difference", "df = ops.apply(df, ops.vector_difference) # The difference of each value compared to the previous"));
                applySubmenuManager.add(new AddVectorOperationAction("Difference quotient", "df = ops.apply(df, ops.vector_diffquot) # The difference quotient at each value"));
                applySubmenuManager.add(new AddVectorOperationAction("Divide by constant", "df = ops.apply(df, ops.vector_divide_by, 1000) # Feel free to change the constant right here"));
                applySubmenuManager.add(new AddVectorOperationAction("Divide by time", "df = ops.apply(df, ops.vector_divtime)"));
                applySubmenuManager.add(new AddVectorOperationAction("Expression", "df = ops.apply(df, ops.vector_expression, 'y + (t - tprev) * 100') # The following identifiers are available: t, y, tprev, yprev"));
                applySubmenuManager.add(new AddVectorOperationAction("Integrate", "df = ops.apply(df, ops.vector_integrate, interpolation='linear') # Possible parameter values: 'sample-hold', 'backward-sample-hold', 'linear'"));
                applySubmenuManager.add(new AddVectorOperationAction("Linear trend", "df = ops.apply(df, ops.vector_lineartrend, 0.5) # Add a linear trend to the values, with the given steepness"));
                applySubmenuManager.add(new AddVectorOperationAction("Modulo", "df = ops.apply(df, ops.vector_modulo, 256.0) # Floating point remainder with the given divisor"));
                applySubmenuManager.add(new AddVectorOperationAction("Moving average", "df = ops.apply(df, ops.vector_movingavg, alpha=0.1) # Moving average using the given smoothing constant in range (0.0, 1.0]"));
                applySubmenuManager.add(new AddVectorOperationAction("Multiply by constant", "df = ops.apply(df, ops.vector_multiply_by, 2) # You can change the multiplier constant"));
                applySubmenuManager.add(new AddVectorOperationAction("Remove repeating values", "df = ops.apply(df, ops.vector_removerepeats) # Removes consequtive equal values"));
                applySubmenuManager.add(new AddVectorOperationAction("Sliding window average", "df = ops.apply(df, ops.vector_slidingwinavg, window_size=10) # The average of the last window_size values"));
                applySubmenuManager.add(new AddVectorOperationAction("Subtract first value", "df = ops.apply(df, ops.vector_subtractfirstval) # Subtracts the first value from all values"));
                applySubmenuManager.add(new AddVectorOperationAction("Time average", "df = ops.apply(df, ops.vector_timeavg, interpolation='linear') # Average over time (integral divided by time), possible parameter values: 'sample-hold', 'backward-sample-hold', 'linear'"));
                applySubmenuManager.add(new AddVectorOperationAction("Time difference", "df = ops.apply(df, ops.vector_timediff) # The elapsed time (delta) since the previous value"));
                applySubmenuManager.add(new AddVectorOperationAction("Time shift", "df = ops.apply(df, ops.vector_timeshift, dt=100) # Shifts all values with the given time delta (in seconds)"));
                applySubmenuManager.add(new AddVectorOperationAction("Time to serial", "df = ops.apply(df, ops.vector_timetoserial) # Replaces all times with a serial number: 0, 1, 2, ..."));
                applySubmenuManager.add(new AddVectorOperationAction("Time window average", "df = ops.apply(df, ops.vector_timewinavg, window_size=0.1) # Average of all values in every window_size long interval (in seconds)"));
                applySubmenuManager.add(new AddVectorOperationAction("Window average", "df = ops.apply(df, ops.vector_winavg, window_size=10) # Average of every window_size long batch of values"));




                IMenuManager computeSubmenuManager = new MenuManager("Compute...", ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_COMPUTE), null);
                computeSubmenuManager.add(new AddVectorOperationAction("Aggregator", "df = ops.compute(df, ops.vector_aggregator, 'average') # Possible parameter values: 'sum', 'average', 'count', 'maximum', 'minimum'"));
                computeSubmenuManager.add(new AddVectorOperationAction("Merger", "df = ops.compute(df, ops.vector_merger)"));
                computeSubmenuManager.add(new AddVectorOperationAction("Mean", "df = ops.compute(df, ops.vector_mean)"));
                computeSubmenuManager.add(new AddVectorOperationAction("Sum", "df = ops.compute(df, ops.vector_sum)"));
                computeSubmenuManager.add(new AddVectorOperationAction("Add constant", "df = ops.compute(df, ops.vector_add, 100) # Feel free to change the constant right here"));
                computeSubmenuManager.add(new AddVectorOperationAction("Compare with threshold", "df = ops.compute(df, ops.vector_compare, threshold=9000, less=-1, equal=0, greater=1) # The last three parameters are all optional"));
                computeSubmenuManager.add(new AddVectorOperationAction("Crop in time", "df = ops.compute(df, ops.vector_crop, from_time=10, to_time=100) # The time values are in seconds, feel free to change them"));
                computeSubmenuManager.add(new AddVectorOperationAction("Difference", "df = ops.compute(df, ops.vector_difference) # The difference of each value compared to the previous"));
                computeSubmenuManager.add(new AddVectorOperationAction("Difference quotient", "df = ops.compute(df, ops.vector_diffquot) # The difference quotient at each value"));
                computeSubmenuManager.add(new AddVectorOperationAction("Divide by constant", "df = ops.compute(df, ops.vector_divide_by, 1000) # Feel free to change the constant right here"));
                computeSubmenuManager.add(new AddVectorOperationAction("Divide by time", "df = ops.compute(df, ops.vector_divtime)"));
                computeSubmenuManager.add(new AddVectorOperationAction("Expression", "df = ops.compute(df, ops.vector_expression, 'y + (t - tprev) * 100') # The following identifiers are available: t, y, tprev, yprev"));
                computeSubmenuManager.add(new AddVectorOperationAction("Integrate", "df = ops.compute(df, ops.vector_integrate, interpolation='linear') # Possible parameter values: 'sample-hold', 'backward-sample-hold', 'linear'"));
                computeSubmenuManager.add(new AddVectorOperationAction("Linear trend", "df = ops.compute(df, ops.vector_lineartrend, 0.5) # Add a linear trend to the values, with the given steepness"));
                computeSubmenuManager.add(new AddVectorOperationAction("Modulo", "df = ops.compute(df, ops.vector_modulo, 256.0) # Floating point remainder with the given divisor"));
                computeSubmenuManager.add(new AddVectorOperationAction("Moving average", "df = ops.compute(df, ops.vector_movingavg, alpha=0.1) # Moving average using the given smoothing constant in range (0.0, 1.0]"));
                computeSubmenuManager.add(new AddVectorOperationAction("Multiply by constant", "df = ops.compute(df, ops.vector_multiply_by, 2) # You can change the multiplier constant"));
                computeSubmenuManager.add(new AddVectorOperationAction("Remove repeating values", "df = ops.compute(df, ops.vector_removerepeats) # Removes consequtive equal values"));
                computeSubmenuManager.add(new AddVectorOperationAction("Sliding window average", "df = ops.compute(df, ops.vector_slidingwinavg, window_size=10) # The average of the last window_size values"));
                computeSubmenuManager.add(new AddVectorOperationAction("Subtract first value", "df = ops.compute(df, ops.vector_subtractfirstval) # Subtracts the first value from all values"));
                computeSubmenuManager.add(new AddVectorOperationAction("Time average", "df = ops.compute(df, ops.vector_timeavg, interpolation='linear') # Average over time (integral divided by time), possible parameter values: 'sample-hold', 'backward-sample-hold', 'linear'"));
                computeSubmenuManager.add(new AddVectorOperationAction("Time difference", "df = ops.compute(df, ops.vector_timediff) # The elapsed time (delta) since the previous value"));
                computeSubmenuManager.add(new AddVectorOperationAction("Time shift", "df = ops.compute(df, ops.vector_timeshift, dt=100) # Shifts all values with the given time delta (in seconds)"));
                computeSubmenuManager.add(new AddVectorOperationAction("Time to serial", "df = ops.compute(df, ops.vector_timetoserial) # Replaces all times with a serial number: 0, 1, 2, ..."));
                computeSubmenuManager.add(new AddVectorOperationAction("Time window average", "df = ops.compute(df, ops.vector_timewinavg, window_size=0.1) # Average of all values in every window_size long interval (in seconds)"));
                computeSubmenuManager.add(new AddVectorOperationAction("Window average", "df = ops.compute(df, ops.vector_winavg, window_size=10) # Average of every window_size long batch of values"));


                if (chart.getType() == ChartType.MATPLOTLIB) {
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

                    manager.add(applySubmenuManager);
                    manager.add(computeSubmenuManager);
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

                    if (chart.getType() == ChartType.LINE) {
                        manager.add(new Separator());
                        manager.add(applySubmenuManager);
                        manager.add(computeSubmenuManager);
                    }

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

                return obj;
            }

        });

        editorInput = new ChartScriptEditorInput(chart);

        chartUpdater = new ChartUpdater(chart, this);

        getDocumentProvider().getDocument(editorInput).addDocumentListener(chartUpdater);
        chart.addListener(chartUpdater);
    }

    public ScaveEditor getScaveEditor() {
        return scaveEditor;
    }

    public void runChartScript() {
        scriptNotYetExecuted = false;

        Display.getDefault().syncExec(() -> {
            console.clearConsole();
            documentProvider.annotationModel.removeAnnotation(errorMarkerAnnotation);

            try {
                if (errorMarker != null)
                    errorMarker.delete();
            }
            catch (CoreException e1) {
                ScavePlugin.logError(e1);
            }
            errorMarker = null;

            Runnable afterRun = () -> {
                Display.getDefault().syncExec(() -> {
                    updateActions();
                });
            };

            ExceptionHandler errorHandler = (proc, e) -> {
                Display.getDefault().syncExec(() -> {
                    if (!proc.isKilledByUs()) {
                        annotatePythonException(e);
                        revealErrorAnnotation();
                    }
                });
            };

            getChartViewer().runPythonScript(chart.getScript(), scaveEditor.getAnfFileDirectory(), afterRun, errorHandler);
        });
    }

    private void annotatePythonException(Exception e) {
        String msg = e.getMessage();

        String problemMessage = null;

        IDocument doc = getDocument();

        int offset = 0;
        int length = doc.getLength();
        int line = 0;

        String[] parts = msg.split("File \"<string>\", line ", 2);

        if (parts.length == 0)
            problemMessage = "Unknown error.";
        else if (parts.length == 1)
            problemMessage = parts[0].trim();
        else {
            String[] parts2 = parts[1].split("(,|\n)", 2);

            line = Integer.parseInt(parts2[0]);

            try {
                offset = doc.getLineOffset(line - 1);
                length = doc.getLineLength(line - 1);
            }
            catch (BadLocationException exc) {
                // ignore
            }

            if (msg.contains("py4j.protocol.Py4JJavaError")) {
                problemMessage = StringUtils.substringAfter(msg, "py4j.protocol.Py4JJavaError: ");
                problemMessage = StringUtils.substringAfterLast(problemMessage, ": ");
                problemMessage = StringUtils.substringBefore(problemMessage, "\n");
            } else
                problemMessage = StringUtils.substringAfterLast(msg.trim(), "\n");
        }

        try {
            documentProvider.annotationModel.removeAnnotation(errorMarkerAnnotation);
            if (errorMarker != null)
                errorMarker.delete();

            errorMarker = scaveEditor.getInputFile().createMarker(IMarker.PROBLEM);
            errorMarker.setAttribute(IMarker.SEVERITY, IMarker.SEVERITY_ERROR);
            errorMarker.setAttribute(IMarker.TRANSIENT, true);
            errorMarker.setAttribute(IMarker.MESSAGE, problemMessage);
            errorMarker.setAttribute(IMarker.LINE_NUMBER, line);
            errorMarker.setAttribute(IMarker.SOURCE_ID, Integer.toString(scaveEditor.getAnalysis().getCharts().getCharts().indexOf(chart)));

            errorMarkerAnnotation = new MarkerAnnotation(errorMarker);
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
        if (pos != null)
            getSourceViewer().revealRange(pos.offset, pos.length);
    }

    public void gotoErrorAnnotation() {
        revealErrorAnnotation();
        Position pos = documentProvider.annotationModel.getPosition(errorMarkerAnnotation);
        getSourceViewer().setSelectedRange(pos.offset, 0);
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
        return getChartViewer().isAlive();
    }

    public void killPythonProcess() {
        getChartViewer().killPythonProcess();
    }

    public MatplotlibChartViewer getMatplotlibChartViewer() {
        return matplotlibChartViewer;
    }

    public NativeChartViewer getNativeChartViewer() {
        return nativeChartViewer;
    }

    public ChartViewerBase getChartViewer() {
        return (chart.getType() == ChartType.MATPLOTLIB) ? matplotlibChartViewer : nativeChartViewer;
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

    public void prepareForSave() {
        chartUpdater.prepareForSave();
    }

    @Override
    public void dispose() {

        getDocumentProvider().getDocument(editorInput).removeDocumentListener(chartUpdater);
        chart.removeListener(chartUpdater);

        if (matplotlibChartViewer != null)
            matplotlibChartViewer.dispose();

        if (nativeChartViewer != null)
            nativeChartViewer.dispose();

        try {
            outputStream.close();
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
            ICommand command = new SetChartScriptCommand(chart, getDocument().get());
            scaveEditor.executeCommand(command);
        }

        runChartScript();
    }

    public void setAutoUpdateEnabled(boolean enabled) {
        toggleAutoUpdateAction.setChecked(enabled);
        chartUpdater.setAutoUpdateChart(enabled);
    }

    @Override
    public void saveState(IMemento memento) {
        super.saveState(memento);
        memento.putBoolean("autoupdate", chartUpdater.isAutoUpdateChart());
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
        if (scriptNotYetExecuted)
            runChartScript();
        scaveEditor.setSelection(new StructuredSelection(chart));
        console.activate();
    }

    public void gotoMarker(IMarker marker) {
        Assert.isTrue(this.errorMarker.equals(marker));
        gotoErrorAnnotation();
    }
}

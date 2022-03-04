/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.DocumentEvent;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IDocumentListener;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Position;
import org.eclipse.jface.text.contentassist.IContentAssistProcessor;
import org.eclipse.jface.text.contentassist.IContentAssistant;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.text.source.SourceViewerConfiguration;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.ISaveablePart2;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IViewReference;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.console.ConsolePlugin;
import org.eclipse.ui.console.IConsole;
import org.eclipse.ui.console.IConsoleManager;
import org.eclipse.ui.console.IHyperlink;
import org.eclipse.ui.console.IOConsole;
import org.eclipse.ui.console.IOConsoleOutputStream;
import org.eclipse.ui.console.IPatternMatchListener;
import org.eclipse.ui.console.PatternMatchEvent;
import org.eclipse.ui.console.TextConsole;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.internal.console.ConsoleView;
import org.eclipse.ui.texteditor.IDocumentProvider;
import org.eclipse.ui.texteditor.MarkerAnnotation;
import org.omnetpp.common.Debug;
import org.omnetpp.common.canvas.LargeScrollableCanvas;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.ClosePageAction;
import org.omnetpp.scave.actions.ConfigureChartAction;
import org.omnetpp.scave.actions.CopyImageToClipboardAction;
import org.omnetpp.scave.actions.ExportChartAction;
import org.omnetpp.scave.actions.KillPythonProcessAction;
import org.omnetpp.scave.actions.RefreshChartAction;
import org.omnetpp.scave.actions.RefreshResultFilesAction;
import org.omnetpp.scave.actions.SaveImageAction;
import org.omnetpp.scave.actions.ToggleAutoRefreshAction;
import org.omnetpp.scave.actions.analysismodel.AddVectorOperationAction;
import org.omnetpp.scave.actions.analysismodel.SaveTempChartAction;
import org.omnetpp.scave.actions.matplotlib.BackAction;
import org.omnetpp.scave.actions.matplotlib.ForwardAction;
import org.omnetpp.scave.actions.matplotlib.HomeAction;
import org.omnetpp.scave.actions.matplotlib.InteractAction;
import org.omnetpp.scave.actions.matplotlib.PanAction;
import org.omnetpp.scave.actions.matplotlib.ZoomAction;
import org.omnetpp.scave.actions.nativeplots.ChartMouseModeAction;
import org.omnetpp.scave.actions.nativeplots.ZoomChartAction;
import org.omnetpp.scave.actions.ui.GotoChartDefinitionAction;
import org.omnetpp.scave.actions.ui.ToggleShowSourceAction;
import org.omnetpp.scave.charting.PlotBase;
import org.omnetpp.scave.editors.VectorOperations.VectorOp;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerChangeEvent;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.ChartType;
import org.omnetpp.scave.model.IModelChangeListener;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.pychart.MatplotlibWidget;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonOutputMonitoringThread.IOutputListener;
import org.omnetpp.scave.pychart.PythonProcess;
import org.omnetpp.scave.python.ChartViewerBase;
import org.omnetpp.scave.python.MatplotlibChartViewer;
import org.omnetpp.scave.python.MatplotlibChartViewer.IStateChangeListener;
import org.omnetpp.scave.python.NativeChartViewer;
import org.python.pydev.core.IIndentPrefs;
import org.python.pydev.core.autoedit.PyAutoIndentStrategy;
import org.python.pydev.editor.PyEdit;
import org.python.pydev.editor.PyEditConfiguration;
import org.python.pydev.plugin.PyDevUiPrefs;
import org.python.pydev.shared_core.callbacks.ICallbackListener;
import org.python.pydev.shared_ui.editor_input.PydevFileEditorInput;


/**
 * A TextEditor that can open a Chart, show its script in an editable way,
 * execute the chart script in a PythonProcess, and show an embedded plot as
 * its output, in the form of either a matplotlib or a native plot widget.
 *
 * An extension of the main PyDev Python code editor, so it has Python syntax
 * highlighting and checking, and content assist support built in.
 */
@SuppressWarnings("restriction")
public class ChartScriptEditor extends PyEdit {  //TODO ChartEditor?

    /** The Chart being shown, edited, and executed */
    private Chart chart;

    /**
     * A copy of the edited Chart as it was when it was opened.
     * Is never modified. Kept around just so all changes made to the chart
     * while this editor is open can be undone later in a single step.
     */
    private Chart originalChart;

    /** Reference to the ScaveEditor this EditorPart is embedded into. */
    private ScaveEditor scaveEditor;

    /** The (closable) page of the parent ScaveEditor this editor is on. */
    private ChartPage chartPage;

    /**
     * The movable "splitter" widget that contains the Python source editor
     * on one side, and the plot viewer widget on the other.
     */
    private SashForm sashForm;

    /**
     * The outermost Container of the PyEdit Control tree, a direct child of
     * sashForm, is shown/hidden as requested by the user.
     */
    private Composite sourceEditorContainer;


    /**
     * The ChartViewer used for native Charts. Puts the native plot widget
     * into sashForm. Is null if the Chart is of type MATPLOTLIB.
     */
    private NativeChartViewer nativeChartViewer;

    /**
     * The ChartViewer used for matplotlib Charts. Puts the MatplotlibWidget
     * into sashForm. Is null if the Chart is not of type MATPLOTLIB.
     */
    private MatplotlibChartViewer matplotlibChartViewer;

    /** Listens for changes on both the Chart object (for property edits)
     * and the Document (for script edits), and triggers a delayed refresh.
     */
    private ChangeListener changeListener;

    /** Wraps chart, so it can be passed to a ChartScriptDocumentProvider. */
    private ChartScriptEditorInput editorInput;

    private IOConsole console;
    private IOConsoleOutputStream outputStream;
    private IOConsoleOutputStream errorStream;

    private ChartScriptDocumentProvider documentProvider;
    protected CommandStack commandStack = new CommandStack("ChartPage");

    private IMarker errorMarker;
    private MarkerAnnotation errorMarkerAnnotation;

    private SaveTempChartAction saveTempChartAction;
    private GotoChartDefinitionAction gotoChartDefinitionAction;

    private SaveImageAction saveImageAction = new SaveImageAction();
    private ExportChartAction exportChartAction = new ExportChartAction();

    private InteractAction interactAction = new InteractAction();
    private PanAction panAction = new PanAction();
    private ZoomAction zoomAction = new ZoomAction();

    private HomeAction homeAction = new HomeAction();
    private BackAction backAction = new BackAction();
    private ForwardAction forwardAction = new ForwardAction();

    private ZoomChartAction zoomToFitAction = new ZoomChartAction(true, true, 0.0);
    private ZoomChartAction zoomInHorizAction = new ZoomChartAction(true, false, 2.0);
    private ZoomChartAction zoomOutHorizAction = new ZoomChartAction(true, false, 1 / 2.0);
    private ZoomChartAction zoomInVertAction = new ZoomChartAction(false, true, 2.0);
    private ZoomChartAction zoomOutVertAction = new ZoomChartAction(false, true, 1 / 2.0);

    private KillPythonProcessAction killAction;

    private boolean scriptNotYetExecuted = true;

    private ToggleShowSourceAction toggleShowSourceAction = new ToggleShowSourceAction();
    private boolean showSource = false;

    private static final int CHART_SCRIPT_EXECUTION_DELAY_MS = 2000;
    private boolean autoRefreshChart = true;
    private ToggleAutoRefreshAction toggleAutoRefreshAction;

    // if the document has changed since last saving (independent of the Chart object in the model)
    private boolean scriptChangedFlag = false;

    private VectorOperations vectorOps;

    public List<VectorOp> getVectorOpData() {
        return vectorOps.getVectorOpData();
    }

    // This runs in an extension point of the PyDev editor creation process.
    // Wedges the splitter (sashForm) control between the text editor and its originally intended parent.
    private final class CreatePartControlListener implements ICallbackListener<Composite> {
        @Override
        public Object call(Composite parent) {
            chartPage = new ChartPage(parent, SWT.NONE, scaveEditor, ChartScriptEditor.this);
            Composite content = chartPage.getContent();

            addToolbarActions();

            chartPage.setFormTitle(getChartDisplayName());
            GridLayout layout = new GridLayout(1, true);
            layout.marginWidth = 0;
            layout.marginHeight = 0;
            content.setLayout(layout);

            sashForm = new SashForm(content, SWT.NONE);
            sashForm.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

            chartPage.getToolbar().layout();
            chartPage.layout();
            content.layout();
            parent.layout();

            return sashForm;
        }
    }

    // This is called by PyDev after the editor control tree has been created.
    // Fills in the "other" side of the splitter (with the appropriate chart widget) and
    // hooks all kinds of listeners everywhere. Performs the rest of the initialization.
    private final class AfterCreatePartControlListener implements ICallbackListener<ISourceViewer> {
        @Override
        public Object call(ISourceViewer sourceViewer) {
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
                public void pythonProcessLivenessChanged(PythonProcess proc) {
                    Display.getDefault().syncExec(() -> {
                        if (!isDisposed()) {
                            if (!proc.getProcess().isAlive()) {
                                if (!proc.isKilledByUs() && proc.getProcess().exitValue() != 0) {
                                    try {
                                        errorStream.write("Python process exited with: " + proc.getProcess().exitValue() + "\n");
                                    }
                                    catch (IOException e) {
                                        e.printStackTrace();
                                    }
                                }
                                updateActions();
                                if (nativeChartViewer != null)
                                    nativeChartViewer.getPlot().setStatusText(null);
                            }
                        }
                    });
                }

                @Override
                public void activeActionChanged(String action) {
                    updateActiveAction(action);
                }
            };

            if (chart.getType() == ChartType.MATPLOTLIB) {
                matplotlibChartViewer = new MatplotlibChartViewer(sashForm, chart, scaveEditor.getPythonProcessPool(), scaveEditor.getResultFileManager(), scaveEditor.getMemoizationCache(), scaveEditor.getFilterCache());

                matplotlibChartViewer.addOutputListener(outputListener);
                matplotlibChartViewer.addStateChangeListener(stateChangeListener);

                MatplotlibWidget plotWidget = matplotlibChartViewer.getPlotWidget();
                plotWidget.setMenu(createMenuManager().createContextMenu(plotWidget));
            }
            else {
                nativeChartViewer = new NativeChartViewer(sashForm, chart, scaveEditor.getPythonProcessPool(), scaveEditor.getResultFileManager(), scaveEditor.getMemoizationCache(), scaveEditor.getFilterCache());

                nativeChartViewer.addOutputListener(outputListener);
                nativeChartViewer.addStateChangeListener(stateChangeListener);

                PlotBase chartViewer = nativeChartViewer.getPlot();

                chartViewer.addPropertyChangeListener(new IPropertyChangeListener() {
                    public void propertyChange(PropertyChangeEvent event) {
                        if (event.getProperty() == LargeScrollableCanvas.PROP_VIEW_X
                                || event.getProperty() == LargeScrollableCanvas.PROP_VIEW_Y
                                || event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_X
                                || event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_Y) {
                            updateActions();
                        }
                    }
                });

                chartViewer.setMenu(createMenuManager().createContextMenu(chartViewer));
                scaveEditor.getActions().registerPlot(chartViewer);
            }

            // delegating the global action redirection to the ActionBarContributor when focus changes
            sourceViewer.getTextWidget().addFocusListener(new FocusListener() {
                @Override
                public void focusLost(FocusEvent e) {
                    scaveEditor.getEditorSite().getActionBarContributor().setActiveEditor(null);
                }

                @Override
                public void focusGained(FocusEvent e) {
                    scaveEditor.getEditorSite().getActionBarContributor().setActiveEditor(null);
                }
            });

            getChartViewer().getWidget().addFocusListener(new FocusListener() {
                @Override
                public void focusLost(FocusEvent e) {
                    scaveEditor.getEditorSite().getActionBarContributor().setActiveEditor(null);
                }

                @Override
                public void focusGained(FocusEvent e) {
                    scaveEditor.getEditorSite().getActionBarContributor().setActiveEditor(null);
                }
            });

            sashForm.layout();

            if (chart.getScript() == null || chart.getScript().isEmpty())
                showSource = true;

            setShowSource(showSource);

            return sourceViewer;
        }
    }

    private class StackFrameHyperlink implements IHyperlink {
        private final String file;
        private final int lineNumber;

        public StackFrameHyperlink(String file, int lineNumber) {
            this.file = file;
            this.lineNumber = lineNumber;
        }

        @Override
        public void linkExited() {
        }

        @Override
        public void linkEntered() {
        }

        @Override
        public void linkActivated() {
            showSourceForStackFrame(file, lineNumber);
        }
    }

    private class PatternMatchListener implements IPatternMatchListener {
        @Override
        public void connect(TextConsole console) {
        }

        @Override
        public void disconnect() {
        }

        @Override
        public void matchFound(PatternMatchEvent event) {
            try {
                IDocument document = console.getDocument();
                String fileAndLine = document.get(event.getOffset(), event.getLength());
                Matcher matcher = Pattern.compile("File \"(.*?)\", line (\\d+)").matcher(fileAndLine);
                Assert.isTrue(matcher.matches());
                if (matcher.matches()) {
                    String fileName = matcher.group(1);
                    int lineNumber = Integer.parseInt(matcher.group(2));
                    console.addHyperlink(new StackFrameHyperlink(fileName, lineNumber), event.getOffset(), event.getLength());
                }
            }
            catch (BadLocationException e) {
                ScavePlugin.logError(e);
            }
        }

        @Override
        public String getPattern() {
            return "File \".*?\", line \\d+";
        }

        @Override
        public int getCompilerFlags() {
            return 0;
        }

        @Override
        public String getLineQualifier() {
            return null;
        }

    }

    private final DelayedJob rerunChartScriptJob = new DelayedJob(CHART_SCRIPT_EXECUTION_DELAY_MS) {
        @Override
        public void run() {
            refreshChart();
        }
    };

    private class ChangeListener implements IModelChangeListener, IDocumentListener, IResultFilesChangeListener {

        @Override
        public void resultFileManagerChanged(ResultFileManagerChangeEvent event) {
            if (autoRefreshChart)
                rerunChartScriptJob.restartTimer();
        }

        @Override
        public void documentAboutToBeChanged(DocumentEvent event) {
            // no-op
        }

        @Override
        public void documentChanged(DocumentEvent event) {
            scriptChangedFlag = true;
            firePropertyChange(ISaveablePart2.PROP_DIRTY);
            if (autoRefreshChart)
                rerunChartScriptJob.restartTimer();
        }

        @Override
        public void modelChanged(ModelChangeEvent event) {
            if (autoRefreshChart)
                rerunChartScriptJob.restartTimer();
        }

    }

    ChartScriptEditor(ScaveEditor scaveEditor, Chart chart) {
        this.chart = chart;
        this.originalChart = (Chart)chart.dup();
        this.scaveEditor = scaveEditor;

        this.documentProvider = new ChartScriptDocumentProvider();
        this.vectorOps = new VectorOperations();

        console = new IOConsole("Chart script output of " + getChartDisplayName(), null);
        IConsoleManager consoleManager = ConsolePlugin.getDefault().getConsoleManager();
        consoleManager.addConsoles(new IConsole[] { console });
        outputStream = console.newOutputStream();
        errorStream = console.newOutputStream();
        errorStream.setColor(new Color(Display.getCurrent(), 220, 10, 10));

        console.addPatternMatchListener(new PatternMatchListener());

        onCreatePartControl.registerListener(new CreatePartControlListener());
        onAfterCreatePartControl.registerListener(new AfterCreatePartControlListener());

        editorInput = new ChartScriptEditorInput(chart);

        changeListener = new ChangeListener();
        scaveEditor.getResultFilesTracker().addChangeListener(changeListener);
        getDocumentProvider().getDocument(editorInput).addDocumentListener(changeListener);
        chart.addListener(changeListener);

        setupConditionalContentAssist();
    }

    public ScaveEditor getScaveEditor() {
        return scaveEditor;
    }

    public CommandStack getCommandStack() {
        return commandStack;
    }


    protected MenuManager createMenuManager() {
        boolean isMatplotlib = chart.getType() == ChartType.MATPLOTLIB;
        boolean isNative = !isMatplotlib;

        // Make the Apply/Compute submenus
        IMenuManager applySubmenuManager = new MenuManager("Apply...", ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_APPLY), null);
        IMenuManager computeSubmenuManager = new MenuManager("Compute...", ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_COMPUTE), null);
        applySubmenuManager.setRemoveAllWhenShown(true);
        computeSubmenuManager.setRemoveAllWhenShown(true);

        IMenuListener submenuListener = (submenuManager) -> {
            // This is done twice, for each submenu independently. Oh well...
            List<VectorOp> vectorOpData = new ArrayList<>(vectorOps.getVectorOpData());
            vectorOpData.add(null); // represents the separator between user-added and built-in ops
            vectorOpData.sort((VectorOp a, VectorOp b) -> {
                Assert.isTrue(a != null || b != null); // only one null accepted

                // custom ops are put first, then the separator, then the built-in ones
                int groupA = (a == null) ? 1 : "omnetpp.scave.vectorops".equals(a.module) ? 2 : 0;
                int groupB = (b == null) ? 1 : "omnetpp.scave.vectorops".equals(b.module) ? 2 : 0;

                // within each group, sort alphabetically
                return (groupA == groupB) ? a.label.compareToIgnoreCase(b.label) : Integer.compare(groupA, groupB);
            });

            for (VectorOp op : vectorOpData) {
                if (op == null)
                    submenuManager.add(new Separator());
                else {
                    String prefix = (submenuManager == applySubmenuManager) ? "apply: " : "compute: ";
                    submenuManager.add(new AddVectorOperationAction(op.label, prefix + op.example, op.hasArgs));
                }
            }
        };

        applySubmenuManager.addMenuListener(submenuListener);
        computeSubmenuManager.addMenuListener(submenuListener);

        ScaveEditorActions actions = scaveEditor.getActions();

        // Assemble the menu
        MenuManager manager = new MenuManager();

        manager.add(new ConfigureChartAction());
        manager.add(new Separator());

        manager.add(actions.undoAction);
        manager.add(actions.redoAction);
        manager.add(new Separator());

        if (isNative) {
            IMenuManager zoomSubmenuManager = new MenuManager("Zoom", ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_ZOOM), null);
            zoomSubmenuManager.add(zoomInHorizAction);
            zoomSubmenuManager.add(zoomOutHorizAction);
            zoomSubmenuManager.add(zoomInVertAction);
            zoomSubmenuManager.add(zoomOutVertAction);
            zoomSubmenuManager.add(new Separator());
            zoomSubmenuManager.add(zoomToFitAction);

            manager.add(zoomSubmenuManager);
            manager.add(new Separator());
        }

        if (isMatplotlib) {
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
        }

        if (isMatplotlib || (isNative && chart.getType() == ChartType.LINE)) {
            manager.add(applySubmenuManager);
            manager.add(computeSubmenuManager);
            manager.add(new Separator());
        }

        manager.add(toggleShowSourceAction);
        manager.add(new RefreshChartAction());
        manager.add(toggleAutoRefreshAction);
        manager.add(killAction);
        manager.add(new Separator());

        if (chart.isTemporary())
            manager.add(new SaveTempChartAction());
        else
            manager.add(new GotoChartDefinitionAction());
        manager.add(new Separator());

        manager.add(new CopyImageToClipboardAction());
        manager.add(saveImageAction);
        manager.add(exportChartAction);

        return manager;
    }

    private void addToolbarActions() {
        saveTempChartAction = new SaveTempChartAction();
        gotoChartDefinitionAction = new GotoChartDefinitionAction();
        toggleAutoRefreshAction = new ToggleAutoRefreshAction();
        toggleAutoRefreshAction.setChecked(true);

        chartPage.addToToolbar(saveTempChartAction);
        chartPage.addToToolbar(gotoChartDefinitionAction);

        chartPage.setToolbarActionVisible(saveTempChartAction, chart.isTemporary());
        chartPage.setToolbarActionVisible(gotoChartDefinitionAction, !chart.isTemporary());

        chartPage.addSeparatorToToolbar();

        chartPage.addToToolbar(new ConfigureChartAction());
        chartPage.addToToolbar(toggleShowSourceAction);

        if (chart.getType() == ChartType.MATPLOTLIB) {
            chartPage.addSeparatorToToolbar();
            interactAction.setChecked(true);
            chartPage.addToToolbar(interactAction);
            chartPage.addToToolbar(panAction);
            chartPage.addToToolbar(zoomAction);
            chartPage.addSeparatorToToolbar();
            chartPage.addToToolbar(homeAction);
            chartPage.addToToolbar(backAction);
            chartPage.addToToolbar(forwardAction);
        }
        else {
            chartPage.addSeparatorToToolbar();
            ChartMouseModeAction panAction = new ChartMouseModeAction(ZoomableCanvasMouseSupport.PAN_MODE);
            chartPage.addToToolbar(panAction);
            panAction.setChecked(true);
            chartPage.addToToolbar(new ChartMouseModeAction(ZoomableCanvasMouseSupport.ZOOM_MODE));
            chartPage.addSeparatorToToolbar();
            chartPage.addToToolbar(zoomToFitAction);
            chartPage.addSeparatorToToolbar();
            chartPage.addToToolbar(zoomInHorizAction);
            chartPage.addToToolbar(zoomOutHorizAction);
            chartPage.addToToolbar(zoomInVertAction);
            chartPage.addToToolbar(zoomOutVertAction);
        }

        chartPage.addSeparatorToToolbar();
        chartPage.addToToolbar(new RefreshChartAction());
        chartPage.addToToolbar(toggleAutoRefreshAction);
        chartPage.addToToolbar(killAction = new KillPythonProcessAction());
        chartPage.addSeparatorToToolbar();
        chartPage.addToToolbar(new CopyImageToClipboardAction());
        chartPage.addToToolbar(saveImageAction);
        chartPage.addToToolbar(exportChartAction);
        chartPage.addSeparatorToToolbar();
        chartPage.addToToolbar(new RefreshResultFilesAction());
        chartPage.addSeparatorToToolbar();
        chartPage.addToToolbar(new ClosePageAction());
    }

    // Overriding this is only necessary because we are replacing the SourceViewerConfiguration,
    // and there were some inconsistencies, with the original IIndentPrefs referring to the original config?
    @Override
    public IIndentPrefs getIndentPrefs() {
        return getAutoEditStrategy().getIndentPrefs();
    }

    // Overriding this is only necessary because we are replacing the SourceViewerConfiguration,
    // and there were some inconsistencies, with the original AutoIndentStrategy referring to the original config?
    public PyAutoIndentStrategy getAutoEditStrategy() {
        return ((PyEditConfiguration)getSourceViewerConfiguration()).getPyAutoIndentStrategy(this);
    }

    // to make sure the Ctrl+Space hotkey does not show the completion popup when
    // the script editor is not visible or not focused.
    private void setupConditionalContentAssist() {
        final SourceViewerConfiguration originalConfiguration = getSourceViewerConfiguration();
        // We have to create and inject two inline proxy subclasses, just so we can insert those
        // conditions for showing the popup. There might be a better way, suppressing this mechanism
        // earlier, but with Eclipse, one can never know...
        setSourceViewerConfiguration(new PyEditConfiguration(getColorCache(), this, PyDevUiPrefs.getChainedPrefStore()) {
            @Override
            public IContentAssistant getContentAssistant(ISourceViewer sourceViewer) {
                final IContentAssistant originalAssistant = originalConfiguration.getContentAssistant(sourceViewer);
                return new IContentAssistant() {
                    @Override
                    public void uninstall() {
                        originalAssistant.uninstall();
                    }

                    @Override
                    public String showPossibleCompletions() {
                        if (!sourceEditorContainer.isVisible() || getChartViewer().getWidget().isFocusControl())
                            return null;
                        else
                            return originalAssistant.showPossibleCompletions();
                    }

                    @Override
                    public String showContextInformation() {
                        if (!sourceEditorContainer.isVisible() || getChartViewer().getWidget().isFocusControl())
                            return null;
                        else
                            return originalAssistant.showContextInformation();
                    }

                    @Override
                    public void install(ITextViewer textViewer) {
                        originalAssistant.install(textViewer);
                    }

                    @Override
                    public IContentAssistProcessor getContentAssistProcessor(String contentType) {
                        return originalAssistant.getContentAssistProcessor(contentType);
                    }
                };
            }
        });
    }

    public void runChartScript() {
        scriptNotYetExecuted = false;

        // the delayed updater job might call us too late
        if (isDisposed())
            return;

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
                vectorOps.updateVectorOpDataFromProcess(getChartViewer().getPythonProcess());
                Display.getDefault().syncExec(() -> {
                    updateActions();
                });
            };

            ExceptionHandler errorHandler = (proc, e) -> {
                Display.getDefault().syncExec(() -> {
                    updateActions();
                    if (!proc.isKilledByUs()) {
                        try {
                            errorStream.write(tweakStacktrace(e.getMessage()));
                        } catch (IOException e1) {
                            ScavePlugin.logError(e);
                        }
                        annotatePythonException(e);
                        if (!isScriptEditorFocused())
                            revealErrorAnnotation();
                    }
                    proc.kill();
                });
            };

            File anfFileDirectory = scaveEditor.getAnfFile().getLocation().removeLastSegments(1).toFile();
            getChartViewer().runPythonScript(getDocument().get(), anfFileDirectory, afterRun, errorHandler);
            updateActions();
        });
    }

    private String tweakStacktrace(String msg) {
        // Tweak the exception message to remove stack frames related to Py4J.
        // Only tweak if the message conforms to the expected pattern, otherwise leave it alone.
        String expectedFirstLine = "An exception was raised by the Python Proxy. Return Message: Traceback (most recent call last):";
        String replacementFirstLine = "An error occurred. Traceback (most recent call last):";
        String startOfFirstRelevantFrame = "  File \"<string>\", line ";

        String pattern = "(?s)" + Pattern.quote(expectedFirstLine) + "\\n.*?\\n" + Pattern.quote(startOfFirstRelevantFrame);
        return msg.replaceFirst(pattern, replacementFirstLine + "\n" + startOfFirstRelevantFrame);
    }

    private void annotatePythonException(Exception e) {
        String msg = e.getMessage();

        String problemMessage = null;

        IDocument doc = getDocument();

        int offset = 0;
        int length = doc.getLength();
        int line = 0;

        String[] parts = msg.split(".+File \"<string>\", line ");

        if (parts.length == 0)
            problemMessage = "Unknown error.";
        else if (parts.length == 1)
            problemMessage = parts[0].trim();
        else {
            String[] parts2 = parts[parts.length-1].split("(,|\n)", 2);

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
            errorMarker.setAttribute(IMarker.SOURCE_ID, chart.getId());
            errorMarker.setAttribute(IMarker.LOCATION, chart.getName() + "; line " + line);

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
        chartPage.setToolbarActionVisible(saveTempChartAction, chart.isTemporary());
        chartPage.setToolbarActionVisible(gotoChartDefinitionAction, !chart.isTemporary());

        saveTempChartAction.update();
        gotoChartDefinitionAction.update();

        zoomInHorizAction.update();
        zoomOutHorizAction.update();
        zoomInVertAction.update();
        zoomOutVertAction.update();

        killAction.update();

        interactAction.update();
        panAction.update();
        zoomAction.update();

        homeAction.update();
        backAction.update();
        forwardAction.update();

        saveImageAction.update();
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

    public Chart getOriginalChart() {
        return originalChart;
    }

    public String getChartDisplayName() {
        String name = chart.getName();
        if (chart.isTemporary())
            name = "[" + name + "]";
        return StringUtils.defaultIfEmpty(name, "<unnamed>");
    }

    /**
     * Suggested chart name, usually set by "chart.set_suggested_chart_name()" in Python.
     * May be null.
     */
    public String getSuggestedChartName() {
        return getChartViewer().getSuggestedChartName();
    }

    /**
     * Observed column names of the Dataframe, set by "chart.set_observed_column_names()" in Python.
     * May be null. Useful for content assist for the legend format string.
     */
    public List<String> getObservedColumnNames() {
        return getChartViewer().getObservedColumnNames();
    }

    @Override
    public boolean isDirty() {
        return scriptChangedFlag || commandStack.isSaveNeeded();
    }

    @Override
    public boolean isSaveAsAllowed() {
        return false;
    }

    public void refreshChart() {
        rerunChartScriptJob.cancel();
        runChartScript();
    }

    public boolean getAutoRefreshChart() {
        return autoRefreshChart;
    }

    public void setAutoRefreshChart(boolean autoUpdateChart) {
        toggleAutoRefreshAction.setChecked(autoUpdateChart);
        if (this.autoRefreshChart != autoUpdateChart) {
            this.autoRefreshChart = autoUpdateChart;
            if (autoUpdateChart)
                rerunChartScriptJob.runNow();
            else
                rerunChartScriptJob.cancel();
        }
    }

    @Override
    public void dispose() {
        if (isDisposed())
            return;

        scaveEditor.getResultFilesTracker().removeChangeListener(changeListener);
        getDocumentProvider().getDocument(editorInput).removeDocumentListener(changeListener);
        chart.removeListener(changeListener);

        if (matplotlibChartViewer != null)
            matplotlibChartViewer.dispose();

        if (nativeChartViewer != null)
            nativeChartViewer.dispose();

        try {
            outputStream.close();
            errorStream.close();
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

        super.dispose();
    }

    @Override
    public void saveState(IMemento memento) {
        super.saveState(memento);
        memento.putBoolean("autoupdate", getAutoRefreshChart());
        memento.putBoolean("sourcevisible", isSourceShown());
        int[] weights = sashForm.getWeights();
        memento.putInteger("sashweight_left", weights[0]);
        memento.putInteger("sashweight_right", weights[1]);
    }

    @Override
    public void restoreState(IMemento memento) {
        super.restoreState(memento);

        Boolean autoUpdate = memento.getBoolean("autoupdate");
        if (autoUpdate != null)
            setAutoRefreshChart(autoUpdate);
        Boolean sourceVisible = memento.getBoolean("sourcevisible");
        if (sourceVisible != null)
            setShowSource(sourceVisible);

        Integer leftWeight = memento.getInteger("sashweight_left");
        Integer rightWeight = memento.getInteger("sashweight_right");
        if (leftWeight != null && rightWeight != null)
            sashForm.setWeights(new int[] {leftWeight, rightWeight});
    }

    public void pageActivated() {
        if (scriptNotYetExecuted)
            runChartScript();
        scaveEditor.setSelection(new StructuredSelection(chart));

        // TODO: also do this when the editor tab is activated
        for (IViewReference viewRef : getEditorSite().getWorkbenchWindow().getActivePage().getViewReferences()) {
            IViewPart view = viewRef.getView(false);
            if (view instanceof ConsoleView)
                ((ConsoleView)view).display(console);
        }

        getChartViewer().getWidget().setFocus();
    }

    public boolean isScriptEditorFocused() {
        return !getChartViewer().getWidget().isFocusControl();
    }

    public void gotoMarker(IMarker marker) {
        Assert.isTrue(this.errorMarker.equals(marker));
        gotoErrorAnnotation();
    }

    protected void showSourceForStackFrame(String filename, int lineNumber) {
        Debug.println("Opening location for: " + filename + ":" + lineNumber);
        try {
            if (filename.equals("<string>")) {
                getEditorSite().getPage().activate(scaveEditor);
                scaveEditor.showPage(chartPage);
                gotoLine(this, lineNumber);
            }
            else {
                IEditorInput editorInput = new PydevFileEditorInput(new File(filename));
                PyEdit pyedit = (PyEdit) IDE.openEditor(getSite().getPage(), editorInput, PyEdit.EDITOR_ID);
                gotoLine(pyedit, lineNumber);
            }
        }
        catch (PartInitException|BadLocationException e) {
            ScavePlugin.logError(e);
        }
    }

    protected void gotoLine(PyEdit pyedit, int lineNumber) throws BadLocationException {
            IDocument document = pyedit.getDocument();
            int offset = document.getLineOffset(lineNumber-1);
            ISourceViewer sourceViewer = pyedit.getEditorSourceViewer();
            sourceViewer.setSelectedRange(offset, 0);
            sourceViewer.revealRange(offset, 0);
    }

    public void saved() {
        commandStack.saved();
        scriptChangedFlag = false;
    }
}

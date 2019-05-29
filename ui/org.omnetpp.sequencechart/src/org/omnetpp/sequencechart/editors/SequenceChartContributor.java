/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.editors;

import java.awt.RenderingHints;
import java.io.File;
import java.lang.reflect.Field;
import java.math.MathContext;
import java.util.ArrayList;
import java.util.List;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.TransformerFactoryConfigurationError;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.apache.batik.svggen.SVGGraphics2D;
import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.text.WordUtils;
import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gmf.runtime.draw2d.ui.render.awt.internal.svg.export.GraphicsSVG;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerFilter;
import org.eclipse.jface.window.Window;
import org.eclipse.osgi.util.NLS;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.FontDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Scale;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ContributionItemFactory;
import org.eclipse.ui.dialogs.ElementListSelectionDialog;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.dialogs.ListDialog;
import org.eclipse.ui.handlers.HandlerUtil;
import org.eclipse.ui.internal.Workbench;
import org.eclipse.ui.keys.IBindingService;
import org.eclipse.ui.menus.CommandContributionItem;
import org.eclipse.ui.menus.CommandContributionItemParameter;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.eclipse.ui.texteditor.StatusLineContributionItem;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.common.eventlog.EventLogFilterParameters;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.FilterEventLogDialog;
import org.omnetpp.common.eventlog.GotoEventDialog;
import org.omnetpp.common.eventlog.GotoSimulationTimeDialog;
import org.omnetpp.common.eventlog.IEventLogChangeListener;
import org.omnetpp.common.eventlog.IFollowSelectionSupport;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.TimeUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.common.util.VectorFileUtil;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.engine.FilteredEventLog;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IMessageDependency;
import org.omnetpp.eventlog.engine.MessageEntry;
import org.omnetpp.eventlog.engine.SequenceChartFacade;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.sequencechart.SequenceChartPlugin;
import org.omnetpp.sequencechart.widgets.SequenceChart;
import org.omnetpp.sequencechart.widgets.SequenceChart.AxisSpacingMode;
import org.omnetpp.sequencechart.widgets.axisrenderer.AxisLineRenderer;
import org.omnetpp.sequencechart.widgets.axisrenderer.AxisVectorBarRenderer;

@SuppressWarnings("restriction")
public class SequenceChartContributor extends EditorActionBarContributor implements IPartListener, ISelectionChangedListener, IEventLogChangeListener {
    public final static String TOOL_IMAGE_DIR = "icons/full/etool16/";
    public final static String IMAGE_TIMELINE_MODE = TOOL_IMAGE_DIR + "timelinemode.png";
    public final static String IMAGE_AXIS_ORDERING_MODE = TOOL_IMAGE_DIR + "axisordering.gif";
    public final static String IMAGE_SHOW_EVENT_NUMBERS = TOOL_IMAGE_DIR + "eventnumbers.png";
    public final static String IMAGE_SHOW_MESSAGE_NAMES = TOOL_IMAGE_DIR + "messagenames.png";
    public final static String IMAGE_SHOW_MESSAGE_SENDS = TOOL_IMAGE_DIR + "messagesends.png";
    public final static String IMAGE_SHOW_SELF_MESSAGE_SENDS = TOOL_IMAGE_DIR + "selfmessagesends.png";
    public final static String IMAGE_SHOW_MESSAGE_REUSES = TOOL_IMAGE_DIR + "messagereuses.png";
    public final static String IMAGE_SHOW_SELF_MESSAGES_REUSES = TOOL_IMAGE_DIR + "selfmessagereuses.png";
    public final static String IMAGE_SHOW_MIXED_MESSAGE_DEPENDENCIES = TOOL_IMAGE_DIR + "mixedmessagedependencies.png";
    public final static String IMAGE_SHOW_MIXED_SELF_MESSAGES_DEPENDENCIES = TOOL_IMAGE_DIR + "mixedselfmessagedependencies.png";
    public final static String IMAGE_SHOW_MODULE_METHOD_CALLS = TOOL_IMAGE_DIR + "modulemethodcall.png";
    public final static String IMAGE_SHOW_TRANSMISSION_DURATIONS = TOOL_IMAGE_DIR + "transmissiondurations.png";
    public final static String IMAGE_SHOW_ARROW_HEADS = TOOL_IMAGE_DIR + "arrowhead.png";
    public final static String IMAGE_SHOW_ZERO_TIME_REGIONS = TOOL_IMAGE_DIR + "zerotimeregions.png";
    public final static String IMAGE_INCREASE_SPACING = TOOL_IMAGE_DIR + "incr_spacing.png";
    public final static String IMAGE_DECREASE_SPACING = TOOL_IMAGE_DIR + "decr_spacing.png";
    public final static String IMAGE_DENSE_AXES = TOOL_IMAGE_DIR + "denseaxes.png";
    public final static String IMAGE_BALANCED_AXES = TOOL_IMAGE_DIR + "balancedaxes.png";
    public final static String IMAGE_ATTACH_VECTOR_TO_AXIS = TOOL_IMAGE_DIR + "attachvector.png";
    public final static String IMAGE_EXPORT_SVG = TOOL_IMAGE_DIR + "export_wiz.gif";

    private SequenceChart sequenceChart;

    private SequenceChartMenuAction timelineModeAction;
    private SequenceChartMenuAction axisOrderingModeAction;
    private SequenceChartAction filterAction;
    private SequenceChartAction showEventNumbersAction;
    private SequenceChartAction showMessageNamesAction;
    private SequenceChartAction showMessageSendsAction;
    private SequenceChartAction showSelfMessageSendsAction;
    private SequenceChartAction showMessageReusesAction;
    private SequenceChartAction showSelfMessageReusesAction;
    private SequenceChartAction showMixedMessageDependenciesAction;
    private SequenceChartAction showMixedSelfMessageDependenciesAction;
    private SequenceChartAction showArrowHeadsAction;
    private SequenceChartAction showZeroSimulationTimeRegionsAction;
    private SequenceChartAction showAxisLabelsAction;
    private SequenceChartAction showAxesWithoutEventsAction;
    private SequenceChartAction showTransmissionDurationsAction;
    private SequenceChartAction showComponentMethodCallsAction;
    private SequenceChartAction changeFontAction;
    private SequenceChartAction increaseSpacingAction;
    private SequenceChartAction decreaseSpacingAction;
    private SequenceChartAction defaultZoomAction;
    private SequenceChartAction zoomToFitAction;
    private SequenceChartAction zoomInAction;
    private SequenceChartAction zoomOutAction;
    private SequenceChartAction denseAxesAction;
    private SequenceChartAction balancedAxesAction;
    private SequenceChartAction toggleBookmarkAction;
    private SequenceChartAction releaseMemoryAction;
    private SequenceChartAction copyToClipboardAction;
    private SequenceChartAction exportToSVGAction;
    private SequenceChartAction refreshAction;
    private SequenceChartAction pinAction;
    private StatusLineContributionItem timelineModeStatus;
    private StatusLineContributionItem filterStatus;

    /*************************************************************************************
     * CONSTRUCTION
     */

    public SequenceChartContributor() {
        this.timelineModeAction = createTimelineModeAction();
        this.axisOrderingModeAction = createAxisOrderingModeAction();
        this.filterAction = createFilterAction();
        this.showEventNumbersAction = createShowEventNumbersAction();
        this.showMessageNamesAction = createShowMessageNamesAction();
        this.showMessageSendsAction = createShowMessageSendsAction();
        this.showSelfMessageSendsAction = createShowSelfMessageSendsAction();
        this.showMessageReusesAction = createShowMessageReusesAction();
        this.showSelfMessageReusesAction = createShowSelfMessageReusesAction();
        this.showMixedMessageDependenciesAction = createShowMessageDependenciesAction();
        this.showMixedSelfMessageDependenciesAction = createShowSelfMessageDependenciesAction();
        this.showArrowHeadsAction = createShowArrowHeadsAction();
        this.showZeroSimulationTimeRegionsAction = createShowZeroSimulationTimeRegionsAction();
        this.showAxisLabelsAction = createShowAxisLabelsAction();
        this.showAxesWithoutEventsAction = createShowAxesWithoutEventsAction();
        this.showTransmissionDurationsAction = createShowTransmissionDurationsAction();
        this.showComponentMethodCallsAction = createShowComponentMethodCallsAction();
        this.changeFontAction = createChangeFontAction();
        this.increaseSpacingAction = createIncreaseSpacingAction();
        this.decreaseSpacingAction = createDecreaseSpacingAction();
        this.defaultZoomAction = createDefaultZoomAction();
        this.zoomToFitAction = createZoomToFitAction();
        this.zoomInAction = createZoomInAction();
        this.zoomOutAction = createZoomOutAction();
        this.denseAxesAction = createDenseAxesAction();
        this.balancedAxesAction = createBalancedAxesAction();
        this.toggleBookmarkAction = createToggleBookmarkAction();
        this.copyToClipboardAction = createCopyToClipboardAction();
        this.releaseMemoryAction = createReleaseMemoryAction();
        this.refreshAction = createRefreshAction();
        this.pinAction = createPinAction();
        this.exportToSVGAction = createExportToSVGAction();

        this.timelineModeStatus = createTimelineModeStatus();
        this.filterStatus = createFilterStatus();
    }

    public SequenceChartContributor(SequenceChart sequenceChart) {
        this();
        this.sequenceChart = sequenceChart;
        sequenceChart.addSelectionChangedListener(this);
    }

    @Override
    public void dispose() {
        if (sequenceChart != null)
            sequenceChart.removeSelectionChangedListener(this);
        sequenceChart = null;
        super.dispose();
    }

    private IEventLog getEventLog() {
        return sequenceChart.getEventLog();
    }

    /*************************************************************************************
     * CONTRIBUTIONS
     */

    public void contributeToPopupMenu(IMenuManager menuManager) {
        menuManager.setRemoveAllWhenShown(true);
        menuManager.addMenuListener(new IMenuListener() {
            public void menuAboutToShow(IMenuManager menuManager) {
                // dynamic menu
                ArrayList<IEvent> events = new ArrayList<IEvent>();
                ArrayList<IMessageDependency> msgs = new ArrayList<IMessageDependency>();
                Point p = sequenceChart.toControl(sequenceChart.getDisplay().getCursorLocation());
                sequenceChart.collectStuffUnderMouse(p.x, p.y, events, msgs, null);

                // events submenu
                for (final IEvent event : events) {
                    IMenuManager subMenuManager = new MenuManager(sequenceChart.getEventText(event, false));
                    menuManager.add(subMenuManager);

                    subMenuManager.add(createFilterEventCausesConsequencesAction(event));
                    subMenuManager.add(createSelectEventAction(event));
                    subMenuManager.add(createCenterEventAction(event));
                }

                if (events.size() != 0)
                    menuManager.add(new Separator());

                // messages submenu
                for (final IMessageDependency msg : msgs) {
                    IMenuManager subMenuManager = new MenuManager(sequenceChart.getMessageDependencyText(msg, false));
                    menuManager.add(subMenuManager);

                    subMenuManager.add(createFilterMessageAction(msg.getMessageEntry()));
                    subMenuManager.add(createGotoCauseAction(msg));
                    subMenuManager.add(createGotoConsequenceAction(msg));
                    subMenuManager.add(createZoomToMessageAction(msg));
                }

                if (msgs.size() != 0)
                    menuManager.add(new Separator());

                // axis submenu
                final ModuleTreeItem axisModule = sequenceChart.findAxisAt(p.y);
                if (axisModule != null) {
                    IMenuManager subMenuManager = new MenuManager(sequenceChart.getAxisText(axisModule, false));
                    menuManager.add(subMenuManager);

                    if (sequenceChart.getAxisRenderer(axisModule) instanceof AxisLineRenderer)
                        subMenuManager.add(createAttachVectorToAxisAction(axisModule));
                    else
                        subMenuManager.add(createDetachVectorFromAxisAction(axisModule));

                    subMenuManager.add(createZoomToAxisValueAction(axisModule, p.x));
                    subMenuManager.add(createCenterAxisAction(axisModule));

                    menuManager.add(new Separator());
                }

                // Show/Hide submenu
                IMenuManager showHideSubmenu = new MenuManager("Show/Hide");
                showHideSubmenu.add(showMessageSendsAction);
                showHideSubmenu.add(showSelfMessageSendsAction);
                showHideSubmenu.add(showMessageReusesAction);
                showHideSubmenu.add(showSelfMessageReusesAction);
                showHideSubmenu.add(showMixedMessageDependenciesAction);
                showHideSubmenu.add(showMixedSelfMessageDependenciesAction);
                showHideSubmenu.add(showComponentMethodCallsAction);
                showHideSubmenu.add(new Separator());
                showHideSubmenu.add(showAxesWithoutEventsAction);
                showHideSubmenu.add(showTransmissionDurationsAction);
                showHideSubmenu.add(showZeroSimulationTimeRegionsAction);
                showHideSubmenu.add(new Separator());
                showHideSubmenu.add(showEventNumbersAction);
                showHideSubmenu.add(showMessageNamesAction);
                showHideSubmenu.add(showArrowHeadsAction);
                showHideSubmenu.add(showAxisLabelsAction);

                // Spacing submenu
                IMenuManager spacingSubmenu = new MenuManager("Spacing");
                spacingSubmenu.add(balancedAxesAction);
                spacingSubmenu.add(denseAxesAction);
                spacingSubmenu.add(increaseSpacingAction);
                spacingSubmenu.add(decreaseSpacingAction);

                // Zoom submenu
                IMenuManager zoomSubmenu = new MenuManager("Zoom");
                zoomSubmenu.add(defaultZoomAction);
                zoomSubmenu.add(zoomToFitAction);
                zoomSubmenu.add(zoomInAction);
                zoomSubmenu.add(zoomOutAction);

                // Show In submenu
                MenuManager showInSubmenu = new MenuManager(getShowInMenuLabel());
                IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                showInSubmenu.add(ContributionItemFactory.VIEWS_SHOW_IN.create(workbenchWindow));

                // context menu static part
                menuManager.add(showHideSubmenu);
                menuManager.add(filterAction);
                menuManager.add(timelineModeAction);
                menuManager.add(axisOrderingModeAction);
                menuManager.add(new Separator());

                menuManager.add(createFindTextCommandContributionItem());
                menuManager.add(createFindNextCommandContributionItem());
                menuManager.add(new Separator());

                // goto submenu
                IMenuManager subMenuManager = new MenuManager("Go To");
                menuManager.add(subMenuManager);
                subMenuManager.add(createGotoEventCommandContributionItem());
                subMenuManager.add(createGotoSimulationTimeCommandContributionItem());
                menuManager.add(new Separator());

                menuManager.add(spacingSubmenu);
                menuManager.add(zoomSubmenu);
                menuManager.add(changeFontAction);
                menuManager.add(new Separator());

                menuManager.add(toggleBookmarkAction);
                menuManager.add(copyToClipboardAction);
                menuManager.add(exportToSVGAction);
                menuManager.add(new Separator());

                menuManager.add(pinAction);
                menuManager.add(createRefreshCommandContributionItem());
                menuManager.add(releaseMemoryAction);
                menuManager.add(new Separator());

                menuManager.add(showInSubmenu);
            }
        });
    }

    @Override
    public void contributeToToolBar(IToolBarManager toolBarManager) {
        contributeToToolBar(toolBarManager, false);
    }

    public void contributeToToolBar(IToolBarManager toolBarManager, boolean view) {
        toolBarManager.add(timelineModeAction);
        toolBarManager.add(axisOrderingModeAction);
        toolBarManager.add(filterAction);
        toolBarManager.add(new Separator());
        toolBarManager.add(showEventNumbersAction);
        toolBarManager.add(showMessageNamesAction);
        toolBarManager.add(showMessageSendsAction);
        toolBarManager.add(showSelfMessageSendsAction);
        toolBarManager.add(showMessageReusesAction);
        toolBarManager.add(showSelfMessageReusesAction);
        toolBarManager.add(showMixedMessageDependenciesAction);
        toolBarManager.add(showMixedSelfMessageDependenciesAction);
        toolBarManager.add(showComponentMethodCallsAction);
        toolBarManager.add(new Separator());
        toolBarManager.add(increaseSpacingAction);
        toolBarManager.add(decreaseSpacingAction);
        toolBarManager.add(new Separator());
        toolBarManager.add(zoomInAction);
        toolBarManager.add(zoomOutAction);
        toolBarManager.add(new Separator());
        toolBarManager.add(refreshAction);
        if (view)
            toolBarManager.add(pinAction);
    }

    @Override
    public void contributeToStatusLine(IStatusLineManager statusLineManager) {
        statusLineManager.add(timelineModeStatus);
        statusLineManager.add(filterStatus);
    }

    @Override
    public void setActiveEditor(IEditorPart targetEditor) {
        if (targetEditor instanceof SequenceChartEditor) {
            EventLogInput eventLogInput;
            if (sequenceChart != null) {
                eventLogInput = sequenceChart.getInput();
                if (eventLogInput != null)
                    eventLogInput.removeEventLogChangedListener(this);
                getPage().removePartListener(this);
                sequenceChart.removeSelectionChangedListener(this);
            }
            sequenceChart = ((SequenceChartEditor)targetEditor).getSequenceChart();
            if (sequenceChart.getSequenceChartContributor() == null)
                sequenceChart.setSequenceChartContributor(this);
            eventLogInput = sequenceChart.getInput();
            if (eventLogInput != null)
                eventLogInput.addEventLogChangedListener(this);
            getPage().addPartListener(this);
            sequenceChart.addSelectionChangedListener(this);
            update();
        }
        else
            sequenceChart = null;
    }

    public void update() {
        try {
            for (Field field : SequenceChartContributor.class.getDeclaredFields()) {
                Class<?> fieldType = field.getType();
                if (fieldType == SequenceChartAction.class || fieldType == SequenceChartMenuAction.class) {
                    SequenceChartAction fieldValue = (SequenceChartAction)field.get(this);
                    if (fieldValue != null && sequenceChart != null) {
                        fieldValue.setEnabled(true);
                        fieldValue.update();
                        if ((getPage() != null && !(getPage().getActivePart() instanceof SequenceChartEditor)) || sequenceChart.getInput().isLongRunningOperationInProgress())
                            fieldValue.setEnabled(false);
                    }
                }
                else if (fieldType == StatusLineContributionItem.class) {
                    StatusLineContributionItem fieldValue = (StatusLineContributionItem)field.get(this);
                    if (sequenceChart != null)
                        fieldValue.update();
                }
            }
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    private String getShowInMenuLabel() {
        String keyBinding = null;

        IBindingService bindingService = (IBindingService)PlatformUI.getWorkbench().getAdapter(IBindingService.class);
        if (bindingService != null)
            keyBinding = bindingService.getBestActiveBindingFormattedFor("org.eclipse.ui.navigate.showInQuickMenu");

        if (keyBinding == null)
            keyBinding = "";

        return NLS.bind("Show In \t{0}", keyBinding);
    }

    /*************************************************************************************
     * NOTIFICATIONS
     */

    public void selectionChanged(SelectionChangedEvent event) {
        update();
    }

    public void eventLogAppended() {
        // void
    }

    public void eventLogOverwritten() {
        // void
    }

    public void eventLogFilterRemoved() {
        update();
    }

    public void eventLogFiltered() {
        update();
    }

    public void eventLogLongOperationEnded() {
        update();
    }

    public void eventLogLongOperationStarted() {
        update();
    }

    public void eventLogProgress() {
        // void
    }

    public void partActivated(IWorkbenchPart part) {
        update();
    }

    public void partBroughtToTop(IWorkbenchPart part) {
    }

    public void partClosed(IWorkbenchPart part) {
    }

    public void partDeactivated(IWorkbenchPart part) {
        update();
    }

    public void partOpened(IWorkbenchPart part) {
    }

    /*************************************************************************************
     * ACTIONS
     */

    private CommandContributionItem createFindTextCommandContributionItem() {
        CommandContributionItemParameter parameter = new CommandContributionItemParameter(Workbench.getInstance(), null, "org.omnetpp.sequencechart.findText", SWT.PUSH);
        parameter.icon = ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_SEARCH);
        return new CommandContributionItem(parameter);
    }

    private CommandContributionItem createFindNextCommandContributionItem() {
        CommandContributionItemParameter parameter = new CommandContributionItemParameter(Workbench.getInstance(), null, "org.omnetpp.sequencechart.findNext", SWT.PUSH);
        parameter.icon = ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_SEARCH_NEXT);
        return new CommandContributionItem(parameter);
    }

    private SequenceChartMenuAction createTimelineModeAction() {
        return new SequenceChartMenuAction("Timeline Mode", Action.AS_DROP_DOWN_MENU, SequenceChartPlugin.getImageDescriptor(IMAGE_TIMELINE_MODE)) {
            @Override
            protected void doRun() {
                sequenceChart.setTimelineMode(EventLogInput.TimelineMode.values()[(sequenceChart.getTimelineMode().ordinal() + 1) % EventLogInput.TimelineMode.values().length]);
                timelineModeStatus.update();
                update();
            }

            @Override
            protected int getMenuIndex() {
                return sequenceChart.getTimelineMode().ordinal();
            }

            @Override
            public IMenuCreator getMenuCreator() {
                return new AbstractMenuCreator() {
                    @Override
                    protected void createMenu(Menu menu) {
                        addSubMenuItem(menu, "Linear", EventLogInput.TimelineMode.SIMULATION_TIME);
                        addSubMenuItem(menu, "Event number", EventLogInput.TimelineMode.EVENT_NUMBER);
                        addSubMenuItem(menu, "Step", EventLogInput.TimelineMode.STEP);
                        addSubMenuItem(menu, "Nonlinear", EventLogInput.TimelineMode.NONLINEAR);

                        MenuItem subMenuItem = new MenuItem(menu, SWT.RADIO);
                        subMenuItem.setText("Custom nonlinear...");
                        subMenuItem.addSelectionListener( new SelectionAdapter() {
                            @Override
                            public void widgetSelected(SelectionEvent e) {
                                TitleAreaDialog dialog = new CustomNonlinearOptionsDialog(Display.getCurrent().getActiveShell(), sequenceChart);
                                dialog.open();
                            }
                        });
                    }

                    private void addSubMenuItem(Menu menu, String text, final EventLogInput.TimelineMode timelineMode) {
                        MenuItem subMenuItem = new MenuItem(menu, SWT.RADIO);
                        subMenuItem.setText(text);
                        subMenuItem.addSelectionListener( new SelectionAdapter() {
                            @Override
                            public void widgetSelected(SelectionEvent e) {
                                MenuItem menuItem = (MenuItem)e.widget;

                                if (menuItem.getSelection()) {
                                    sequenceChart.setTimelineMode(timelineMode);
                                    timelineModeStatus.update();
                                    update();
                                }
                            }
                        });
                    }
                };
            }
        };
    }

    private SequenceChartMenuAction createAxisOrderingModeAction() {
        return new SequenceChartMenuAction("Axis Ordering Mode", Action.AS_DROP_DOWN_MENU, SequenceChartPlugin.getImageDescriptor(IMAGE_AXIS_ORDERING_MODE)) {
            @Override
            protected void doRun() {
                sequenceChart.setAxisOrderingMode(SequenceChart.AxisOrderingMode.values()[(sequenceChart.getAxisOrderingMode().ordinal() + 1) % SequenceChart.AxisOrderingMode.values().length]);
                update();
            }

            @Override
            protected int getMenuIndex() {
                return sequenceChart.getAxisOrderingMode().ordinal();
            }

            @Override
            public IMenuCreator getMenuCreator() {
                return new AbstractMenuCreator() {
                    @Override
                    protected void createMenu(Menu menu) {
                        addSubMenuItem(menu, "Manual...", SequenceChart.AxisOrderingMode.MANUAL);
                        addSubMenuItem(menu, "Module Id", SequenceChart.AxisOrderingMode.MODULE_ID);
                        addSubMenuItem(menu, "Module Name", SequenceChart.AxisOrderingMode.MODULE_FULL_PATH);
                        addSubMenuItem(menu, "Minimize Crossings", SequenceChart.AxisOrderingMode.MINIMIZE_CROSSINGS);
                    }

                    private void addSubMenuItem(Menu menu, String text, final SequenceChart.AxisOrderingMode axisOrderingMode) {
                        MenuItem subMenuItem = new MenuItem(menu, SWT.RADIO);
                        subMenuItem.setText(text);
                        subMenuItem.addSelectionListener( new SelectionAdapter() {
                            @Override
                            public void widgetSelected(SelectionEvent e) {
                                MenuItem menuItem = (MenuItem)e.widget;

                                if (menuItem.getSelection()) {
                                    if (axisOrderingMode == SequenceChart.AxisOrderingMode.MANUAL &&
                                        sequenceChart.showManualOrderingDialog() == Window.CANCEL)
                                        return;

                                    sequenceChart.setAxisOrderingMode(axisOrderingMode);
                                    update();
                                }
                            }
                        });
                    }
                };
            }
        };
    }

    private SequenceChartAction createFilterAction() {
        return new SequenceChartMenuAction("Filter", Action.AS_DROP_DOWN_MENU, ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_FILTER)) {
            @Override
            protected void doRun() {
                if (isFilteredEventLog())
                    removeFilter();
                else
                    filter();
            }

            @Override
            protected int getMenuIndex() {
                if (isFilteredEventLog())
                    return 1;
                else
                    return 0;
            }

            private boolean isFilteredEventLog() {
                return getEventLog() instanceof FilteredEventLog;
            }

            @Override
            public IMenuCreator getMenuCreator() {
                return new AbstractMenuCreator() {
                    @Override
                    protected void createMenu(Menu menu) {
                        addSubMenuItem(menu, "Show All", new Runnable() {
                            public void run() {
                                removeFilter();
                            }
                        });
                        addSubMenuItem(menu, "Filter...", new Runnable() {
                            public void run() {
                                filter();
                            }
                        });
                    }

                    private void addSubMenuItem(Menu menu, String text, final Runnable runnable) {
                        MenuItem subMenuItem = new MenuItem(menu, SWT.RADIO);
                        subMenuItem.setText(text);
                        subMenuItem.addSelectionListener( new SelectionAdapter() {
                            @Override
                            public void widgetSelected(SelectionEvent e) {
                                MenuItem menuItem = (MenuItem)e.widget;

                                if (menuItem.getSelection()) {
                                    runnable.run();
                                    update();
                                }
                            }
                        });
                    }
                };
            }

            private void filter() {
                if (sequenceChart.getInput().openFilterDialog() == Window.OK)
                    SequenceChartContributor.this.filter();
            }
        };
    }

    private void removeFilter() {
        final EventLogInput eventLogInput = sequenceChart.getInput();
        final boolean wasCanceled = eventLogInput.isCanceled();
        eventLogInput.resetCanceled();

        eventLogInput.runWithProgressMonitor(new Runnable() {
            public void run() {
                org.omnetpp.common.engine.BigDecimal centerSimulationTime = org.omnetpp.common.engine.BigDecimal.getMinusOne();

                if (!wasCanceled)
                    centerSimulationTime = sequenceChart.getViewportCenterSimulationTime();

                eventLogInput.removeFilter();
                sequenceChart.setInput(eventLogInput);

                if (!wasCanceled) {
                    sequenceChart.scrollToSimulationTimeWithCenter(centerSimulationTime);
                    sequenceChart.defaultZoom();
                }
                else
                    sequenceChart.scrollToBegin();

                update();
            }
        });
    }

    private void filter() {
        final EventLogInput eventLogInput = sequenceChart.getInput();
        final boolean wasCanceled = eventLogInput.isCanceled();

        eventLogInput.runWithProgressMonitor(new Runnable() {
            public void run() {
                org.omnetpp.common.engine.BigDecimal centerSimulationTime = org.omnetpp.common.engine.BigDecimal.getMinusOne();

                if (!wasCanceled)
                    centerSimulationTime = sequenceChart.getViewportCenterSimulationTime();

                eventLogInput.filter();
                sequenceChart.setInput(eventLogInput);

                if (!wasCanceled) {
                    sequenceChart.scrollToSimulationTimeWithCenter(centerSimulationTime);
                    sequenceChart.defaultZoom();
                }
                else
                    sequenceChart.scrollToBegin();

                update();
            }
        });
    }

    private StatusLineContributionItem createFilterStatus() {
        return new StatusLineContributionItem("Filter") {
            @Override
            public void update() {
                setText(isFilteredEventLog() ? "Filtered" : "Unfiltered");
            }

            private boolean isFilteredEventLog() {
                return getEventLog() instanceof FilteredEventLog;
            }
        };
    }

    private SequenceChartAction createShowEventNumbersAction() {
        return new SequenceChartAction("Show Event Numbers", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_EVENT_NUMBERS)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowEventNumbers(!sequenceChart.getShowEventNumbers());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowEventNumbers());
            }
        };
    }

    private SequenceChartAction createShowMessageNamesAction() {
        return new SequenceChartAction("Show Message Names", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_MESSAGE_NAMES)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowMessageNames(!sequenceChart.getShowMessageNames());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowMessageNames());
            }
        };
    }

    private SequenceChartAction createShowMessageSendsAction() {
        return new SequenceChartAction("Show Message Sends", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_MESSAGE_SENDS)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowMessageSends(!sequenceChart.getShowMessageSends());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowMessageSends());
            }
        };
    }

    private SequenceChartAction createShowSelfMessageSendsAction() {
        return new SequenceChartAction("Show Self-Message Sends", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_SELF_MESSAGE_SENDS)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowSelfMessageSends(!sequenceChart.getShowSelfMessageSends());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowSelfMessageSends());
            }
        };
    }

    private SequenceChartAction createShowMessageReusesAction() {
        return new SequenceChartAction("Show Message Reuses", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_MESSAGE_REUSES)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowMessageReuses(!sequenceChart.getShowMessageReuses());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowMessageReuses());
            }
        };
    }

    private SequenceChartAction createShowSelfMessageReusesAction() {
        return new SequenceChartAction("Show Self-Message Reuses", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_SELF_MESSAGES_REUSES)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowSelfMessageReuses(!sequenceChart.getShowSelfMessageReuses());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowSelfMessageReuses());
            }
        };
    }

    private SequenceChartAction createShowMessageDependenciesAction() {
        return new SequenceChartAction("Show Message Dependencies", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_MIXED_MESSAGE_DEPENDENCIES)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowMixedMessageDependencies(!sequenceChart.getShowMixedMessageDependencies());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowMixedMessageDependencies());
            }
        };
    }

    private SequenceChartAction createShowSelfMessageDependenciesAction() {
        return new SequenceChartAction("Show Self-Message Dependencies", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_MIXED_SELF_MESSAGES_DEPENDENCIES)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowMixedSelfMessageDependencies(!sequenceChart.getShowMixedSelfMessageDependencies());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowMixedSelfMessageDependencies());
            }
        };
    }

    private SequenceChartAction createShowArrowHeadsAction() {
        return new SequenceChartAction("Show Arrowheads", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_ARROW_HEADS)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowArrowHeads(!sequenceChart.getShowArrowHeads());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowArrowHeads());
            }
        };
    }

    private SequenceChartAction createShowZeroSimulationTimeRegionsAction() {
        return new SequenceChartAction("Show Zero Simulation Time Regions", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_ZERO_TIME_REGIONS)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowZeroSimulationTimeRegions(!sequenceChart.getShowZeroSimulationTimeRegions());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowZeroSimulationTimeRegions());
            }
        };
    }

    private SequenceChartAction createShowAxisLabelsAction() {
        return new SequenceChartAction("Show Axis Labels", Action.AS_CHECK_BOX) {
            @Override
            protected void doRun() {
                sequenceChart.setShowAxisLabels(!sequenceChart.getShowAxisLabels());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowAxisLabels());
            }
        };
    }

    private SequenceChartAction createShowAxesWithoutEventsAction() {
        return new SequenceChartAction("Show Axes Without Events", Action.AS_CHECK_BOX) {
            @Override
            protected void doRun() {
                sequenceChart.setShowAxesWithoutEvents(!sequenceChart.getShowAxesWithoutEvents());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowAxesWithoutEvents());
            }
        };
    }

    private SequenceChartAction createShowTransmissionDurationsAction() {
        return new SequenceChartAction("Show Transmission Durations", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_TRANSMISSION_DURATIONS)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowTransmissionDurations(!sequenceChart.getShowTransmissionDurations());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowTransmissionDurations());
            }
        };
    }

    private SequenceChartAction createShowComponentMethodCallsAction() {
        return new SequenceChartAction("Show Module Method Calls", Action.AS_CHECK_BOX, SequenceChartPlugin.getImageDescriptor(IMAGE_SHOW_MODULE_METHOD_CALLS)) {
            @Override
            protected void doRun() {
                sequenceChart.setShowComponentMethodCalls(!sequenceChart.getShowComponentMethodCalls());
                update();
            }

            @Override
            public void update() {
                setChecked(sequenceChart.getShowComponentMethodCalls());
            }
        };
    }

    private SequenceChartAction createChangeFontAction() {
        return new SequenceChartAction("Change Font...", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                FontDialog dialog = new FontDialog(Display.getCurrent().getActiveShell());
                dialog.setFontList(sequenceChart.getFont().getFontData());
                FontData fontData = dialog.open();
                if (fontData != null)
                    sequenceChart.setFont(new Font(Display.getDefault(), fontData));
            }
        };
    }

    private SequenceChartAction createIncreaseSpacingAction() {
        return new SequenceChartAction("Increase Spacing", Action.AS_PUSH_BUTTON, SequenceChartPlugin.getImageDescriptor(IMAGE_INCREASE_SPACING)) {
            @Override
            protected void doRun() {
                sequenceChart.setAxisSpacingMode(AxisSpacingMode.MANUAL);
                sequenceChart.setAxisSpacing(sequenceChart.getAxisSpacing() + 5);
            }
        };
    }

    private SequenceChartAction createDecreaseSpacingAction() {
        return new SequenceChartAction("Decrease Spacing", Action.AS_PUSH_BUTTON, SequenceChartPlugin.getImageDescriptor(IMAGE_DECREASE_SPACING)) {
            @Override
            protected void doRun() {
                sequenceChart.setAxisSpacingMode(AxisSpacingMode.MANUAL);
                sequenceChart.setAxisSpacing(sequenceChart.getAxisSpacing() - 5);
            }
        };
    }

    private SequenceChartAction createDefaultZoomAction() {
        return new SequenceChartAction("Default Zoom", Action.AS_PUSH_BUTTON, ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOM)) {
            @Override
            protected void doRun() {
                sequenceChart.defaultZoom();
            }
        };
    }

    private SequenceChartAction createZoomToFitAction() {
        return new SequenceChartAction("Zoom to Fit", Action.AS_PUSH_BUTTON, ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOM)) {
            @Override
            protected void doRun() {
                sequenceChart.zoomToFit();
            }
        };
    }

    private SequenceChartAction createZoomInAction() {
        return new SequenceChartAction("Zoom In", Action.AS_PUSH_BUTTON, ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOMPLUS)) {
            @Override
            protected void doRun() {
                sequenceChart.zoomIn();
            }
        };
    }

    private SequenceChartAction createZoomOutAction() {
        return new SequenceChartAction("Zoom Out", Action.AS_PUSH_BUTTON, ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOMMINUS)) {
            @Override
            protected void doRun() {
                sequenceChart.zoomOut();
            }
        };
    }

    private SequenceChartAction createDenseAxesAction() {
        return new SequenceChartAction("Dense Axes", Action.AS_PUSH_BUTTON, SequenceChartPlugin.getImageDescriptor(IMAGE_DENSE_AXES)) {
            @Override
            protected void doRun() {
                sequenceChart.setAxisSpacingMode(AxisSpacingMode.MANUAL);
                sequenceChart.setAxisSpacing(sequenceChart.getFontHeight(null) + 1);
            }
        };
    }

    private SequenceChartAction createBalancedAxesAction() {
        return new SequenceChartAction("Balanced Axes", Action.AS_PUSH_BUTTON, SequenceChartPlugin.getImageDescriptor(IMAGE_BALANCED_AXES)) {
            @Override
            protected void doRun() {
                sequenceChart.setAxisSpacingMode(AxisSpacingMode.AUTO);
            }
        };
    }

    private SequenceChartAction createCenterEventAction(final IEvent event) {
        return new SequenceChartAction("Center", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                sequenceChart.scrollToSimulationTimeWithCenter(event.getSimulationTime());
            }
        };
    }

    private SequenceChartAction createSelectEventAction(final IEvent event) {
        return new SequenceChartAction("Select", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                sequenceChart.setSelectionEvent(event);
            }
        };
    }

    private SequenceChartAction createFilterEventCausesConsequencesAction(final IEvent event) {
        return new SequenceChartAction("Filter Causes/Consequences...", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                EventLogInput eventLogInput = sequenceChart.getInput();
                EventLogFilterParameters filterParameters = eventLogInput.getFilterParameters();

                filterParameters.enableTraceFilter = true;
                filterParameters.tracedEventNumber = event.getEventNumber();

                if (!(getEventLog() instanceof FilteredEventLog) &&
                    (filterParameters.isAnyEventFilterEnabled() || filterParameters.isAnyMessageFilterEnabled() || filterParameters.isAnyModuleFilterEnabled()))
                {
                    FilterEventLogDialog dialog = new FilterEventLogDialog(Display.getCurrent().getActiveShell(), eventLogInput, filterParameters);

                    if (dialog.open("Cause/consequence filter") == Window.OK)
                        filter();
                }
                else
                    filter();
            }
        };
    }

    private SequenceChartAction createFilterMessageAction(final MessageEntry messageEntry) {
        return new SequenceChartAction("Filter Message...", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                EventLogInput eventLogInput = sequenceChart.getInput();
                EventLogFilterParameters filterParameters = eventLogInput.getFilterParameters();

                // message filter
                filterParameters.enableMessageFilter = true;
                filterParameters.enableMessageEncapsulationTreeIdFilter = true;

                EventLogFilterParameters.EnabledInt enabledInt = null;

                if (filterParameters.messageEncapsulationTreeIds != null) {
                    for (EventLogFilterParameters.EnabledInt messageEncapsulationTreeId : filterParameters.messageEncapsulationTreeIds) {
                        if (messageEncapsulationTreeId.value == messageEntry.getMessageEncapsulationId()) {
                            enabledInt = messageEncapsulationTreeId;
                            messageEncapsulationTreeId.enabled = true;
                        }
                        else
                            messageEncapsulationTreeId.enabled = false;
                    }
                }

                if (enabledInt == null) {
                    enabledInt = new EventLogFilterParameters.EnabledInt(true, messageEntry.getMessageEncapsulationTreeId());
                    filterParameters.messageEncapsulationTreeIds = (EventLogFilterParameters.EnabledInt[])ArrayUtils.add(filterParameters.messageEncapsulationTreeIds, enabledInt);
                }

                // range filter
                filterParameters.enableRangeFilter = true;
                filterParameters.enableEventNumberFilter = true;
                filterParameters.lowerEventNumberLimit = Math.max(0, messageEntry.getEvent().getEventNumber() - 1000);
                filterParameters.upperEventNumberLimit = Math.min(getEventLog().getLastEvent().getEventNumber(), messageEntry.getEvent().getEventNumber() + 1000);

                if (!(getEventLog() instanceof FilteredEventLog) &&
                    (filterParameters.isAnyEventFilterEnabled() || filterParameters.isAnyMessageFilterEnabled() || filterParameters.isAnyModuleFilterEnabled()))
                {
                    FilterEventLogDialog dialog = new FilterEventLogDialog(Display.getCurrent().getActiveShell(), eventLogInput, filterParameters);

                    if (dialog.open("Range") == Window.OK)
                        filter();
                }
                else
                    filter();
            }
        };
    }

    private SequenceChartAction createZoomToMessageAction(final IMessageDependency messageDependency) {
        return new SequenceChartAction("Zoom to Message", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                sequenceChart.zoomToMessageDependency(messageDependency);
            }
        };
    }

    private SequenceChartAction createGotoCauseAction(final IMessageDependency messageDependency) {
        return new SequenceChartAction("Go to Cause Event", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                sequenceChart.gotoElement(messageDependency.getCauseEvent());
            }
        };
    }

    private SequenceChartAction createGotoConsequenceAction(final IMessageDependency messageDependency) {
        return new SequenceChartAction("Go to Consequence Event", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                sequenceChart.gotoElement(messageDependency.getConsequenceEvent());
            }
        };
    }

    private SequenceChartAction createCenterAxisAction(final ModuleTreeItem axisModule) {
        return new SequenceChartAction("Center", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                sequenceChart.scrollToAxisModule(axisModule);
            }
        };
    }

    private SequenceChartAction createZoomToAxisValueAction(final ModuleTreeItem axisModule, final int x) {
        return new SequenceChartAction("Zoom to Value", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                sequenceChart.zoomToAxisValue(axisModule, sequenceChart.getSimulationTimeForViewportCoordinate(x));
            }
        };
    }

    private SequenceChartAction createAttachVectorToAxisAction(final ModuleTreeItem axisModule) {
        return new SequenceChartAction("Attach Vector to Axis", Action.AS_PUSH_BUTTON, SequenceChartPlugin.getImageDescriptor(IMAGE_ATTACH_VECTOR_TO_AXIS)) {
            @Override
            protected void doRun() {
                // open a vector file with the same name as the sequence chart's input file name with .vec extension by default
                EventLogInput eventLogInput = sequenceChart.getInput();
                IFile inputFile = eventLogInput.getFile();
                String inputFileName = inputFile.getName();
                IFile vectorFile = inputFile.getParent().getFile(new Path(inputFileName.substring(0, inputFileName.indexOf(".")) + ".vec"));

                // select a vector file
                ElementTreeSelectionDialog vectorFileDialog = new ElementTreeSelectionDialog(Display.getDefault().getActiveShell(), new WorkbenchLabelProvider(), new WorkbenchContentProvider());
                vectorFileDialog.setTitle("Select File");
                vectorFileDialog.setMessage("Select a vector file to browse for runs and vectors:");
                vectorFileDialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
                vectorFileDialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
                vectorFileDialog.setAllowMultiple(false);
                vectorFileDialog.setInitialSelection(vectorFile.exists() ? vectorFile : inputFile.getParent());
                vectorFileDialog.addFilter(new ViewerFilter() {
                    @Override
                    public boolean select(Viewer viewer, Object parentElement, Object element) {
                        return !(element instanceof IFile) || "vec".equals(((IFile)element).getFileExtension());
                    }
                });

                if (vectorFileDialog.open() == IDialogConstants.CANCEL_ID)
                    return;

                String vectorFileName = ((IResource)vectorFileDialog.getFirstResult()).getLocation().toOSString();

                // load vector file
                ResultFile resultFile = null;
                final ResultFileManager resultFileManager = new ResultFileManager();
                try {
                    resultFile = resultFileManager.loadFile(vectorFileName);
                }
                catch (Throwable te) {
                    MessageDialog.openError(null, "Error", "Could not load vector file " + vectorFileName);
                    return;
                }

                // select a run
                Run run = null;
                RunList runList = resultFileManager.getRunsInFile(resultFile);
                String eventlogRunName = getEventLog().getSimulationBeginEntry().getRunId();
                if (runList.size() == 0) {
                    MessageBox messageBox = new MessageBox(Display.getCurrent().getActiveShell(), SWT.OK | SWT.APPLICATION_MODAL | SWT.ICON_ERROR);
                    messageBox.setText("No runs in result file");
                    messageBox.setMessage("The result file " + vectorFileName + " does not contain any runs");
                    messageBox.open();
                    return;
                }
                else if (runList.size() == 1)
                    run = runList.get(0);
                else if (runList.size() > 1) {
                    ElementListSelectionDialog dialog = new ElementListSelectionDialog(null, new LabelProvider() {
                        @Override
                        public String getText(Object element) {
                            Run run = (Run)element;

                            return run.getRunName();
                        }
                    });
                    dialog.setFilter(eventlogRunName);
                    dialog.setElements(runList.toArray());
                    dialog.setTitle("Run selection");
                    dialog.setMessage("Select a run to browse for vectors:");
                    if (dialog.open() == ListDialog.CANCEL)
                        return;
                    run = (Run)dialog.getFirstResult();
                }

                // compare eventlog run id against vector file's run id
                String vectorRunName = run.getRunName();
                if (!eventlogRunName.equals(vectorRunName)) {
                    MessageBox messageBox = new MessageBox(Display.getCurrent().getActiveShell(), SWT.OK | SWT.CANCEL | SWT.APPLICATION_MODAL | SWT.ICON_WARNING);
                    messageBox.setText("Run ID mismatch");
                    messageBox.setMessage("The eventlog run ID: " + eventlogRunName + " and the vector file run ID: " + vectorRunName + " does not match. Do you want to continue?");

                    if (messageBox.open() == SWT.CANCEL)
                        return;
                }

                // select a vector from the loaded file and run
                long id;
                IDList idList = resultFileManager.getVectorsInFileRun(resultFileManager.getFileRun(resultFile, run));

                if (idList.size() == 0) {
                    MessageBox messageBox = new MessageBox(Display.getCurrent().getActiveShell(), SWT.OK | SWT.APPLICATION_MODAL | SWT.ICON_ERROR);
                    messageBox.setText("No vectors in run");
                    messageBox.setMessage("The run " + run.getRunName() + " in the vector file " + vectorFileName + " does not contain any vectors");
                    messageBox.open();
                    return;
                }
                else {
                    ElementListSelectionDialog dialog = new ElementListSelectionDialog(null, new LabelProvider() {
                        @Override
                        public String getText(Object element) {
                            long id = (Long)element;
                            ResultItem resultItem = resultFileManager.getItem(id);

                            return resultItem.getModuleName() + ":" + resultItem.getName();
                        }
                    });
                    Long[] ids = new Long[idList.size()];
                    for (int i = 0; i < idList.size(); i++)
                        ids[i] = idList.get(i);
                    dialog.setFilter(axisModule.getModuleFullPath());
                    dialog.setElements(ids);
                    dialog.setTitle("Vector selection");
                    dialog.setMessage("Select a vector to attach:");
                    if (dialog.open() == ListDialog.CANCEL)
                        return;
                    id = (Long)dialog.getFirstResult();
                }

                // attach vector data
                ResultItem resultItem = resultFileManager.getItem(id);
                XYArray data = VectorFileUtil.getDataOfVector(resultFileManager, id, true, true);
                sequenceChart.setAxisRenderer(axisModule,
                    new AxisVectorBarRenderer(sequenceChart, vectorFileName, vectorRunName, resultItem.getModuleName(), resultItem.getName(), resultItem, data));
            }
        };
    }

    private SequenceChartAction createDetachVectorFromAxisAction(final ModuleTreeItem axisModule) {
        return new SequenceChartAction("Detach Vector from Axis", Action.AS_PUSH_BUTTON, SequenceChartPlugin.getImageDescriptor(IMAGE_ATTACH_VECTOR_TO_AXIS)) {
            @Override
            protected void doRun() {
                sequenceChart.setAxisRenderer(axisModule, new AxisLineRenderer(sequenceChart, axisModule));
            }
        };
    }


    private SequenceChartAction createToggleBookmarkAction() {
        return new SequenceChartAction("Toggle bookmark", Action.AS_PUSH_BUTTON, ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_TOGGLE_BOOKMARK)) {
            @Override
            protected void doRun() {
                try {
                    EventLogInput eventLogInput = sequenceChart.getInput();
                    IEvent event = sequenceChart.getSelectionEvent();

                    if (event != null) {
                        boolean found = false;
                        IMarker[] markers = eventLogInput.getFile().findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);

                        for (IMarker marker : markers)
                            if (marker.getAttribute("EventNumber", "-1").equals(String.valueOf(event.getEventNumber()))) {
                                marker.delete();
                                found = true;
                            }

                        if (!found) {
                            InputDialog dialog = new InputDialog(null, "Add Bookmark", "Enter Bookmark name:", "", null);

                            if (dialog.open() == Window.OK) {
                                IMarker marker = eventLogInput.getFile().createMarker(IMarker.BOOKMARK);
                                marker.setAttribute(IMarker.LOCATION, "# " + event.getEventNumber());
                                marker.setAttribute("EventNumber", String.valueOf(event.getEventNumber()));
                                marker.setAttribute(IMarker.MESSAGE, dialog.getValue());
                            }
                        }

                        update();
                        sequenceChart.redraw();
                    }
                }
                catch (CoreException e) {
                    throw new RuntimeException(e);
                }
            }

            @Override
            public void update() {
                setEnabled(sequenceChart.getSelectionEvent() != null);
            }
        };
    }

    private SequenceChartAction createCopyToClipboardAction() {
        return new SequenceChartAction("Copy to Clipboard", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                sequenceChart.copyImageToClipboard();
            }
        };
    }

    private SequenceChartAction createExportToSVGAction() {
        return new SequenceChartAction("Export to SVG...", Action.AS_PUSH_BUTTON, SequenceChartPlugin.getImageDescriptor(IMAGE_EXPORT_SVG)) {
            @Override
            protected void doRun() {
                long[] exportRegion = askExportRegion();

                if (exportRegion != null) {
                    String fileName = askFileName();

                    if (fileName != null) {
                        long exportBeginX = exportRegion[0];
                        long exportEndX = exportRegion[1];
                        GraphicsSVG graphics = createGraphics(exportBeginX, exportEndX);

                        long top = sequenceChart.getViewportTop();
                        long left = sequenceChart.getViewportLeft();

                        try {
                            sequenceChart.scrollHorizontalTo(exportBeginX + sequenceChart.getViewportLeft());
                            sequenceChart.scrollVerticalTo(0);
                            sequenceChart.paintArea(graphics);
                            writeXML(graphics, fileName);
                        }
                        catch (Exception e) {
                            throw new RuntimeException(e);
                        }
                        finally {
                            graphics.dispose();
                            sequenceChart.scrollHorizontalTo(left);
                            sequenceChart.scrollVerticalTo(top);
                        }
                    }
                }
            }

            private String askFileName() {
                FileDialog fileDialog = new FileDialog(Display.getCurrent().getActiveShell(), SWT.SAVE);
                IPath location = sequenceChart.getInput().getFile().getLocation().makeAbsolute();
                fileDialog.setFileName(location.removeFileExtension().addFileExtension("svg").lastSegment());
                fileDialog.setFilterPath(location.removeLastSegments(1).toOSString());
                String fileName = fileDialog.open();

                if (fileName != null) {
                    File file = new File(fileName);

                    if (file.exists()) {
                        MessageBox messageBox = new MessageBox(Display.getCurrent().getActiveShell(), SWT.OK | SWT.CANCEL | SWT.APPLICATION_MODAL | SWT.ICON_WARNING);
                        messageBox.setText("File already exists");
                        messageBox.setMessage("The file " + fileName + " already exists and will be overwritten. Do you want to continue the operation?");

                        if (messageBox.open() == SWT.CANCEL)
                            fileName = null;
                    }
                }

                return fileName;
            }

            private long[] askExportRegion() {
                ExportToSVGDialog dialog = new ExportToSVGDialog(Display.getCurrent().getActiveShell());

                if (dialog.open() == Window.OK) {
                    IEventLog eventLog = getEventLog();

                    long exportBeginX;
                    long exportEndX;

                    switch (dialog.getSelectedRangeType()) {
                        case 0:
                            List<IEvent> selectionEvents = sequenceChart.getSelectionEvents();

                            IEvent e0 = selectionEvents.get(0);
                            IEvent e1 = selectionEvents.get(1);

                            if (e0.getEventNumber() < e1.getEventNumber()) {
                                exportBeginX = sequenceChart.getEventXViewportCoordinate(e0.getCPtr());
                                exportEndX = sequenceChart.getEventXViewportCoordinate(e1.getCPtr());
                            }
                            else {
                                exportBeginX = sequenceChart.getEventXViewportCoordinate(e1.getCPtr());
                                exportEndX = sequenceChart.getEventXViewportCoordinate(e0.getCPtr());
                            }
                            break;
                        case 1:
                            exportBeginX = 0;
                            exportEndX = sequenceChart.getViewportWidth();
                            break;
                        case 2:
                            exportBeginX = sequenceChart.getEventXViewportCoordinate(eventLog.getFirstEvent().getCPtr());
                            exportEndX = sequenceChart.getEventXViewportCoordinate(eventLog.getLastEvent().getCPtr());
                            break;
                        default:
                            return null;
                    }

                    int extraSpace = dialog.getExtraSpace();

                    return new long[] {exportBeginX - extraSpace, exportEndX + extraSpace};
                }
                else
                    return null;
            }

            private GraphicsSVG createGraphics(long exportBeginX, long exportEndX) {
                int width = (int)(exportEndX - exportBeginX);
                int height = (int)sequenceChart.getVirtualHeight() + sequenceChart.getGutterHeight(null) * 2 + 2;

                GraphicsSVG graphics = GraphicsSVG.getInstance(new Rectangle(0, -1, width, height));
                SVGGraphics2D g = graphics.getSVGGraphics2D();
                g.setClip(0, 0, width, height);
                g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
                //graphics.setClip(new Rectangle(-1000, -100, 1000, 100));
                graphics.translate(0, 1);
                graphics.setAntialias(SWT.ON);

                return graphics;
            }

            private void writeXML(GraphicsSVG graphics, String fileName)
                throws TransformerConfigurationException, TransformerFactoryConfigurationError, TransformerException
            {
                Source source = new DOMSource(graphics.getRoot());
                StreamResult streamResult = new StreamResult(new File(fileName));
                Transformer transformer = TransformerFactory.newInstance().newTransformer();
                transformer.setOutputProperty(OutputKeys.INDENT, "yes");
                transformer.transform(source, streamResult);
            }

            @Override
            public void update() {
                setEnabled(sequenceChart.getInput() != null);
            }

            class ExportToSVGDialog extends TitleAreaDialog {
                private int extraSpace;

                private int selectedRangeType;

                public ExportToSVGDialog(Shell shell) {
                    super(shell);
                    setShellStyle(getShellStyle() | SWT.RESIZE);
                }

                public int getExtraSpace() {
                    return extraSpace;
                }

                public int getSelectedRangeType() {
                    return selectedRangeType;
                }

                @Override
                protected IDialogSettings getDialogBoundsSettings() {
                    return UIUtils.getDialogSettings(SequenceChartPlugin.getDefault(), getClass().getName());
                }

                @Override
                protected Control createDialogArea(Composite parent) {
                    setHelpAvailable(false);

                    Composite container = new Composite((Composite)super.createDialogArea(parent), SWT.NONE);
                    container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
                    container.setLayout(new GridLayout(2, false));

                    Group group = new Group(container, SWT.NONE);
                    GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
                    gridData.horizontalSpan = 2;
                    group.setText("Select range to export");
                    group.setLayoutData(gridData);
                    group.setLayout(new GridLayout(1, false));

                    // radio buttons
                    createButton(group, "Range of two selected events", 0).setEnabled(sequenceChart.getSelectionEvents().size() == 2);
                    createButton(group, "Visible area only", 1);
                    createButton(group, "Whole event log", 2);

                    Label label = new Label(container, SWT.NONE);
                    label.setText("Extra space in pixels around both ends: ");
                    label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

                    final Text text = new Text(container, SWT.BORDER | SWT.SINGLE);
                    text.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
                    text.setText(String.valueOf(extraSpace));
                    text.addModifyListener(new ModifyListener() {
                        public void modifyText(ModifyEvent e) {
                            try {
                                extraSpace = Integer.parseInt(text.getText());
                            }
                            catch (Exception x) {
                                // void
                            }
                        }
                    });

                    setTitle("Export to SVG");
                    setMessage("Please select which part of the event log should be exported");

                    return container;
                }

                private Button createButton(Group group, String text, final int type) {
                    Button button = new Button(group, SWT.RADIO);
                    button.setText(text);
                    button.addSelectionListener(new SelectionAdapter() {
                        @Override
                        public void widgetSelected(SelectionEvent e) {
                            selectedRangeType = type;
                        }
                    });

                    return button;
                }

                @Override
                protected void configureShell(Shell newShell) {
                    newShell.setText("Export to SVG");
                    super.configureShell(newShell);
                }
            };
        };
    }

    private SequenceChartAction createRefreshAction() {
        return new SequenceChartAction("Refresh", Action.AS_PUSH_BUTTON, ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_REFRESH)) {
            @Override
            protected void doRun() {
                sequenceChart.refresh();
            }
        };
    }

    private SequenceChartAction createPinAction() {
        return new SequenceChartAction("Pin", Action.AS_CHECK_BOX, ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_UNPIN)) {
            @Override
            protected void doRun() {
                IWorkbenchPart workbenchPart = sequenceChart.getWorkbenchPart();
                if (workbenchPart instanceof IFollowSelectionSupport) {
                    IFollowSelectionSupport followSelectionSupport = (IFollowSelectionSupport)workbenchPart;
                    followSelectionSupport.setFollowSelection(!followSelectionSupport.getFollowSelection());
                }
            }

            @Override
            public void update() {
                IWorkbenchPart workbenchPart = sequenceChart.getWorkbenchPart();
                if (workbenchPart instanceof IFollowSelectionSupport)
                    setChecked(!((IFollowSelectionSupport)workbenchPart).getFollowSelection());
            }
        };
    }

    private CommandContributionItem createRefreshCommandContributionItem() {
        CommandContributionItemParameter parameter = new CommandContributionItemParameter(Workbench.getInstance(), null, "org.omnetpp.sequencechart.refresh", SWT.PUSH);
        parameter.icon = ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_REFRESH);
        return new CommandContributionItem(parameter);
    }

    private SequenceChartAction createReleaseMemoryAction() {
        return new SequenceChartAction("Release Memory", Action.AS_PUSH_BUTTON) {
            @Override
            protected void doRun() {
                sequenceChart.getInput().synchronize(FileReader.FileChangedState.OVERWRITTEN);
            }
        };
    }

    private StatusLineContributionItem createTimelineModeStatus() {
        return new StatusLineContributionItem("Timeline Mode", true, "SIMULATION_TIME".length()) {
            @Override
            public void update() {
                String timelineModeName = sequenceChart.getTimelineMode().name();
                setText(WordUtils.capitalize(timelineModeName.replaceAll("_", " ").toLowerCase()));
            }
        };
    }

    static class CustomNonlinearOptionsDialog extends TitleAreaDialog {
        private SequenceChart sequenceChart;
        private SequenceChartFacade sequenceChartFacade;
        private double oldNonLinearMinimumTimelineCoordinateDelta;
        private double oldNonLinearFocus;
        private org.omnetpp.common.engine.BigDecimal[] oldLeftRightSimulationTimeRange;
        private Label minimumLabel;
        private Label focusLabel;
        private Scale minimum;
        private Scale focus;

        private CustomNonlinearOptionsDialog(Shell parentShell, SequenceChart sequenceChart) {
            super(parentShell);
            this.sequenceChart = sequenceChart;
            setShellStyle(getShellStyle() | SWT.RESIZE);
        }

        @Override
        protected Control createDialogArea(Composite parent) {
            sequenceChartFacade = sequenceChart.getInput().getSequenceChartFacade();
            oldLeftRightSimulationTimeRange = sequenceChart.getViewportSimulationTimeRange();
            oldNonLinearMinimumTimelineCoordinateDelta = sequenceChartFacade.getNonLinearMinimumTimelineCoordinateDelta();
            oldNonLinearFocus = sequenceChartFacade.getNonLinearFocus();

            setHelpAvailable(false);
            setTitle("Custom nonlinear timeline mode");
            setMessage("Please select appropriate nonlinearity factors");

            Composite container = new Composite((Composite)super.createDialogArea(parent), SWT.NONE);
            container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
            container.setLayout(new GridLayout());

            minimumLabel = new Label(container, SWT.NONE);
            minimumLabel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

            minimum = new Scale(container, SWT.NONE);
            minimum.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
            minimum.setMinimum(0);
            minimum.setMaximum(1000);
            minimum.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    setNonLinearMinimumTimelineCoordinateDeltaText();
                    apply();
                }
            });
            minimum.setSelection(getNonLinearMinimumTimelineCoordinateDeltaScale());
            setNonLinearMinimumTimelineCoordinateDeltaText();

            focusLabel = new Label(container, SWT.NONE);
            focusLabel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

            focus = new Scale(container, SWT.NONE);
            focus.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
            focus.setMinimum(0);
            focus.setMaximum(1000);
            focus.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    setNonLinearFocusText();
                    apply();
                }
            });
            focus.setSelection(getNonLinearFocusScale());
            setNonLinearFocusText();

            return container;
        }

        @Override
        protected void configureShell(Shell newShell) {
            newShell.setText("Custom nonlinear timeline mode");
            super.configureShell(newShell);
        }

        @Override
        protected void okPressed() {
            apply();
            super.okPressed();
        }

        @Override
        protected void cancelPressed() {
            sequenceChartFacade.setNonLinearMinimumTimelineCoordinateDelta(oldNonLinearMinimumTimelineCoordinateDelta);
            sequenceChartFacade.setNonLinearFocus(oldNonLinearFocus);

            redrawSequenceChart();

            super.cancelPressed();
        }

        private void apply() {
            sequenceChartFacade.setNonLinearFocus(getNonLinearFocus());
            sequenceChartFacade.setNonLinearMinimumTimelineCoordinateDelta(getNonLinearMinimumTimelineCoordinateDelta());

            redrawSequenceChart();
        }

        private void redrawSequenceChart() {
            sequenceChartFacade.relocateTimelineCoordinateSystem(sequenceChartFacade.getTimelineCoordinateSystemOriginEvent());
            sequenceChart.setViewportSimulationTimeRange(oldLeftRightSimulationTimeRange);
        }

        private void setNonLinearMinimumTimelineCoordinateDeltaText() {
            java.math.BigDecimal value = new java.math.BigDecimal(100 * getNonLinearMinimumTimelineCoordinateDelta());
            value = value.round(new MathContext(3));
            minimumLabel.setText("Relative minimum distance to maximum distance: " + value + "%");
        }

        private int getNonLinearMinimumTimelineCoordinateDeltaScale() {
            return (int)(1000 * sequenceChartFacade.getNonLinearMinimumTimelineCoordinateDelta());
        }

        private double getNonLinearMinimumTimelineCoordinateDelta() {
            return (double)minimum.getSelection() / 1000;
        }

        private int getNonLinearFocusScale() {
            return (int)((Math.log10(sequenceChartFacade.getNonLinearFocus()) + 18) * 40);
        }

        private double getNonLinearFocus() {
            return Math.pow(10, ((double)focus.getSelection() / 40) - 18);
        }

        private void setNonLinearFocusText() {
            java.math.BigDecimal value = new java.math.BigDecimal(getNonLinearFocus());
            value = value.round(new MathContext(3));
            focusLabel.setText("Nonlinear simulation time focus: " + TimeUtils.secondsToTimeString(value));
        }
    }

    private abstract class SequenceChartAction extends Action {
        public SequenceChartAction(String text, int style) {
            super(text, style);
        }

        public SequenceChartAction(String text, int style, ImageDescriptor image) {
            super(text, style);
            setImageDescriptor(image);
        }

        public void update() {
        }

        @Override
        public void run() {
            try {
                doRun();
            }
            catch (Exception e) {
                MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
                SequenceChartPlugin.logError(e);
            }
        }

        protected abstract void doRun();
    }

    private abstract class SequenceChartMenuAction extends SequenceChartAction {
        protected ArrayList<Menu> menus = new ArrayList<Menu>();

        public SequenceChartMenuAction(String text, int style, ImageDescriptor image) {
            super(text, style, image);
        }

        @Override
        public void update() {
            for (Menu menu : menus)
                if (!menu.isDisposed())
                    updateMenu(menu);
        }

        protected void addMenu(Menu menu) {
            Assert.isTrue(menu != null);

            menus.add(menu);
            updateMenu(menu);
        }

        protected void removeMenu(Menu menu) {
            Assert.isTrue(menu != null);

            menus.remove(menu);
        }

        protected abstract int getMenuIndex();

        protected void updateMenu(Menu menu) {
            for (int i = 0; i < menu.getItemCount(); i++) {
                boolean selection = i == getMenuIndex();
                MenuItem menuItem = menu.getItem(i);

                if (menuItem.getSelection() != selection)
                    menuItem.setSelection(selection);
            }
        }

        protected abstract class AbstractMenuCreator implements IMenuCreator {
            private Menu controlMenu;

            private Menu parentMenu;

            public void dispose() {
                if (controlMenu != null) {
                    controlMenu.dispose();
                    removeMenu(controlMenu);
                }

                if (parentMenu != null) {
                    parentMenu.dispose();
                    removeMenu(parentMenu);
                }
            }

            public Menu getMenu(Control parent) {
                if (controlMenu == null) {
                    controlMenu = new Menu(parent);
                    createMenu(controlMenu);
                    addMenu(controlMenu);
                }

                return controlMenu;
            }

            public Menu getMenu(Menu parent) {
                if (parentMenu == null) {
                    parentMenu = new Menu(parent);
                    createMenu(parentMenu);
                    addMenu(parentMenu);
                }

                return parentMenu;
            }

            protected abstract void createMenu(Menu menu);
        }
    }

    public static class FindTextHandler extends AbstractHandler {
        public Object execute(ExecutionEvent event) throws ExecutionException {
            IWorkbenchPart part = HandlerUtil.getActivePartChecked(event);

            if (part instanceof ISequenceChartProvider)
                ((ISequenceChartProvider)part).getSequenceChart().findText(false);

            return null;
        }
    }

    public static class FindNextHandler extends AbstractHandler {
        public Object execute(ExecutionEvent event) throws ExecutionException {
            IWorkbenchPart part = HandlerUtil.getActivePartChecked(event);

            if (part instanceof ISequenceChartProvider)
                ((ISequenceChartProvider)part).getSequenceChart().findText(true);

            return null;
        }
    }

    private CommandContributionItem createGotoEventCommandContributionItem() {
        return new CommandContributionItem(new CommandContributionItemParameter(Workbench.getInstance(), null, "org.omnetpp.sequencechart.gotoEvent", SWT.PUSH));
    }

    public static class GotoEventHandler extends AbstractHandler {
        // TODO: setEnabled(!getEventLog().isEmpty());
        public Object execute(ExecutionEvent executionEvent) throws ExecutionException {
            IWorkbenchPart part = HandlerUtil.getActivePartChecked(executionEvent);
            if (part instanceof ISequenceChartProvider) {
                SequenceChart sequenceChart = ((ISequenceChartProvider)part).getSequenceChart();
                GotoEventDialog dialog = new GotoEventDialog(sequenceChart.getEventLog());
                if (dialog.open() == Window.OK)
                    sequenceChart.gotoElement(dialog.getEvent());
            }
            return null;
        }
    }

    private CommandContributionItem createGotoSimulationTimeCommandContributionItem() {
        return new CommandContributionItem(new CommandContributionItemParameter(Workbench.getInstance(), null, "org.omnetpp.sequencechart.gotoSimulationTime", SWT.PUSH));
    }

    public static class GotoSimulationTimeHandler extends AbstractHandler {
        // TODO: setEnabled(!getEventLog().isEmpty());
        public Object execute(ExecutionEvent executionEvent) throws ExecutionException {
            IWorkbenchPart part = HandlerUtil.getActivePartChecked(executionEvent);
            if (part instanceof ISequenceChartProvider) {
                SequenceChart sequenceChart = ((ISequenceChartProvider)part).getSequenceChart();
                GotoSimulationTimeDialog dialog = new GotoSimulationTimeDialog(sequenceChart.getEventLog(), sequenceChart.getSimulationTimeForViewportCoordinate(0));
                if (dialog.open() == Window.OK)
                    sequenceChart.scrollToSimulationTime(dialog.getSimulationTime());
            }
            return null;
        }
    }

    public static class RefreshHandler extends AbstractHandler {
        public Object execute(ExecutionEvent event) throws ExecutionException {
            IWorkbenchPart part = HandlerUtil.getActivePartChecked(event);

            if (part instanceof ISequenceChartProvider)
                ((ISequenceChartProvider)part).getSequenceChart().refresh();

            return null;
        }
    }
}

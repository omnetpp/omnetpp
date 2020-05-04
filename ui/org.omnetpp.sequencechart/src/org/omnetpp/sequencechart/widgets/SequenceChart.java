/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.widgets;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.math.MathContext;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

import org.apache.commons.collections.map.MultiValueMap;
import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Transform;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.ScrollBar;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.Debug;
import org.omnetpp.common.canvas.CachingCanvas;
import org.omnetpp.common.canvas.LargeRect;
import org.omnetpp.common.canvas.RubberbandSupport;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.eventlog.EventLogFilterParameters;
import org.omnetpp.common.eventlog.EventLogFindTextDialog;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.EventLogInput.TimelineMode;
import org.omnetpp.common.eventlog.EventLogSelection;
import org.omnetpp.common.eventlog.EventNumberRangeSet;
import org.omnetpp.common.eventlog.IEventLogChangeListener;
import org.omnetpp.common.eventlog.IEventLogProvider;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.common.eventlog.ModuleTreeItem.IModuleTreeItemVisitor;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.common.util.Pair;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.common.util.TimeUtils;
import org.omnetpp.common.virtualtable.IVirtualContentWidget;
import org.omnetpp.eventlog.engine.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.engine.FilteredEventLog;
import org.omnetpp.eventlog.engine.FilteredMessageDependency;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IMessageDependency;
import org.omnetpp.eventlog.engine.IMessageDependencyList;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.PtrVector;
import org.omnetpp.eventlog.engine.SequenceChartFacade;
import org.omnetpp.ned.core.NedResources;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.scave.engine.FileRun;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ScaveEngine;
import org.omnetpp.scave.engine.XYArrayVector;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.sequencechart.SequenceChartPlugin;
import org.omnetpp.sequencechart.editors.SequenceChartContributor;
import org.omnetpp.sequencechart.widgets.axisorder.AxisOrderByModuleId;
import org.omnetpp.sequencechart.widgets.axisorder.AxisOrderByModuleName;
import org.omnetpp.sequencechart.widgets.axisorder.FlatAxisOrderByMinimizingCost;
import org.omnetpp.sequencechart.widgets.axisorder.ManualAxisOrder;
import org.omnetpp.sequencechart.widgets.axisrenderer.AxisLineRenderer;
import org.omnetpp.sequencechart.widgets.axisrenderer.AxisMultiRenderer;
import org.omnetpp.sequencechart.widgets.axisrenderer.AxisVectorBarRenderer;
import org.omnetpp.sequencechart.widgets.axisrenderer.IAxisRenderer;

/**
 * The sequence chart figure shows the events and the messages passed along between several modules.
 * The chart consists of a series of horizontal lines each representing a simple or compound module.
 * Message dependencies are represented by straight or elliptic arrows pointing from the cause event to the consequence event.
 *
 * Zooming, scrolling, tooltips and event selections are also provided.
 *
 * @author andras, levy
 */
// TODO: proper "hand" cursor - current one is not very intuitive
public class SequenceChart
    extends CachingCanvas
    implements IVirtualContentWidget<IEvent>, ISelectionProvider, IEventLogChangeListener, IEventLogProvider
{
    /*************************************************************************************
     * PARAMETERS
     */

    private static final String STATE_PROPERTY = "SequenceChartState";
    private static final Cursor DRAG_CURSOR = new Cursor(null, SWT.CURSOR_SIZEALL);
    private static final int ANTIALIAS_TURN_ON_AT_MSEC = 100;
    private static final int ANTIALIAS_TURN_OFF_AT_MSEC = 300;
    private static final int MOUSE_TOLERANCE = 3;

    private boolean debug = false;

    private IEventLog eventLog; // the C++ wrapper for the data to be displayed
    private EventLogInput eventLogInput; // the Java input object

    private SequenceChartFacade sequenceChartFacade; // helpful C++ facade on eventlog
    private SequenceChartContributor sequenceChartContributor; // for popup menu

    private ISequenceChartLabelProvider labelProvider = new SequenceChartLabelProvider();
    private ISequenceChartStyleProvider styleProvider = new SequenceChartStyleProvider();

    private HoverSupport hoverSupport;
    private RubberbandSupport rubberbandSupport;

    private IWorkbenchPart workbenchPart;

    /*************************************************************************************
     * INTERNAL STATE
     */

    private long fixPointViewportCoordinate; // the viewport coordinate of the coordinate system's origin event stored in the facade
    private double pixelPerTimelineUnit = 0; // horizontal zoom factor

    private int fontHeight = -1; // cached for cases where a graphics is not available

    private boolean isDragging; // indicates ongoing drag operation
    private int dragStartX = -1, dragStartY = -1, dragDeltaX, dragDeltaY; // temporary variables for drag handling

    private boolean showArrowHeads = true; // show or hide arrow heads
    private boolean showAxes = true;
    private boolean showAxisHeaders = true;
    private boolean showAxisInfo = true;
    private boolean showAxisLabels = true;
    private boolean showAxisVectorData = true;
    private boolean showComponentMethodCalls = true; // show or hide module method call arrows
    private boolean showEmptyAxes = false;
    private boolean showEventLogInfo = false;
    private boolean showEventMarks = true;
    private boolean showEventNumbers = true;
    private boolean showHairlines = true;
    private boolean showInitializationEvent = false;
    private boolean showMessageNames = true; // show or hide message names
    private boolean showMessageReuses = false; // show or hide message reuse arrows
    private boolean showMessageSends = true; // show or hide message send arrows
    private boolean showMethodNames = true; // show or hide method names
    private boolean showMixedMessageDependencies = true; // show or hide mixed message dependency arrows
    private boolean showMixedSelfMessageDependencies = true; // show or hide mixed self message dependency arrows
    private boolean showPositionAndRange = true;
    private boolean showSelfMessageReuses = false; // show or hide self message reuse arrows
    private boolean showSelfMessageSends = true; // show or hide self message send arrows
    private boolean showTimeDifferences = true;
    private boolean showTransmissionDurations = true;
    private boolean showZeroSimulationTimeRegions = true;

    private ArrayList<BigDecimal> ticks; // a list of simulation times drawn on the axis as tick marks
    private BigDecimal tickPrefix; // the common part of all ticks on the gutter

    private QuadTree labelQuadTree = new QuadTree();
    private Map<Long, Point> labelPositions = new HashMap<Long, Point>();

    private ArrayList<ModuleTreeItem> openAxisModules = new ArrayList<ModuleTreeItem>(); // the modules (in no particular order) which may have an axis (they must be part of the module tree!) on the chart

    private boolean invalidVisibleAxisModules = true; // requests recalculation of visibleAxisModules
    private ArrayList<ModuleTreeItem> visibleAxisModules = new ArrayList<ModuleTreeItem>(); // the modules (in no particular order) which actually have an axis (they must be part of the module tree!) on the chart

    private boolean invalidAxisHeaders = true;
    private AxisHeader rootAxisHeader = null; // root of the axis header tree

    private boolean invalidAxes = true;
    private ArrayList<Axis> axes = new ArrayList<Axis>(); // axes in vertical order

    private boolean invalidAxisSpacing = true; // true means that the spacing value must be recalculated due to axis spacing mode is set to auto
    private double axisSpacing = 0; // y distance between two axes, might be fractional pixels to have precise positioning for several axes
    private AxisSpacingMode axisSpacingMode = AxisSpacingMode.AUTO;

    private AxisOrderingMode axisOrderingMode = AxisOrderingMode.MODULE_FULL_PATH; // specifies the ordering mode of axes
    private ManualAxisOrder manualAxisOrder = new ManualAxisOrder(); // remembers manual ordering

    private boolean invalidAxisModulePositions = true; // requests recalculation
    private int[] axisModulePositions; // specifies y order of the axis modules (in the same order as axisModules); this is a permutation of the 0 .. axisModule.size() - 1 numbers

    private boolean invalidReverseAxisModulePositions = true; // requests recalculation
    private int[] reverseAxisModulePositions;

    private boolean invalidModuleIdToAxisModuleIndexMap = true; // requests recalculation
    private Map<Integer, Integer> moduleIdToAxisModuleIndexMap; // some modules do not have axis but events occurred in them are still drawn on the chart

    private boolean invalidModuleIdToAxisRendererMap = true; // requests recalculation
    private Map<Integer, IAxisRenderer> moduleIdToAxisRendererMap = new HashMap<Integer, IAxisRenderer>(); // this map is not cleared when the eventlog is filtered or the filter is removed

    private boolean invalidVirtualSize = false; // requests recalculation
    private boolean invalidViewportSize = false; // requests recalculation

    private boolean drawWithAntialias = true; // antialias gets turned on/off automatically
    private boolean paintHasBeenFinished = false; // true means the user did not cancel the last paint

    private RuntimeException internalError;
    private boolean internalErrorHappenedDuringPaint = false; // if this is true, then paint only draws an error message

    private boolean followEnd = false; // when the eventlog changes should we follow it or not?

    /*************************************************************************************
     * SELECTION STATE
     */

    private ArrayList<SelectionListener> selectionListeners = new ArrayList<SelectionListener>(); // SWT selection listeners
    private ListenerList<ISelectionChangedListener> selectionChangedListeners = new ListenerList<ISelectionChangedListener>(); // list of selection change listeners (type ISelectionChangedListener).
    private boolean isSelectionChangeInProgress;

    private ArrayList<Object> highlightedObjects = new ArrayList<Object>();
    private ArrayList<Object> selectedObjects = new ArrayList<Object>();

    /*************************************************************************************
     * PUBLIC INNER TYPES
     */

    /**
     * Specifies how the vertical spacing between axes is determined.
     */
    public enum AxisSpacingMode {
        MANUAL,
        AUTO
    }

    /**
     * Determines the order of visible axes on the sequence chart.
     */
    public enum AxisOrderingMode {
        MODULE_ID,
        MODULE_FULL_PATH,
        MINIMIZE_CROSSINGS,
        MANUAL
    }

    /*************************************************************************************
     * CONSTRUCTOR, GETTERS, SETTERS
     */

    public SequenceChart(Composite parent, int style) {
        super(parent, style);
        setBackground(styleProvider.getBackgroundColor());
        setupHoverSupport();
        setupRubberbandSupport();
        setupMouseListener();
        setupKeyListener();
        setupListeners();
    }

    public IWorkbenchPart getWorkbenchPart() {
        return workbenchPart;
    }

    public void setWorkbenchPart(IWorkbenchPart workbenchPart) {
        this.workbenchPart = workbenchPart;
    }

    public ISequenceChartStyleProvider getStyleProvider() {
        return styleProvider;
    }

    public void getStyleProvider(ISequenceChartStyleProvider styleProvider) {
        this.styleProvider = styleProvider;
        clearCanvasCacheAndRedraw();
    }

    public ISequenceChartLabelProvider getLabelProvider() {
        return labelProvider;
    }

    public void getLabelProvider(ISequenceChartLabelProvider labelProvider) {
        this.labelProvider = labelProvider;
        clearCanvasCacheAndRedraw();
    }

    public SequenceChartContributor getSequenceChartContributor() {
        return sequenceChartContributor;
    }

    /**
     * Sets the contributor used to build the pop-up menu on the chart.
     */
    public void setSequenceChartContributor(SequenceChartContributor sequenceChartContributor) {
        this.sequenceChartContributor = sequenceChartContributor;
        MenuManager menuManager = new MenuManager();
        sequenceChartContributor.contributeToPopupMenu(menuManager);
        setMenu(menuManager.createContextMenu(this));
    }

    @Override
    public void setFont(Font font) {
        super.setFont(font);
        fontHeight = -1;
        invalidateVisibleAxisModules();
        invalidateViewportSize();
        clearCanvasCacheAndRedraw();
    }

    /*************************************************************************************
     * SETUP BEHAVIOR
     */

    private void setupHoverSupport() {
        hoverSupport = new HoverSupport();
        hoverSupport.setHoverSizeConstaints(700, 200);
        hoverSupport.adapt(this, new IHoverInfoProvider() {
            @Override
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                if (!internalErrorHappenedDuringPaint) {
                    ArrayList<Object> objects = collectVisibleObjectsAtPosition(x, y);
                    String tooltip = labelProvider.getDescriptiveText(objects, true);
                    return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(tooltip));
                }
                else
                    return null;
            }
        });
    }

    private void setupRubberbandSupport() {
        rubberbandSupport = new RubberbandSupport(this, SWT.MOD1) {
            @Override
            public void rubberBandSelectionMade(Rectangle r) {
                zoomToRectangle(r);
            }
        };
    }

    private void setupListeners() {
        addDisposeListener(new DisposeListener() {
            public void widgetDisposed(DisposeEvent e) {
                if (eventLogInput != null) {
                    storeState(eventLogInput.getFile());
                    eventLogInput.removeEventLogChangedListener(SequenceChart.this);
                }
            }
        });

        addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                if (eventLogInput != null) {
                    invalidateVisibleAxisModules();
                    invalidateAxisSpacing();
                    invalidateViewportSize();
                    invalidateVirtualSize();
                }
            }
        });
    }

    /*************************************************************************************
     * SHOW/HIDE
     */

    public void setShowAll(boolean showAll) {
        setShowArrowHeads(showAll);
        setShowAxes(showAll);
        setShowAxisHeaders(showAll);
        setShowAxisInfo(showAll);
        setShowAxisLabels(showAll);
        setShowAxisVectorData(showAll);
        setShowComponentMethodCalls(showAll);
        setShowEmptyAxes(showAll);
        setShowEventLogInfo(showAll);
        setShowEventMarks(showAll);
        setShowEventNumbers(showAll);
        setShowHairlines(showAll);
        setShowInitializationEvent(showAll);
        setShowMessageNames(showAll);
        setShowMessageReuses(showAll);
        setShowMessageSends(showAll);
        setShowMethodNames(showAll);
        setShowMixedMessageDependencies(showAll);
        setShowMixedSelfMessageDependencies(showAll);
        setShowPositionAndRange(showAll);
        setShowSelfMessageReuses(showAll);
        setShowSelfMessageSends(showAll);
        setShowTimeDifferences(showAll);
        setShowTransmissionDurations(showAll);
        setShowZeroSimulationTimeRegions(showAll);
    }

    /**
     * Returns whether position and range is displayed.
     */
    public boolean getShowPositionAndRange() {
        return showPositionAndRange;
    }

    /**
     * Hide/show simulation time range.
     */
    public void setShowPositionAndRange(boolean showPositionAndRange) {
        this.showPositionAndRange = showPositionAndRange;
        redraw();
    }

    /**
     * Returns whether eventlog info is displayed.
     */
    public boolean getShowEventLogInfo() {
        return showEventLogInfo;
    }

    /**
     * Hide/show eventlog info.
     */
    public void setShowEventLogInfo(boolean showEventLogInfo) {
        this.showEventLogInfo = showEventLogInfo;
        redraw();
    }

    /**
     * Returns whether initialization event is displayed.
     */
    public boolean getShowInitializationEvent() {
        return showInitializationEvent;
    }

    /**
     * Hide/show initialization event.
     */
    public void setShowInitializationEvent(boolean showInitializationEvent) {
        this.showInitializationEvent = showInitializationEvent;
        invalidateVisibleAxisModules();
        clearCanvasCacheAndRedraw();
    }

    /**
     * Returns whether message names are displayed on the arrows.
     */
    public boolean getShowMessageNames() {
        return showMessageNames;
    }

    /**
     * Hide/show message names on the arrows.
     */
    public void setShowMessageNames(boolean showMessageNames) {
        this.showMessageNames = showMessageNames;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Returns whether method names are displayed on the arrows.
     */
    public boolean getShowMethodNames() {
        return showMethodNames;
    }

    /**
     * Hide/show method names on the arrows.
     */
    public void setShowMethodNames(boolean showMethodNames) {
        this.showMethodNames = showMethodNames;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Returns whether message sends are shown on the chart.
     */
    public boolean getShowMessageSends() {
        return showMessageSends;
    }

    /**
     * Shows/Hides message sends.
     */
    public void setShowMessageSends(boolean showMessageSends) {
        this.showMessageSends = showMessageSends;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Returns whether self messages are shown on the chart.
     */
    public boolean getShowSelfMessageSends() {
        return showSelfMessageSends;
    }

    /**
     * Shows/Hides self messages.
     */
    public void setShowSelfMessageSends(boolean showSelfMessageSends) {
        this.showSelfMessageSends = showSelfMessageSends;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Returns whether reuse messages are shown on the chart.
     */
    public boolean getShowMessageReuses() {
        return showMessageReuses;
    }

    /**
     * Shows/Hides reuse messages.
     */
    public void setShowMessageReuses(boolean showMessageReuses) {
        this.showMessageReuses = showMessageReuses;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Returns whether reuse messages are shown on the chart.
     */
    public boolean getShowSelfMessageReuses() {
        return showSelfMessageReuses;
    }

    /**
     * Shows/Hides reuse messages.
     */
    public void setShowSelfMessageReuses(boolean showSelfMessageReuses) {
        this.showSelfMessageReuses = showSelfMessageReuses;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Returns whether mixed messages dependencies are shown on the chart.
     */
    public boolean getShowMixedMessageDependencies() {
        return showMixedMessageDependencies;
    }

    /**
     * Shows/Hides mixed messages dependencies.
     */
    public void setShowMixedMessageDependencies(boolean showMessageDependencies) {
        this.showMixedMessageDependencies = showMessageDependencies;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Returns whether mixed self messages dependencies are shown on the chart.
     */
    public boolean getShowMixedSelfMessageDependencies() {
        return showMixedSelfMessageDependencies;
    }

    /**
     * Shows/Hides dependenci messages.
     */
    public void setShowMixedSelfMessageDependencies(boolean showMixedSelfMessageDependencies) {
        this.showMixedSelfMessageDependencies = showMixedSelfMessageDependencies;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides module method calls.
     */
    public boolean getShowComponentMethodCalls() {
        return showComponentMethodCalls;
    }

    /**
     * Returns whether module method calls are shown on the chart.
     */
    public void setShowComponentMethodCalls(boolean showComponentMethodCalls) {
        this.showComponentMethodCalls = showComponentMethodCalls;
        sequenceChartFacade.setSeparateEventLogEntries(showComponentMethodCalls);
        if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() != -1)
            relocateFixPoint(sequenceChartFacade.getTimelineCoordinateSystemOriginEvent(), fixPointViewportCoordinate);
        invalidateVisibleAxisModules();
    }

    /**
     * Returns whether event marks are shown on the chart.
     */
    public boolean getShowEventMarks() {
        return showEventMarks;
    }

    /**
     * Shows/Hides event marks.
     */
    public void setShowEventMarks(boolean showEventMarks) {
        this.showEventMarks = showEventMarks;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Returns whether event numbers are shown on the chart.
     */
    public boolean getShowEventNumbers() {
        return showEventNumbers;
    }

    /**
     * Shows/Hides event numbers.
     */
    public void setShowEventNumbers(boolean showEventNumbers) {
        this.showEventNumbers = showEventNumbers;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides arrow heads.
     */
    public boolean getShowArrowHeads() {
        return showArrowHeads;
    }

    /**
     * Returns whether arrow heads are shown on the chart.
     */
    public void setShowArrowHeads(boolean showArrowHeads) {
        this.showArrowHeads = showArrowHeads;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides zero simulation time regions.
     */
    public boolean getShowZeroSimulationTimeRegions() {
        return showZeroSimulationTimeRegions;
    }

    /**
     * Returns whether zero simulation time regions are shown on the chart.
     */
    public void setShowZeroSimulationTimeRegions(boolean showZeroSimulationTimeRegions) {
        this.showZeroSimulationTimeRegions = showZeroSimulationTimeRegions;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides axis headers.
     */
    public boolean getShowAxisHeaders() {
        return showAxisHeaders;
    }

    /**
     * Returns whether axis headers are shown on the chart.
     */
    public void setShowAxisHeaders(boolean showAxisHeaders) {
        this.showAxisHeaders = showAxisHeaders;
        invalidateAxes();
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides axis info.
     */
    public boolean getShowAxisInfo() {
        return showAxisInfo;
    }

    /**
     * Returns whether axis info is shown on the chart.
     */
    public void setShowAxisInfo(boolean showAxisInfo) {
        this.showAxisInfo = showAxisInfo;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides axis labels.
     */
    public boolean getShowAxisLabels() {
        return showAxisLabels;
    }

    /**
     * Returns whether axis labels are shown on the chart.
     */
    public void setShowAxisLabels(boolean showAxisLabels) {
        this.showAxisLabels = showAxisLabels;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides axis vector data.
     */
    public boolean getShowAxisVectorData() {
        return showAxisVectorData;
    }

    /**
     * Returns whether axis vector data is shown on the chart.
     */
    public void setShowAxisVectorData(boolean showAxisVectorData) {
        this.showAxisVectorData = showAxisVectorData;
        for (Axis axis : getAxes()) {
            if (axis.axisRenderer instanceof AxisMultiRenderer) {
                AxisMultiRenderer axisRenderer = (AxisMultiRenderer)axis.axisRenderer;
                axisRenderer.setSelectedRendererIndex(showAxisVectorData ? axisRenderer.getRendererCount() - 1 : 0);
            }
        }
        invalidateAxisSpacing();
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides axes.
     */
    public boolean getShowAxes() {
        return showAxes;
    }

    /**
     * Returns whether axes are shown on the chart.
     */
    public void setShowAxes(boolean showAxes) {
        this.showAxes = showAxes;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides empty axes.
     */
    public boolean getShowEmptyAxes() {
        return showEmptyAxes;
    }

    /**
     * Returns whether empty axes are shown on the chart.
     */
    public void setShowEmptyAxes(boolean showEmptyAxes) {
        this.showEmptyAxes = showEmptyAxes;
        invalidateVisibleAxisModules();
    }

    /**
     * Shows/hides transmission durations.
     */
    public boolean getShowTransmissionDurations() {
        return showTransmissionDurations;
    }

    /**
     * Returns whether transmission durations are shown on the chart.
     */
    public void setShowTransmissionDurations(boolean showTransmissionDurations) {
        this.showTransmissionDurations = showTransmissionDurations;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides hairlines.
     */
    public boolean getShowHairlines() {
        return showHairlines;
    }

    /**
     * Returns whether hairlines are shown on the chart.
     */
    public void setShowHairlines(boolean showHairlines) {
        this.showHairlines = showHairlines;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Hide/show time differences.
     */
    public void setShowTimeDifferences(boolean showTimeDifferences) {
        this.showTimeDifferences = showTimeDifferences;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Returns whether time differences are shown on the chart.
     */
    public boolean getShowTimeDifferences() {
        return showTimeDifferences;
    }

    /**
     * Returns the current timeline mode.
     */
    public TimelineMode getTimelineMode() {
        return TimelineMode.values()[sequenceChartFacade.getTimelineMode()];
    }

    /**
     * Sets the timeline mode and updates the figure accordingly.
     * Tries to show the current simulation time range which was visible before the change
     * after changing the timeline coordinate system.
     */
    public void setTimelineMode(TimelineMode timelineMode) {
        org.omnetpp.common.engine.BigDecimal[] leftRightSimulationTimes = null;

        if (!eventLog.isEmpty())
            leftRightSimulationTimes = getViewportSimulationTimeRange();

        sequenceChartFacade.setTimelineMode(timelineMode.ordinal());

        if (!eventLog.isEmpty())
            setViewportSimulationTimeRange(leftRightSimulationTimes);
    }

    /**
     * Returns the current axis ordering mode.
     */
    public AxisOrderingMode getAxisOrderingMode() {
        return axisOrderingMode;
    }

    /**
     * Sets the axis ordering mode and updates the figure accordingly.
     */
    public void setAxisOrderingMode(AxisOrderingMode axisOrderingMode) {
        this.axisOrderingMode = axisOrderingMode;
        invalidateAxisHeaders();
        invalidateAxes();
        invalidateAxisModulePositions();
    }

    /**
     * Shows the manual ordering dialog partially filled up with the current ordering.
     */
    public int showManualOrderingDialog() {
        ModuleTreeItem[] sortedAxisModules = new ModuleTreeItem[getVisibleAxisModules().size()];

        for (int i = 0; i < sortedAxisModules.length; i++)
            sortedAxisModules[getAxisModulePositions()[i]] = getVisibleAxisModules().get(i);

        return manualAxisOrder.showManualOrderDialog(sortedAxisModules);
    }

    /*************************************************************************************
     * VIEWPORT
     */

    /**
     * Returns the currently visible range of simulation times as an array of two simulation times.
     */
    public org.omnetpp.common.engine.BigDecimal[] getViewportSimulationTimeRange()
    {
        return new org.omnetpp.common.engine.BigDecimal[] {getViewportLeftSimulationTime(), getViewportRightSimulationTime()};
    }

    /**
     * Sets the range of visible simulation times as an array of two simulation times.
     */
    public void setViewportSimulationTimeRange(org.omnetpp.common.engine.BigDecimal[] leftRightSimulationTimes) {
        zoomToSimulationTimeRange(leftRightSimulationTimes[0], leftRightSimulationTimes[1]);
    }

    /**
     * Returns the smallest visible simulation time within the viewport.
     */
    public org.omnetpp.common.engine.BigDecimal getViewportLeftSimulationTime() {
        return getSimulationTimeForViewportCoordinate(0);
    }

    /**
     * Returns the simulation time visible at the viewport's center.
     */
    public org.omnetpp.common.engine.BigDecimal getViewportCenterSimulationTime() {
        return getSimulationTimeForViewportCoordinate(getViewportWidth() / 2);
    }

    /**
     * Returns the biggest visible simulation time within the viewport.
     */
    public org.omnetpp.common.engine.BigDecimal getViewportRightSimulationTime() {
        return getSimulationTimeForViewportCoordinate(getViewportWidth());
    }

    /*************************************************************************************
     * SCROLLING, GOTOING
     */

    private void relocateFixPoint(IEvent event, long fixPointViewportCoordinate) {
        this.fixPointViewportCoordinate = fixPointViewportCoordinate;
        invalidateVisibleAxisModules();

        // the new event will be the origin of the timeline coordinate system
        if (event != null)
            sequenceChartFacade.relocateTimelineCoordinateSystem(event);
        else
            sequenceChartFacade.undefineTimelineCoordinateSystem();

        if (eventLog != null && !eventLog.isEmpty()) {
            // don't go after the very end
            if (eventLog.getLastEvent().getSimulationTime().less(getViewportRightSimulationTime())) {
                this.fixPointViewportCoordinate = getViewportWidth();
                sequenceChartFacade.relocateTimelineCoordinateSystem(eventLog.getLastEvent());
            }

            // don't go before the very beginning
            if (getSimulationTimeForViewportCoordinate(0).less(org.omnetpp.common.engine.BigDecimal.getZero())) {
                this.fixPointViewportCoordinate = 0;
                sequenceChartFacade.relocateTimelineCoordinateSystem(eventLog.getFirstEvent());
            }
        }
    }

    @Override
    protected long clipX(long x) {
        // the position of the visible area is not limited to a [0, max] range
        // this is due to the fact that the coordinate system is linked to the fixPoint event
        return x;
    }

    @Override
    protected int configureHorizontalScrollBar(ScrollBar scrollBar, long virtualSize, long virtualPos, int widgetSize) {
        ScrollBar horizontalBar = getHorizontalBar();
        horizontalBar.setMinimum(0);

        long[] eventPtrRange = null;

        if (eventLog != null && !eventLogInput.isCanceled())
            eventPtrRange = getFirstLastEventPtrForViewportRange(0, getViewportWidth());

        if (eventPtrRange != null && eventPtrRange[0] != 0 && eventPtrRange[1] != 0 &&
            (sequenceChartFacade.IEvent_getPreviousEvent(eventPtrRange[0]) != 0 || sequenceChartFacade.IEvent_getNextEvent(eventPtrRange[1]) != 0))
        {
            long numberOfElements = eventLog.getApproximateNumberOfEvents();
            horizontalBar.setMaximum((int)Math.max(numberOfElements, 1E+6));
            horizontalBar.setThumb((int)((double)horizontalBar.getMaximum() / numberOfElements));
            horizontalBar.setIncrement(1);
            horizontalBar.setPageIncrement(10);
            horizontalBar.setVisible(true);
        }
        else {
            horizontalBar.setMaximum(0);
            horizontalBar.setThumb(0);
            horizontalBar.setIncrement(0);
            horizontalBar.setPageIncrement(0);
            horizontalBar.setVisible(false);
        }

        return 0;
    }

    /**
     * Update horizontal scrollbar position based on the first and last visible events.
     */
    @Override
    protected void adjustHorizontalScrollBar() {
        if (eventLog != null && !eventLogInput.isCanceled()) {
            long[] eventPtrRange = getFirstLastEventPtrForViewportRange(0, getViewportWidth());
            long startEventPtr = eventPtrRange[0];
            long endEventPtr = eventPtrRange[1];
            double topPercentage = startEventPtr == 0 ? 0 : eventLog.getApproximatePercentageForEventNumber(sequenceChartFacade.IEvent_getEventNumber(startEventPtr));
            double bottomPercentage = endEventPtr == 0 ? 1 : eventLog.getApproximatePercentageForEventNumber(sequenceChartFacade.IEvent_getEventNumber(endEventPtr));
            double topWeight = 1 / topPercentage;
            double bottomWeight = 1 / (1 - bottomPercentage);
            double percentage;

            if (Double.isInfinite(topWeight))
                percentage = topPercentage;
            else if (Double.isInfinite(bottomWeight))
                percentage = bottomPercentage;
            else
                percentage = (topPercentage * topWeight + bottomPercentage * bottomWeight) / (topWeight + bottomWeight);

            ScrollBar horizontalBar = getHorizontalBar();
            horizontalBar.setSelection((int)((horizontalBar.getMaximum() - horizontalBar.getThumb()) * percentage));
        }
    }

    @Override
    protected void horizontalScrollBarChanged(SelectionEvent e) {
        ScrollBar scrollBar = getHorizontalBar();
        double percentage = (double)scrollBar.getSelection() / (scrollBar.getMaximum() - scrollBar.getThumb());
        followEnd = false;

        if (e.detail == SWT.ARROW_UP)
            scrollHorizontal(-10);
        else if (e.detail == SWT.ARROW_DOWN)
            scrollHorizontal(10);
        else if (e.detail == SWT.PAGE_UP)
            scrollHorizontal(-getViewportWidth());
        else if (e.detail == SWT.PAGE_DOWN)
            scrollHorizontal(getViewportWidth());
        else if (percentage == 0)
            scrollToBegin();
        else if (percentage == 1)
            scrollToEnd();
        else
            reveal(eventLog.getApproximateEventAt(percentage));
    }

    @Override
    public void scrollHorizontalTo(long x) {
        fixPointViewportCoordinate -= x - getViewportLeft();
        followEnd = false; // don't follow eventlog's end after a horizontal scroll
        invalidateVisibleAxisModules();
        super.scrollHorizontalTo(x);
    }

    public void scrollToBegin() {
        if (!eventLogInput.isCanceled() && !eventLog.isEmpty()) {
            IEvent event = eventLog.getFirstEvent();
            if (event != null && !showInitializationEvent && isInitializationEvent(event))
                event = event.getNextEvent();
            if (event != null && isVisibleObject(event)) {
                int x = (showAxisHeaders && getRootAxisHeader() != null ? getAxisHeaderWidth(getRootAxisHeader()) : 0) + styleProvider.getEventSelectionRadius() * 2;
                scrollToEvent(event, x);
            }
            else
                scrollToSimulationTime(org.omnetpp.common.engine.BigDecimal.getZero(), styleProvider.getEventSelectionRadius() * 2);
        }
    }

    public void scrollToEnd() {
        if (!eventLogInput.isCanceled() && !eventLog.isEmpty()) {
            IEvent event = eventLog.getLastEvent();
            if (event != null && isVisibleObject(event))
                scrollToEvent(event, getViewportWidth() - (int)getEventViewportWidth(event.getCPtr()) - styleProvider.getEventSelectionRadius() * 2);
            else
                scrollToSimulationTime(eventLog.getLastSimulationTime(), getViewportWidth() - styleProvider.getEventSelectionRadius() * 2);
            followEnd = true;
        }
    }

    public void scroll(int numberOfEvents) {
        long[] eventPtrRange = getFirstLastEventPtrForViewportRange(0, getViewportWidth());
        long startEventPtr = eventPtrRange[0];
        long endEventPtr = eventPtrRange[1];
        long eventPtr;

        if (numberOfEvents < 0) {
            eventPtr = startEventPtr;
            numberOfEvents++;
        }
        else {
            eventPtr = endEventPtr;
            numberOfEvents--;
        }

        IEvent neighbourEvent = eventLog.getNeighbourEvent(sequenceChartFacade.IEvent_getEvent(eventPtr), numberOfEvents);

        if (neighbourEvent != null)
            reveal(neighbourEvent);
    }

    public void reveal(IEvent event) {
        scrollToEvent(event);
    }

    public void scrollToEvent(IEvent event) {
        scrollToEvent(event, getViewportWidth() / 2);
    }

    /**
     * Scroll both horizontally and vertically to the given element.
     */
    public void scrollToEvent(IEvent event, int viewportX) {
        Assert.isTrue(event.getEventLog().equals(eventLog));

        boolean found = false;
        if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() != -1) {
            long[] eventPtrRange = getFirstLastEventPtrForViewportRange(0, getViewportWidth());
            long startEventPtr = eventPtrRange[0];
            long endEventPtr = eventPtrRange[1];

            if (startEventPtr != 0 && endEventPtr != 0) {
                // look one event backward
                long previousEventPtr = sequenceChartFacade.IEvent_getPreviousEvent(startEventPtr);
                if (previousEventPtr != 0)
                    startEventPtr = previousEventPtr;

                // and forward so that one additional event scrolling can be done with less distraction
                long nextEventPtr = sequenceChartFacade.IEvent_getNextEvent(endEventPtr);
                if (nextEventPtr != 0)
                    endEventPtr = nextEventPtr;

                for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.IEvent_getNextEvent(eventPtr)) {
                    if (eventPtr == event.getCPtr()) {
                        found = true;
                        break;
                    }

                    if (eventPtr == endEventPtr)
                        break;
                }
            }
        }

        long d = styleProvider.getEventSelectionRadius() * 2;
        if (!found)
            relocateFixPoint(event, viewportX);
        else {
            long x = getViewportLeft() + getEventXViewportCoordinateBegin(event.getCPtr());
            scrollHorizontalToRange(x - d, x + d);

            if (!getModuleIdToAxisModuleIndexMap().containsKey(event.getModuleId()))
                invalidateVisibleAxisModules();
        }

        long y = getViewportTop() + (isInitializationEvent(event) ? getViewportHeight() / 2 : getEventYViewportCoordinate(event.getCPtr()));
        scrollVerticalToRange(y - d, y + d);
        adjustHorizontalScrollBar();

        followEnd = false;

        redraw();
    }

    public void revealFocus() {
        IEvent event = getSelectedEvent();

        if (event != null)
            reveal(event);
    }

    public void scrollToSimulationTime(org.omnetpp.common.engine.BigDecimal simulationTime) {
        scrollToSimulationTime(simulationTime, getViewportWidth() / 2);
    }

    /**
     * Scroll the canvas making the simulation time visible at the left edge of the viewport.
     */
    public void scrollToSimulationTime(org.omnetpp.common.engine.BigDecimal simulationTime, int x) {
        scrollHorizontal(getViewportCoordinateForSimulationTime(simulationTime) - x);
        redraw();
    }

    public void scrollToTimelineCoordinate(double timelineCoordinate) {
        scrollHorizontal(getViewportCoordinateForTimelineCoordinate(timelineCoordinate));
        redraw();
    }

    /**
     * Scroll vertically to make the axis module visible.
     */
    public void scrollToAxisModule(ModuleTreeItem axisModule) {
        for (Axis axis : getAxes()) {
            if (axis.axisHeader.module == axisModule) {
                scrollVerticalTo(axis.y - axis.axisRenderer.getHeight() / 2 - getViewportHeight() / 2);
                break;
            }
        }
    }

    public void scrollToMessageDependency(IMessageDependency messageDependency) {
        org.omnetpp.common.engine.BigDecimal causeSimulationTime = messageDependency.getCauseEvent().getSimulationTime();
        org.omnetpp.common.engine.BigDecimal consequenceSimulationTime = messageDependency.getConsequenceEvent().getSimulationTime();
        scrollToSimulationTime(new org.omnetpp.common.engine.BigDecimal(causeSimulationTime.add(consequenceSimulationTime).doubleValue() / 2));
    }

    public void scrollToComponentMethodCall(ComponentMethodBeginEntry componentMethodBeginEntry) {
        scrollToEvent(componentMethodBeginEntry.getEvent());
    }

    public void moveFocus(int numberOfEvents) {
        IEvent selectedEvent = getSelectedEvent();

        if (selectedEvent != null) {
            IEvent neighbourEvent = eventLog.getNeighbourEvent(selectedEvent, numberOfEvents);

            if (neighbourEvent != null)
                gotoElement(neighbourEvent);
        }
    }

    public void gotoBegin() {
        IEvent event = eventLog.getFirstEvent();

        if (event != null) {
            if (!showInitializationEvent && isInitializationEvent(event))
                event = event.getNextEvent();
            if (event != null && isVisibleObject(event))
                setSelectedEvent(event);
        }

        scrollToBegin();
    }

    public void gotoEnd() {
        IEvent event = eventLog.getLastEvent();

        if (event != null && isVisibleObject(event))
            setSelectedEvent(event);

        scrollToEnd();
    }

    public void gotoElement(IEvent event) {
        setSelectedEvent(event);
        reveal(event);
    }

    public void gotoClosestElement(IEvent event) {
        if (eventLog instanceof FilteredEventLog) {
            FilteredEventLog filteredEventLog = (FilteredEventLog)eventLogInput.getEventLog();
            IEvent closestEvent = filteredEventLog.getMatchingEventInDirection(event.getEventNumber(), false);

            if (closestEvent != null)
                gotoElement(closestEvent);
            else {
                closestEvent = filteredEventLog.getMatchingEventInDirection(sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber(), true);

                if (closestEvent != null)
                    gotoElement(closestEvent);
            }
        }
        else
            gotoElement(event);
    }

    public void gotoSimulationTime(org.omnetpp.common.engine.BigDecimal simulationTime) {
        setSelectedSimulationTime(simulationTime);
        scrollToSimulationTime(simulationTime);
    }

    /*************************************************************************************
     * ZOOMING
     */

    /**
     * Sets zoom level so that the default number of events fit into the viewport.
     */
    public void defaultZoom() {
        eventLogInput.runWithProgressMonitor(new Runnable() {
            public void run() {
                setDefaultPixelPerTimelineUnit(getViewportWidth());
            }
        });
    }

    /**
     * Sets zoom level so that the default number of events fit into the viewport.
     */
    public void zoomToFit() {
        eventLogInput.runWithProgressMonitor(new Runnable() {
            public void run() {
                int padding = 20;
                double firstTimelineCoordinate = sequenceChartFacade.getTimelineCoordinateBegin(eventLog.getFirstEvent());
                double lastTimelineCoordinate = sequenceChartFacade.getTimelineCoordinateBegin(eventLog.getLastEvent());
                double timelineCoordinateDelta = lastTimelineCoordinate - firstTimelineCoordinate;
                if (timelineCoordinateDelta == 0)
                    setPixelPerTimelineUnit(1);
                else
                    setPixelPerTimelineUnit((getViewportWidth() - padding * 2) / timelineCoordinateDelta);
                scrollToBegin();
            }
        });
    }

    /**
     * Increases pixels per timeline coordinate.
     */
    public void zoomIn() {
        zoomBy(1.5);
    }

    /**
     * Decreases pixels per timeline coordinate.
     */
    public void zoomOut() {
        zoomBy(1.0 / 1.5);
    }

    /**
     * Multiplies pixel per timeline coordinate by zoomFactor.
     */
    public void zoomBy(final double zoomFactor) {
        eventLogInput.runWithProgressMonitor(new Runnable() {
            public void run() {
                org.omnetpp.common.engine.BigDecimal simulationTime = null;

                if (!eventLog.isEmpty())
                    simulationTime = getViewportCenterSimulationTime();

                setPixelPerTimelineUnit(getPixelPerTimelineUnit() * zoomFactor);

                if (!eventLog.isEmpty())
                    scrollToSimulationTime(simulationTime, getViewportWidth() / 2);
            }
        });
    }

    /**
     * Zoom to the given rectangle, given by viewport coordinates relative to the
     * top-left corner of the canvas.
     */
    public void zoomToRectangle(final Rectangle r) {
        eventLogInput.runWithProgressMonitor(new Runnable() {
            public void run() {
                double timelineCoordinate = getTimelineCoordinateForViewportCoordinate(r.x);
                double timelineCoordinateDelta = getTimelineCoordinateForViewportCoordinate(r.right()) - timelineCoordinate;
                setPixelPerTimelineUnit(getViewportWidth() / timelineCoordinateDelta);
                scrollHorizontal(getViewportCoordinateForTimelineCoordinate(timelineCoordinate));
            }
        });
    }

    /**
     * Zoom and scroll to make the given start and end simulation times visible.
     */
    public void zoomToSimulationTimeRange(final org.omnetpp.common.engine.BigDecimal startSimulationTime, final org.omnetpp.common.engine.BigDecimal endSimulationTime) {
        eventLogInput.runWithProgressMonitor(new Runnable() {
            public void run() {
                if (!endSimulationTime.isNaN() && !startSimulationTime.equals(endSimulationTime)) {
                    double timelineUnitDelta = sequenceChartFacade.getTimelineCoordinateForSimulationTime(endSimulationTime) - sequenceChartFacade.getTimelineCoordinateForSimulationTime(startSimulationTime);

                    if (timelineUnitDelta > 0)
                        setPixelPerTimelineUnit(getViewportWidth() / timelineUnitDelta);
                }

                scrollHorizontal(getViewportCoordinateForSimulationTime(startSimulationTime));
            }
        });
    }

    /**
     * Zoom and scroll to make the given start and end simulation times visible, but leave some extra pixels on both sides.
     */
    public void zoomToSimulationTimeRange(org.omnetpp.common.engine.BigDecimal startSimulationTime, org.omnetpp.common.engine.BigDecimal endSimulationTime, int pixelInset) {
        if (pixelInset > 0) {
            // NOTE: we can't go directly there, so here is an approximation
            for (int i = 0; i < 3; i++) {
                org.omnetpp.common.engine.BigDecimal newStartSimulationTime = getSimulationTimeForViewportCoordinate(getViewportCoordinateForSimulationTime(startSimulationTime) - pixelInset);
                org.omnetpp.common.engine.BigDecimal newEndSimulationTime = getSimulationTimeForViewportCoordinate(getViewportCoordinateForSimulationTime(endSimulationTime) + pixelInset);

                zoomToSimulationTimeRange(newStartSimulationTime, newEndSimulationTime);
            }
        }
        else
            zoomToSimulationTimeRange(startSimulationTime, endSimulationTime);
    }

    /**
     * Zoom and scroll to the given message dependency so that it fits into the viewport.
     */
    public void zoomToMessageDependency(IMessageDependency messageDependency) {
        zoomToSimulationTimeRange(messageDependency.getCauseEvent().getSimulationTime(), messageDependency.getConsequenceEvent().getSimulationTime(), (int)(getViewportWidth() * 0.1));
    }

    /**
     * Zoom and scroll to make the value at the given simulation time visible at once.
     */
    public void zoomToAxisValue(ModuleTreeItem axisModule, org.omnetpp.common.engine.BigDecimal simulationTime) {
        for (int i = 0; i < getVisibleAxisModules().size(); i++) {
            if (getVisibleAxisModules().get(i) == axisModule) {
                IAxisRenderer axisRenderer = getAxis(i).axisRenderer;
                if (axisRenderer instanceof AxisVectorBarRenderer) {
                    AxisVectorBarRenderer axisVectorBarRenderer = (AxisVectorBarRenderer)axisRenderer;
                    zoomToSimulationTimeRange(
                        axisVectorBarRenderer.getSimulationTime(axisVectorBarRenderer.getIndex(simulationTime, true)),
                        axisVectorBarRenderer.getSimulationTime(axisVectorBarRenderer.getIndex(simulationTime, false)),
                        (int)(getViewportWidth() * 0.1));
                }
            }
        }
    }

    /*************************************************************************************
     * INPUT HANDLING
     */

    /**
     * Returns the currently displayed EventLogInput object.
     */
    public EventLogInput getInput() {
        return eventLogInput;
    }

    /**
     * Returns the eventlog (data) to be displayed on the chart.
     */
    public IEventLog getEventLog() {
        return eventLog;
    }

    /**
     * Sets a new EventLogInput to be displayed.
     */
    public void setInput(final EventLogInput newEventLogInput) {
        if (newEventLogInput != eventLogInput) {
            // store current settings
            if (eventLogInput != null) {
                eventLogInput.runWithProgressMonitor(new Runnable() {
                    public void run() {
                        eventLogInput.removeEventLogChangedListener(SequenceChart.this);
                        storeState(eventLogInput.getFile());
                    }
                });
            }
            if (newEventLogInput == null) {
                eventLog = null;
                eventLogInput = null;
                sequenceChartFacade = null;
                styleProvider.setEventLogInput(null);
                labelProvider.setEventLogInput(null);
            }
            else {
                eventLog = newEventLogInput.getEventLog();
                eventLogInput = newEventLogInput;
                sequenceChartFacade = newEventLogInput.getSequenceChartFacade();
                styleProvider.setEventLogInput(newEventLogInput);
                labelProvider.setEventLogInput(newEventLogInput);
                eventLogInput.addEventLogChangedListener(SequenceChart.this);
                // restore last known settings
                eventLogInput.runWithProgressMonitor(new Runnable() {
                    public void run() {
                        setup();
                    }
                });
            }
        }
    }

    private void setup() {
        if (!restoreState(eventLogInput.getFile())) {
            sequenceChartFacade.setSeparateEventLogEntries(showComponentMethodCalls);
            if (!eventLog.isEmpty()) {
                // don't use relocateFixPoint, because viewportWidth is not yet set during initializing
                sequenceChartFacade.relocateTimelineCoordinateSystem(eventLog.getFirstEvent());
                fixPointViewportCoordinate = 0;
                openModule(eventLogInput.getModuleTreeRoot());
            }
            else {
                sequenceChartFacade.undefineTimelineCoordinateSystem();
                fixPointViewportCoordinate = 0;
                clearOpenAxisModules();
            }
        }
        selectedObjects.clear();
        highlightedObjects.clear();
        invalidate();
    }

    /*************************************************************************************
     * PERSISTENT STATE
     */

    /**
     * Store persistent sequence chart settings for the given resource.
     */
    public void storeState(IResource resource) {
        try {
            PersistentResourcePropertyManager manager = new PersistentResourcePropertyManager(SequenceChartPlugin.PLUGIN_ID);

            if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() == -1)
                manager.removeProperty(resource, STATE_PROPERTY);
            else {
                SequenceChartState sequenceChartState = new SequenceChartState();
                sequenceChartState.viewportTop = (int)getViewportTop();
                sequenceChartState.fixPointEventNumber = sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber();
                sequenceChartState.fixPointViewportCoordinate = fixPointViewportCoordinate;
                sequenceChartState.pixelPerTimelineCoordinate = getPixelPerTimelineUnit();

                // store attached vectors
                SequenceChartState.AxisState[] axisStates = new SequenceChartState.AxisState[getOpenAxisModules().size()];

                for (int i = 0; i < getOpenAxisModules().size(); i++) {
                    ModuleTreeItem moduleTreeItem = getOpenAxisModules().get(i);
                    SequenceChartState.AxisState axisState = axisStates[i] = new SequenceChartState.AxisState();
                    axisState.moduleFullPath = moduleTreeItem.getModuleFullPath();
                    int index = getVisibleAxisModules().indexOf(moduleTreeItem);
                    if (index != -1) {
                        IAxisRenderer axisRenderer = getAxis(index).axisRenderer;
                        if (axisRenderer instanceof AxisVectorBarRenderer) {
                            AxisVectorBarRenderer renderer = (AxisVectorBarRenderer)axisRenderer;
                            axisState.vectorFileName = renderer.getVectorFileName();
                            axisState.vectorRunName = renderer.getVectorRunName();
                            axisState.vectorModuleFullPath = renderer.getVectorModuleFullPath();
                            axisState.vectorName = renderer.getVectorName();
                        }
                    }
                }

                sequenceChartState.axisStates = axisStates;

                // store manual axis order
                sequenceChartState.axisOrderingMode = axisOrderingMode;
                ArrayList<ModuleTreeItem> axisOrder = manualAxisOrder.getAxisOrder();
                String[] moduleFullPathesManualAxisOrder = new String[axisOrder.size()];
                for (int i = 0; i < moduleFullPathesManualAxisOrder.length; i++) {
                    ModuleTreeItem axisModule = axisOrder.get(i);
                    if (axisModule != null)
                        moduleFullPathesManualAxisOrder[i] = axisModule.getModuleFullPath();
                }
                sequenceChartState.moduleFullPathesManualAxisOrder = moduleFullPathesManualAxisOrder;

                // store timeline mode
                sequenceChartState.timelineMode = getTimelineMode();

                // store options
                sequenceChartState.axisSpacingMode = getAxisSpacingMode();
                sequenceChartState.axisSpacing = getAxisSpacing();
                sequenceChartState.showPositionAndRange = getShowPositionAndRange();
                sequenceChartState.showEventLogInfo = getShowEventLogInfo();
                sequenceChartState.showInitializationEvent = getShowInitializationEvent();
                sequenceChartState.showMessageNames = getShowMessageNames();
                sequenceChartState.showMethodNames = getShowMethodNames();
                sequenceChartState.showEventMarks = getShowEventMarks();
                sequenceChartState.showEventNumbers = getShowEventNumbers();
                sequenceChartState.showMessageSends = getShowMessageSends();
                sequenceChartState.showSelfMessageSends = getShowSelfMessageSends();
                sequenceChartState.showMessageReuses = getShowMessageReuses();
                sequenceChartState.showSelfMessageReuses = getShowSelfMessageReuses();
                sequenceChartState.showMixedMessageDependencies = getShowMixedMessageDependencies();
                sequenceChartState.showMixedSelfMessageDependencies = getShowMixedSelfMessageDependencies();
                sequenceChartState.showComponentMethodCalls = getShowComponentMethodCalls();
                sequenceChartState.showArrowHeads = getShowArrowHeads();
                sequenceChartState.showZeroSimulationTimeRegions = getShowZeroSimulationTimeRegions();
                sequenceChartState.showAxisHeaders = getShowAxisHeaders();
                sequenceChartState.showAxisInfo = getShowAxisInfo();
                sequenceChartState.showAxisLabels = getShowAxisLabels();
                sequenceChartState.showAxisVectorData = getShowAxisVectorData();
                sequenceChartState.showAxes = getShowAxes();
                sequenceChartState.showEmptyAxes = getShowEmptyAxes();
                sequenceChartState.showTimeDifferences = getShowTimeDifferences();
                sequenceChartState.showTransmissionDurations = getShowTransmissionDurations();
                sequenceChartState.showHairlines = getShowHairlines();

                Font font = getFont();
                sequenceChartState.fontName = font.getFontData()[0].getName();
                sequenceChartState.fontHeight = font.getFontData()[0].getHeight();

                manager.setProperty(resource, STATE_PROPERTY, sequenceChartState);
            }
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Restore persistent sequence chart settings for the given resource.
     * NOTE: the state of the filter menu Show all/filtered is intentionally not saved
     * because a the log file can change between saved sessions. So there is no guarantee how
     * a filter will perform next time if the user opens the sequence chart. It can throw error
     * or run too long to be acceptable.
     */
    public boolean restoreState(IResource resource) {
        PersistentResourcePropertyManager manager = new PersistentResourcePropertyManager(SequenceChartPlugin.PLUGIN_ID, getClass().getClassLoader());

        try {
            if (manager.hasProperty(resource, STATE_PROPERTY)) {
                SequenceChartState sequenceChartState = (SequenceChartState)manager.getProperty(resource, STATE_PROPERTY);
                IEvent fixPointEvent = eventLog.getEventForEventNumber(sequenceChartState.fixPointEventNumber);

                if (fixPointEvent != null) {
                    clearOpenAxisModules();

                    setPixelPerTimelineUnit(sequenceChartState.pixelPerTimelineCoordinate);
                    relocateFixPoint(fixPointEvent, sequenceChartState.fixPointViewportCoordinate);
                    scrollVerticalTo(sequenceChartState.viewportTop);

                    // restore attached vectors
                    if (sequenceChartState.axisStates != null) {
                        ResultFileManagerEx resultFileManager = new ResultFileManagerEx();

                        for (int i = 0; i < sequenceChartState.axisStates.length; i++) {
                            SequenceChartState.AxisState axisState = sequenceChartState.axisStates[i];
                            ModuleTreeItem moduleTreeItem = eventLogInput.getModuleTreeRoot().findDescendantModule(axisState.moduleFullPath);
                            addOpenAxisModule(moduleTreeItem);

                            if (axisState.vectorFileName != null) {
                                int loadFlags = ResultFileManagerEx.RELOAD_IF_CHANGED | ResultFileManagerEx.IGNORE_LOCK_FILE | ResultFileManagerEx.ALLOW_LOADING_WITHOUT_INDEX;
                                ResultFile resultFile = resultFileManager.loadFile(axisState.vectorFileName, axisState.vectorFileName, loadFlags, null); //TODO would be better to do it from a job that's interruptible
                                Assert.isNotNull(resultFile); // could only happen if loadFlags contain some "SKIP" flag, but it doesn't
                                FileRun fileRun = resultFileManager.getFileRun(resultFile, resultFileManager.getRunByName(axisState.vectorRunName));
                                long id = resultFileManager.getItemByName(fileRun, axisState.vectorModuleFullPath, axisState.vectorName);
                                // TODO: compare vector's run against log file's run
                                ResultItem resultItem = resultFileManager.getItem(id);
                                IDList selectedIdList = new IDList(id);
                                XYArrayVector dataVector = ScaveEngine.readVectorsIntoArrays2(resultFileManager, selectedIdList, true, true);
                                IAxisRenderer axisRenderer = new AxisVectorBarRenderer(this, axisState.vectorFileName, axisState.vectorRunName, axisState.vectorModuleFullPath, axisState.vectorName, resultItem, dataVector, 0);
                                axisRenderer = new AxisMultiRenderer(new IAxisRenderer[] {new AxisLineRenderer(this, moduleTreeItem), axisRenderer}, 1);
                                setAxisRenderer(eventLogInput.getModuleTreeRoot().findDescendantModule(axisState.moduleFullPath), axisRenderer);
                            }
                        }
                    }

                    // restore axis order
                    if (sequenceChartState.axisOrderingMode != null)
                        axisOrderingMode = sequenceChartState.axisOrderingMode;
                    if (sequenceChartState.moduleFullPathesManualAxisOrder != null) {
                        ArrayList<ModuleTreeItem> axisOrder = new ArrayList<ModuleTreeItem>();
                        for (int i = 0; i < sequenceChartState.moduleFullPathesManualAxisOrder.length; i++)
                            axisOrder.add(eventLogInput.getModuleTreeRoot().findDescendantModule(sequenceChartState.moduleFullPathesManualAxisOrder[i]));
                        manualAxisOrder.setAxisOrder(axisOrder);
                    }

                    // restore options
                    if (sequenceChartState.axisSpacingMode != null)
                        axisSpacingMode = sequenceChartState.axisSpacingMode;
                    if (sequenceChartState.axisSpacing != -1)
                        axisSpacing = sequenceChartState.axisSpacing;
                    if (sequenceChartState.showPositionAndRange != null)
                        setShowPositionAndRange(sequenceChartState.showPositionAndRange);
                    if (sequenceChartState.showEventLogInfo != null)
                        setShowEventLogInfo(sequenceChartState.showEventLogInfo);
                    if (sequenceChartState.showInitializationEvent != null)
                        setShowInitializationEvent(sequenceChartState.showInitializationEvent);
                    if (sequenceChartState.showMessageNames != null)
                        setShowMessageNames(sequenceChartState.showMessageNames);
                    if (sequenceChartState.showMethodNames != null)
                        setShowMethodNames(sequenceChartState.showMethodNames);
                    if (sequenceChartState.showEventMarks != null)
                        setShowEventMarks(sequenceChartState.showEventMarks);
                    if (sequenceChartState.showEventNumbers != null)
                        setShowEventNumbers(sequenceChartState.showEventNumbers);
                    if (sequenceChartState.showMessageSends != null)
                        setShowMessageSends(sequenceChartState.showMessageSends);
                    if (sequenceChartState.showSelfMessageSends != null)
                        setShowSelfMessageSends(sequenceChartState.showSelfMessageSends);
                    if (sequenceChartState.showMessageReuses != null)
                        setShowMessageReuses(sequenceChartState.showMessageReuses);
                    if (sequenceChartState.showSelfMessageReuses != null)
                        setShowSelfMessageReuses(sequenceChartState.showSelfMessageReuses);
                    if (sequenceChartState.showMixedMessageDependencies != null)
                        setShowMixedMessageDependencies(sequenceChartState.showMixedMessageDependencies);
                    if (sequenceChartState.showMixedSelfMessageDependencies != null)
                        setShowMixedSelfMessageDependencies(sequenceChartState.showMixedSelfMessageDependencies);
                    if (sequenceChartState.showComponentMethodCalls != null)
                        setShowComponentMethodCalls(sequenceChartState.showComponentMethodCalls);
                    if (sequenceChartState.showArrowHeads != null)
                        setShowArrowHeads(sequenceChartState.showArrowHeads);
                    if (sequenceChartState.showZeroSimulationTimeRegions != null)
                        setShowZeroSimulationTimeRegions(sequenceChartState.showZeroSimulationTimeRegions);
                    if (sequenceChartState.showAxisHeaders != null)
                        setShowAxisHeaders(sequenceChartState.showAxisHeaders);
                    if (sequenceChartState.showAxisInfo != null)
                        setShowAxisInfo(sequenceChartState.showAxisInfo);
                    if (sequenceChartState.showAxisLabels != null)
                        setShowAxisLabels(sequenceChartState.showAxisLabels);
                    if (sequenceChartState.showAxisVectorData != null)
                        setShowAxisVectorData(sequenceChartState.showAxisVectorData);
                    if (sequenceChartState.showAxes != null)
                        setShowAxes(sequenceChartState.showAxes);
                    if (sequenceChartState.showEmptyAxes != null)
                        setShowEmptyAxes(sequenceChartState.showEmptyAxes);
                    if (sequenceChartState.showTimeDifferences != null)
                        setShowTimeDifferences(sequenceChartState.showTimeDifferences);
                    if (sequenceChartState.showTransmissionDurations != null)
                        setShowTransmissionDurations(sequenceChartState.showTransmissionDurations);
                    if (sequenceChartState.showHairlines != null)
                        setShowHairlines(sequenceChartState.showHairlines);

                    // restore timeline mode
                    if (sequenceChartState.timelineMode != null)
                        setTimelineMode(sequenceChartState.timelineMode);

                    if (sequenceChartState.fontName != null) {
                        FontData fontData = new FontData(sequenceChartState.fontName, sequenceChartState.fontHeight, SWT.NONE);
                        Font font = new Font(getFont().getDevice(), fontData);
                        setFont(font);
                    }

                    return true;
                }
            }

            return false;
        }
        catch (Exception e) {
            manager.removeProperty(resource, STATE_PROPERTY);

            SequenceChartPlugin.logError(e);
            MessageDialog.openError(getShell(), "Error", "Could not restore saved sequence chart state, ignoring.");
            return false;
        }
    }

    /*************************************************************************************
     * EVENTLOG NOTIFICATIONS
     */

    public void eventLogAppended() {
        Display.getCurrent().asyncExec(new Runnable() {
            public void run() {
                try {
                    eventLogChanged();
                }
                catch (RuntimeException e) {
                    if (eventLogInput.isFileChangedException(e))
                        eventLogInput.synchronize(e);
                    else
                        throw e;
                }
            }
        });
    }

    public void eventLogOverwritten() {
        // NOTE: it must be synchronous because the coordinate system origin is already cleared
        // NOTE: and making this asynchronous allows paint events to kick in too early
        Display.getCurrent().syncExec(new Runnable() {
            public void run() {
                try {
                    setup();
                    eventLogChanged();
                }
                catch (RuntimeException e) {
                    if (eventLogInput.isFileChangedException(e))
                        eventLogInput.synchronize(e);
                    else
                        throw e;
                }
            }
        });
    }

    private void eventLogChanged() {
        if (eventLog.isEmpty())
            relocateFixPoint(null, 0);
        else if (followEnd) {
            if (debug)
                Debug.println("Scrolling to follow eventlog change");
            if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() == -1)
                relocateFixPoint(eventLog.getLastEvent(), 0);
            scrollToEnd();
        }
        else if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() == -1)
        {
            relocateFixPoint(eventLog.getFirstEvent(), 0);
            scrollToBegin();
        }
        if (debug)
            Debug.println("SequenceChart got notification about eventlog change");
        configureScrollBars();
        adjustHorizontalScrollBar();
        clearCanvasCacheAndRedraw();
    }

    public void eventLogFiltered() {
        eventLog = eventLogInput.getEventLog();

        if (eventLog.isEmpty())
            relocateFixPoint(null, 0);
        else if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() != -1) {
            FilteredEventLog filteredEventLog = (FilteredEventLog)eventLogInput.getEventLog();
            IEvent closestEvent = filteredEventLog.getMatchingEventInDirection(sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber(), false);

            if (closestEvent != null)
                relocateFixPoint(closestEvent, 0);
            else {
                closestEvent = filteredEventLog.getMatchingEventInDirection(sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber(), true);

                if (closestEvent != null)
                    relocateFixPoint(closestEvent, 0);
                else
                    scrollToBegin();
            }
        }
        else
            scrollToBegin();

        selectedObjects.clear();
        highlightedObjects.clear();
        sequenceChartContributor.update();
        invalidateVisibleAxisModules();
    }

    public void eventLogFilterRemoved() {
        eventLog = eventLogInput.getEventLog();

        if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() != -1)
            relocateFixPoint(sequenceChartFacade.getTimelineCoordinateSystemOriginEvent(), 0);
        else
            scrollToBegin();

        sequenceChartContributor.update();
        invalidateVisibleAxisModules();
    }

    public void eventLogLongOperationStarted() {
    }

    public void eventLogLongOperationEnded() {
        if (!paintHasBeenFinished)
            redraw();
    }

    public void eventLogProgress() {
        if (eventLogInput.getEventLogProgressManager().isCanceled())
            redraw();
    }

    /*************************************************************************************
     * FINDING
     */

    /**
     * Starts or continues a find operation and immediately jumps to the result.
     */
    public void findText(boolean continueSearch) {
        String findText = null;
        EventLogEntry foundEventLogEntry = null;

        EventLogFindTextDialog findTextDialog = eventLogInput.getFindTextDialog();

        if (continueSearch || findTextDialog.open() == Window.OK) {
            findText = findTextDialog.getValue();

            if (findText != null) {
                try {
                    IEvent event = getSelectedEvent();

                    if (event == null) {
                        long[] eventPtrRange = getFirstLastEventPtrForViewportRange(0, 0);
                        event = sequenceChartFacade.IEvent_getEvent(eventPtrRange[0]);
                        if (event == null)
                            return;
                    }

                    EventLogEntry startEventLogEntry = null;
                    if (findTextDialog.isBackward()) {
                        event = event.getPreviousEvent();
                        if (event == null)
                            return;
                        startEventLogEntry = event.getEventLogEntry(event.getNumEventLogEntries() - 1);
                    }
                    else {
                        event = event.getNextEvent();
                        if (event == null)
                            return;
                        startEventLogEntry = event.getEventEntry();
                    }

                    foundEventLogEntry = eventLog.findEventLogEntry(startEventLogEntry, findText, !findTextDialog.isBackward(), !findTextDialog.isCaseInsensitive());
                }
                finally {
                    if (foundEventLogEntry != null)
                        gotoClosestElement(foundEventLogEntry.getEvent());
                    else
                        MessageDialog.openInformation(null, "Find raw text", "No more matches found for " + findText);
                }
            }
        }
    }

    /*************************************************************************************
     * OPEN AXIS MODULES
     */

    /**
     * Returns which modules may have axes on the chart.
     * The returned value should not be modified.
     */
    public ArrayList<ModuleTreeItem> getOpenAxisModules() {
        return openAxisModules;
    }

    /**
     * Sets the open axis modules to the given list.
     */
    public void setOpenAxisModules(ArrayList<ModuleTreeItem> moduleTreeItems) {
        openAxisModules = new ArrayList<ModuleTreeItem>(moduleTreeItems);
        invalidateVisibleAxisModules();
    }

    /**
     * Clears the list of open axis modules.
     */
    public void clearOpenAxisModules() {
        openAxisModules.clear();
        invalidateVisibleAxisModules();
    }

    /**
     * Adds a module to the list of open axis modules.
     */
    public void addOpenAxisModule(ModuleTreeItem moduleTreeItem) {
        openAxisModules.add(moduleTreeItem);
        invalidateVisibleAxisModules();
    }

    /**
     * Removes a module from the list of open axis modules.
     */
    public void removeOpenAxisModule(ModuleTreeItem moduleTreeItem) {
        openAxisModules.remove(moduleTreeItem);
        invalidateVisibleAxisModules();
    }

    /**
     * Opens the given module one level deep. Sets the open axis modules to contain
     * the compound module and all its submodules.
     */
    public void openModule(ModuleTreeItem moduleTreeItem) {
        ArrayList<ModuleTreeItem> axisModules = new ArrayList<ModuleTreeItem>();
        axisModules.add(moduleTreeItem);
        for (ModuleTreeItem submoduleTreeItem : moduleTreeItem.getSubmodules())
            axisModules.add(submoduleTreeItem);
        setOpenAxisModules(axisModules);
    }

    /**
     * Opens the given module recursively. Sets the open axis modules to contain
     * the compound module and all its submodules recursively.
     */
    public void openModuleRecursively(ModuleTreeItem moduleTreeItem) {
        ArrayList<ModuleTreeItem> axisModules = new ArrayList<ModuleTreeItem>();
        addAllSubmodules(axisModules, moduleTreeItem);
        setOpenAxisModules(axisModules);
    }

    /**
     * Opens all modules that are matched by the current eventlog filter.
     */
    public void openFilteredModules() {
        ArrayList<ModuleTreeItem> collectedAxisModules = new ArrayList<ModuleTreeItem>();
        eventLogInput.getModuleTreeRoot().visitItems(new ModuleTreeItem.IModuleTreeItemVisitor() {
            public void visit(ModuleTreeItem moduleTreeItem) {
                if (isSelectedAxisModule(moduleTreeItem) && !collectedAxisModules.contains(moduleTreeItem))
                    collectedAxisModules.add(moduleTreeItem);
            }
        });
        setOpenAxisModules(collectedAxisModules);
    }

    public void expandOpenAxisModule(ModuleTreeItem moduleTreeItem) {
        for (ModuleTreeItem submoduleTreeItem : moduleTreeItem.getSubmodules())
            if (!openAxisModules.contains(submoduleTreeItem))
                openAxisModules.add(submoduleTreeItem);
        setOpenAxisModules(openAxisModules);
    }

    public void collapseOpenAxisModule(ModuleTreeItem moduleTreeItem) {
        for (ModuleTreeItem axisModule : new ArrayList<ModuleTreeItem>(openAxisModules))
            if (axisModule != moduleTreeItem && moduleTreeItem.isDescendantModule(axisModule))
                openAxisModules.remove(axisModule);
        setOpenAxisModules(openAxisModules);
    }

    private void addAllSubmodules(ArrayList<ModuleTreeItem> axisModules, ModuleTreeItem moduleTreeItem) {
        axisModules.add(moduleTreeItem);
        for (ModuleTreeItem childModuleTreeItem : moduleTreeItem.getSubmodules()) {
            if (!childModuleTreeItem.isCompoundModule())
                axisModules.add(childModuleTreeItem);
            else
                addAllSubmodules(axisModules, childModuleTreeItem);
        }
    }

    /**
     * True means the module is selected to have an axis. Even if a module is selected it
     * might still be excluded depending on the showEmptyAxes flag.
     */
    private boolean isSelectedAxisModule(ModuleTreeItem moduleTreeItem) {
        if (eventLog instanceof FilteredEventLog && eventLogInput.getFilterParameters().enableModuleFilter) {
            ModuleCreatedEntry moduleCreatedEntry = eventLog.getModuleCreatedEntry(moduleTreeItem.getModuleId());
            Assert.isTrue(moduleCreatedEntry != null);
            return ((FilteredEventLog)eventLog).matchesModuleCreatedEntry(moduleCreatedEntry);
        }
        else
            // If the module has any kind of custom behavior attached to it (via @class)
            // - whether it is a simple module, or a "tricky" compound module -,
            // it's possible that it "does something"_ calls methods, handles events,
            // sends messages, etc.; so let's include it.
            return moduleTreeItem.hasCustomClass();
    }

    /*************************************************************************************
     * VISIBLE AXIS MODULES
     */

    private ArrayList<ModuleTreeItem> getVisibleAxisModules() {
        validateVisibleAxisModules();
        return visibleAxisModules;
    }

    private void validateVisibleAxisModules() {
        if (invalidVisibleAxisModules) {
            ArrayList<ModuleTreeItem> newVisibleAxisModules = calculateVisibleAxisModules();
            if (!newVisibleAxisModules.equals(visibleAxisModules)) {
                visibleAxisModules = newVisibleAxisModules;
                invalidateAxes();
                invalidateAxisHeaders();
                invalidateAxisSpacing();
                invalidateAxisModulePositions();
                invalidateReverseAxisModulePositions();
                invalidateVirtualSize();
                invalidateModuleIdToAxisModuleIndexMap();
                invalidateModuleIdToAxisRendererMap();
            }
            invalidVisibleAxisModules = false;
            removeInvisibleObjects(selectedObjects);
            removeInvisibleObjects(highlightedObjects);
        }
    }

    private void invalidateVisibleAxisModules() {
        if (debug)
            Debug.println("invalidateVisibleAxisModules(): enter");
        invalidVisibleAxisModules = true;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Collects the modules which may have an axis in the currently visible region.
     * It checks all events and all message dependencies within the visible region
     * but ignores whether the module is selected or not. The result may contain
     * module ids that will not be used in the chart.
     */
    private Set<Integer> collectPotentiallyVisibleModuleIds() {
        Set<Integer> axisModuleIds = new HashSet<Integer>();
        // check potentially visible events
        int extraClipping = getExtraClippingForEvents() + 30; // add an extra for the caching canvas tile cache width (clipping may be negative in paint)
        long[] eventPtrRange = getFirstLastEventPtrForViewportRange(-extraClipping, getViewportWidth() + extraClipping);
        long startEventPtr = eventPtrRange[0];
        long endEventPtr = eventPtrRange[1];
        if (startEventPtr != 0 && endEventPtr != 0) {
            if (debug)
                Debug.println("Collecting axis modules for events using event range: " + sequenceChartFacade.IEvent_getEventNumber(startEventPtr) + " -> " + sequenceChartFacade.IEvent_getEventNumber(endEventPtr));
            for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.IEvent_getNextEvent(eventPtr)) {
                if (!isInitializationEvent(eventPtr))
                    axisModuleIds.add(sequenceChartFacade.IEvent_getModuleId(eventPtr));
                if (eventPtr == endEventPtr)
                    break;
            }
        }
        // check potentially visible message dependencies
        eventPtrRange = getFirstLastEventPtrForMessageDependencies();
        startEventPtr = eventPtrRange[0];
        endEventPtr = eventPtrRange[1];
        if (startEventPtr != 0 && endEventPtr != 0) {
            if (debug)
                Debug.println("Collecting axis modules for message dependencies using event range: " + sequenceChartFacade.IEvent_getEventNumber(startEventPtr) + " -> " + sequenceChartFacade.IEvent_getEventNumber(endEventPtr));
            PtrVector messageDependencies = sequenceChartFacade.getIntersectingMessageDependencies(startEventPtr, endEventPtr);
            for (int i = 0; i < messageDependencies.size(); i++) {
                long messageDependencyPtr = messageDependencies.get(i);
                long causeEventPtr = sequenceChartFacade.IMessageDependency_getCauseEvent(messageDependencyPtr);
                long consequenceEventPtr = sequenceChartFacade.IMessageDependency_getConsequenceEvent(messageDependencyPtr);
                long messageEntryPtr = sequenceChartFacade.IMessageDependency_getMessageEntry(messageDependencyPtr);
                boolean isReuse = sequenceChartFacade.IMessageDependency_isReuse(messageDependencyPtr);
                if (causeEventPtr != 0) {
                    if (isInitializationEvent(causeEventPtr)) {
                        if (showInitializationEvent)
                            axisModuleIds.add(sequenceChartFacade.EventLogEntry_getContextModuleId(messageEntryPtr));
                    }
                    else
                        axisModuleIds.add(!isReuse && messageEntryPtr != 0 ? sequenceChartFacade.EventLogEntry_getContextModuleId(messageEntryPtr) : sequenceChartFacade.IEvent_getModuleId(causeEventPtr));
                }
                if (consequenceEventPtr != 0)
                    axisModuleIds.add(isReuse && messageEntryPtr != 0 ? sequenceChartFacade.EventLogEntry_getContextModuleId(messageEntryPtr) : sequenceChartFacade.IEvent_getModuleId(consequenceEventPtr));
            }
        }
        // check potentially visible module method calls
        if (showComponentMethodCalls) {
            extraClipping = getExtraClippingForEvents();
            eventPtrRange = getFirstLastEventPtrForViewportRange(-extraClipping, getViewportWidth() + extraClipping);
            startEventPtr = eventPtrRange[0];
            endEventPtr = eventPtrRange[1];
            if (startEventPtr != 0 && endEventPtr != 0) {
                if (debug)
                    Debug.println("Collecting axis modules for module method calls using event range: " + sequenceChartFacade.IEvent_getEventNumber(startEventPtr) + " -> " + sequenceChartFacade.IEvent_getEventNumber(endEventPtr));
                PtrVector componentMethodBeginEntries = sequenceChartFacade.getComponentMethodBeginEntries(startEventPtr, endEventPtr);
                for (int i = 0; i < componentMethodBeginEntries.size(); i++) {
                    long componentMethodCallPtr = componentMethodBeginEntries.get(i);
                    if (isInitializationEvent(sequenceChartFacade.EventLogEntry_getEvent(componentMethodCallPtr)) && !showInitializationEvent)
                        continue;
                    // TODO: source/target component is not necessarily a module, it may be a channel as well
                    axisModuleIds.add(sequenceChartFacade.ComponentMethodBeginEntry_getSourceComponentId(componentMethodCallPtr));
                    axisModuleIds.add(sequenceChartFacade.ComponentMethodBeginEntry_getTargetComponentId(componentMethodCallPtr));
                }
            }
        }
        if (debug)
            Debug.println("Module ids that will potentially have axes: " + axisModuleIds);
        return axisModuleIds;
    }

    private ArrayList<ModuleTreeItem> calculateVisibleAxisModules() {
        if (debug)
            Debug.println("calculateVisibleAxisModules(): enter");
        long startMillis = System.currentTimeMillis();
        ArrayList<ModuleTreeItem> result = new ArrayList<ModuleTreeItem>();
        if (showEmptyAxes) {
            result.clear();
            result.addAll(getOpenAxisModules());
        }
        else {
            for (int moduleId : collectPotentiallyVisibleModuleIds()) {
                ModuleTreeItem moduleTreeRoot = eventLogInput.getModuleTreeRoot();
                ModuleTreeItem moduleTreeItem = moduleTreeRoot.findDescendantModule(moduleId);

                // create module tree item on demand
                if (moduleTreeItem == null) {
                    ModuleCreatedEntry entry = eventLog.getModuleCreatedEntry(moduleId);

                    if (entry != null)
                        moduleTreeItem = moduleTreeRoot.addDescendantModule(entry.getParentModuleId(), entry.getModuleId(), entry.getNedTypeName(), entry.getModuleClassName(), entry.getFullName(), entry.getCompoundModule());
                    else
                        // FIXME: this is not correct and will not be replaced automagically when the ModuleCreatedEntry is found later on
                        moduleTreeItem = new ModuleTreeItem(moduleId, "<unknown>", "<unknown>", "<unknown>", moduleTreeRoot, false);
                }

                // find the open module axis and add it
                while (moduleTreeItem != null) {
                    if (getOpenAxisModules().contains(moduleTreeItem)) {
                        if (!result.contains(moduleTreeItem)) {
                            if (debug)
                                Debug.println("Adding module axis " + moduleTreeItem.getModuleFullPath() + " because of " + moduleTreeRoot.findDescendantModule(moduleId).getModuleFullPath());
                            result.add(moduleTreeItem);
                        }
                        break;
                    }

                    moduleTreeItem = moduleTreeItem.getParentModule();
                }
            }
        }

        long totalMillis = System.currentTimeMillis() - startMillis;
        if (debug)
            Debug.println("calculateVisibleAxisModules(): leave after " + totalMillis + "ms");
        return result;
    }

    /*************************************************************************************
     * AXES
     */

    public ArrayList<Axis> getAxes() {
        validateAxes();
        return axes;
    }

    public Axis getAxis(int index) {
        return getAxes().get(getAxisModulePositions()[index]);
    }

    private void validateAxes() {
        validateAxisHeaders();
        validateAxisSpacing();
        validateModuleIdToAxisRendererMap();
        if (invalidAxes) {
            calculateAxes();
            layoutAxes();
            invalidAxes = false;
        }
    }

    private void invalidateAxes() {
        if (debug)
            Debug.println("invalidateAxes(): enter");
        invalidAxes = true;
        clearCanvasCacheAndRedraw();
    }

    private void calculateAxes() {
        axes.clear();
        if (getRootAxisHeader() != null)
            calculateAxes(getRootAxisHeader());
    }

    private void calculateAxes(AxisHeader axisHeader) {
        if (axisHeader.children.size() == 0) {
            Axis axis = new Axis();
            axis.axisHeader = axisHeader;
            axis.axisRenderer = getAxisRenderer(axis.axisHeader.module);
            axes.add(axis);
        }
        for (AxisHeader childAxisHeader : axisHeader.children)
            calculateAxes(childAxisHeader);
    }

    private void layoutAxes() {
        GC gc = new GC(this);
        double y = 0;
        for (Axis axis : axes) {
            axis.y = (int)Math.round((styleProvider.getAxisOffset() + getAxisSpacing() + y));
            layoutAxis(gc, axis);
            y += getAxisSpacing() + axis.axisRenderer.getHeight();
        }
        gc.dispose();
    }

    private void layoutAxis(GC gc, Axis axis) {
        AxisHeader axisHeader = axis.axisHeader;
        int y = axis.y;
        Point labelSize = gc.textExtent(axisHeader.modulePathFragment);
        int boxWidth = labelSize.y + 4;
        int offset = showAxisHeaders ? axisHeader.level * boxWidth : 0;
        if (axisHeader.module.isCompoundModule()) {
            if (!axisHeader.allSubmodulesOpen) {
                Image image = labelProvider.getExpandImage();
                org.eclipse.swt.graphics.Rectangle bounds = image.getBounds();
                axis.expandImageBounds = new Rectangle(offset + 2, y - labelSize.y / 2 - bounds.height / 2, bounds.width, bounds.height);
                offset += bounds.width;
            }
            if (!axisHeader.noSubmodulesOpen) {
                Image image = labelProvider.getCollapseImage();
                org.eclipse.swt.graphics.Rectangle bounds = image.getBounds();
                axis.collapseImageBounds = new Rectangle(offset + 2, y - labelSize.y / 2 - bounds.height / 2, bounds.width, bounds.height);
                offset += bounds.width;
            }
        }
        axis.labelElementBounds = new Rectangle[axis.axisHeader.labelElementBounds.length];
        for (int i = 0; i < axis.labelElementBounds.length; i++) {
            int dx = axisHeader.labelBounds.bottom() - axisHeader.labelElementBounds[i].bottom();
            axis.labelElementBounds[i] = new Rectangle(offset + 2 + dx, y - labelSize.y - 2, axisHeader.labelElementBounds[i].height, axisHeader.labelElementBounds[i].width);
        }
        offset += labelSize.x;
        Image image = labelProvider.getCloseImage();
        org.eclipse.swt.graphics.Rectangle bounds = image.getBounds();
        axis.closeImageBounds = new Rectangle(offset + 2, y - labelSize.y / 2 - bounds.height / 2 - 1, bounds.width, bounds.height);
    }

    /*************************************************************************************
     * AXIS HEADERS
     */

    public AxisHeader getRootAxisHeader() {
        validateAxisHeaders();
        return rootAxisHeader;
    }

    private void validateAxisHeaders() {
        validateVisibleAxisModules();
        validateAxisModulePositions();
        if (invalidAxisHeaders) {
            calculateAxisHeaders();
            layoutAxisHeaders();
            invalidAxisHeaders = false;
        }
    }

    private void invalidateAxisHeaders() {
        if (debug)
            Debug.println("invalidateAxisHeaders(): enter");
        invalidAxisHeaders = true;
        clearCanvasCacheAndRedraw();
    }

    private void calculateAxisHeaders() {
        if (getVisibleAxisModules().size() == 0)
            rootAxisHeader = null;
        else {
            rootAxisHeader = new AxisHeader();
            rootAxisHeader.axisIndex = 0;
            rootAxisHeader.axisCount = getVisibleAxisModules().size();
            rootAxisHeader.modulePathFragment = "";
            for (int i = 0; i < getVisibleAxisModules().size(); i++) {
                ModuleTreeItem moduleTreeItem = getVisibleAxisModules().get(getReverseAxisModulePositions()[i]);
                AxisHeader axisHeader = new AxisHeader();
                axisHeader.axisIndex = i;
                axisHeader.axisCount = 1;
                axisHeader.modulePathFragment = moduleTreeItem.getModuleFullPath();
                axisHeader.module = moduleTreeItem;
                rootAxisHeader.children.add(axisHeader);
            }
            mergeSiblingAxisHeaders(rootAxisHeader);
            mergeDescendantAxisHeaders(rootAxisHeader);
        }
    }

    private void mergeSiblingAxisHeaders(AxisHeader axisHeader) {
        if (axisHeader.children.size() < 2)
            return;
        String commonPrefix = null;
        ArrayList<AxisHeader> commonAxisHeaders = new ArrayList<AxisHeader>();
        ArrayList<AxisHeader> mergedAxisHeaders = new ArrayList<AxisHeader>();
        for (int i = 0; i < axisHeader.children.size(); i++) {
            AxisHeader childAxisHeader = axisHeader.children.get(i);
            String childPrefix = null;
            int j = childAxisHeader.modulePathFragment.indexOf('.');
            if (j == -1)
                childPrefix = childAxisHeader.modulePathFragment;
            else
                childPrefix = childAxisHeader.modulePathFragment.substring(0, j);
            if (commonPrefix == null) 
                commonPrefix = childPrefix;
            if (commonPrefix.equals(childPrefix))
                commonAxisHeaders.add(childAxisHeader);
            else {
                addMergedAxisHeaders(axisHeader.module, commonAxisHeaders, commonPrefix, mergedAxisHeaders);
                commonPrefix = childPrefix;
                commonAxisHeaders.clear();
                commonAxisHeaders.add(childAxisHeader);
            }
            if (i == axisHeader.children.size() - 1)
                addMergedAxisHeaders(axisHeader.module, commonAxisHeaders, commonPrefix, mergedAxisHeaders);
        }
        axisHeader.children.clear();
        axisHeader.children.addAll(mergedAxisHeaders);
        for (AxisHeader childrenAxisHeader : axisHeader.children)
            mergeSiblingAxisHeaders(childrenAxisHeader);
    }

    private void addMergedAxisHeaders(ModuleTreeItem moduleTreeItem, ArrayList<AxisHeader> commonAxisHeaders, String commonPrefix, ArrayList<AxisHeader> mergedAxisHeaders) {
        if (commonAxisHeaders.size() == 1)
            mergedAxisHeaders.add(commonAxisHeaders.get(0));
        else {
            AxisHeader mergedAxisHeader = new AxisHeader();
            mergedAxisHeader.axisIndex = commonAxisHeaders.get(0).axisIndex;
            mergedAxisHeader.modulePathFragment = commonPrefix;
            String fullPath = moduleTreeItem == null ? commonPrefix : moduleTreeItem.getModuleFullPath() + "." + commonPrefix;
            mergedAxisHeader.module = eventLogInput.getModuleTreeRoot().findDescendantModule(fullPath);
            int axisCount = 0;
            for (AxisHeader commonAxisHeader : commonAxisHeaders) {
                axisCount += commonAxisHeader.axisCount;
                int index = commonAxisHeader.modulePathFragment.indexOf('.');
                commonAxisHeader.modulePathFragment = index != -1 ? commonAxisHeader.modulePathFragment.substring(index + 1) : "";
                mergedAxisHeader.children.add(commonAxisHeader);
            }
            mergedAxisHeader.axisCount = axisCount;
            mergedAxisHeaders.add(mergedAxisHeader);
        }
    }

    private void mergeDescendantAxisHeaders(AxisHeader axisHeader) {
        if (axisHeader.children.size() == 1) {
            AxisHeader childAxisHeader = axisHeader.children.get(0);
            if (axisHeader.modulePathFragment.equals(""))
                axisHeader.modulePathFragment = childAxisHeader.modulePathFragment;
            else
                axisHeader.modulePathFragment += "." + childAxisHeader.modulePathFragment;
            axisHeader.module = childAxisHeader.module;
            axisHeader.children = childAxisHeader.children;
            mergeDescendantAxisHeaders(axisHeader);
        }
        else
            for (AxisHeader childAxisHeader : axisHeader.children)
                mergeDescendantAxisHeaders(childAxisHeader);
    }

    private void layoutAxisHeaders() {
        if (rootAxisHeader != null) {
            GC gc = new GC(this);
            layoutAxisHeaders(gc, rootAxisHeader, null, 0, styleProvider.getAxisOffset());
            gc.dispose();
        }
    }

    private void layoutAxisHeaders(GC gc, AxisHeader axisHeader, AxisHeader parentAxisHeader, int level, double offset) {
        layoutAxisHeader(gc, axisHeader, parentAxisHeader, level, offset);
        for (AxisHeader childAxisHeader : axisHeader.children) {
            layoutAxisHeaders(gc, childAxisHeader, axisHeader, level + 1, offset);
            offset += getAxisHeaderHeight(childAxisHeader);
        }
    }

    private void layoutAxisHeader(GC gc, AxisHeader axisHeader, AxisHeader parentAxisHeader, int level, double offset) {
        axisHeader.level = level;
        axisHeader.allSubmodulesOpen = true;
        axisHeader.noSubmodulesOpen = true;
        for (ModuleTreeItem childModuleTreeItem : axisHeader.module.getSubmodules()) {
            if (!getOpenAxisModules().contains(childModuleTreeItem))
                axisHeader.allSubmodulesOpen = false;
            else
                axisHeader.noSubmodulesOpen = false;
        }
        Point labelSize = gc.textExtent(axisHeader.modulePathFragment);
        int width = labelSize.y + 4;
        double height = getAxisHeaderHeight(axisHeader) - 4;
        int x = axisHeader.level * width;
        double y = offset + 3;
        offset = y + (height + labelSize.x) / 2;
        axisHeader.bounds = new Rectangle(x, (int)y, width, (int)height);
        axisHeader.labelBounds = new Rectangle(x + 2, (int)Math.round(y + (height - labelSize.x) / 2), labelSize.y, labelSize.x);
        if (axisHeader.module.isCompoundModule()) {
            if (!axisHeader.noSubmodulesOpen) {
                Image image = labelProvider.getCollapseImage();
                org.eclipse.swt.graphics.Rectangle bounds = image.getBounds();
                axisHeader.collapseImageBounds = new Rectangle(x + 3 + (labelSize.y - bounds.width) / 2, (int)offset + 2, bounds.width, bounds.height);
            }
            if (!axisHeader.allSubmodulesOpen) {
                Image image = labelProvider.getExpandImage();
                org.eclipse.swt.graphics.Rectangle bounds = image.getBounds();
                axisHeader.expandImageBounds = new Rectangle(x + 3 + (labelSize.y - bounds.width) / 2, (int)offset + 2 + (!axisHeader.noSubmodulesOpen ? bounds.width + 2 : 0), bounds.width, bounds.height);
            }
        }
        String[] modulePathElements = axisHeader.modulePathFragment.split("\\.");
        axisHeader.labelElementBounds = new Rectangle[modulePathElements.length];
        axisHeader.labelElementModules = new ModuleTreeItem[modulePathElements.length];
        Point dotSize = gc.textExtent(".");
        String fullPath = parentAxisHeader == null || parentAxisHeader.module == null ? null : parentAxisHeader.module.getModuleFullPath();
        for (int i = 0; i < modulePathElements.length; i++) {
            fullPath = fullPath == null ? modulePathElements[i] : fullPath + "." + modulePathElements[i];
            Point size = gc.textExtent(modulePathElements[i]);
            axisHeader.labelElementBounds[i] = new Rectangle(x + 2, (int)Math.round(offset) - size.x, size.y, size.x);
            axisHeader.labelElementModules[i] = eventLogInput.getModuleTreeRoot().findDescendantModule(fullPath);
            offset -= size.x;
            offset -= dotSize.x;
        }
        Image image = labelProvider.getCloseImage();
        org.eclipse.swt.graphics.Rectangle bounds = image.getBounds();
        axisHeader.closeImageBounds = new Rectangle(x + 3 + (labelSize.y - bounds.width) / 2, (int)offset - bounds.height, bounds.width, bounds.height);
    }

    /*************************************************************************************
     * MODULE ID TO AXIS RENDERER MAP
     */

    /**
     * Returns the map from module ids to axis renderers, lazily updates the map if necessary.
     */
    public Map<Integer, IAxisRenderer> getModuleIdToAxisRendererMap() {
        validateModuleIdToAxisRendererMap();
        return moduleIdToAxisRendererMap;
    }

    private void validateModuleIdToAxisRendererMap() {
        validateVisibleAxisModules();
        if (invalidModuleIdToAxisRendererMap) {
            calculateModuleIdToAxisRendererMap();
            invalidModuleIdToAxisRendererMap = false;
        }
    }
    /**
     * Returns the axis renderer associated with the module.
     */
    public IAxisRenderer getAxisRenderer(ModuleTreeItem axisModule) {
        return getModuleIdToAxisRendererMap().get(axisModule.getModuleId());
    }

    /**
     * Associates the axis renderer with the given axis module.
     */
    public void setAxisRenderer(ModuleTreeItem axisModule, IAxisRenderer axisRenderer) {
        moduleIdToAxisRendererMap.put(axisModule.getModuleId(), axisRenderer);
        int index = getVisibleAxisModules().indexOf(axisModule);
        if (index != -1) {
            getAxis(index).axisRenderer = axisRenderer;
            invalidateAxisSpacing();
        }
    }

    private void invalidateModuleIdToAxisModuleIndexMap() {
        if (debug)
            Debug.println("invalidateModuleIdToAxisModuleIndexMap(): enter");
        invalidModuleIdToAxisModuleIndexMap = true;
        clearCanvasCacheAndRedraw();
    }

    private void calculateModuleIdToAxisRendererMap() {
        ResultFileManagerEx resultFileManager = new ResultFileManagerEx();
        for (ModuleTreeItem axisModule : getVisibleAxisModules()) {
            IAxisRenderer axisRenderer = moduleIdToAxisRendererMap.get(axisModule.getModuleId());
            if (axisRenderer == null) {
                IProject project = eventLogInput.getFile().getProject();
                String typeName = axisModule.getNedTypeName();
                INedTypeInfo typeInfo = NedResources.getInstance().getToplevelNedType(typeName, project);
                if (typeInfo != null) {
                    PropertyElementEx property = typeInfo.getProperty("defaultStatistic", null);
                    if (property != null) {
                        String vectorName = property.getSimpleValue();
                        IFile inputFile = eventLogInput.getFile();
                        String inputFileName = inputFile.getName();
                        IFile vectorFile = inputFile.getParent().getFile(new Path(inputFileName.substring(0, inputFileName.indexOf(".")) + ".vec"));
                        if (vectorFile.exists()) {
                            ResultFile resultFile = resultFileManager.loadFile(vectorFile.getName(), vectorFile.getLocation().toOSString(), 0, null);
                            String eventlogRunName = getEventLog().getSimulationBeginEntry().getRunId();
                            FileRun fileRun = resultFileManager.getFileRun(resultFile, resultFileManager.getRunByName(eventlogRunName));
                            long id = resultFileManager.getItemByName(fileRun, axisModule.getModuleFullPath(), vectorName);
                            // TODO: compare vector's run against log file's run
                            if (id != 0) {
                                ResultItem resultItem = resultFileManager.getItem(id);
                                IDList selectedIdList = new IDList(id);
                                XYArrayVector dataVector = ScaveEngine.readVectorsIntoArrays2(resultFileManager, selectedIdList, true, true);
                                axisRenderer = new AxisVectorBarRenderer(this, vectorFile.getLocation().toOSString(), eventlogRunName, axisModule.getModuleFullPath(), vectorName, resultItem, dataVector, 0);
                            }
                        }
                    }
                }
                if (axisRenderer == null)
                    axisRenderer = new AxisLineRenderer(this, axisModule);
                else
                    axisRenderer = new AxisMultiRenderer(new IAxisRenderer[] {new AxisLineRenderer(this, axisModule), axisRenderer}, 1);
                moduleIdToAxisRendererMap.put(axisModule.getModuleId(), axisRenderer);
            }
        }
    }

    /*************************************************************************************
     * AXIS MODULE POSITIONS
     */

    /**
     * Returns the axis module positions as an array, lazily updates the array if necessary.
     * The nth value specifies the position of the nth axis module.
     */
    public int[] getAxisModulePositions() {
        validateAxisModulePositions();
        return axisModulePositions;
    }

    private void validateAxisModulePositions() {
        validateVisibleAxisModules();
        validateAxisSpacing();
        if (invalidAxisModulePositions) {
            int[] newAxisModulePositions = calculateAxisModulePositions();
            if (!newAxisModulePositions.equals(axisModulePositions)) {
                axisModulePositions = newAxisModulePositions;
                invalidateAxes();
                invalidReverseAxisModulePositions = true;
            }
            invalidAxisModulePositions = false;
        }
    }

    private void invalidateAxisModulePositions() {
        if (debug)
            Debug.println("invalidateAxisModulePositions(): enter");
        invalidAxisModulePositions = true;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Sorts axis modules depending on timeline ordering mode.
     */
    private int[] calculateAxisModulePositions() {
        Object[] result = new Object[1];
        eventLogInput.runWithProgressMonitor(new Runnable() {
            public void run() {
                ModuleTreeItem[] axisModulesArray = getVisibleAxisModules().toArray(new ModuleTreeItem[0]);
                switch (axisOrderingMode) {
                    case MANUAL:
                        result[0] = manualAxisOrder.calculateOrdering(axisModulesArray);
                        break;
                    case MODULE_ID:
                        result[0] = new AxisOrderByModuleId().calculateOrdering(axisModulesArray);
                        break;
                    case MODULE_FULL_PATH:
                        result[0] = new AxisOrderByModuleName().calculateOrdering(axisModulesArray);
                        break;
                    case MINIMIZE_CROSSINGS:
                        int extraClipping = getExtraClippingForEvents();
                        long[] eventPtrRange = getFirstLastEventPtrForViewportRange(Rectangle.SINGLETON.x - extraClipping, Rectangle.SINGLETON.right() + extraClipping);
                        long startEventPtr = eventPtrRange[0];
                        long endEventPtr = eventPtrRange[1];
                        axisModulesArray = manualAxisOrder.getCurrentAxisModuleOrder(axisModulesArray).toArray(new ModuleTreeItem[0]);
                        result[0] = new FlatAxisOrderByMinimizingCost(eventLogInput, startEventPtr, endEventPtr).calculateOrdering(axisModulesArray, getModuleIdToAxisModuleIndexMap());
                        break;
                    default:
                        throw new RuntimeException("Unknown axis ordering mode");
                }
            }
        });
        return (int[])result[0];
    }

    public int[] getReverseAxisModulePositions() {
        validateReverseAxisModulePositions();
        return reverseAxisModulePositions;
    }

    private void invalidateReverseAxisModulePositions() {
        if (debug)
            Debug.println("invalidateReverseAxisModulePositions(): enter");
        invalidReverseAxisModulePositions = true;
        clearCanvasCacheAndRedraw();
    }

    private void validateReverseAxisModulePositions() {
        validateVisibleAxisModules();
        if (invalidReverseAxisModulePositions) {
            reverseAxisModulePositions = calculateReverseAxisModulePositions();
            invalidReverseAxisModulePositions = false;
        }
    }

    private int[] calculateReverseAxisModulePositions() {
        int[] result = new int[getAxisModulePositions().length];
        for (int i = 0; i < getAxisModulePositions().length; i++)
            result[getAxisModulePositions()[i]] = i;
        return result;
    }

    /*************************************************************************************
     * MODULE ID TO AXIS MODULE INDEX MAP
     */

    /**
     * Returns a map from module id to axis module index, lazily updates the map if necessary.
     */
    public Map<Integer, Integer> getModuleIdToAxisModuleIndexMap() {
        validateModuleIdToAxisModuleIndexMap();
        return moduleIdToAxisModuleIndexMap;
    }


    private void invalidateModuleIdToAxisRendererMap() {
        if (debug)
            Debug.println("invalidateModuleIdToAxisRendererMap(): enter");
        invalidModuleIdToAxisRendererMap = true;
        clearCanvasCacheAndRedraw();
    }

    private void validateModuleIdToAxisModuleIndexMap() {
        validateVisibleAxisModules();
        if (invalidModuleIdToAxisModuleIndexMap) {
            calculateModuleIdToAxisModuleIndexMap();
            invalidModuleIdToAxisModuleIndexMap = false;
        }
    }

    /**
     * Calculates axis indices for all modules by finding an axis module ancestor.
     */
    private void calculateModuleIdToAxisModuleIndexMap() {
        if (debug)
            Debug.println("calculateModuleIdToAxisModuleIndexMap()");

        moduleIdToAxisModuleIndexMap = new HashMap<Integer, Integer>();

        // this algorithm allows to have two module axes on the chart
        // which are in ancestor-descendant relationship,
        // and allows events still be drawn on the most specific axis
        eventLogInput.getModuleTreeRoot().visitItems(new ModuleTreeItem.IModuleTreeItemVisitor() {
            public void visit(ModuleTreeItem descendantAxisModule) {
                ModuleTreeItem axisModule = descendantAxisModule;

                while (axisModule != null) {
                    int index = getVisibleAxisModules().indexOf(axisModule);

                    if (index != -1) {
                        moduleIdToAxisModuleIndexMap.put(descendantAxisModule.getModuleId(), index);
                        return;
                    }

                    axisModule = axisModule.getParentModule();
                }
            }
        });
    }

    /*************************************************************************************
     * VIRTUAL SIZE
     */

    @Override
    public long getVirtualWidth() {
        validateVirtualSize();
        return super.getVirtualWidth();
    }

    @Override
    public long getVirtualHeight() {
        validateVirtualSize();
        return super.getVirtualHeight();
    }

    private void invalidateVirtualSize() {
        if (debug)
            Debug.println("invalidateVirtualSize(): enter");
        invalidVirtualSize = true;
        clearCanvasCacheAndRedraw();
    }

    private void validateVirtualSize() {
        validateVisibleAxisModules();
        validateAxisSpacing();
        if (invalidVirtualSize) {
            calculateVirtualSize();
            invalidVirtualSize = false;
        }
    }

    /**
     * Calculates virtual size of the canvas. The width is an approximation while height is precise.
     */
    private void calculateVirtualSize() {
        int height = getTotalAxesHeight() + (int)Math.round(getVisibleAxisModules().size() * getAxisSpacing()) + styleProvider.getAxisOffset() * 2;
        setVirtualSize(getViewportWidth() * eventLog.getApproximateNumberOfEvents(), height);
    }

    /**
     * Sums up the height of all axes.
     */
    private int getTotalAxesHeight() {
        int height = 0;
        for (ModuleTreeItem moduleTreeItem : getVisibleAxisModules())
            height += getModuleIdToAxisRendererMap().get(moduleTreeItem.getModuleId()).getHeight();
        return height;
    }

    /*************************************************************************************
     * VIEWPORT SIZE
     */

    private void validateViewportSize(Graphics graphics) {
        if (invalidViewportSize) {
            calculateViewportSize(graphics);
            invalidateAxisSpacing();
            invalidViewportSize = false;
        }
    }

    private void calculateViewportSize(Graphics graphics) {
        Rectangle r = new Rectangle(getClientArea());
        setViewportRectangle(new Rectangle(r.x, r.y + getGutterHeight(graphics), r.width, r.height - getGutterHeight(graphics) * 2));
    }

    private void invalidateViewportSize() {
        if (debug)
            Debug.println("invalidateViewportSize(): enter");
        invalidViewportSize = true;
        clearCanvasCacheAndRedraw();
    }

    /*************************************************************************************
     * AXIS SPACING
     */

    /**
     * Returns the pixel distance between adjacent axes on the chart.
     */
    public double getAxisSpacing() {
        validateAxisSpacing();
        return axisSpacing;
    }

    private void validateAxisSpacing() {
        validateVisibleAxisModules();
        if (invalidAxisSpacing) {
            calculateAxisSpacing();
            invalidateAxes();
            invalidateAxisHeaders();
            invalidateAxisModulePositions();
            invalidateVirtualSize();
            invalidAxisSpacing = false;
        }
    }

    /**
     * Sets the pixel distance between adjacent axes on the chart.
     */
    public void setAxisSpacing(double axisSpacing) {
        this.axisSpacingMode = AxisSpacingMode.MANUAL;
        this.axisSpacing = Math.max(1, axisSpacing);
        invalidateAxisSpacing();
    }

    /**
     * Returns the current axis spacing mode.
     */
    public AxisSpacingMode getAxisSpacingMode() {
        return axisSpacingMode;
    }

    /**
     * Sets the axis spacing mode either to manual or auto.
     */
    public void setAxisSpacingMode(AxisSpacingMode axisSpacingMode) {
        this.axisSpacingMode = axisSpacingMode;
        invalidateAxisSpacing();
    }

    private void invalidateAxisSpacing() {
        if (debug)
            Debug.println("invalidateAxisSpacing(): enter");
        invalidAxisSpacing = true;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Distributes available window space among axes evenly if auto axis spacing mode is turned on.
     */
    private void calculateAxisSpacing() {
        if (axisSpacingMode == AxisSpacingMode.AUTO) {
            if (getVisibleAxisModules().size() == 0)
                axisSpacing = 1;
            else
                axisSpacing = Math.max(getFontHeight(null) + 1, (double)(getViewportHeight() - styleProvider.getAxisOffset() * 2 - getTotalAxesHeight()) / getVisibleAxisModules().size());
        }
    }

    /*************************************************************************************
     * HEIGHTS
     */

    public int getGutterHeight(Graphics graphics) {
        return getFontHeight(graphics) + 2;
    }

    public int getFontHeight(Graphics graphics) {
        if (fontHeight == -1) {
            if (graphics != null) {
                graphics.setFont(getFont());
                fontHeight = graphics.getFontMetrics().getHeight();
            }
            else
                // NOTE: do not cache this value, because it might be different from the one returned by graphics
                return getFont().getFontData()[0].getHeight() * getFont().getDevice().getDPI().y / 72;
        }

        return fontHeight;
    }

    /*************************************************************************************
     * PIXEL PER TIMELINE UNIT
     */

    /**
     * Returns chart scale, that is, the number of pixels a "timeline unit" maps to.
     *
     * The meaning of "timeline unit" depends on the timeline mode (see enum TimelineMode).
     */
    public double getPixelPerTimelineUnit() {
        if (pixelPerTimelineUnit == 0)
            setDefaultPixelPerTimelineUnit(getViewportWidth());

        return pixelPerTimelineUnit;
    }

    /**
     * Set chart scale (number of pixels a "timeline unit" maps to).
     */
    public void setPixelPerTimelineUnit(double pixelPerTimelineUnit) {
        if (this.pixelPerTimelineUnit != pixelPerTimelineUnit) {
            Assert.isTrue(pixelPerTimelineUnit > 0 && !Double.isInfinite(pixelPerTimelineUnit) && !Double.isNaN(pixelPerTimelineUnit));
            this.pixelPerTimelineUnit = pixelPerTimelineUnit;
            invalidateVisibleAxisModules();
        }
    }

    /**
     * Sets default pixelPerTimelineUnit so that the default number of events fit into the screen.
     */
    private void setDefaultPixelPerTimelineUnit(int viewportWidth) {
        if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() != -1) {
            IEvent referenceEvent = sequenceChartFacade.getTimelineCoordinateSystemOriginEvent();
            IEvent neighbourEvent = referenceEvent;

            // the idea is to find two events with different timeline coordinates at most 20 events from each other
            // and focus to the range of those events
            int distance = 20;
            while (--distance > 0 || sequenceChartFacade.getTimelineCoordinateBegin(referenceEvent) == sequenceChartFacade.getTimelineCoordinateBegin(neighbourEvent)) {
                IEvent newNeighbourEvent = neighbourEvent.getNextEvent();

                if (newNeighbourEvent != null)
                    neighbourEvent = newNeighbourEvent;
                else {
                    IEvent newReferenceEvent = referenceEvent.getPreviousEvent();

                    if (newReferenceEvent != null)
                        referenceEvent = newReferenceEvent;
                    else
                        break;
                }
            }

            if (referenceEvent.getEventNumber() != neighbourEvent.getEventNumber()) {
                double referenceEventTimelineCoordinate = sequenceChartFacade.getTimelineCoordinateBegin(referenceEvent);
                double neighbourEventTimelineCoordinate = sequenceChartFacade.getTimelineCoordinateBegin(neighbourEvent);
                double timelineCoordinateDelta = Math.abs(neighbourEventTimelineCoordinate - referenceEventTimelineCoordinate);

                if (timelineCoordinateDelta == 0)
                    setPixelPerTimelineUnit(1);
                else
                    setPixelPerTimelineUnit(viewportWidth / timelineCoordinateDelta);
            }
            else
                setPixelPerTimelineUnit(1);
        }
    }

    /*************************************************************************************
     * PAINTING
     */

    @Override
    protected void paint(final GC gc) {
        paintHasBeenFinished = false;
        if (internalErrorHappenedDuringPaint)
            drawNotificationMessage(gc, "Internal error happened during painting. Try to reset zoom, position, filter, etc. and press refresh. Sorry for your inconvenience.\n\n" + internalError.getMessage());
        else if (eventLogInput == null) {
            super.paint(gc);
            paintHasBeenFinished = true;
        }
        else if (eventLogInput.isCanceled())
            drawNotificationMessage(gc,
                "Processing of a long running eventlog operation was cancelled, therefore the chart is incomplete and cannot be drawn.\n" +
                "Either try changing some parameters or select refresh. Sorry for your inconvenience.");
        else if (eventLogInput.isLongRunningOperationInProgress())
            drawNotificationMessage(gc, "Processing a long running eventlog operation. Please wait.");
        else {
            try {
                eventLogInput.runWithProgressMonitor(new Runnable() {
                    public void run() {
                        try {
                            SequenceChart.super.paint(gc);
                            paintHasBeenFinished = true;
                            if (debug && eventLogInput != null)
                                Debug.println("Read " + eventLog.getFileReader().getNumReadBytes() + " bytes, " + eventLog.getFileReader().getNumReadLines() + " lines, " + eventLog.getNumParsedEvents() + " events from " + eventLogInput.getFile().getName());
                        }
                        catch (RuntimeException e) {
                            if (eventLogInput.isFileChangedException(e))
                                eventLogInput.synchronize(e);
                            else
                                throw e;
                        }
                    }
                });
            }
            catch (RuntimeException e) {
                SequenceChartPlugin.logError("Internal error happened during painting", e);
                internalError = e;
                internalErrorHappenedDuringPaint = true;
            }
        }
    }

    // KLUDGE: this job is here to workaround a GTK3 bug that redraws the area under the overlay scrollbar incorrectly
    // when either the horizontal or the vertical overlay scrollbars fade out, the underlying area is painted white
    // completely ignoring anything that has been painted by the widget for the area under the scrollbars
    //
    // NOTE: actually this bug can be reproduced by simply subclassing Canvas, setting one of the scrollbars invisible and
    // filling the whole clipping area with a color, when the other scrollbar fades out, the underlying area will be painted white
    Job overlayScrollBarAreaRedrawKludgeJob = new Job("KLUDGE") {
        @Override
        protected IStatus run(IProgressMonitor monitor) {
            Display.getDefault().asyncExec(new Runnable() {
                @Override
                public void run() {
                    int scrollBarSize = 16;
                    org.eclipse.swt.graphics.Rectangle clientArea = getClientArea();
                    redraw(0, clientArea.height - scrollBarSize, clientArea.width, scrollBarSize, true);
                    redraw(clientArea.width - scrollBarSize, 0, scrollBarSize, clientArea.height, true);
                }
            });
            return Status.OK_STATUS;
        }
    };

    {
        overlayScrollBarAreaRedrawKludgeJob.setSystem(true);
    }

    private boolean isPaintingOverlayScrollBarArea(Graphics graphics) {
        Rectangle clip = new Rectangle();
        graphics.getClip(clip);
        org.eclipse.swt.graphics.Rectangle clientArea = getClientArea();
        int scrollBarSize = 16;
        return (clip.x == 0 && clip.x + clip.width == clientArea.width && clip.y + clip.height == clientArea.height && clip.height < scrollBarSize) ||
               (clip.y == 0 && clip.y + clip.height == clientArea.height && clip.x + clip.width == clientArea.width && clip.width < scrollBarSize);
    }

    @Override
    protected void paint(Graphics graphics) {
        validate(graphics);
        super.paint(graphics);
        // KLUDGE: see comment at the job field
        if ("gtk".equals(SWT.getPlatform()) && isPaintingOverlayScrollBarArea(graphics)) {
            overlayScrollBarAreaRedrawKludgeJob.cancel();
            // NOTE: we actually have to wait about 100ms, less than that makes this workaround unreliable (e.g. a simple asyncExec doesn't work)
            overlayScrollBarAreaRedrawKludgeJob.schedule(100);
        }
    }

    @Override
    protected void paintCachableLayer(Graphics graphics) {
        if (eventLogInput != null) {
            drawSequenceChart(graphics);
//            System.out.println(labelQuadTree);
        }
    }

    @Override
    protected void paintNoncachableLayer(Graphics graphics) {
        if (eventLogInput != null) {
            drawTimelineBookmarks(graphics);
            if (showAxisLabels && getFontHeight(graphics) < getAxisSpacing())
                drawAxisLabels(graphics);
            if (showEventMarks) {
                drawEventBookmarks(graphics);
                drawEventSelectionMarks(graphics);
            }
            if (showMessageSends || showMessageReuses) {
                drawMessageDependencyBookmarks(graphics);
                drawMessageDependencySelectionMarks(graphics);
            }
            if (showComponentMethodCalls) {
                drawComponentMethodCallBookmarks(graphics);
                drawComponentMethodCallSelectionMarks(graphics);
            }
            if (showAxes) {
                drawAxisBookmarks(graphics);
                drawAxisSelectionMarks(graphics);
            }
            drawTimelineSelectionMarks(graphics);
            drawGutters(graphics, getViewportHeight());
            drawTickUnderMouse(graphics, getViewportHeight());
            drawHightlightedObjects(graphics);
            if (showTimeDifferences)
                drawTimeDifferences(graphics);
            if (showAxisHeaders)
                drawAxisHeaders(graphics);
            if (showAxisInfo)
                drawAxisInfo(graphics);
            if (showPositionAndRange)
                drawPositionAndRange(graphics, getViewportWidth());
            if (showEventLogInfo)
                drawEventLogInfo(graphics);
            drawTickPrefix(graphics);
            rubberbandSupport.drawRubberband(graphics);
        }
    }

    /**
     * Used to paint the sequence chart offline without using the SWT widget framework.
     */
    public void paintArea(Graphics graphics) {
        validate(graphics);
        drawSequenceChart(graphics);
        if (showAxisLabels && getFontHeight(graphics) < getAxisSpacing())
            drawAxisLabels(graphics);
        drawGutters(graphics, getViewportHeight());
        if (showAxisHeaders)
            drawAxisHeaders(graphics);
        if (showPositionAndRange)
            drawPositionAndRange(graphics, getViewportWidth());
        drawTickPrefix(graphics);
    }

    /**
     * Clears internal error markers, all caches and forces a redraw.
     */
    public void refresh() {
        internalError = null;
        internalErrorHappenedDuringPaint = false;
        eventLogInput.resetCanceled();
        if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() != -1)
            relocateFixPoint(sequenceChartFacade.getTimelineCoordinateSystemOriginEvent(), fixPointViewportCoordinate);
        else {
            if (!eventLog.isEmpty()) {
                // don't use relocateFixPoint, because viewportWidth is not yet set during initializing
                sequenceChartFacade.relocateTimelineCoordinateSystem(eventLog.getFirstEvent());
                fixPointViewportCoordinate = 0;
            }
            else {
                sequenceChartFacade.undefineTimelineCoordinateSystem();
                fixPointViewportCoordinate = 0;
            }
        }
        invalidate();
    }

    private void invalidate() {
        invalidateAxes();
        invalidateAxisHeaders();
        invalidateAxisModulePositions();
        invalidateAxisSpacing();
        invalidateModuleIdToAxisRendererMap();
        invalidateModuleIdToAxisModuleIndexMap();
        invalidateReverseAxisModulePositions();
        invalidateViewportSize();
        invalidateVirtualSize();
        invalidateVisibleAxisModules();
    }

    private void validate(Graphics graphics) {
        validateVisibleAxisModules();
        // NOTE: there's a circular dependency between viewport size, axis spacing and virtual size
        //  - scrollbar visibility depends on viewport size and virtual size
        //  - client area depends on scrollbar visibility
        //  - viewport size depends on client area
        //  - axis spacing depends on viewport size
        //  - virtual size depends on axis spacing
        //  - scrollbar visibility depends on virtual size
        for (int i = 0; i < 10 && (invalidViewportSize || invalidAxisSpacing || invalidVirtualSize); i++) {
            validateViewportSize(graphics);
            validateAxisSpacing();
            validateVirtualSize();
            if (debug)
                Debug.println("viewportSize = (" + getViewportWidth() + ", " + getViewportHeight() + "), axisSpacing = " + axisSpacing + ", virtualSize = (" + getVirtualWidth() + ", " + getVirtualHeight() + ")");
        }
        validateAxisModulePositions();
        validateReverseAxisModulePositions();
        validateAxisHeaders();
        validateAxes();
        validateModuleIdToAxisRendererMap();
        validateModuleIdToAxisModuleIndexMap();
        Assert.isTrue(!invalidViewportSize);
        Assert.isTrue(!invalidAxes);
        Assert.isTrue(!invalidAxisHeaders);
        Assert.isTrue(!invalidAxisModulePositions);
        Assert.isTrue(!invalidAxisSpacing);
        Assert.isTrue(!invalidModuleIdToAxisModuleIndexMap);
        Assert.isTrue(!invalidModuleIdToAxisRendererMap);
        Assert.isTrue(!invalidReverseAxisModulePositions);
        Assert.isTrue(!invalidViewportSize);
        Assert.isTrue(!invalidVirtualSize);
        Assert.isTrue(!invalidVisibleAxisModules);
    }

    /**
     * Clears the canvas cache (aka. the saved bitmaps) and forces a redraw.
     */
    public void clearCanvasCacheAndRedraw() {
        labelPositions.clear();
        labelQuadTree.clear();
        clearCanvasCache();
        redraw();
    }

    @Override
    protected Graphics createGraphics(GC gc) {
        Graphics graphics = super.createGraphics(gc);
        graphics.setAntialias(drawWithAntialias ? SWT.ON : SWT.OFF);
        graphics.setTextAntialias(SWT.ON);
        return graphics;
    }

    /*************************************************************************************
     * DRAWING
     */

    /**
     * Draws a notification message to the center of the viewport.
     */
    private void drawNotificationMessage(GC gc, String text) {
        Graphics graphics = createGraphics(gc);
        graphics.setForegroundColor(ColorFactory.RED4);
        graphics.setBackgroundColor(ColorFactory.WHITE);
        graphics.setFont(getFont());
        int x = getViewportWidth() / 2;
        int y = getViewportHeight() / 2;
        String[] lines = text.split("\n");
        for (int i = 0; i < lines.length; i++) {
            String line = lines[i];
            Point p = GraphicsUtils.getTextExtent(graphics, line);
            graphics.fillString(line, x - p.x / 2, y - (lines.length / 2 - i) * p.y);
        }
        graphics.dispose();
    }

    /**
     * Draw simulation time range on the top right of the viewport.
     */
    private void drawPositionAndRange(Graphics graphics, int viewportWidth) {
        BigDecimal leftTick = calculateTick(0, 1);
        BigDecimal rightTick = calculateTick(viewportWidth, 1);
        BigDecimal simulationTimeRange = rightTick.subtract(leftTick);
        long[] eventPtrRange = getFirstLastEventPtrForViewportRange(0, viewportWidth);
        long startEventPtr = eventPtrRange[0];
        long endEventPtr = eventPtrRange[1];
        if (startEventPtr != 0 && endEventPtr != 0) {
            long startEventNumber = sequenceChartFacade.IEvent_getEventNumber(startEventPtr);
            long endEventNumber = sequenceChartFacade.IEvent_getEventNumber(endEventPtr);
            String positionString = "Position: #" + startEventNumber + ", " + TimeUtils.secondsToTimeString(leftTick);
            String rangeString = "Range: #" + (endEventNumber - startEventNumber + 1) + ", " + TimeUtils.secondsToTimeString(simulationTimeRange);
            int width = Math.max(GraphicsUtils.getTextExtent(graphics, positionString).x, GraphicsUtils.getTextExtent(graphics, rangeString).x);
            int x = viewportWidth - width - 3;
            int gutterHeight = getGutterHeight(graphics);
            int spacing = (gutterHeight - getFontHeight(graphics)) / 2;
            graphics.pushState();
            graphics.setFont(getFont());
            graphics.setBackgroundColor(styleProvider.getInfoBackgroundColor());
            graphics.fillRectangle(x - 3, gutterHeight, width + 6, 2 * gutterHeight + 1);
            graphics.setLineStyle(SWT.LINE_SOLID);
            graphics.setForegroundColor(styleProvider.getInfoLabelColor());
            graphics.drawRectangle(x - 3, gutterHeight, width + 5, 2 * gutterHeight);
            drawText(graphics, positionString, x, gutterHeight + 2 - spacing);
            drawText(graphics, rangeString, x, 2 * gutterHeight + 2 - spacing);
            graphics.popState();
        }
    }

    /**
     * Draw eventlog info on the bottom right of the viewport.
     */
    private void drawEventLogInfo(Graphics graphics) {
        IEvent firstEvent = eventLog.getFirstEvent();
        IEvent lastEvent = eventLog.getLastEvent();
        FileReader fileReader = eventLog.getFileReader();
        String firstLine = "File: " + fileReader.getFileSize() + " bytes, " +
                           "~" + eventLog.getApproximateNumberOfEvents() + " events (#" + firstEvent.getEventNumber() + " .. #" + lastEvent.getEventNumber() + "), " +
                           lastEvent.getSimulationTime().toString() + " seconds";
        String secondLine = "Read: " +
                            eventLog.getNumModuleCreatedEntries() + " modules, " +
                            eventLog.getNumParsedEvents() + " events, " +
                            fileReader.getNumReadLines() + " lines, " +
                            fileReader.getNumReadBytes() + " bytes";
        Point line1Size = GraphicsUtils.getTextExtent(graphics, firstLine);
        Point line2Size = GraphicsUtils.getTextExtent(graphics, secondLine);
        int gutterHeight = getGutterHeight(graphics);
        int spacing = (gutterHeight - getFontHeight(graphics)) / 2;
        int width = Math.max(line1Size.x, line2Size.x);
        int height = line1Size.y + line2Size.y + spacing * 3;
        int viewportWidth = getViewportWidth();
        int viewportHeight = getViewportHeight();
        int x = viewportWidth - width - 3;
        graphics.pushState();
        graphics.translate(0, gutterHeight);
        graphics.setFont(getFont());
        graphics.setBackgroundColor(styleProvider.getInfoBackgroundColor());
        graphics.fillRectangle(x - 3, viewportHeight - height - 1, width + 6, height);
        graphics.setLineStyle(SWT.LINE_SOLID);
        graphics.setForegroundColor(styleProvider.getInfoLabelColor());
        graphics.drawRectangle(x - 3, viewportHeight - height - 1, width + 5, height);
        drawText(graphics, firstLine, x, viewportHeight - height + spacing);
        drawText(graphics, secondLine, x, viewportHeight - height + line1Size.y + spacing);
        graphics.popState();
    }

    /**
     * Draw the tick prefix (in simulation time) on the very left side of the top gutter.
     */
    private void drawTickPrefix(Graphics graphics) {
        String timeString = "+ " + TimeUtils.secondsToTimeString(tickPrefix);
        int width = GraphicsUtils.getTextExtent(graphics, timeString).x + 4;
        int x = getViewportWidth() - width - 1;
        int gutterHeight = getGutterHeight(graphics);
        FontData fontData = getFont().getFontData()[0];
        Font newFont = new Font(getFont().getDevice(), fontData.getName(), fontData.getHeight(), SWT.BOLD);
        graphics.pushState();
        graphics.setFont(newFont);
        graphics.setBackgroundColor(styleProvider.getInfoBackgroundColor());
        graphics.fillRectangle(x, 0, width, gutterHeight);
        graphics.setLineStyle(SWT.LINE_SOLID);
        graphics.setForegroundColor(styleProvider.getInfoLabelColor());
        graphics.drawRectangle(x, 0, width, gutterHeight);
        drawText(graphics, timeString, x + 2, (gutterHeight - getFontHeight(graphics)) / 2);
        graphics.popState();
        newFont.dispose();
    }

    /**
     * Draws the actual sequence chart part including events, message dependencies,
     * axes, zero simulation time regions, etc.
     */
    private void drawSequenceChart(Graphics graphics) {
        graphics.pushState();
        graphics.translate(0, getGutterHeight(graphics));

        long startMillis = System.currentTimeMillis();
        if (debug)
            Debug.println("drawSequenceChart(): enter");

        graphics.getClip(Rectangle.SINGLETON);
        if (debug)
            Debug.println("Clipping rectangle: " + Rectangle.SINGLETON);

        int extraClipping = getExtraClippingForEvents();
        long[] eventPtrRange = getFirstLastEventPtrForViewportRange(Rectangle.SINGLETON.x - extraClipping, Rectangle.SINGLETON.right() + extraClipping);
        long startEventPtr = eventPtrRange[0];
        long endEventPtr = eventPtrRange[1];

        if (showZeroSimulationTimeRegions)
            drawZeroSimulationTimeRegions(graphics, startEventPtr, endEventPtr);

        if (showAxes)
            drawAxes(graphics, startEventPtr, endEventPtr);

        drawEvents(graphics, startEventPtr, endEventPtr);
        drawMessageDependencies(graphics);

        if (showComponentMethodCalls)
            drawComponentMethodCalls(graphics);

        long totalMillis = System.currentTimeMillis() - startMillis;
        if (debug)
            Debug.println("drawSequenceChart(): leave after " + totalMillis + "ms");

        // turn on/off anti-alias
        if (drawWithAntialias && totalMillis > ANTIALIAS_TURN_OFF_AT_MSEC)
            drawWithAntialias = false;
        else if (!drawWithAntialias && totalMillis < ANTIALIAS_TURN_ON_AT_MSEC)
            drawWithAntialias = true;

        graphics.popState();
    }

    private int getExtraClippingForEvents() {
        return (showMessageNames || showMethodNames || showEventNumbers) ? 300 : 100;
    }

    private void drawZeroSimulationTimeRegions(Graphics graphics, long startEventPtr, long endEventPtr) {
        long previousEventPtr = -1;
        graphics.getClip(Rectangle.SINGLETON);
        LargeRect clip = new LargeRect(Rectangle.SINGLETON);
        LargeRect r = new LargeRect();
        graphics.setBackgroundColor(styleProvider.getZeroSimulationTimeRegionColor());

        if (startEventPtr != 0 && endEventPtr != 0) {
            // draw rectangle before the very beginning of the simulation
            long x = clip.x;
            if (getSimulationTimeForViewportCoordinate(x).equals(org.omnetpp.common.engine.BigDecimal.getZero())) {
//                long startX = getViewportCoordinateForSimulationTime(org.omnetpp.common.engine.BigDecimal.getZero(), true);
                long startX = getEventXViewportCoordinateEnd(startEventPtr);

                if (x != startX)
                    drawZeroSimulationTimeRegion(graphics, r, clip, x, startX - x);
            }

            // draw rectangles where simulation time has not elapsed between events
            for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.IEvent_getNextEvent(eventPtr)) {
                int xBegin = (int)getEventXViewportCoordinateBegin(eventPtr);
                int xEnd = (int)getEventXViewportCoordinateEnd(eventPtr);
                if (xBegin != xEnd)
                    drawZeroSimulationTimeRegion(graphics, r, clip, xBegin, xEnd - xBegin);
                if (previousEventPtr != -1) {
                    x = getEventXViewportCoordinateEnd(eventPtr);
                    long previousX = getEventXViewportCoordinateBegin(previousEventPtr);
                    org.omnetpp.common.engine.BigDecimal simulationTime = sequenceChartFacade.IEvent_getSimulationTime(eventPtr);
                    org.omnetpp.common.engine.BigDecimal previousSimulationTime = sequenceChartFacade.IEvent_getSimulationTime(previousEventPtr);

                    if (simulationTime.equals(previousSimulationTime) && x != previousX)
                        drawZeroSimulationTimeRegion(graphics, r, clip, previousX, x - previousX);
                }

                previousEventPtr = eventPtr;

                if (eventPtr == endEventPtr)
                    break;
            }

            // draw rectangle after the very end of the simulation
            if (sequenceChartFacade.IEvent_getNextEvent(endEventPtr) == 0) {
                x = clip.right();
                long endX = getEventXViewportCoordinateEnd(endEventPtr);

                if (x != endX)
                    drawZeroSimulationTimeRegion(graphics, r, clip, endX, x - endX);
            }
        }
        else
            graphics.fillRectangle(Rectangle.SINGLETON);
    }

    // KLUDGE: SWG fillRectange overflow bug and cut down big coordinates with clipping
    private void drawZeroSimulationTimeRegion(Graphics graphics, LargeRect r, LargeRect clip, long x, long width) {
        r.setLocation(x, clip.y);
        r.setSize(width, clip.height);
        r.intersect(clip);
        graphics.fillRectangle((int)r.x, (int)r.y, (int)r.width, (int)r.height);
    }

    /**
     * Draws all axes in the given event range.
     */
    private void drawAxes(Graphics graphics, long startEventPtr, long endEventPtr) {
        for (Axis axis : getAxes())
            drawAxis(graphics, startEventPtr, endEventPtr, axis);
    }

    /**
     * Draws a single axis in the given event range.
     */
    private void drawAxis(Graphics graphics, long startEventPtr, long endEventPtr, Axis axis) {
        graphics.translate(0, axis.y - (int)getViewportTop());
        axis.axisRenderer.drawAxis(graphics, startEventPtr, endEventPtr);
        graphics.translate(0, -axis.y + (int)getViewportTop());
    }

    private void drawMessageDependencyBookmarks(Graphics graphics) {
        IMarker[] markers;
        try {
            markers = eventLogInput.getFile().findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
        for (int i = 0; i < markers.length; i++) {
            IMarker marker = markers[i];
            if ("MessageDependency".equals(marker.getAttribute("Kind", null))) {
                String eventNumberString = marker.getAttribute("EventNumber", null);
                IEvent event = eventNumberString != null ? eventLogInput.getEventLog().getEventForEventNumber(Long.parseLong(eventNumberString)) : null;
                if (event != null) {
                    String messageDependencyIndexString = marker.getAttribute("MessageDependencyIndex", null);
                    IMessageDependency messageDependency = event.getConsequences().get(Integer.parseInt(messageDependencyIndexString));
                    drawMessageDependencyMark(graphics, messageDependency.getCPtr(), styleProvider.getBookmarkColor());
                }
            }
        }
    }

    private void drawMessageDependencySelectionMarks(Graphics graphics) {
        for (Object object : selectedObjects)
            if (object instanceof IMessageDependency)
                drawMessageDependencyMark(graphics, ((IMessageDependency)object).getCPtr(), styleProvider.getSelectionColor());
    }

    private void drawMessageDependencyMark(Graphics graphics, long mesageDependencyPtr, Color color) {
        graphics.pushState();
        graphics.setLineWidthFloat(1.5f);
        graphics.setForegroundColor(color);
        graphics.setBackgroundColor(color);
        graphics.translate(0, getGutterHeight(graphics));
        long[] eventPtrRange = getFirstLastEventPtrForMessageDependencies();
        long startEventPtr = eventPtrRange[0];
        long endEventPtr = eventPtrRange[1];
        VLineBuffer vlineBuffer = new VLineBuffer();
        drawOrFitMessageDependency(graphics, mesageDependencyPtr, -1, -1, vlineBuffer, startEventPtr, endEventPtr);
        graphics.popState();
    }

    private void drawComponentMethodCallBookmarks(Graphics graphics) {
        IMarker[] markers;
        try {
            markers = eventLogInput.getFile().findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
        for (int i = 0; i < markers.length; i++) {
            IMarker marker = markers[i];
            if ("ComponentMethodCall".equals(marker.getAttribute("Kind", null))) {
                String eventNumberString = marker.getAttribute("EventNumber", null);
                String entryIndexString = marker.getAttribute("EventLogEntryIndex", null);
                IEvent event = eventLogInput.getEventLog().getEventForEventNumber(Long.parseLong(eventNumberString));
                EventLogEntry eventLogEntry = entryIndexString != null ? event.getEventLogEntry(Integer.parseInt(entryIndexString)) : null;
                if (eventLogEntry instanceof ComponentMethodBeginEntry)
                    drawComponentMethodCallMark(graphics, ((ComponentMethodBeginEntry)eventLogEntry).getCPtr(), styleProvider.getBookmarkColor());
            }
        }
    }

    private void drawComponentMethodCallSelectionMarks(Graphics graphics) {
        for (Object object : selectedObjects)
            if (object instanceof ComponentMethodBeginEntry)
                drawComponentMethodCallMark(graphics, ((ComponentMethodBeginEntry)object).getCPtr(), styleProvider.getSelectionColor());
    }

    private void drawComponentMethodCallMark(Graphics graphics, long componentMethodCallPtr, Color color) {
        graphics.pushState();
        graphics.setLineWidthFloat(1.5f);
        graphics.setForegroundColor(color);
        graphics.setBackgroundColor(color);
        graphics.translate(0, getGutterHeight(graphics));
        drawOrFitComponentMethodCall(graphics, componentMethodCallPtr, -1, -1);
        graphics.popState();
    }

    private void drawAxisBookmarks(Graphics graphics) {
        IMarker[] markers;
        try {
            markers = eventLogInput.getFile().findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
        for (int i = 0; i < markers.length; i++) {
            IMarker marker = markers[i];
            if ("Axis".equals(marker.getAttribute("Kind", null))) {
                String modulePath = marker.getAttribute("ModulePath", null);
                ModuleTreeItem moduleTreeItem = eventLogInput.getModuleTreeRoot().findDescendantModule(modulePath);
                drawAxisMark(graphics, moduleTreeItem, styleProvider.getBookmarkColor());
            }
        }
    }

    private void drawAxisSelectionMarks(Graphics graphics) {
        for (Object object : selectedObjects)
            if (object instanceof ModuleTreeItem)
                drawAxisMark(graphics, (ModuleTreeItem)object, styleProvider.getSelectionColor());
    }

    private void drawAxisMark(Graphics graphics, ModuleTreeItem axisModule, Color color) {
        Rectangle rect = graphics.getClip(Rectangle.SINGLETON);
        int index = getAxisModuleIndexByModuleId(axisModule.getModuleId());
        if (index != -1) {
            Axis axis = getAxis(index);
            int y = axis.y - (int)getViewportTop();
            IAxisRenderer axisRenderer = axis.axisRenderer;
            graphics.pushState();
            graphics.translate(0, getGutterHeight(graphics));
            graphics.setLineStyle(SWT.LINE_SOLID);
            graphics.setLineWidthFloat(1.5f);
            graphics.setForegroundColor(color);
            graphics.drawLine(rect.x, y - 1, rect.right(), y - 1);
            graphics.drawLine(rect.x, y + axisRenderer.getHeight(), rect.right(), y + axisRenderer.getHeight());
            graphics.popState();
        }
    }

    private void drawComponentMethodCalls(Graphics graphics) {
        int extraClipping = getExtraClippingForEvents();
        long[] eventPtrRange = getFirstLastEventPtrForViewportRange(-extraClipping, getViewportWidth() + extraClipping);
        long startEventPtr = eventPtrRange[0];
        long endEventPtr = eventPtrRange[1];

        if (startEventPtr != 0 && endEventPtr != 0) {
            if (debug)
                Debug.println("Drawing module method calls in event range: " + sequenceChartFacade.IEvent_getEventNumber(startEventPtr) + " ->: " + sequenceChartFacade.IEvent_getEventNumber(endEventPtr));

            PtrVector componentMethodBeginEntries = sequenceChartFacade.getComponentMethodBeginEntries(startEventPtr, endEventPtr);

            if (debug)
                Debug.println("Drawing " + componentMethodBeginEntries.size() + " module method calls");

            for (int i = 0; i < componentMethodBeginEntries.size(); i++) {
                long componentMethodCallPtr = componentMethodBeginEntries.get(i);
                if (isInitializationEvent(sequenceChartFacade.EventLogEntry_getEvent(componentMethodCallPtr)) && !showInitializationEvent)
                    continue;
                Color color = styleProvider.getComponentMethodCallColor(componentMethodCallPtr);
                graphics.setForegroundColor(color);
                graphics.setBackgroundColor(color);
                drawOrFitComponentMethodCall(graphics, componentMethodCallPtr, -1, -1);
            }
        }
    }

    private boolean drawOrFitComponentMethodCall(Graphics graphics, long componentMethodCallPtr, int fitX, int fitY) {
        int sourceComponentId = sequenceChartFacade.ComponentMethodBeginEntry_getSourceComponentId(componentMethodCallPtr);
        int targetComponentId = sequenceChartFacade.ComponentMethodBeginEntry_getTargetComponentId(componentMethodCallPtr);

        if ((sourceComponentId == 1 || getModuleIdToAxisModuleIndexMap().containsKey(sourceComponentId)) && getModuleIdToAxisModuleIndexMap().containsKey(targetComponentId)) {
            long eventPtr = sequenceChartFacade.EventLogEntry_getEvent(componentMethodCallPtr);
            long eventNumber = sequenceChartFacade.IEvent_getEventNumber(eventPtr);
            // handle the filtered eventlog case
            eventPtr = sequenceChartFacade.IEvent_getEventForEventNumber(eventNumber);
            if (showInitializationEvent || !isInitializationEvent(eventPtr)) {
                int eventAxisIndex = getEventAxisModuleIndex(eventPtr);
                int fromAxisIndex = getAxisModuleIndexByModuleId(sourceComponentId);
                int toAxisIndex = getAxisModuleIndexByModuleId(targetComponentId);
                if ((sourceComponentId == 1 || fromAxisIndex != -1) && toAxisIndex != -1) {
                    int toY = getModuleYViewportCoordinateByModuleIndex(toAxisIndex);
                    int fromY = sourceComponentId == 1 ? toY - 100 : getModuleYViewportCoordinateByModuleIndex(fromAxisIndex);
                    if (showEventMarks) {
                        if (toY > fromY) {
                            if (toAxisIndex == eventAxisIndex)
                                toY -= styleProvider.getEventRadius();
                            if (fromAxisIndex == eventAxisIndex)
                                fromY += styleProvider.getEventRadius();
                        }
                        else if (toY < fromY) {
                            if (toAxisIndex == eventAxisIndex)
                                toY += styleProvider.getEventRadius();
                            if (fromAxisIndex == eventAxisIndex)
                                fromY -= styleProvider.getEventRadius();
                        }
                    }
                    int xBegin = (int)getEventLogEntryXViewportCoordinateBegin(componentMethodCallPtr);
                    long componentMethodEndEntryPtr = sequenceChartFacade.ComponentMethodBeginEntry_getComponentMethodEndEntry(componentMethodCallPtr);
                    int xEnd = (int)getEventLogEntryXViewportCoordinateBegin(componentMethodEndEntryPtr);
                    if (graphics != null && toY != fromY) {
                        graphics.setLineStyle(styleProvider.getComponentMethodCallLineStyle(componentMethodCallPtr));
                        if (styleProvider.getComponentMethodCallLineDash(componentMethodCallPtr) != null)
                            graphics.setLineDash(styleProvider.getComponentMethodCallLineDash(componentMethodCallPtr));
                        graphics.setAlpha(24);
                        graphics.fillRectangle(xBegin, fromY, xEnd - xBegin, toY - fromY);
                        graphics.setAlpha(255);
                        int width = (int)(getPixelPerTimelineUnit() * 0.1) + 1;
                        graphics.setBackgroundColor(styleProvider.getComponentMethodCallColor(componentMethodCallPtr));
                        if (fromAxisIndex != eventAxisIndex)
                            graphics.fillRectangle(xEnd, fromY - 2, width, 5);
                        if (toAxisIndex != eventAxisIndex)
                            graphics.fillRectangle(xBegin, toY - 2, width, 5);
                        graphics.drawLine(xBegin, fromY, xBegin, toY);
                        graphics.drawLine(xEnd, fromY, xEnd, toY);
                        if (showArrowHeads && toY != fromY) {
                            drawArrowHead(graphics, null, xBegin, toY, 0, toY - fromY);
                            drawArrowHead(graphics, null, xEnd, fromY, 0, fromY - toY);
                        }
                        if (showMethodNames) {
                            String text = sequenceChartFacade.ComponentMethodBeginEntry_getMethod(componentMethodCallPtr);
                            int index = text.indexOf('(');
                            if (index != -1)
                                text = text.substring(0, index);
                            graphics.pushState();
                            graphics.setFont(getFont());
                            int dx = GraphicsUtils.getTextExtent(graphics, text).x;
                            graphics.translate(xBegin, (fromY + toY) / 2);
                            graphics.rotate(-90);
                            drawText(graphics, text, -dx / 2, -getFontHeight(graphics) - 2);
                            graphics.popState();
                        }
                    }
                    else
                        return lineContainsPoint(xBegin, fromY, xBegin, toY, fitX, fitY, MOUSE_TOLERANCE);
                }
            }
        }

        return false;
    }

    /**
     * Draws all message arrows which have visual representation in the given event range.
     */
    private void drawMessageDependencies(Graphics graphics) {
        long[] eventPtrRange = getFirstLastEventPtrForMessageDependencies();
        long startEventPtr = eventPtrRange[0];
        long endEventPtr = eventPtrRange[1];
        if (startEventPtr != 0 && endEventPtr != 0) {
            if (debug)
                Debug.println("Drawing message dependencies in event range: " + sequenceChartFacade.IEvent_getEventNumber(startEventPtr) + " ->: " + sequenceChartFacade.IEvent_getEventNumber(endEventPtr));
            PtrVector messageDependencies = sequenceChartFacade.getIntersectingMessageDependencies(startEventPtr, endEventPtr);
            if (debug)
                Debug.println("Drawing " + messageDependencies.size() + " message dependencies");
            VLineBuffer vlineBuffer = new VLineBuffer();
            for (int i = 0; i < messageDependencies.size(); i++) {
                long messageDependencyPtr = messageDependencies.get(i);
                long causeEventPtr = sequenceChartFacade.IMessageDependency_getCauseEvent(messageDependencyPtr);
                Color color = styleProvider.getMessageDependencyColor(messageDependencyPtr);
                graphics.setForegroundColor(color);
                graphics.setBackgroundColor(color);
                if (!isInitializationEvent(causeEventPtr) || showInitializationEvent)
                    drawOrFitMessageDependency(graphics, messageDependencyPtr, -1, -1, vlineBuffer, startEventPtr, endEventPtr);
            }
        }
    }

    /**
     * Draws all events within the given event range.
     */
    private void drawEvents(Graphics graphics, long startEventPtr, long endEventPtr) {
        if (startEventPtr != 0 && endEventPtr != 0) {
            if (debug)
                Debug.println("Drawing events with event range: " + sequenceChartFacade.IEvent_getEventNumber(startEventPtr) + " ->: " + sequenceChartFacade.IEvent_getEventNumber(endEventPtr));

            HashMap<Integer, Integer> axisYtoLastXBegin = new HashMap<Integer, Integer>();
            HashMap<Integer, Integer> axisYtoLastXEnd = new HashMap<Integer, Integer>();

            // NOTE: navigating through next event takes care about leaving events out which are not in the filter's result
            for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.IEvent_getNextEvent(eventPtr)) {
                if (isInitializationEvent(eventPtr)) {
                    if (showInitializationEvent)
                        drawEvent(graphics, eventPtr);
                }
                else if (getEventAxisModuleIndex(eventPtr) != -1) {
                    int xBegin = (int)getEventXViewportCoordinateBegin(eventPtr);
                    int xEnd = (int)getEventXViewportCoordinateEnd(eventPtr);
                    int y = getEventYViewportCoordinate(eventPtr);
                    Integer lastXBegin = axisYtoLastXBegin.get(y);
                    Integer lastXEnd = axisYtoLastXEnd.get(y);

                    // performance optimization: don't draw event if there's one already drawn exactly there
                    if (lastXBegin == null || lastXEnd == null || lastXBegin.intValue() != xBegin || lastXEnd.intValue() != xEnd) {
                        axisYtoLastXBegin.put(y, xBegin);
                        axisYtoLastXEnd.put(y, xEnd);
                        drawEvent(graphics, eventPtr, getEventAxisModuleIndex(eventPtr), xBegin, xEnd, y);
                    }
                }

                if (eventPtr == endEventPtr)
                    break;
            }
        }
    }

    /**
     * Draws a single event.
     */
    private void drawEvent(Graphics graphics, long eventPtr) {
        int xBegin = (int)getEventXViewportCoordinateBegin(eventPtr);
        int xEnd = (int)getEventXViewportCoordinateEnd(eventPtr);

        if (isInitializationEvent(eventPtr)) {
            if (showInitializationEvent)
                drawInitializationEvent(graphics, eventPtr, xBegin, xEnd);
        }
        else
            drawEvent(graphics, eventPtr, getEventAxisModuleIndex(eventPtr), xBegin, xEnd, getEventYViewportCoordinate(eventPtr));
    }

    /**
     * Draws the initialization event on all axes.
     */
    private void drawInitializationEvent(Graphics graphics, long eventPtr, int xBegin, int xEnd) {
        for (int i = 0; i < sequenceChartFacade.IEvent_getNumConsequences(eventPtr); i++) {
            long consequencePtr = sequenceChartFacade.IEvent_getConsequence(eventPtr, i);
            long messageEntryPtr = sequenceChartFacade.IMessageDependency_getMessageEntry(consequencePtr);

            if (messageEntryPtr != 0) {
                int contextModuleId = sequenceChartFacade.EventLogEntry_getContextModuleId(messageEntryPtr);
                int moduleIndex = getAxisModuleIndexByModuleId(contextModuleId);
                if (moduleIndex != -1) {
                    int y = getModuleYViewportCoordinateByModuleIndex(moduleIndex);
                    drawEvent(graphics, eventPtr, moduleIndex, xBegin, xEnd, y);
                }
            }
        }
    }

    /**
     * Draws a single event at the given coordinates and axis module.
     */
    private void drawEvent(Graphics graphics, long eventPtr, int axisModuleIndex, int xBegin, int xEnd, int y) {
        if (showEventMarks || showEventNumbers) {
            int radius = styleProvider.getEventRadius();
            int diameter = 2 * radius;
            Axis axis = getAxis(axisModuleIndex);
            if (showEventMarks) {
                if (!isInitializationEvent(eventPtr) && sequenceChartFacade.IEvent_getModuleId(eventPtr) != axis.axisHeader.module.getModuleId())
                    graphics.setAlpha(64);
                graphics.setForegroundColor(styleProvider.getEventStrokeColor(eventPtr));
                graphics.setBackgroundColor(styleProvider.getEventFillColor(eventPtr));
                graphics.setLineStyle(SWT.LINE_SOLID);
                graphics.fillArc(xBegin - radius, y - radius, diameter, diameter, 90, 180);
                graphics.fillArc(xEnd - radius, y - radius, diameter, diameter, 270, 180);
                graphics.drawArc(xBegin - radius, y - radius, diameter, diameter, 90, 180);
                graphics.drawArc(xEnd - radius, y - radius, diameter, diameter, 270, 180);
                if (xBegin != xEnd) {
                    graphics.fillRectangle(xBegin, y - radius, xEnd - xBegin, diameter);
                    graphics.drawLine(xBegin, y - radius, xEnd, y - radius);
                    graphics.drawLine(xBegin, y + radius, xEnd, y + radius);
                }
                graphics.setAlpha(255);
            }
            if (showEventNumbers) {
                graphics.setForegroundColor(styleProvider.getEventLabelColor(eventPtr));
                if (styleProvider.getEventLabelFont(eventPtr) != null)
                    graphics.setFont(styleProvider.getEventLabelFont(eventPtr));
                else
                    graphics.setFont(getFont());
                drawText(graphics, labelProvider.getEventLabel(eventPtr), xBegin + diameter, y + radius + axis.axisRenderer.getHeight() / 2);
            }
        }
    }

    /**
     * Draws the top and bottom gutters which will display ticks.
     */
    private void drawGutters(Graphics graphics, int viewportHeigth) {
        graphics.pushState();
        graphics.setFont(getFont());
        org.eclipse.swt.graphics.Rectangle r = getClientArea();
        int gutterHeight = getGutterHeight(graphics);

        // fill gutter backgrounds
        graphics.setBackgroundColor(styleProvider.getGutterBackgroundColor());
        graphics.fillRectangle(r.x, 0, r.x + r.width, gutterHeight);
        graphics.fillRectangle(r.x, viewportHeigth + gutterHeight, r.x + r.width, gutterHeight);

        if (showHairlines)
            drawTicks(graphics, viewportHeigth);

        // draw border around gutters
        graphics.setForegroundColor(styleProvider.getGutterBorderColor());
        graphics.setLineStyle(SWT.LINE_SOLID);
        graphics.drawRectangle(r.x, 0, r.x + r.width - 1, gutterHeight);
        graphics.drawRectangle(r.x, viewportHeigth + gutterHeight - 1, r.x + r.width - 1, gutterHeight);
        graphics.popState();
    }

    /**
     * Draws ticks on the gutter.
     */
    private void drawTicks(Graphics graphics, int viewportHeigth) {
        calculateTicks(getViewportWidth());

        IEvent lastEvent = eventLog.getLastEvent();
        org.omnetpp.common.engine.BigDecimal endSimulationTime = lastEvent == null ? org.omnetpp.common.engine.BigDecimal.getZero() : lastEvent.getSimulationTime();

        for (BigDecimal tick : ticks) {
            // BigDecimal to double conversions loose precision both in Java and C++ but we must stick to the one in C++
            // so that strange problems do not occur (think of comparing the tick's time to the last known simulation time)
            org.omnetpp.common.engine.BigDecimal simulationTime = new org.omnetpp.common.engine.BigDecimal(tick.doubleValue());
            if (endSimulationTime.less(simulationTime))
                simulationTime = endSimulationTime;
            drawTick(graphics, viewportHeigth, styleProvider.getTickLineColor(), styleProvider.getGutterBackgroundColor(), tick, (int)getViewportCoordinateForSimulationTime(simulationTime), false);
        }
    }

    /**
     * Draws a tick under the mouse.
     */
    private void drawTickUnderMouse(Graphics graphics, int viewportHeigth) {
        graphics.pushState();
        int gutterHeight = getGutterHeight(graphics);
        Point p = toControl(Display.getDefault().getCursorLocation());

        if (0 <= p.x && p.x < getViewportWidth() && 0 <= p.y && p.y < getViewportHeight() + gutterHeight * 2) {
            BigDecimal tick = calculateTick(p.x, 1);
            drawTick(graphics, viewportHeigth, styleProvider.getMouseTickLineColor(), styleProvider.getInfoBackgroundColor(), tick, p.x, true);
        }
        graphics.popState();
    }

    private void drawHightlightedObjects(Graphics graphics) {
        long[] eventPtrRange = getFirstLastEventPtrForMessageDependencies();
        long startEventPtr = eventPtrRange[0];
        long endEventPtr = eventPtrRange[1];
        graphics.pushState();
        graphics.translate(0, getGutterHeight(graphics));
        graphics.setLineWidth(3);
        VLineBuffer vlineBuffer = new VLineBuffer();
        // NOTE: draw highlighted objects here which are part of the cached layer
        for (Object object : highlightedObjects) {
            if (object instanceof IEvent) {
                IEvent event = (IEvent)object;
                drawEvent(graphics, event.getCPtr());
            }
            else if (object instanceof IMessageDependency) {
                IMessageDependency messageDependency = (IMessageDependency)object;
                long causeEventPtr = sequenceChartFacade.IMessageDependency_getCauseEvent(messageDependency.getCPtr());
                Color color = selectedObjects.contains(object) ? styleProvider.getSelectionColor() : styleProvider.getMessageDependencyColor(messageDependency.getCPtr());
                graphics.setForegroundColor(color);
                graphics.setBackgroundColor(color);
                if (!isInitializationEvent(causeEventPtr) || showInitializationEvent)
                    drawOrFitMessageDependency(graphics, messageDependency.getCPtr(), -1, -1, vlineBuffer, startEventPtr, endEventPtr);
            }
            else if (object instanceof ComponentMethodBeginEntry) {
                ComponentMethodBeginEntry componentMethodBeginEntry = (ComponentMethodBeginEntry)object;
                Color color = selectedObjects.contains(object) ? styleProvider.getSelectionColor() : styleProvider.getComponentMethodCallColor(componentMethodBeginEntry.getCPtr());
                graphics.setForegroundColor(color);
                graphics.setBackgroundColor(color);
                drawOrFitComponentMethodCall(graphics, componentMethodBeginEntry.getCPtr(), -1, -1);
            }
            else if (object instanceof ModuleTreeItem) {
                ModuleTreeItem axisModule = (ModuleTreeItem)object;
                int index = getAxisModuleIndexByModuleId(axisModule.getModuleId());
                if (index != -1)
                    drawAxis(graphics, startEventPtr, endEventPtr, getAxis(index));
            }
        }
        graphics.popState();
    }

    /**
     * Draws a single tick on the gutters and the chart.
     * The tick value will be drawn on both gutters with a hair line connecting them.
     */
    private void drawTick(Graphics graphics, int viewportHeight, Color tickColor, Color backgroundColor, BigDecimal tick, int x, boolean mouseTick) {
        String string = labelProvider.getTickLabel(tick.subtract(tickPrefix));
        if (tickPrefix.doubleValue() != 0.0)
            string = "+" + string;

        if (styleProvider.getTickLabelFont(tick) != null)
            graphics.setFont(styleProvider.getTickLabelFont(tick));
        else
            graphics.setFont(getFont());
        int stringWidth = GraphicsUtils.getTextExtent(graphics, string).x;
        int boxWidth = stringWidth + 6;
        int boxX = mouseTick ? Math.min(getViewportWidth() - boxWidth, x) : x;
        int gutterHeight = getGutterHeight(graphics);

        // draw background
        graphics.setBackgroundColor(backgroundColor);
        graphics.fillRectangle(boxX, 0, boxWidth, gutterHeight + 1);
        graphics.fillRectangle(boxX, viewportHeight + gutterHeight - 1, boxWidth, gutterHeight);

        // draw border only for mouse tick
        if (mouseTick) {
            graphics.setForegroundColor(styleProvider.getGutterBorderColor());
            graphics.setLineStyle(SWT.LINE_SOLID);
            graphics.drawRectangle(boxX, 0, boxWidth - 1, gutterHeight);
            graphics.drawRectangle(boxX, viewportHeight + gutterHeight - 1, boxWidth - 1, gutterHeight);
        }

        // draw tick value
        graphics.setForegroundColor(styleProvider.getTickLabelColor(tick));
        graphics.setBackgroundColor(backgroundColor);
        int spacing = (gutterHeight - getFontHeight(graphics)) / 2;
        drawText(graphics, string, boxX + 3, spacing);
        drawText(graphics, string, boxX + 3, viewportHeight + gutterHeight + spacing - 1);

        // draw hair line
        graphics.setLineStyle(SWT.LINE_DOT);
        graphics.setForegroundColor(tickColor);
        if (mouseTick)
            graphics.drawLine(x, gutterHeight, x, viewportHeight + gutterHeight);
        else
            graphics.drawLine(x, 0, x, viewportHeight + gutterHeight * 2);
    }

    /**
     * Draws the visual representation of selections around events.
     */
    private void drawEventSelectionMarks(Graphics graphics) {
        graphics.pushState();
        graphics.translate(0, getGutterHeight(graphics));
        long[] eventPtrRange = getFirstLastEventPtrForViewportRange(0 - styleProvider.getEventSelectionRadius(), getViewportWidth() + styleProvider.getEventSelectionRadius());
        long startEventPtr = eventPtrRange[0];
        long endEventPtr = eventPtrRange[1];

        if (startEventPtr != 0 && endEventPtr != 0) {
            long startEventNumber = sequenceChartFacade.IEvent_getEventNumber(startEventPtr);
            long endEventNumber = sequenceChartFacade.IEvent_getEventNumber(endEventPtr);

            for (Object object : selectedObjects) {
                if (object instanceof EventNumberRangeSet) {
                    EventNumberRangeSet eventNumberRangeSet = (EventNumberRangeSet)object;
                    if (!eventNumberRangeSet.isEmpty()) {
                        for (long eventNumber = startEventNumber; eventNumber <= endEventNumber; eventNumber++)
                            if (eventNumberRangeSet.contains(eventNumber))
                                drawEventMark(graphics, styleProvider.getSelectionColor(), eventLog.getEventForEventNumber(eventNumber));
                    }
                }
            }
        }
        graphics.popState();
    }

    private void drawTimelineBookmarks(Graphics graphics) {
        IMarker[] markers;
        try {
            markers = eventLogInput.getFile().findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
        for (int i = 0; i < markers.length; i++) {
            IMarker marker = markers[i];
            if ("Position".equals(marker.getAttribute("Kind", null))) {
                String simulationTimeString = marker.getAttribute("SimulationTime", null);
                org.omnetpp.common.engine.BigDecimal simulationTime = org.omnetpp.common.engine.BigDecimal.parse(simulationTimeString);
                int viewportCoordinate = (int)getViewportCoordinateForSimulationTime(simulationTime);
                drawTimelineMark(graphics, viewportCoordinate, styleProvider.getBookmarkColor());
            }
        }
    }

    private void drawTimelineSelectionMarks(Graphics graphics) {
        for (Object object : selectedObjects) {
            if (object instanceof Double) {
                Double timelineCoordinate = (Double)object;
                int viewportCoordinate = (int)getViewportCoordinateForTimelineCoordinate(timelineCoordinate);
                drawTimelineMark(graphics, viewportCoordinate, styleProvider.getSelectionColor());
            }
        }
    }

    /**
     * Draws a vertical line to represent the viewport coordinate.
     */
    private void drawTimelineMark(Graphics graphics, int viewportCoordinate, Color color) {
        graphics.pushState();
        graphics.translate(0, getGutterHeight(graphics));
        graphics.setLineStyle(SWT.LINE_SOLID);
        graphics.setLineWidthFloat(1.5f);
        graphics.setForegroundColor(color);
        graphics.drawLine(viewportCoordinate, 0, viewportCoordinate, getViewportHeight());
        graphics.setLineWidth(1);
        graphics.popState();
    }

    private void drawTimeDifferences(Graphics graphics) {
        MultiValueMap map = MultiValueMap.decorate(new TreeMap<Double, Pair<org.omnetpp.common.engine.BigDecimal, Point>>());
        for (Object object : selectedObjects) {
            if (object instanceof EventNumberRangeSet) {
                EventNumberRangeSet eventNumberRangeSet = (EventNumberRangeSet)object;
                for (long eventNumber : eventNumberRangeSet) {
                    IEvent event = eventLog.getEventForEventNumber(eventNumber);
                    org.omnetpp.common.engine.BigDecimal simulationTime = sequenceChartFacade.IEvent_getSimulationTime(event.getCPtr());
                    double timelineCoordinateBegin = sequenceChartFacade.IEvent_getTimelineCoordinateBegin(event.getCPtr());
                    double timelineCoordinateEnd = sequenceChartFacade.IEvent_getTimelineCoordinateEnd(event.getCPtr());
                    double timelineCoordinate = (timelineCoordinateEnd + timelineCoordinateBegin) / 2;
                    int xBegin = (int)getEventXViewportCoordinateBegin(event.getCPtr());
                    int xEnd = (int)getEventXViewportCoordinateEnd(event.getCPtr());
                    int y = isInitializationEvent(event) ? getViewportHeight() / 2 : getEventYViewportCoordinate(event.getCPtr());
                    map.put(timelineCoordinate, new Pair<org.omnetpp.common.engine.BigDecimal, Point>(simulationTime, new Point((xBegin + xEnd) / 2, y)));
                }
            }
            else if (object instanceof Double) {
                Double timelineCoordinate = (Double)object;
                org.omnetpp.common.engine.BigDecimal simulationTime = sequenceChartFacade.getSimulationTimeForTimelineCoordinate(timelineCoordinate);
                long x = getViewportCoordinateForTimelineCoordinate(timelineCoordinate);
                map.put(timelineCoordinate, new Pair<org.omnetpp.common.engine.BigDecimal, Point>(simulationTime, new Point((int)x, getViewportHeight() / 2)));
            }
        }
        graphics.pushState();
        graphics.translate(0, getGutterHeight(graphics));
        graphics.setLineWidthFloat(1.5f);
        graphics.setLineStyle(SWT.LINE_DASH);
        graphics.setForegroundColor(styleProvider.getSelectionColor());
        org.omnetpp.common.engine.BigDecimal previousSimulationTime = null;
        Point previousViewportCoordinate = null;
        for (Object object : map.entrySet()) {
            @SuppressWarnings("unchecked")
            Map.Entry<Double, ArrayList<Pair<org.omnetpp.common.engine.BigDecimal, Point>>> entry = (Map.Entry<Double, ArrayList<Pair<org.omnetpp.common.engine.BigDecimal, Point>>>)object;
            for (Pair<org.omnetpp.common.engine.BigDecimal, Point> pair : entry.getValue()) {
                org.omnetpp.common.engine.BigDecimal simulationTime = pair.first;
                Point viewportCoordinate = pair.second;
                if (previousSimulationTime != null) {
                    org.omnetpp.common.engine.BigDecimal simulationTimeDifference = simulationTime.subtract(previousSimulationTime);
                    graphics.drawLine(previousViewportCoordinate.x, previousViewportCoordinate.y, viewportCoordinate.x, viewportCoordinate.y);
                    String text = TimeUtils.secondsToTimeString(new BigDecimal(simulationTimeDifference.toString()));
                    Point size = GraphicsUtils.getTextExtent(graphics, text);
                    int x = (previousViewportCoordinate.x + viewportCoordinate.x) / 2;
                    int y = (previousViewportCoordinate.y + viewportCoordinate.y) / 2;
                    graphics.drawText(text, x - size.x / 2, y - size.y - 2);
                }
                previousSimulationTime = simulationTime;
                previousViewportCoordinate = viewportCoordinate;
            }
        }
        graphics.popState();
    }

    /**
     * Draw bookmarks associated with the input file.
     */
    private void drawEventBookmarks(Graphics graphics) {
        IMarker[] markers;
        try {
            markers = eventLogInput.getFile().findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
        long[] eventPtrRange = getFirstLastEventPtrForViewportRange(0 - styleProvider.getEventSelectionRadius(), getViewportWidth() + styleProvider.getEventSelectionRadius());
        long startEventPtr = eventPtrRange[0];
        long endEventPtr = eventPtrRange[1];
        if (startEventPtr != 0 && endEventPtr != 0) {
            graphics.pushState();
            graphics.translate(0, getGutterHeight(graphics));
            long startEventNumber = sequenceChartFacade.IEvent_getEventNumber(startEventPtr);
            long endEventNumber = sequenceChartFacade.IEvent_getEventNumber(endEventPtr);

            for (int i = 0; i < markers.length; i++) {
                IMarker marker = markers[i];
                if ("Event".equals(marker.getAttribute("Kind", null))) {
                    String eventNumberString = marker.getAttribute("EventNumber", null);
                    if (eventNumberString != null) {
                        long eventNumber = Long.parseLong(eventNumberString);
                        if (startEventNumber <= eventNumber && eventNumber <= endEventNumber) {
                            IEvent bookmarkedEvent = eventLog.getEventForEventNumber(eventNumber);
                            if (bookmarkedEvent != null)
                                drawEventMark(graphics, styleProvider.getBookmarkColor(), bookmarkedEvent);
                        }
                    }
                }
            }
            graphics.popState();
        }
    }

    /**
     * Draws a mark around the given event, handles initialize event.
     */
    private void drawEventMark(Graphics graphics, Color color, IEvent event) {
        int xBegin = (int)getEventXViewportCoordinateBegin(event.getCPtr());
        int xEnd = (int)getEventXViewportCoordinateEnd(event.getCPtr());

        if (isInitializationEvent(event)) {
            if (showInitializationEvent) {
                long eventPtr = event.getCPtr();
                for (int i = 0; i < sequenceChartFacade.IEvent_getNumConsequences(eventPtr); i++) {
                    long consequencePtr = sequenceChartFacade.IEvent_getConsequence(eventPtr, i);
                    int y = getInitializationEventYViewportCoordinate(consequencePtr);
                    drawEventMark(graphics, color, xBegin, xEnd, y);
                }
            }
        }
        else {
            int y = getEventYViewportCoordinate(event.getCPtr());
            drawEventMark(graphics, color, xBegin, xEnd, y);
        }
    }

    /**
     * Draws a single mark at the given coordinates.
     */
    private void drawEventMark(Graphics graphics, Color color, int xBegin, int xEnd, int y) {
        int halfSize = styleProvider.getEventSelectionRadius();
        int size = halfSize * 2;
        graphics.setAlpha(128);
        graphics.setForegroundColor(ColorFactory.WHITE);
        graphics.setLineStyle(SWT.LINE_SOLID);
        graphics.setLineWidth(6);
        graphics.drawArc(xBegin - halfSize, y - halfSize, size, size, 90, 180);
        graphics.drawArc(xEnd - halfSize, y - halfSize, size, size, 270, 180);
        if (xBegin != xEnd) {
            graphics.drawLine(xBegin, y - halfSize, xEnd, y - halfSize);
            graphics.drawLine(xBegin, y + halfSize, xEnd, y + halfSize);
        }
        graphics.setAlpha(255);
        graphics.setForegroundColor(color);
        graphics.setLineWidthFloat(1.5f);
        graphics.drawArc(xBegin - halfSize, y - halfSize, size, size, 90, 180);
        graphics.drawArc(xEnd - halfSize, y - halfSize, size, size, 270, 180);
        if (xBegin != xEnd) {
            graphics.drawLine(xBegin, y - halfSize, xEnd, y - halfSize);
            graphics.drawLine(xBegin, y + halfSize, xEnd, y + halfSize);
        }
    }

    private void drawAxisHeaders(Graphics graphics) {
        AxisHeader rootAxisHeader = getRootAxisHeader();
        if (rootAxisHeader != null) {
            graphics.pushState();
            graphics.setFont(getFont());
            graphics.setBackgroundColor(ColorFactory.LIGHT_CYAN);
            int gutterHeight = getGutterHeight(graphics);
            Rectangle clipping = new Rectangle();
            graphics.getClip(clipping);
            clipping.y += gutterHeight + 1;
            clipping.height -= 2 * gutterHeight + 2;
            graphics.setClip(clipping);
            graphics.translate(0, gutterHeight - (int)getViewportTop());
            drawAxisHeaders(graphics, rootAxisHeader);
            graphics.popState();
        }
    }

    private void drawAxisHeaders(Graphics graphics, AxisHeader axisHeader) {
        if (axisHeader.children.size() != 0)
            drawAxisHeader(graphics, axisHeader);
        for (AxisHeader childAxisHeader : axisHeader.children)
            drawAxisHeaders(graphics, childAxisHeader);
    }

    private void drawAxisHeader(Graphics graphics, AxisHeader axisHeader) {
        Point mouseLocation = toControl(Display.getCurrent().getCursorLocation());
        mouseLocation.y += (int)getViewportTop() - getGutterHeight(graphics);
        Rectangle bounds = axisHeader.bounds;
        Rectangle clipping = new Rectangle();
        graphics.getClip(clipping);
        if (!bounds.contains(mouseLocation.x, mouseLocation.y))
            graphics.setClip(clipping.getIntersection(bounds).expand(1, 1));
        graphics.setForegroundColor(ColorFactory.WHITE);
        graphics.fillGradient(bounds, true);
        if (axisHeader.expandImageBounds != null) {
            Image image = labelProvider.getExpandImage();
            graphics.drawImage(image, axisHeader.expandImageBounds.x, axisHeader.expandImageBounds.y);
        }
        if (axisHeader.collapseImageBounds != null) {
            Image image = labelProvider.getCollapseImage();
            graphics.drawImage(image, axisHeader.collapseImageBounds.x, axisHeader.collapseImageBounds.y);
        }
        graphics.rotate(-90);
        String[] modulePaths = axisHeader.modulePathFragment.split("\\.");
        for (int i = 0; i < axisHeader.labelElementBounds.length; i++) {
            Rectangle moduleLabelBounds = axisHeader.labelElementBounds[i];
            if (i != axisHeader.labelElementBounds.length - 1)
                graphics.drawText(".", -moduleLabelBounds.y, moduleLabelBounds.x);
            if (highlightedObjects.contains(axisHeader.labelElementModules[i]))
                graphics.setForegroundColor(styleProvider.getHighlightColor());
            else
                graphics.setForegroundColor(ColorFactory.BLACK);
            graphics.drawText(modulePaths[i], -moduleLabelBounds.bottom(), moduleLabelBounds.x);
        }
        graphics.rotate(90);
        Image image = labelProvider.getCloseImage();
        if (axisHeader.closeImageBounds.contains(mouseLocation.x, mouseLocation.y))
            graphics.drawImage(image, axisHeader.closeImageBounds.x, axisHeader.closeImageBounds.y);
        graphics.setLineWidthFloat(highlightedObjects.contains(axisHeader) ? 1.5f : 0);
        graphics.setForegroundColor(ColorFactory.BLACK);
        graphics.drawRectangle(bounds);
        graphics.setClip(clipping);
    }

    private void drawAxisInfo(Graphics graphics) {
        int closedAxisCount = eventLogInput.getModuleTreeRoot().getModuleCount() - openAxisModules.size();
        int invisibleAxisCount = openAxisModules.size() - visibleAxisModules.size();
        if (closedAxisCount != 0 || invisibleAxisCount != 0) {
            graphics.pushState();
            graphics.setFont(getFont());
            int gutterHeight = getGutterHeight(graphics);
            Rectangle clipping = new Rectangle();
            graphics.getClip(clipping);
            clipping.y += gutterHeight + 1;
            clipping.height -= 2 * gutterHeight + 2;
            graphics.setClip(clipping);
            graphics.translate(0, gutterHeight);
            String closedAxisCountText = "Closed: " + closedAxisCount + (closedAxisCount == 1 ? " axis" : " axes");
            String invisibleAxisCountText = "Invisible: " + invisibleAxisCount + (invisibleAxisCount == 1 ? " axis" : " axes");
            Point size1 = GraphicsUtils.getTextExtent(graphics, closedAxisCountText);
            Point size2 = GraphicsUtils.getTextExtent(graphics, invisibleAxisCountText);
            Point size = new Point(Math.max(size1.x, size2.x) + 4, size1.y + size2.y + 6); 
            graphics.setBackgroundColor(styleProvider.getInfoBackgroundColor());
            graphics.fillRectangle(0, 0, size.x, size.y);
            graphics.setLineStyle(SWT.LINE_SOLID);
            graphics.setBackgroundColor(styleProvider.getInfoLabelColor());
            graphics.drawRectangle(0, 0, size.x, size.y);
            drawText(graphics, closedAxisCountText, 2, 2);
            drawText(graphics, invisibleAxisCountText, 2, size1.y + 4);
            graphics.popState();
        }
    }

    private void drawAxisLabels(Graphics graphics) {
        graphics.pushState();
        graphics.setFont(getFont());
        graphics.translate(0, getGutterHeight(graphics) - (int)getViewportTop());
        for (Axis axis : getAxes())
            drawAxisLabel(graphics, axis);
        graphics.popState();
    }

    private void drawAxisLabel(Graphics graphics, Axis axis) {
        AxisHeader axisHeader = axis.axisHeader;
        if (axisHeader.module.isCompoundModule()) {
            if (axis.expandImageBounds != null) {
                Image image = labelProvider.getExpandImage();
                graphics.drawImage(image, axis.expandImageBounds.x, axis.expandImageBounds.y);
            }
            if (axis.collapseImageBounds != null) {
                Image image = labelProvider.getCollapseImage();
                graphics.drawImage(image, axis.collapseImageBounds.x, axis.collapseImageBounds.y);
            }
        }
        String[] modulePaths = axisHeader.modulePathFragment.split("\\.");
        for (int i = 0; i < axis.labelElementBounds.length; i++) {
            Rectangle moduleLabelBounds = axis.labelElementBounds[i];
            if (i != axis.labelElementBounds.length - 1)
                graphics.drawText(".", moduleLabelBounds.right(), moduleLabelBounds.y);
            if (highlightedObjects.contains(axisHeader.labelElementModules[i]))
                graphics.setForegroundColor(styleProvider.getHighlightColor());
            else
                graphics.setForegroundColor(ColorFactory.BLACK);
            graphics.drawText(modulePaths[i], moduleLabelBounds.x, moduleLabelBounds.y);
        }
        Image image = labelProvider.getCloseImage();
        Point mouseLocation = toControl(Display.getCurrent().getCursorLocation());
        mouseLocation.y += (int)getViewportTop() - getGutterHeight(graphics);
        if (axis.closeImageBounds.contains(mouseLocation.x, mouseLocation.y))
            graphics.drawImage(image, axis.closeImageBounds.x, axis.closeImageBounds.y);
    }

    /**
     * Either draws to the graphics or matches to a point a single message arrow represented by the given message dependency.
     * It is either drawn as a straight line from the cause event to the consequence event or as a half ellipse if it is a self message.
     * A message dependency pointing too far away in the figure is drawn by a split dotted straight line or a dotted half ellipse.
     * Since it is meaningless and expensive to calculate the exact shapes these are drawn differently.
     *
     * The line buffer is used to skip drawing message arrows where there is one already drawn. (very dense arrows)
     */
    private boolean drawOrFitMessageDependency(Graphics graphics, long messageDependencyPtr, int fitX, int fitY, VLineBuffer vlineBuffer, long startEventPtr, long endEventPtr) {
        long causeEventPtr = sequenceChartFacade.IMessageDependency_getCauseEvent(messageDependencyPtr);
        long consequenceEventPtr = sequenceChartFacade.IMessageDependency_getConsequenceEvent(messageDependencyPtr);

        // events may be omitted from the log
        if (causeEventPtr == 0 || consequenceEventPtr == 0)
            return false;

        // cache message dependency state
        boolean isReuse = sequenceChartFacade.IMessageDependency_isReuse(messageDependencyPtr);
        // TODO: not always BeginSendEntry?!
        long messageEntryPtr = sequenceChartFacade.IMessageDependency_getMessageEntry(messageDependencyPtr);
        long endSendEntryPtr = messageEntryPtr == 0 || !sequenceChartFacade.MessageEntry_isBeginSendEntry(messageEntryPtr) ? 0 : sequenceChartFacade.BeginSendEntry_getEndSendEntry(messageEntryPtr);
        int messageId = messageEntryPtr == 0 ? 0 : sequenceChartFacade.MessageEntry_getMessageId(messageEntryPtr);
        long startEventNumber = sequenceChartFacade.IEvent_getEventNumber(startEventPtr);
        long endEventNumber = sequenceChartFacade.IEvent_getEventNumber(endEventPtr);
        long causeEventNumber = sequenceChartFacade.IEvent_getEventNumber(causeEventPtr);
        long consequenceEventNumber = sequenceChartFacade.IEvent_getEventNumber(consequenceEventPtr);
        boolean isFilteredMessageDependency = sequenceChartFacade.IMessageDependency_isFilteredMessageDependency(messageDependencyPtr);
        int filteredMessageDependencyKind = isFilteredMessageDependency ? sequenceChartFacade.FilteredMessageDependency_getKind(messageDependencyPtr) : FilteredMessageDependency.Kind.UNDEFINED;
        boolean isTransmissionStart = true; // TODO: KLUDGE: revive this when topic/packetprogress is merged into master: endSendEntryPtr == 0 ? false : sequenceChartFacade.EndSendEntry_isTransmissionStart(endSendEntryPtr);
        boolean isReceptionStart = endSendEntryPtr == 0 ? false : sequenceChartFacade.EndSendEntry_isReceptionStart(endSendEntryPtr);

        org.omnetpp.common.engine.BigDecimal transmissionDelay = null;
        if (showTransmissionDurations && !isFilteredMessageDependency && endSendEntryPtr != 0) {
            transmissionDelay = sequenceChartFacade.BeginSendEntry_getTransmissionDelay(messageEntryPtr);
            if (transmissionDelay.equals(org.omnetpp.common.engine.BigDecimal.getZero()))
                transmissionDelay = null;
        }

        // calculate pixel coordinates for message arrow endings
        // TODO: what about integer overflow in (int) casts? now that we have converted to long
        int invalid = Integer.MAX_VALUE;
        int x1 = invalid, x2 = invalid, y1 = invalid, y2 = invalid;
        int fontHeight = getFontHeight(graphics);
        if (isInitializationEvent(causeEventPtr))
            y1 = getInitializationEventYViewportCoordinate(messageDependencyPtr, invalid);
        else if (!isReuse && messageEntryPtr != 0) {
            int moduleIndex = getAxisModuleIndexByModuleId(sequenceChartFacade.EventLogEntry_getContextModuleId(messageEntryPtr));
            if (moduleIndex != -1)
                y1 = getModuleYViewportCoordinateByModuleIndex(moduleIndex);
        }
        else
            y1 = getEventYViewportCoordinate(causeEventPtr, invalid);
        if (isReuse && messageEntryPtr != 0) {
            int moduleIndex = getAxisModuleIndexByModuleId(sequenceChartFacade.EventLogEntry_getContextModuleId(messageEntryPtr));
            if (moduleIndex != -1)
                y2 = getModuleYViewportCoordinateByModuleIndex(moduleIndex);
        }
        else
            y2 = getEventYViewportCoordinate(consequenceEventPtr, invalid);
        // skip if one of the axes is filtered out
        if (y1 == invalid || y2 == invalid)
            return false;
        // TODO: this should take the angle into account
        if (showEventMarks) {
            if (y1 > y2) {
                y1 -= styleProvider.getEventRadius();
                y2 += styleProvider.getEventRadius();
            }
            else if (y1 < y2) {
                y1 += styleProvider.getEventRadius();
                y2 -= styleProvider.getEventRadius();
            }
            else {
                y1 -= styleProvider.getEventRadius();
                y2 -= styleProvider.getEventRadius();
            }
        }
        boolean isSelfArrow = y1 == y2;
        // calculate horizontal coordinates based on timeline coordinate limit
        double timelineCoordinateLimit = getMaximumMessageDependencyDisplayWidth() / getPixelPerTimelineUnit();
        if (consequenceEventNumber < startEventNumber || endEventNumber < consequenceEventNumber) {
            // consequence event is out of drawn message dependency range
            x1 = (int)getEventXViewportCoordinateEnd(causeEventPtr);
            double causeTimelineCoordinate = sequenceChartFacade.getTimelineCoordinateBegin(causeEventPtr);
            double consequenceTimelineCoordinate = sequenceChartFacade.getTimelineCoordinateBegin(consequenceEventPtr, -Double.MAX_VALUE, causeTimelineCoordinate + timelineCoordinateLimit);

            if (Double.isNaN(consequenceTimelineCoordinate) || consequenceTimelineCoordinate > causeTimelineCoordinate + timelineCoordinateLimit)
                x2 = invalid;
            else
                x2 = (int)getEventXViewportCoordinateBegin(consequenceEventPtr);
        }
        else if (causeEventNumber < startEventNumber || endEventNumber < causeEventNumber) {
            // cause event is out of drawn message dependency range
            x2 = (int)getEventXViewportCoordinateBegin(consequenceEventPtr);
            double consequenceTimelineCoordinate = sequenceChartFacade.getTimelineCoordinateBegin(consequenceEventPtr);
            double causeTimelineCoordinate = sequenceChartFacade.getTimelineCoordinateBegin(causeEventPtr, consequenceTimelineCoordinate - timelineCoordinateLimit, Double.MAX_VALUE);

            if (Double.isNaN(causeTimelineCoordinate) || causeTimelineCoordinate < consequenceTimelineCoordinate - timelineCoordinateLimit)
                x1 = invalid;
            else
                x1 = (int)getEventXViewportCoordinateEnd(causeEventPtr);
        }
        else {
            // both events are inside
            x1 = (int)getEventXViewportCoordinateEnd(causeEventPtr);
            x2 = (int)getEventXViewportCoordinateBegin(consequenceEventPtr);
            if (messageEntryPtr != 0 && !isReuse)
                x1 = (int)getEventLogEntryXViewportCoordinateBegin(messageEntryPtr);
            if (messageEntryPtr != 0 && isReuse)
                x2 = (int)getEventLogEntryXViewportCoordinateBegin(messageEntryPtr);

            double causeTimelineCoordinate = sequenceChartFacade.getTimelineCoordinateBegin(causeEventPtr);
            double consequenceTimelineCoordinate = sequenceChartFacade.getTimelineCoordinateBegin(consequenceEventPtr);

            if (consequenceTimelineCoordinate - causeTimelineCoordinate > timelineCoordinateLimit) {
                int viewportCenter = getViewportWidth() / 2;

                if (Math.abs(viewportCenter - x1) < Math.abs(viewportCenter - x2))
                    x2 = invalid;
                else
                    x1 = invalid;
            }
        }
        // at least one of the events must be in range or we don't draw anything
        if (x1 == invalid && x2 == invalid)
            return false;
        // line color and style depends on message kind
        if (isFilteredMessageDependency) {
            switch (filteredMessageDependencyKind) {
                case FilteredMessageDependency.Kind.SENDS:
                    if ((isSelfArrow && !showSelfMessageSends) || (!isSelfArrow && !showMessageSends))
                        return false;
                    break;
                case FilteredMessageDependency.Kind.REUSES:
                    if ((isSelfArrow && !showSelfMessageReuses) || (!isSelfArrow && !showMessageReuses))
                        return false;
                    break;
                case FilteredMessageDependency.Kind.MIXED:
                    if ((isSelfArrow && !showMixedSelfMessageDependencies) || (!isSelfArrow && !showMixedMessageDependencies))
                        return false;
                    break;
                default:
                    throw new RuntimeException("Unknown kind");
            }
        }
        else {
            if (isReuse) {
                if ((isSelfArrow && !showSelfMessageReuses) || (!isSelfArrow && !showMessageReuses))
                    return false;
            }
            else {
                if ((isSelfArrow && !showSelfMessageSends) || (!isSelfArrow && !showMessageSends))
                    return false;
            }
        }
        if (graphics != null) {
            graphics.setLineStyle(styleProvider.getMessageDependencyLineStyle(messageDependencyPtr));
            int[] lineDash = styleProvider.getMessageDependencyLineDash(messageDependencyPtr);
            if (lineDash != null)
                graphics.setLineDash(lineDash);
        }
        // and finally the actual drawing
        int longMessageArrowWidth = styleProvider.getLongMessageArrowWidth();
        if (isSelfArrow) {
            long eventNumberDelta = messageId + consequenceEventNumber - causeEventNumber;
            int numberOfPossibleEllipseHeights = Math.max(1, (int)Math.round((getAxisSpacing() - fontHeight) / (fontHeight + 10)));
            int halfEllipseHeight = (int)Math.max(getAxisSpacing() * (eventNumberDelta % numberOfPossibleEllipseHeights + 1) / (numberOfPossibleEllipseHeights + 1), styleProvider.getMinimumHalfEllipseHeight());

            // test if it is a vertical line (as zero-width half ellipse)
            if (x1 == x2) {
                y2 = y1 - halfEllipseHeight;

                if (graphics != null) {
                    if (vlineBuffer.vlineContainsNewPixel(x1, y2, y1))
                        graphics.drawLine(x1, y1, x2, y2);

                    if (showArrowHeads)
                        drawArrowHead(graphics, null, x1, y1, 0, 1);

                    if (showMessageNames)
                        drawMessageDependencyLabel(graphics, messageDependencyPtr, x1 + 2, y1 - fontHeight);
                }
                else
                    return lineContainsPoint(x1, y1, x2, y2, fitX, fitY, MOUSE_TOLERANCE);
            }
            else {
                boolean showArrowHeads = this.showArrowHeads;
                int xm, ym = y1 - halfEllipseHeight;

                // cause is too far away
                if (x1 == invalid) {
                    x1 = x2 - longMessageArrowWidth;
                    xm = x1 + longMessageArrowWidth / 2 ;

                    // draw quarter ellipse starting with a horizontal straight line from the left
                    Rectangle.SINGLETON.setLocation(x2 - longMessageArrowWidth, ym);
                    Rectangle.SINGLETON.setSize(longMessageArrowWidth, halfEllipseHeight * 2);

                    if (graphics != null) {
                        graphics.drawArc(Rectangle.SINGLETON, 0, 90);
                        graphics.setLineStyle(SWT.LINE_DOT);
                        graphics.drawLine(x1, ym, xm, ym);

                        if (isFilteredMessageDependency)
                            drawFilteredMessageDependencySign(graphics, x1, ym, x2, ym);
                    }
                    else
                        return lineContainsPoint(x1, ym, xm, ym, fitX, fitY, MOUSE_TOLERANCE) ||
                               halfEllipseContainsPoint(1, x1, x2, y1, halfEllipseHeight, fitX, fitY, MOUSE_TOLERANCE);
                }
                // consequence is too far away
                else if (x2 == invalid) {
                    x2 = x1 + longMessageArrowWidth;
                    xm = x1 + longMessageArrowWidth / 2;

                    // draw quarter ellipse ending in a horizontal straight line to the right
                    Rectangle.SINGLETON.setLocation(x1, ym);
                    Rectangle.SINGLETON.setSize(longMessageArrowWidth, halfEllipseHeight * 2);

                    if (graphics != null) {
                        graphics.drawArc(Rectangle.SINGLETON, 90, 90);
                        graphics.setLineStyle(SWT.LINE_DOT);
                        graphics.drawLine(xm, ym, x2, ym);

                        if (isFilteredMessageDependency)
                            drawFilteredMessageDependencySign(graphics, x1, ym, x2, ym);

                        if (showArrowHeads)
                            drawArrowHead(graphics, styleProvider.getLongArrowheadColor(), x2, ym, 1, 0);

                        showArrowHeads = false;
                    }
                    else
                        return lineContainsPoint(xm, ym, x2, ym, fitX, fitY, MOUSE_TOLERANCE) ||
                               halfEllipseContainsPoint(0, x1, x2, y1, halfEllipseHeight, fitX, fitY, MOUSE_TOLERANCE);
                }
                // both events are close enough
                else {
                    // draw half ellipse
                    Rectangle.SINGLETON.setLocation(x1, ym);
                    Rectangle.SINGLETON.setSize(x2 - x1, halfEllipseHeight * 2);

                    if (graphics != null) {
                        graphics.drawArc(Rectangle.SINGLETON, 0, 180);

                        if (isFilteredMessageDependency)
                            drawFilteredMessageDependencySign(graphics, x1, ym, x2, ym);
                    }
                    else
                        return halfEllipseContainsPoint(-1, x1, x2, y1, halfEllipseHeight, fitX, fitY, MOUSE_TOLERANCE);
                }

                if (showArrowHeads) {
                    // intersection of the ellipse and a circle with the arrow length centered at the end point
                    // origin is in the center of the ellipse
                    // mupad: solve([x^2/a^2+(r^2-(x-a)^2)/b^2=1],x,IgnoreSpecialCases)
                    double a = Rectangle.SINGLETON.width / 2;
                    double b = Rectangle.SINGLETON.height / 2;
                    double a2 = a * a;
                    double b2 = b * b;
                    double r = styleProvider.getArrowheadLength();
                    double r2 = r *r;
                    double x = a == b ? (2 * a2 - r2) / 2 / a : a * (-Math.sqrt(a2 * r2 + b2 * b2 - b2 * r2) + a2) / (a2 - b2);
                    double y = -Math.sqrt(r2 - (x - a) * (x - a));

                    // if the solution falls outside of the top right quarter of the ellipse
                    if (x < 0)
                        drawArrowHead(graphics, null, x2, y2, 0, 1);
                    else {
                        // shift solution to the coordinate system of the canvas
                        x = (x1 + x2) / 2 + x;
                        y = y1 + y;
                        drawArrowHead(graphics, null, x2, y2, x2 - x, y2 - y);
                    }
                }

                if (showMessageNames)
                    drawMessageDependencyLabel(graphics, messageDependencyPtr, (x1 + x2) / 2, y1 - halfEllipseHeight - fontHeight);
            }
        }
        else {
            int y = (y2 + y1) / 2;
            Color arrowHeadFillColor = null;

            // cause is too far away
            if (x1 == invalid) {
                x1 = x2 - longMessageArrowWidth * 2;

                if (transmissionDelay != null) {
                    if (!isReceptionStart)
                        x1 -= longMessageArrowWidth;
                    else
                        x1 += longMessageArrowWidth;
                }

                if (graphics != null) {
                    int xm = x2 - longMessageArrowWidth;

                    if (transmissionDelay != null) {
                        if (!isReceptionStart)
                            xm -= longMessageArrowWidth / 2;
                        else
                            xm += longMessageArrowWidth / 2;

                        drawTransmissionDuration(graphics, causeEventPtr, consequenceEventPtr, transmissionDelay, isTransmissionStart, isReceptionStart, x1, y1, x2, y2, true, true);
                    }

                    graphics.drawLine(xm, y, x2, y2);
                    graphics.setLineStyle(SWT.LINE_DOT);
                    graphics.drawLine(x1, y1, xm, y);
                }
            }
            // consequence is too far away
            else if (x2 == invalid) {
                x2 = x1 + longMessageArrowWidth * 2;

                if (transmissionDelay != null) {
                    if (!isReceptionStart)
                        x2 += longMessageArrowWidth;
                    else
                        x2 -= longMessageArrowWidth;
                }

                if (graphics != null) {
                    int xm = x1 + longMessageArrowWidth;

                    if (transmissionDelay != null) {
                        if (!isReceptionStart)
                            xm += longMessageArrowWidth / 2;
                        else
                            xm -= longMessageArrowWidth / 2;

                        drawTransmissionDuration(graphics, causeEventPtr, consequenceEventPtr, transmissionDelay, isTransmissionStart, isReceptionStart, x1, y1, x2, y2, true, false);
                    }

                    graphics.drawLine(x1, y1, xm, y);
                    graphics.setLineStyle(SWT.LINE_DOT);
                    graphics.drawLine(xm, y, x2, y2);
                    arrowHeadFillColor = styleProvider.getLongArrowheadColor();
                }
            }
            // both events are in range
            else {
                if (graphics != null) {
                    if (x1 != x2 || vlineBuffer.vlineContainsNewPixel(x1, y1, y2))
                    {
                        if (transmissionDelay != null)
                            drawTransmissionDuration(graphics, causeEventPtr, consequenceEventPtr, transmissionDelay, isTransmissionStart, isReceptionStart, x1, y1, x2, y2, false, false);

                        graphics.drawLine(x1, y1, x2, y2);
                    }
                }
            }

            if (graphics == null)
                return lineContainsPoint(x1, y1, x2, y2, fitX, fitY, MOUSE_TOLERANCE);

            if (graphics != null && isFilteredMessageDependency)
                drawFilteredMessageDependencySign(graphics, x1, y1, x2, y2);

            if (showArrowHeads)
                drawArrowHead(graphics, arrowHeadFillColor, x2, y2, x2 - x1, y2 - y1);

            if (showMessageNames) {
                Point position = labelPositions.get(messageDependencyPtr);
                if (position != null)
                    drawMessageDependencyLabel(graphics, messageDependencyPtr, position.x, position.y);
                else {
                    int rowCount = Math.min(15, Math.max(1, Math.abs(y2 - y1) / fontHeight));
                    int mx = (x1 + x2) / 2;
                    int my = (y1 + y2) / 2;
                    if (labelQuadTree.getSize() < 1000) {
                        if (styleProvider.getMessageDependencyLabelFont(messageDependencyPtr) != null)
                            graphics.setFont(styleProvider.getMessageDependencyLabelFont(messageDependencyPtr));
                        else
                            graphics.setFont(getFont());
                        String labelText = labelProvider.getMessageDependencyLabel(messageDependencyPtr);
                        Point labelSize = GraphicsUtils.getTextExtent(graphics, labelText);
                        Point bestPosition = null;
                        int bestTotalIntersectionArea = Integer.MAX_VALUE;
                        for (int i = 0; i < rowCount; i++) {
                            int rowIndex = ((rowCount + (i % 2 == 0 ? i : -i) - (rowCount % 2)) / 2) % rowCount - rowCount / 2;
                            int dy = rowIndex * fontHeight / 2;
                            int dx = y2 == y1 ? 0 : (int)((double)(x2 - x1) / (y2 - y1) * dy);
                            Point labelPosition = new Point(mx + dx + 3, my + dy - (y1 < y2 ? fontHeight : 0));
                            Rectangle labelRectangle = new Rectangle(labelPosition.x, labelPosition.y, labelSize.x, labelSize.y);
                            int[] totalIntersectionArea = new int[] {0};
                            labelQuadTree.query(labelRectangle, (Rectangle region, Object object) -> {
                                Rectangle r = region.getIntersection(labelRectangle);
                                totalIntersectionArea[0] += r.width * r.height;
                            });
                            if (totalIntersectionArea[0] < bestTotalIntersectionArea) {
                                bestTotalIntersectionArea = totalIntersectionArea[0];
                                bestPosition = labelPosition;
                                if (bestTotalIntersectionArea == 0)
                                    break;
                            }
                        }
                        labelPositions.put(messageDependencyPtr, bestPosition);
                        labelQuadTree.insert(new Rectangle(bestPosition.x, bestPosition.y, labelSize.x, labelSize.y), labelText);
                        drawMessageDependencyLabel(graphics, messageDependencyPtr, bestPosition.x, bestPosition.y);
                    }
                    else {
                        int rowIndex = ((int)(messageId + causeEventNumber + consequenceEventNumber) % rowCount) - rowCount / 2;
                        int dy = rowIndex * fontHeight / 2;
                        int dx = y2 == y1 ? 0 : (int)((double)(x2 - x1) / (y2 - y1) * dy);
                        drawMessageDependencyLabel(graphics, messageDependencyPtr, mx + dx + 3, my + dy - (y1 < y2 ? fontHeight : 0));
                    }
                }
            }
        }

        // when fitting we should have already returned
        Assert.isTrue(graphics != null);

        return false;
    }

    /**
     * Draws a semi-transparent region to represent a transmission duration.
     * The coordinates specify the arrow that will be draw on top of the transmission duration area.
     */
    private void drawTransmissionDuration(Graphics graphics, long causeEventPtr, long consequenceEventPtr, org.omnetpp.common.engine.BigDecimal transmissionDelay, boolean isTransmissionStart, boolean isReceptionStart, int xCause, int y1, int xConsequence, int y2, boolean splitArrow, boolean endingSplit) {
        int causeModuleId = sequenceChartFacade.IEvent_getModuleId(causeEventPtr);
        int consequenceModuleId = sequenceChartFacade.IEvent_getModuleId(consequenceEventPtr);

        org.omnetpp.common.engine.BigDecimal t3 = isTransmissionStart ?
            sequenceChartFacade.IEvent_getSimulationTime(causeEventPtr).add(transmissionDelay) :
            sequenceChartFacade.IEvent_getSimulationTime(causeEventPtr);
        org.omnetpp.common.engine.BigDecimal t4 = isReceptionStart ?
            sequenceChartFacade.IEvent_getSimulationTime(consequenceEventPtr).add(transmissionDelay) :
            sequenceChartFacade.IEvent_getSimulationTime(consequenceEventPtr).subtract(transmissionDelay);

        // check simulation times for being out of range
        org.omnetpp.common.engine.BigDecimal lastEventSimulationTime = eventLog.getLastEvent().getSimulationTime();
        if (t3.greater(lastEventSimulationTime) || t4.greater(lastEventSimulationTime) || t4.less(org.omnetpp.common.engine.BigDecimal.getZero()))
            return;

        int x1 = isTransmissionStart ? xCause : (int)getViewportCoordinateForTimelineCoordinate(sequenceChartFacade.getTimelineCoordinateForSimulationTimeAndEventInModule(sequenceChartFacade.IEvent_getSimulationTime(causeEventPtr).subtract(transmissionDelay), causeModuleId));
        int x2 = isReceptionStart ? xConsequence : (int)getViewportCoordinateForTimelineCoordinate(sequenceChartFacade.getTimelineCoordinateForSimulationTimeAndEventInModule(sequenceChartFacade.IEvent_getSimulationTime(consequenceEventPtr).subtract(transmissionDelay), consequenceModuleId));;
        int x3 = !isTransmissionStart ? xCause : (int)getViewportCoordinateForTimelineCoordinate(sequenceChartFacade.getTimelineCoordinateForSimulationTimeAndEventInModule(t3, causeModuleId));
        int x4 = !isReceptionStart ? xConsequence : (int)getViewportCoordinateForTimelineCoordinate(sequenceChartFacade.getTimelineCoordinateForSimulationTimeAndEventInModule(t4, consequenceModuleId));
        int xLimit = getViewportWidth();
        boolean drawStrips = false;

        int longMessageArrowWidth = styleProvider.getLongMessageArrowWidth();
        if (isReceptionStart && (x3 - x1 > xLimit || x4 - x2 > xLimit)) {
            x3 = x1 + longMessageArrowWidth;
            x4 = x2 + longMessageArrowWidth;
            drawStrips = true;
        }

        if (splitArrow) {
            if (isReceptionStart) {
                x3 = x1 + longMessageArrowWidth;
                x4 = x2 + longMessageArrowWidth;
            }
            else {
                if (endingSplit) {
                    x3 = x1 + longMessageArrowWidth * 2;
                    x4 = x2 - longMessageArrowWidth;
                    x1 += longMessageArrowWidth;
                }
                else {
                    x3 = x1 + longMessageArrowWidth;
                    x4 = x2 - longMessageArrowWidth * 2;
                    x2 -= longMessageArrowWidth;
                }
            }
            drawStrips = true;
        }

        // create the polygon
        int[] points = new int[8];
        points[0] = x1;
        points[1] = y1;
        points[2] = x2;
        points[3] = y2;
        points[4] = x4;
        points[5] = y2;
        points[6] = x3;
        points[7] = y1;

        // draw it
        graphics.pushState();
        graphics.setAlpha(64);
        graphics.setBackgroundColor(graphics.getForegroundColor());
        graphics.setForegroundColor(ColorFactory.BLACK);

        if (drawStrips) {
            graphics.fillPolygon(points);

            int shift = isReceptionStart ? longMessageArrowWidth : endingSplit ? -longMessageArrowWidth * 2 : 0;
            points[0] += shift;
            points[2] += shift;

            int numberOfStrips = 4;
            int s = longMessageArrowWidth / numberOfStrips;

            for (int i = 0; i < numberOfStrips; i++) {
                points[4] = points[2] + s;
                points[6] = points[0] + s;

                if (i % 2 == (!isReceptionStart && endingSplit ? 0 : 1))
                    graphics.fillPolygon(points);

                points[0] += s;
                points[2] += s;
            }
        }
        else {
            graphics.fillPolygon(points);
            graphics.drawPolygon(points);
        }
        graphics.popState();
    }

    /**
     * Draws a message arrow label with the corresponding message line color.
     */
    private void drawMessageDependencyLabel(Graphics graphics, long messageDependencyPtr, int x, int y) {
        if (sequenceChartFacade.IMessageDependency_isFilteredMessageDependency(messageDependencyPtr))
            // not to overlap with filtered sign
            x += 7;
        if (styleProvider.getMessageDependencyLabelFont(messageDependencyPtr) != null)
            graphics.setFont(styleProvider.getMessageDependencyLabelFont(messageDependencyPtr));
        else
            graphics.setFont(getFont());
        drawText(graphics, labelProvider.getMessageDependencyLabel(messageDependencyPtr), x, y);
    }

    // KLUDGE: This is a workaround for SWT bug https://bugs.eclipse.org/215243
    private void drawText(Graphics g, String s, int x, int y) {
        g.drawText(s, x, y);
        // KLUDGE: clear the cairo lib internal state (on Linux)
        if ("gtk".equals(SWT.getPlatform()))
            g.drawPoint(-1000000, -1000000);
    }

    /**
     * Draws a zig-zag sign at the middle point of the given line segment.
     */
    private void drawFilteredMessageDependencySign(Graphics graphics, int x1, int y1, int x2, int y2) {
        int size = 5;
        int spacing = 4;
        int count = 2;
        int halfSize = size / 2;
        int halfSpacing = spacing / 2;
        int x = (x1 + x2) / 2;
        int y = (y1 + y2) / 2;
        int yy1 = - size - halfSize;
        int yy2 = yy1 + size;
        int yy3 = yy2 + size;
        int yy4 = yy3 + size;

        // transform coordinates so that zigzag will be orthogonal to x1, y1, x2, y2
        double dx = x2 - x1;
        double dy = y2 - y1;
        double length = Math.sqrt(dx * dx + dy * dy);
        dx /= length;
        dy /= length;
        float angle = (float)(180 * Math.atan2(dy, dx) / Math.PI);

        Transform transform = new Transform(null);
        transform.translate(x, y);
        transform.rotate(angle);

        // draw some zig zags
        graphics.setLineStyle(SWT.LINE_SOLID);
        for (int i = 0; i < count; i++) {
            int xx1 = - halfSize - halfSpacing + i * spacing;
            int xx2 = xx1 + size;

            float[] points = new float[] {xx1, yy1, xx2, yy2, xx1, yy3, xx2, yy4};
            transform.transform(points);

            int[] ps = new int[8];
            for (int j = 0; j < 8; j++)
                ps[j] = Math.round(points[j]);
            graphics.drawPolyline(ps);
        }
    }

    /**
     * Draws an arrow head at the given location in the given direction.
     *
     * @param graphics
     * @param x the x coordinate of the arrow's end
     * @param y the y coordinate of the arrow's end
     * @param dx the x coordinate of the direction vector
     * @param dy the y coordinate of the direction vector
     */
    private void drawArrowHead(Graphics graphics, Color fillColor, int x, int y, double dx, double dy) {
        double n = Math.sqrt(dx * dx + dy * dy);
        int arrowheadWidth = styleProvider.getArrowheadWidth();
        int arrowheadLength = styleProvider.getArrowheadLength();
        double dwx = -dy / n * arrowheadWidth / 2;
        double dwy = dx / n * arrowheadWidth / 2;
        double xt = x - dx * arrowheadLength / n;
        double yt = y - dy * arrowheadLength / n;
        int x1 = (int)Math.round(xt - dwx);
        int y1 = (int)Math.round(yt - dwy);
        int x2 = (int)Math.round(xt + dwx);
        int y2 = (int)Math.round(yt + dwy);
        graphics.setLineWidth(1);
        graphics.setLineStyle(SWT.LINE_SOLID);
        graphics.setBackgroundColor(fillColor == null ? graphics.getForegroundColor() : fillColor);
        graphics.fillPolygon(new int[] {x, y, x1, y1, x2, y2});
        graphics.drawPolygon(new int[] {x, y, x1, y1, x2, y2});
    }

    private double getAxisHeaderHeight(AxisHeader axisHeader) {
        if (axisHeader.children.size() == 0)
            return getAxisSpacing() + getModuleIdToAxisRendererMap().get(axisHeader.module.getModuleId()).getHeight();
        else {
            double height = 0;
            for (AxisHeader childAxisHeader : axisHeader.children)
                height += getAxisHeaderHeight(childAxisHeader);
            return height;
        }
    }

    private int getAxisHeaderWidth(AxisHeader axisHeader) {
        if (axisHeader.children.size() == 0)
            return 0;
        else {
            int width = 0;
            for (AxisHeader childAxisHeader : axisHeader.children) {
                int childWidth = getAxisHeaderWidth(childAxisHeader);
                if (childWidth > width)
                    width = childWidth;
            }
            return axisHeader.bounds.width + width;
        }
    }

    /*************************************************************************************
     * COLLECTING OBJECTS
     */

    /**
     * Determines what is drawn at the given viewport position.
     */
    public ArrayList<Object> collectVisibleObjectsAtPosition(final int viewportX, final int viewportY) {
        ArrayList<Object> result = new ArrayList<Object>();
        if (!eventLogInput.isCanceled() && !eventLogInput.isLongRunningOperationInProgress()) {
            eventLogInput.runWithProgressMonitor(new Runnable() {
                public void run() {
                    if (eventLog != null) {
                        long startMillis = System.currentTimeMillis();
                        if (debug)
                            Debug.println("collectVisibleObjectsAtPosition(): enter");

                        int x = viewportX;
                        int y = viewportY - getGutterHeight(null);

                        if (showEventMarks) {
                            long[] eventPtrRange = getFirstLastEventPtrForViewportRange(0, getViewportWidth());
                            long startEventPtr = eventPtrRange[0];
                            long endEventPtr = eventPtrRange[1];

                            if (startEventPtr != 0 && endEventPtr != 0) {
                                for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.IEvent_getNextEvent(eventPtr)) {
                                    int xBegin = (int)getEventXViewportCoordinateBegin(eventPtr);
                                    int xEnd = (int)getEventXViewportCoordinateEnd(eventPtr);
                                    if (isInitializationEvent(eventPtr)) {
                                        if (showInitializationEvent) {
                                            for (int i = 0; i < sequenceChartFacade.IEvent_getNumConsequences(eventPtr); i++) {
                                                long consequencePtr = sequenceChartFacade.IEvent_getConsequence(eventPtr, i);
                                                int yInitializationEvent = getInitializationEventYViewportCoordinate(consequencePtr, -1);
                                                if (yInitializationEvent != -1 && eventSymbolContainsPoint(x, y, xBegin, xEnd, yInitializationEvent, MOUSE_TOLERANCE + 3))
                                                    result.add(sequenceChartFacade.IEvent_getEvent(eventPtr));
                                            }
                                        }
                                    }
                                    else if (getEventAxisModuleIndex(eventPtr) != -1) {
                                        if (eventSymbolContainsPoint(x, y, xBegin, xEnd, getEventYViewportCoordinate(eventPtr), MOUSE_TOLERANCE + 3))
                                            result.add(sequenceChartFacade.IEvent_getEvent(eventPtr));
                                    }

                                    if (eventPtr == endEventPtr)
                                        break;
                                }
                            }
                        }

                        if (showMessageSends || showMessageReuses) {
                            long[] eventPtrRange = getFirstLastEventPtrForMessageDependencies();
                            long startEventPtr = eventPtrRange[0];
                            long endEventPtr = eventPtrRange[1];

                            if (startEventPtr != 0 && endEventPtr != 0) {
                                PtrVector messageDependencies = sequenceChartFacade.getIntersectingMessageDependencies(startEventPtr, endEventPtr);

                                for (int i = 0; i < messageDependencies.size(); i++) {
                                    long messageDependencyPtr = messageDependencies.get(i);
                                    long causeEventPtr = sequenceChartFacade.IMessageDependency_getCauseEvent(messageDependencyPtr);
                                    if (!isInitializationEvent(causeEventPtr) || showInitializationEvent) {
                                        if (drawOrFitMessageDependency(null, messageDependencyPtr, x, y, null, startEventPtr, endEventPtr))
                                            result.add(sequenceChartFacade.IMessageDependency_getMessageDependency(messageDependencyPtr));
                                    }
                                }
                            }
                        }

                        if (showAxisHeaders)
                            collectAxisHeaders(x, y + (int)getViewportTop(), result);
                        if (showAxisLabels)
                            collectAxisLabels(x, y + (int)getViewportTop(), result);

                        if (showAxes) {
                            for (Axis axis : getAxes()) {
                                ModuleTreeItem axisModule = axis.axisHeader.module;
                                IAxisRenderer axisRenderer = axis.axisRenderer;
                                int axisY = axis.y - (int)getViewportTop();
                                if (axisY - MOUSE_TOLERANCE <= y && y <= axisY + axisRenderer.getHeight() + MOUSE_TOLERANCE)
                                    result.add(axisModule);
                            }
                        }

                        if (showComponentMethodCalls) {
                            long[] eventPtrRange = getFirstLastEventPtrForViewportRange(0, getViewportWidth());
                            long startEventPtr = eventPtrRange[0];
                            long endEventPtr = eventPtrRange[1];

                            if (startEventPtr != 0 && endEventPtr != 0) {
                                PtrVector componentMethodBeginEntries = sequenceChartFacade.getComponentMethodBeginEntries(startEventPtr, endEventPtr);

                                for (int i = 0; i < componentMethodBeginEntries.size(); i++) {
                                    long componentMethodBeginEntryPtr = componentMethodBeginEntries.get(i);

                                    if (drawOrFitComponentMethodCall(null, componentMethodBeginEntryPtr, x, y))
                                        result.add((ComponentMethodBeginEntry)sequenceChartFacade.EventLogEntry_getEventLogEntry(componentMethodBeginEntryPtr));
                                }
                            }
                        }

                        result.add(getTimelineCoordinateForViewportCoordinate(x));

                        long totalMillis = System.currentTimeMillis() - startMillis;
                        if (debug)
                            Debug.println("collectVisibleObjectsAtPosition(): leave after " + totalMillis + "ms - " + result.size() + " objects");
                    }
                }
            });
        }
        return result;
    }

    private void collectAxisHeaders(int x, int y, ArrayList<Object> objects) {
        if (getRootAxisHeader() != null)
            collectAxisHeaders(getRootAxisHeader(), x, y, objects);
    }

    private void collectAxisHeaders(AxisHeader axisHeader, int x, int y, ArrayList<Object> objects) {
        if (axisHeader.children.size() != 0)
            collectAxisHeader(axisHeader, x, y, objects);
        for (AxisHeader childAxisHeader : axisHeader.children)
            collectAxisHeaders(childAxisHeader, x, y, objects);
    }

    private void collectAxisHeader(AxisHeader axisHeader, int x, int y, ArrayList<Object> objects) {
        Rectangle bounds = axisHeader.bounds;
        if (axisHeader.module.isCompoundModule()) {
            if (axisHeader.expandImageBounds != null && axisHeader.expandImageBounds.contains(x, y))
                objects.add(new ModuleAction(axisHeader.module, axisHeader, labelProvider.getExpandImage()));
            if (axisHeader.collapseImageBounds != null && axisHeader.collapseImageBounds.contains(x, y))
                objects.add(new ModuleAction(axisHeader.module, axisHeader, labelProvider.getCollapseImage()));
        }
        if (axisHeader.closeImageBounds.contains(x, y))
            objects.add(new ModuleAction(axisHeader.module, axisHeader, labelProvider.getCloseImage()));
        boolean found = false;
        for (int i = 0; i < axisHeader.labelElementBounds.length; i++) {
            if (axisHeader.labelElementBounds[i].contains(x, y) && axisHeader.labelElementModules[i] != null && !objects.contains(axisHeader.labelElementModules[i])) {
                objects.add(axisHeader.labelElementModules[i]);
                found = true;
            }
        }
        if (!found && bounds.contains(x, y)) {
            objects.add(axisHeader);
            if (!objects.contains(axisHeader.module))
                objects.add(axisHeader.module);
        }
    }

    private void collectAxisLabels(int x, int y, ArrayList<Object> objects) {
        for (Axis axis : getAxes())
            collectAxisLabel(axis, x, y, objects);
    }

    private void collectAxisLabel(Axis axis, int x, int y, ArrayList<Object> objects) {
        AxisHeader axisHeader = axis.axisHeader;
        if (axisHeader.module.isCompoundModule()) {
            if (axis.expandImageBounds != null && axis.expandImageBounds.contains(x, y))
                objects.add(new ModuleAction(axisHeader.module, axis, labelProvider.getExpandImage()));
            if (axis.collapseImageBounds != null && axis.collapseImageBounds.contains(x, y))
                objects.add(new ModuleAction(axisHeader.module, axis, labelProvider.getCollapseImage()));
        }
        if (axis.closeImageBounds.contains(x, y))
            objects.add(new ModuleAction(axisHeader.module, axis, labelProvider.getCloseImage()));
        for (int i = 0; i < axis.labelElementBounds.length; i++)
            if (axis.labelElementBounds[i].contains(x, y) && axisHeader.labelElementModules[i] != null)
                objects.add(axisHeader.labelElementModules[i]);
    }

    /**
     * Determines whether the given point is "on" the symbol representing the event.
     */
    private boolean eventSymbolContainsPoint(int xPoint, int yPoint, int xBegin, int xEnd, int y, int tolerance) {
        return xBegin - tolerance <= xPoint && xPoint <= xEnd + tolerance && Math.abs(yPoint - y) <= tolerance;
    }

    /**
     * Determines whether the given point is "on" the half ellipse.
     */
    private boolean halfEllipseContainsPoint(int quarter, int x1, int x2, int y, int height, int px, int py, int tolerance) {
        tolerance++;

        int x;
        int xm = (x1 + x2) / 2;
        int width;

        switch (quarter) {
            case 0:
                x = x1;
                width = (x2 - x1) / 2;
                break;
            case 1:
                x = (x1 + x2) / 2;
                width = (x2 - x1) / 2;
                break;
            default:
                x = x1;
                width = x2 - x1;
                break;
        }

        Rectangle.SINGLETON.setLocation(x, y - height);
        Rectangle.SINGLETON.setSize(width, height);
        Rectangle.SINGLETON.expand(tolerance, tolerance);

        if (!Rectangle.SINGLETON.contains(px, py))
            return false;

        x = xm;
        int rx = Math.abs(x1 - x2) / 2;
        int ry = height;

        if (rx == 0)
            return true;

        int dxnorm = (x - px) * ry / rx;
        int dy = y - py;
        int distSquare = dxnorm * dxnorm + dy * dy;

        return distSquare < (ry + tolerance) * (ry + tolerance) && distSquare > (ry - tolerance) * (ry - tolerance);
    }

    /**
     * Utility function, copied from org.eclipse.draw2d.Polyline.
     */
    private boolean lineContainsPoint(int x1, int y1, int x2, int y2, int px, int py, int tolerance) {
        Rectangle.SINGLETON.setSize(0, 0);
        Rectangle.SINGLETON.setLocation(x1, y1);
        Rectangle.SINGLETON.union(x2, y2);
        Rectangle.SINGLETON.expand(tolerance, tolerance);
        if (!Rectangle.SINGLETON.contains(px, py))
            return false;

        int v1x, v1y, v2x, v2y;
        int numerator, denominator;
        int result = 0;

        // calculates the length squared of the cross product of two vectors, v1 & v2.
        if (x1 != x2 && y1 != y2) {
            v1x = x2 - x1;
            v1y = y2 - y1;
            v2x = px - x1;
            v2y = py - y1;

            numerator = v2x * v1y - v1x * v2y;

            denominator = v1x * v1x + v1y * v1y;

            result = (int)((long)numerator * numerator / denominator);
        }

        // if it is the same point, and it passes the bounding box test,
        // the result is always true.
        return result <= tolerance * tolerance;
    }

    /*************************************************************************************
     * EVENT RANGE HANDLING
     */

    /**
     * Determines the event range that covers the given viewport range.
     * Returns an array of size 2 with the two event pointers, may return null pointers.
     */
    private long[] getFirstLastEventPtrForViewportRange(long x1, long x2) {
        if (eventLog == null || eventLog.isEmpty() || eventLog.getApproximateNumberOfEvents() == 0)
            return new long[] {0, 0};
        else {
            double leftTimelineCoordinate = getTimelineCoordinateForViewportCoordinate(x1);
            double rightTimelineCoordinate = getTimelineCoordinateForViewportCoordinate(x2);

            IEvent startEvent = sequenceChartFacade.getLastEventNotAfterTimelineCoordinate(leftTimelineCoordinate);
            if (startEvent == null)
                startEvent = eventLog.getFirstEvent();

            IEvent endEvent = sequenceChartFacade.getFirstEventNotBeforeTimelineCoordinate(rightTimelineCoordinate);
            if (endEvent == null)
                endEvent = eventLog.getLastEvent();

            return new long[] {startEvent == null ? 0 : startEvent.getCPtr(), endEvent == null ? 0 : endEvent.getCPtr()};
        }
    }

    /**
     * Determines the event range that covers all message dependencies for the current viewport.
     */
    private long[] getFirstLastEventPtrForMessageDependencies() {
        int width = getViewportWidth();
        int maximumWidth = getMaximumMessageDependencyDisplayWidth();
        int extraWidth = (maximumWidth - width) / 2;
        return getFirstLastEventPtrForViewportRange(-extraWidth, extraWidth * 2);
    }

    /**
     * Returns the maximum width after which message dependencies are drawn as a split arrow.
     */
    private int getMaximumMessageDependencyDisplayWidth() {
        return 3 * getViewportWidth();
    }

    private int getAxisModuleIndexByModuleId(int moduleId) {
        // NOTE: a module may be filtered out even if there's a need to draw something related to it
        Integer moduleIndex = getModuleIdToAxisModuleIndexMap().get(moduleId);
        if (moduleIndex == null)
            return -1;
        else
            return moduleIndex;
    }

    private int getModuleYViewportCoordinateByModuleIndex(int index) {
        Axis axis = getAxis(index);
        return axis.y + axis.axisRenderer.getHeight() / 2 - (int)getViewportTop();
    }

    private int getEventAxisModuleIndex(long eventPtr) {
        return getAxisModuleIndexByModuleId(sequenceChartFacade.IEvent_getModuleId(eventPtr));
    }

    private boolean isInitializationEvent(long eventPtr) {
        return sequenceChartFacade.IEvent_getEventNumber(eventPtr) == 0;
    }

    private boolean isInitializationEvent(IEvent event) {
        return event.getEventNumber() == 0;
    }

    /**
     * The initialization event is drawn on multiple module axes (one for each message sent).
     * The vertical coordinate is determined by the message dependency between the initialize event and the event it caused.
     */
    private int getInitializationEventYViewportCoordinate(long messageDependencyPtr) {
        int contextModuleId = getInitializationEventContextModuleId(messageDependencyPtr);
        int moduleIndex = getAxisModuleIndexByModuleId(contextModuleId);
        return getModuleYViewportCoordinateByModuleIndex(moduleIndex);
    }

    private int getInitializationEventYViewportCoordinate(long messageDependencyPtr, int defaultValue) {
        int contextModuleId = getInitializationEventContextModuleId(messageDependencyPtr);
        int moduleIndex = getAxisModuleIndexByModuleId(contextModuleId);
        if (moduleIndex == -1)
            return defaultValue;
        else
            return getModuleYViewportCoordinateByModuleIndex(moduleIndex);
    }

    private int getInitializationEventContextModuleId(long messageDependencyPtr) {
        long messageEntryPtr = sequenceChartFacade.IMessageDependency_getMessageEntry(messageDependencyPtr);
        return sequenceChartFacade.EventLogEntry_getContextModuleId(messageEntryPtr);
    }

    public long getEventXViewportCoordinateBegin(long eventPtr) {
        return getViewportCoordinateForTimelineCoordinate(sequenceChartFacade.IEvent_getTimelineCoordinateBegin(eventPtr));
    }

    public long getEventXViewportCoordinateEnd(long eventPtr) {
        return getViewportCoordinateForTimelineCoordinate(sequenceChartFacade.IEvent_getTimelineCoordinateEnd(eventPtr));
    }

    public long getEventViewportWidth(long eventPtr) {
        return getEventXViewportCoordinateEnd(eventPtr) - getEventXViewportCoordinateBegin(eventPtr);
    }

    public long getEventLogEntryXViewportCoordinateBegin(long eventLogEntryPtr) {
        return getViewportCoordinateForTimelineCoordinate(sequenceChartFacade.EventLogEntry_getTimelineCoordinate(eventLogEntryPtr));
    }

    public int getEventYViewportCoordinate(long eventPtr) {
        Assert.isTrue(!isInitializationEvent(eventPtr));
        return getModuleYViewportCoordinateByModuleIndex(getEventAxisModuleIndex(eventPtr));
    }

    public int getEventYViewportCoordinate(long eventPtr, int defaultValue) {
        Assert.isTrue(!isInitializationEvent(eventPtr));
        int index = getEventAxisModuleIndex(eventPtr);
        if (index == -1)
            return defaultValue;
        else
            return getModuleYViewportCoordinateByModuleIndex(index);
    }

    /*************************************************************************************
     * CALCULATING TICKS
     */

    /**
     * Calculates and stores ticks as simulation times based on tick spacing. Tries to round tick values
     * to have as short numbers as possible within a range of pixels.
     */
    private void calculateTicks(int viewportWidth) {
        ticks = new ArrayList<BigDecimal>();
        BigDecimal leftSimulationTime = calculateTick(0, 1);
        BigDecimal rightSimulationTime = calculateTick(viewportWidth, 1);
        tickPrefix = TimeUtils.commonPrefix(leftSimulationTime, rightSimulationTime);

        if (!eventLog.isEmpty()) {
            int tickSpacing = styleProvider.getTickSpacing();
            if (getTimelineMode() == TimelineMode.SIMULATION_TIME) {
                // puts ticks to constant distance from each other measured in timeline units
                int tickScale = (int)Math.ceil(Math.log10(tickSpacing / getPixelPerTimelineUnit()));
                BigDecimal tickSpacingInTimelineUnits = BigDecimal.valueOf(tickSpacing / getPixelPerTimelineUnit());
                BigDecimal tickStart = leftSimulationTime.setScale(-tickScale, RoundingMode.FLOOR);
                BigDecimal tickEnd = rightSimulationTime.setScale(-tickScale, RoundingMode.CEILING);
                BigDecimal tickIntvl = new BigDecimal(1).scaleByPowerOfTen(tickScale);

                // use 2, 4, 6, 8, etc. if possible
                if (tickIntvl.divide(BigDecimal.valueOf(5)).compareTo(tickSpacingInTimelineUnits) > 0)
                    tickIntvl = tickIntvl.divide(BigDecimal.valueOf(5));
                // use 5, 10, 15, 20, etc. if possible
                else if (tickIntvl.divide(BigDecimal.valueOf(2)).compareTo(tickSpacingInTimelineUnits) > 0)
                    tickIntvl = tickIntvl.divide(BigDecimal.valueOf(2));

                for (BigDecimal tick = tickStart; tick.compareTo(tickEnd)<0; tick = tick.add(tickIntvl))
                    if (tick.compareTo(BigDecimal.ZERO) >= 0)
                        ticks.add(tick);
            }
            else {
                // tries to put ticks constant distance from each other measured in pixels
                long modX = fixPointViewportCoordinate % tickSpacing;
                long tleft = modX - tickSpacing;
                long tright = modX + viewportWidth + tickSpacing;

                IEvent lastEvent = eventLog.getLastEvent();

                if (lastEvent != null) {
                    BigDecimal endSimulationTime = lastEvent.getSimulationTime().toBigDecimal();

                    for (long t = tleft; t < tright; t += tickSpacing) {
                        BigDecimal tick = calculateTick(t, tickSpacing / 2);

                        if (tick.compareTo(BigDecimal.ZERO) >= 0 && tick.compareTo(endSimulationTime) <= 0)
                            ticks.add(tick);
                    }
                }
            }
        }
    }

    /**
     * Calculates a single tick near simulation time. The range and position is defined in terms of viewport pixels.
     * Minimizes the number of digits to have a short tick label and returns the simulation time to be printed.
     */
    private BigDecimal calculateTick(long x, double tickRange) {
        IEvent lastEvent = eventLog.getLastEvent();

        if (lastEvent == null)
            return BigDecimal.ZERO;

        // query the simulation time for the given coordinate
        BigDecimal simulationTime = getSimulationTimeForViewportCoordinate(x).toBigDecimal();

        // defines the range of valid simulation times for the given tick range
        BigDecimal tMin = getSimulationTimeForViewportCoordinate(x - tickRange / 2).toBigDecimal();
        BigDecimal tMax = getSimulationTimeForViewportCoordinate(x + tickRange / 2).toBigDecimal();

        // check some invariants
        // originally we were checking these invariants, but it is impossible to always make these hold
        // due to the fact that a linear approximation is inherently non monotonic between two simulation
        // times based on a double timeline lambda value between 0.0 and 1.0
        // NOTE: leave it as a comment: Assert.isTrue(tMin.compareTo(simulationTime) <= 0);
        // NOTE: leave it as a comment: Assert.isTrue(tMax.compareTo(simulationTime) >= 0);
        if (tMin.compareTo(simulationTime) > 0)
            tMin = simulationTime;

        if (tMax.compareTo(simulationTime) < 0)
            tMax = simulationTime;
        Assert.isTrue(tMin.compareTo(tMax) <= 0);

        // the idea is to round the simulation time to the shortest (in terms of digits) value
        // as long as it still fits into the range of min and max
        // first get the number of digits
        int tMinPrecision = tMin.stripTrailingZeros().precision();
        int tMaxPrecision = tMax.stripTrailingZeros().precision();
        int tDeltaPrecision = tMax.subtract(tMin).stripTrailingZeros().precision();
        int precision = Math.max(1, 1 + Math.max(tMinPrecision - tDeltaPrecision, tMaxPrecision - tDeltaPrecision));
        // establish initial rounding contexts
        MathContext mcMin = new MathContext(precision, RoundingMode.FLOOR);
        MathContext mcMax = new MathContext(precision, RoundingMode.CEILING);
        BigDecimal tRoundedMin = simulationTime;
        BigDecimal tRoundedMax = simulationTime;
        BigDecimal tBestRoundedMin = simulationTime;
        BigDecimal tBestRoundedMax = simulationTime;

        // decrease precision and check if values still fit in range
        do
        {
            if (mcMin.getPrecision() > 0) {
                tRoundedMin = simulationTime.round(mcMin);

                if (tRoundedMin.compareTo(tMin) > 0)
                    tBestRoundedMin = tRoundedMin;

                mcMin = new MathContext(mcMin.getPrecision() - 1, RoundingMode.FLOOR);
            }

            if (mcMax.getPrecision() > 0) {
                tRoundedMax = simulationTime.round(mcMax);

                if (tRoundedMax.compareTo(tMax) < 0)
                    tBestRoundedMin = tRoundedMax;

                mcMax = new MathContext(mcMax.getPrecision() - 1, RoundingMode.CEILING);
            }
        }
        while (mcMin.getPrecision() > 0 || mcMax.getPrecision() > 0);

        // the last digit might be still rounded to 2 or 5
        tBestRoundedMin = calculateTickBestLastDigit(tBestRoundedMin, tMin, tMax);
        tBestRoundedMax = calculateTickBestLastDigit(tBestRoundedMax, tMin, tMax);

        // find the best solution by looking at the number of digits and the last digit
        if (tBestRoundedMin.precision() < tBestRoundedMax.precision())
            return tBestRoundedMin;
        else if (tBestRoundedMin.precision() > tBestRoundedMax.precision())
            return tBestRoundedMax;
        else {
            String sBestMin = tBestRoundedMin.toPlainString();
            String sBestMax = tBestRoundedMax.toPlainString();

            if (sBestMin.charAt(sBestMin.length() - 1) == '5')
                return tBestRoundedMin;

            if (sBestMax.charAt(sBestMax.length() - 1) == '5')
                return tBestRoundedMax;

            if ((sBestMin.charAt(sBestMin.length() - 1) - '0') % 2 == 0)
                return tBestRoundedMin;

            if ((sBestMax.charAt(sBestMin.length() - 1) - '0') % 2 == 0)
                return tBestRoundedMax;

            return tBestRoundedMin;
        }
    }

    /**
     * Replaces the last digit of value with 5 or the closest even number and
     * returns it when it is between min and max.
     */
    private BigDecimal calculateTickBestLastDigit(BigDecimal value, BigDecimal min, BigDecimal max) {
        BigDecimal candidate = new BigDecimal(value.unscaledValue().divide(BigInteger.TEN).multiply(BigInteger.TEN).add(BigInteger.valueOf(5)), value.scale());

        if (min.compareTo(candidate) < 0 && max.compareTo(candidate) > 0)
            return candidate;

        candidate = new BigDecimal(value.unscaledValue().clearBit(0), value.scale());

        if (min.compareTo(candidate) < 0 && max.compareTo(candidate) > 0)
            return candidate;

        return value;
    }

    /*************************************************************************************
     * COORDINATE TRANSFORMATIONS
     */

    /**
     * Translates viewport pixel x coordinate to simulation time lower limit.
     */
    public org.omnetpp.common.engine.BigDecimal getSimulationTimeForViewportCoordinate(long x) {
        return getSimulationTimeForViewportCoordinate(x, false);
    }

    /**
     * Translates viewport pixel x coordinate to simulation time.
     */
    public org.omnetpp.common.engine.BigDecimal getSimulationTimeForViewportCoordinate(long x, boolean upperLimit) {
        return sequenceChartFacade.getSimulationTimeForTimelineCoordinate(getTimelineCoordinateForViewportCoordinate(x), upperLimit);
    }

    /**
     * Translates viewport pixel x coordinate to simulation time lower limit.
     */
    public org.omnetpp.common.engine.BigDecimal getSimulationTimeForViewportCoordinate(double x) {
        return getSimulationTimeForViewportCoordinate(x, false);
    }

    /**
     * Translates viewport pixel x coordinate to simulation time.
     */
    public org.omnetpp.common.engine.BigDecimal getSimulationTimeForViewportCoordinate(double x, boolean upperLimit) {
        return sequenceChartFacade.getSimulationTimeForTimelineCoordinate(getTimelineCoordinateForViewportCoordinate(x), upperLimit);
    }

    /**
     * Translates simulation time to viewport pixel x coordinate lower limit.
     */
    public long getViewportCoordinateForSimulationTime(org.omnetpp.common.engine.BigDecimal t) {
        return getViewportCoordinateForSimulationTime(t, false);
    }

    /**
     * Translates simulation time to viewport pixel x coordinate.
     */
    public long getViewportCoordinateForSimulationTime(org.omnetpp.common.engine.BigDecimal t, boolean upperLimit) {
        Assert.isTrue(t.greaterOrEqual(org.omnetpp.common.engine.BigDecimal.getZero()));
        return Math.round(sequenceChartFacade.getTimelineCoordinateForSimulationTime(t, upperLimit) * getPixelPerTimelineUnit()) + fixPointViewportCoordinate;
    }

    /**
     * Translates from viewport pixel x coordinate to timeline coordinate, using pixelPerTimelineCoordinate.
     */
    public double getTimelineCoordinateForViewportCoordinate(long x) {
        Assert.isTrue(getPixelPerTimelineUnit() > 0);
        return (x - fixPointViewportCoordinate) / getPixelPerTimelineUnit();
    }

    /**
     * Translates from viewport pixel x coordinate to timeline coordinate, using pixelPerTimelineCoordinate.
     */
    public double getTimelineCoordinateForViewportCoordinate(double x) {
        Assert.isTrue(getPixelPerTimelineUnit() > 0);
        return (x - fixPointViewportCoordinate) / getPixelPerTimelineUnit();
    }

    /**
     * Translates timeline coordinate to viewport pixel x coordinate, using pixelPerTimelineCoordinate.
     */
    public long getViewportCoordinateForTimelineCoordinate(double t) {
        return Math.round(t * getPixelPerTimelineUnit()) + fixPointViewportCoordinate;
    }

    /*************************************************************************************
     * KEYBOARD HANDLING
     */

    private void setupKeyListener() {
        addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.keyCode == SWT.F5)
                    refresh();
                else if (e.keyCode == SWT.TAB)
                    setShowTimeDifferences(!getShowTimeDifferences());
                else if (e.keyCode == SWT.DEL) {
                    org.omnetpp.common.engine.BigDecimal startSimulationTime = getViewportLeftSimulationTime();
                    org.omnetpp.common.engine.BigDecimal endSimulationTime = getViewportRightSimulationTime();
                    EventLogFilterParameters parameters = eventLogInput.getFilterParameters();
                    ArrayList<Long> excludedEventNumbers = new ArrayList<Long>();
                    for (Object object : selectedObjects) {
                        if (object instanceof EventNumberRangeSet) {
                            EventNumberRangeSet eventNumberRangeSet = (EventNumberRangeSet)object;
                            outer:
                                for (long eventNumber : eventNumberRangeSet) {
                                    for (EventLogFilterParameters.EnabledLong excludedEventNumber : parameters.excludedEventNumbers)
                                        if (excludedEventNumber.value == eventNumber)
                                            continue outer;
                                    excludedEventNumbers.add(eventNumber);
                                }
                            for (long excludedEventNumber : excludedEventNumbers)
                                eventNumberRangeSet.remove(excludedEventNumber);
                            break;
                        }
                    }
                    if (excludedEventNumbers.size() != 0) {
                        clearCanvasCacheAndRedraw();
                        parameters.excludedEventNumbers = Arrays.copyOf(parameters.excludedEventNumbers, parameters.excludedEventNumbers.length + excludedEventNumbers.size());
                        for (int i = 0; i < excludedEventNumbers.size(); i++)
                            parameters.excludedEventNumbers[parameters.excludedEventNumbers.length - excludedEventNumbers.size() + i] = new EventLogFilterParameters.EnabledLong(true, excludedEventNumbers.get(i));
                        eventLogInput.filter();
                        zoomToSimulationTimeRange(startSimulationTime, endSimulationTime);
                    }
                }
                else if (e.keyCode == SWT.ARROW_LEFT) {
                    if (e.stateMask == 0)
                        moveFocus(-1);
                    else if (e.stateMask == SWT.MOD1) {
                        IEvent event = getSelectedEvent();

                        if (event != null) {
                            event = event.getCauseEvent();

                            if (event != null)
                                gotoClosestElement(event);
                        }
                    }
                    else if (e.stateMask == SWT.SHIFT) {
                        IEvent event = getSelectedEvent();

                        if (event != null) {
                            int moduleId = event.getModuleId();

                            while (event != null) {
                                event = event.getPreviousEvent();

                                if (event != null && moduleId == event.getModuleId()) {
                                    gotoClosestElement(event);
                                    break;
                                }
                            }
                        }
                    }
                }
                else if (e.keyCode == SWT.ARROW_RIGHT) {
                    if (e.stateMask == 0)
                        moveFocus(1);
                    else if (e.stateMask == SWT.MOD1) {
                        IEvent event = getSelectedEvent();

                        if (event != null) {
                            IMessageDependencyList consequences = event.getConsequences();

                            if (consequences.size() > 0) {
                                event = consequences.get(0).getConsequenceEvent();

                                if (event != null)
                                    gotoClosestElement(event);
                            }
                        }
                    }
                    else if (e.stateMask == SWT.SHIFT) {
                        IEvent event = getSelectedEvent();

                        if (event != null) {
                            int moduleId = event.getModuleId();

                            while (event != null) {
                                event = event.getNextEvent();

                                if (event != null && moduleId == event.getModuleId()) {
                                    gotoClosestElement(event);
                                    break;
                                }
                            }
                        }
                    }
                }
                else if (e.keyCode == SWT.ARROW_UP)
                    scrollVertical((int)Math.floor(-getAxisSpacing() - 1));
                else if (e.keyCode == SWT.ARROW_DOWN)
                    scrollVertical((int)Math.ceil(getAxisSpacing() + 1));
                else if (e.keyCode == SWT.PAGE_UP)
                    scrollVertical(-getViewportHeight());
                else if (e.keyCode == SWT.PAGE_DOWN)
                    scrollVertical(getViewportHeight());
                else if (e.keyCode == SWT.HOME)
                    gotoBegin();
                else if (e.keyCode == SWT.END)
                    gotoEnd();
                else if (e.keyCode == SWT.KEYPAD_ADD || e.character == '+' || e.character == '=')
                    zoomIn();
                else if (e.keyCode == SWT.KEYPAD_SUBTRACT || e.character == '-')
                    zoomOut();
                else if (e.keyCode == SWT.KEYPAD_MULTIPLY)
                    defaultZoom();
            }
        });
    }

   /*************************************************************************************
     * MOUSE HANDLING
     */

    private void setupMouseListener() {
        // zoom by wheel
        addListener(SWT.MouseWheel, new Listener() {
            public void handleEvent(Event event) {
                if (!eventLogInput.isCanceled()) {
                    if ((event.stateMask & SWT.MOD1) != 0) {
                        for (int i = 0; i < event.count; i++)
                            zoomBy(1.1);

                        for (int i = 0; i < -event.count; i++)
                            zoomBy(1.0 / 1.1);
                    }
                    else if ((event.stateMask & SWT.SHIFT) != 0)
                        scrollHorizontal(-getViewportWidth() * event.count / 20);
                }
            }
        });

        addMouseTrackListener(new MouseTrackAdapter() {
            @Override
            public void mouseEnter(MouseEvent e) {
                redraw();
            }

            @Override
            public void mouseExit(MouseEvent e) {
                redraw();
            }

            @Override
            public void mouseHover(MouseEvent e) {
                highlightedObjects = collectVisibleObjectsAtPosition(e.x, e.y);
                redraw();
            }
        });

        // dragging
        addMouseMoveListener(new MouseMoveListener() {
            public void mouseMove(MouseEvent e) {
                if (eventLogInput != null && !eventLogInput.isCanceled()) {
                    highlightedObjects.clear();
                    if (dragStartX != -1 && dragStartY != -1 && (e.stateMask & SWT.BUTTON_MASK) != 0 && (e.stateMask & SWT.MODIFIER_MASK) == 0)
                        mouseDragged(e);
                    else {
                        setCursor(null); // restore cursor at end of drag (must do it here too, because we
                                         // don't get the "released" event if user releases mouse outside the canvas)
                        redraw();
                    }
                }
            }

            private void mouseDragged(MouseEvent e) {
                if (eventLogInput != null && !eventLogInput.isCanceled()) {
                    if (!isDragging)
                        setCursor(DRAG_CURSOR);

                    isDragging = true;

                    // scroll by the amount moved since last drag call
                    dragDeltaX = e.x - dragStartX;
                    dragDeltaY = e.y - dragStartY;
                    scrollHorizontal(-dragDeltaX);
                    scrollVertical(-dragDeltaY);
                    dragStartX = e.x;
                    dragStartY = e.y;
                }
            }
        });

        // selection handling
        addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDoubleClick(MouseEvent mouseEvent) {
                if (eventLogInput != null && !eventLogInput.isCanceled()) {
                    ArrayList<Object> objects = collectVisibleObjectsAtPosition(mouseEvent.x, mouseEvent.y);
                    for (Object object : objects) {
                        if (object instanceof IMessageDependency) {
                            zoomToMessageDependency((IMessageDependency)object);
                            break;
                        }
                        else if (object instanceof ModuleTreeItem) {
                            if ((mouseEvent.stateMask & SWT.MOD1) != 0)
                                openModuleRecursively((ModuleTreeItem)object);
                            else
                                openModule((ModuleTreeItem)object);
                            break;
                        }
                        else if (object instanceof AxisHeader) {
                            AxisHeader axisHeader = (AxisHeader)object;
                            if (axisHeader.module != null) {
                                if ((mouseEvent.stateMask & SWT.MOD1) != 0)
                                    openModuleRecursively(axisHeader.module);
                                else
                                    openModule(axisHeader.module);
                                break;
                            }
                        }
                    }
                }
            }

            @Override
            public void mouseDown(MouseEvent e) {
                if (eventLogInput != null && !eventLogInput.isCanceled()) {
                    setFocus();

                    if (e.button == 1) {
                        dragStartX = e.x;
                        dragStartY = e.y;
                    }
                }
            }

            @Override
            public void mouseUp(MouseEvent mouseEvent) {
                if (eventLogInput != null && !eventLogInput.isCanceled()) {
                    if (mouseEvent.button == 1 && mouseEvent.count == 1 && !isDragging) {
                        ArrayList<Object> objects = collectVisibleObjectsAtPosition(mouseEvent.x, mouseEvent.y);
                        if (objects.stream().filter(o -> o instanceof ModuleAction).count() != 0)
                            objects = new ArrayList<Object>(Arrays.asList(objects.stream().filter(o -> o instanceof ModuleAction).toArray()));
                        else if (objects.stream().filter(o -> o instanceof IEvent).count() != 0)
                            objects = new ArrayList<Object>(Arrays.asList(objects.stream().filter(o -> o instanceof IEvent).toArray()));
                        else if (objects.stream().filter(o -> o instanceof IMessageDependency).count() != 0)
                            objects = new ArrayList<Object>(Arrays.asList(objects.stream().filter(o -> o instanceof IMessageDependency).toArray()));
                        else if (objects.stream().filter(o -> o instanceof ComponentMethodBeginEntry).count() != 0)
                            objects = new ArrayList<Object>(Arrays.asList(objects.stream().filter(o -> o instanceof ComponentMethodBeginEntry).toArray()));
                        else if (objects.stream().filter(o -> o instanceof ModuleTreeItem).count() != 0)
                            objects = new ArrayList<Object>(Arrays.asList(objects.stream().filter(o -> o instanceof ModuleTreeItem).toArray()));
                        else if (objects.stream().filter(o -> o instanceof Double).count() != 0)
                            objects = new ArrayList<Object>(Arrays.asList(objects.stream().filter(o -> o instanceof Double).toArray()));
                        // CTRL key toggles selection
                        if ((mouseEvent.stateMask & SWT.MOD1) != 0) {
                            boolean isSelectionChanged = false;
                            EventNumberRangeSet selectedEventNumberRangeSet = (EventNumberRangeSet)selectedObjects.stream().filter(o -> o instanceof EventNumberRangeSet).findFirst().orElse(new EventNumberRangeSet());
                            for (Object object : objects) {
                                if (object instanceof IEvent) {
                                    IEvent event = (IEvent)object;
                                    long eventNumber = event.getEventNumber();
                                    if (selectedEventNumberRangeSet.contains(eventNumber))
                                        selectedEventNumberRangeSet.remove(eventNumber);
                                    else
                                        selectedEventNumberRangeSet.add(eventNumber);
                                    isSelectionChanged = true;
                                }
                                else {
                                    if (selectedObjects.contains(object))
                                        selectedObjects.remove(object);
                                    else
                                        selectedObjects.add(object);
                                    isSelectionChanged = true;
                                }
                            }
                            if (selectedEventNumberRangeSet.isEmpty())
                                selectedObjects.remove(selectedEventNumberRangeSet);
                            else if (!selectedObjects.contains(selectedEventNumberRangeSet))
                                selectedObjects.add(selectedEventNumberRangeSet);
                            if (isSelectionChanged) {
                                fireSelection(false);
                                fireSelectionChanged();
                                redraw();
                            }
                        }
                        else if (objects.stream().filter(o -> o instanceof ModuleAction).count() != 0) {
                            for (Object object : objects) {
                                if (object instanceof ModuleAction) {
                                    ModuleAction action = (ModuleAction)object;
                                    if (action.identifier.equals(labelProvider.getExpandImage()))
                                        expandOpenAxisModule(action.module);
                                    else if (action.identifier.equals(labelProvider.getCollapseImage()))
                                        collapseOpenAxisModule(action.module);
                                    else if (action.identifier.equals(labelProvider.getCloseImage())) {
                                        if (action.source instanceof Axis)
                                            removeOpenAxisModule(action.module);
                                        else if (action.source instanceof AxisHeader) {
                                            action.module.visitItems(new IModuleTreeItemVisitor() {
                                                public void visit(ModuleTreeItem moduleTreeItem) {
                                                    removeOpenAxisModule(moduleTreeItem);
                                                }
                                            });
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            ArrayList<Object> newSelectedObjects = new ArrayList<Object>();
                            EventNumberRangeSet newSelectedEventNumbers = new EventNumberRangeSet();
                            for (Object object : objects) {
                                if (object instanceof IEvent) {
                                    if (!newSelectedObjects.contains(newSelectedEventNumbers))
                                        newSelectedObjects.add(newSelectedEventNumbers);
                                    newSelectedEventNumbers.add(((IEvent)object).getEventNumber());
                                }
                                else
                                    newSelectedObjects.add(object);
                            }
                            if (!selectedObjects.equals(newSelectedObjects)) {
                                selectedObjects = newSelectedObjects;
                                fireSelection(false);
                                fireSelectionChanged();
                                redraw();
                            }
                            else if (newSelectedObjects.size() != 0) {
                                selectedObjects.clear();
                                fireSelection(false);
                                fireSelectionChanged();
                                redraw();
                            }
                        }
                    }

                    setCursor(null); // restore cursor at end of drag
                    dragStartX = dragStartY = -1;
                    isDragging = false;
                }
            }
        });
    }

    /*************************************************************************************
     * SELECTION
     */

    public void addSelectionListener(SelectionListener listener) {
        selectionListeners.add(listener);
    }

    public void removeSelectionListener(SelectionListener listener) {
        selectionListeners.remove(listener);
    }

    private void fireSelection(boolean defaultSelection) {
        Event event = new Event();
        event.display = getDisplay();
        event.widget = this;
        SelectionEvent se = new SelectionEvent(event);
        for (SelectionListener listener : selectionListeners) {
            if (defaultSelection)
                listener.widgetDefaultSelected(se);
            else
                listener.widgetSelected(se);
        }
    }

    @Override
    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.add(listener);
    }

    @Override
    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.remove(listener);
    }

    /**
     * Notifies any selection changed listeners that the viewer's selection has changed.
     * Only listeners registered at the time this method is called are notified.
     */
    private void fireSelectionChanged() {
        fireSelectionChanged(getSelection());
    }

    private void fireSelectionChanged(ISelection selection) {
        final SelectionChangedEvent event = new SelectionChangedEvent(this, selection);
        for (ISelectionChangedListener l : selectionChangedListeners) {
            SafeRunnable.run(new SafeRunnable() {
                public void run() {
                    l.selectionChanged(event);
                }
            });
        }
    }

    /**
     * Returns the currently "selected" events as an instance of IEventLogSelection.
     * Selection is shown as red circles on the chart.
     */
    @Override
    public IEventLogSelection getSelection() {
        if (eventLogInput == null)
            return null;
        else {
            ArrayList<Object> elements = new ArrayList<Object>(selectedObjects);
            for (Object object : selectedObjects)
                if (object instanceof Double)
                    elements.add(sequenceChartFacade.getSimulationTimeForTimelineCoordinate((Double)object));
            elements.removeIf(o -> o instanceof Double);
            return new EventLogSelection(eventLogInput, elements);
        }
    }

    /**
     * Sets the currently "selected" events. The selection must be an instance of IEventLogSelection.
     */
    @Override
    public void setSelection(ISelection selection) {
        if (selection instanceof IEventLogSelection) {
            IEventLogSelection eventLogSelection = (IEventLogSelection)selection;
            EventLogInput selectionEventLogInput = eventLogSelection.getEventLogInput();
            if (eventLogInput != selectionEventLogInput)
                setInput(selectionEventLogInput);
            org.omnetpp.common.engine.BigDecimal simulationTime = eventLogSelection.getFirstSimulationTime();
            Double newSelectedTimelineCoordinate = simulationTime != null ? sequenceChartFacade.getTimelineCoordinateForSimulationTime(simulationTime) : null;
            Object selectedTimelineCoordinate = selectedObjects.stream().filter(o -> o instanceof Double).findFirst().get();
            EventNumberRangeSet selectedEventNumberRangeSet = (EventNumberRangeSet)selectedObjects.stream().filter(o -> o instanceof EventNumberRangeSet).findFirst().get();
            EventNumberRangeSet newSelectedEventNumberRangeSet = eventLogSelection.getEventNumbers();
            boolean isSelectionChanged = !newSelectedEventNumberRangeSet.equals(selectedEventNumberRangeSet) || !ObjectUtils.equals(newSelectedTimelineCoordinate, selectedTimelineCoordinate);
            if (isSelectionChanged && !isSelectionChangeInProgress) {
                try {
                    isSelectionChangeInProgress = true;
                    selectedObjects.clear();
                    selectedObjects.add(newSelectedEventNumberRangeSet);
                    if (newSelectedTimelineCoordinate != null)
                        selectedObjects.add(newSelectedTimelineCoordinate);
                    // go to the time of the first event selected
                    if (!newSelectedEventNumberRangeSet.isEmpty())
                        reveal(eventLog.getEventForEventNumber(newSelectedEventNumberRangeSet.iterator().next()));
                    fireSelectionChanged();
                    redraw();
                }
                finally {
                    isSelectionChangeInProgress = false;
                }
            }
        }
    }

    public void clearSelection() {
        if (!selectedObjects.isEmpty()) {
            selectedObjects.clear();
            fireSelectionChanged();
        }
    }

    public IEvent getSelectedEvent() {
        for (Object object : selectedObjects) {
            if (object instanceof EventNumberRangeSet) {
                EventNumberRangeSet eventNumberRangeSet = (EventNumberRangeSet)object;
                if (eventNumberRangeSet.size() == 1)
                    return eventLog.getEventForEventNumber(eventNumberRangeSet.iterator().next());
                else
                    return null;
            }
        }
        return null;
    }

    public List<IEvent> getSelectedEvents() {
        ArrayList<IEvent> events = new ArrayList<IEvent>();
        for (Object object : selectedObjects) {
            if (object instanceof EventNumberRangeSet) {
                EventNumberRangeSet eventNumberRangeSet = (EventNumberRangeSet)object;
                for (long eventNumber : eventNumberRangeSet)
                    events.add(eventLog.getEventForEventNumber(eventNumber));
            }
        }
        return events;
    }

    public void setSelectedEvent(IEvent event) {
        EventNumberRangeSet eventNumberRangeSet = new EventNumberRangeSet();
        eventNumberRangeSet.add(event.getEventNumber());
        selectedObjects.clear();
        selectedObjects.add(eventNumberRangeSet);
        fireSelectionChanged();
        redraw();
    }

    public org.omnetpp.common.engine.BigDecimal getSelectedSimulationTime() {
        Double timelineCoordinate = (Double)selectedObjects.stream().filter(o -> o instanceof Double).findFirst().get();
        if (timelineCoordinate == null)
            return null;
        else
            return sequenceChartFacade.getSimulationTimeForTimelineCoordinate(timelineCoordinate);
    }

    public void setSelectedSimulationTime(org.omnetpp.common.engine.BigDecimal simulationTime) {
        selectedObjects.clear();
        selectedObjects.add(sequenceChartFacade.getTimelineCoordinateForSimulationTime(simulationTime));
        fireSelectionChanged();
        redraw();
    }

    public ModuleTreeItem getSelectedAxisModule() {
        return (ModuleTreeItem)selectedObjects.stream().filter(o -> o instanceof ModuleTreeItem).findFirst().get();
    }

    public void setSelectedAxisModule(ModuleTreeItem moduleTreeItem) {
        selectedObjects.clear();
        selectedObjects.add(moduleTreeItem);
        fireSelectionChanged();
        redraw();
    }

    private void removeInvisibleObjects(ArrayList<Object> objects) {
        objects.removeIf(o -> !isVisibleObject(o));
        for (Object object : objects) {
            if (object instanceof EventNumberRangeSet) {
                EventNumberRangeSet eventNumberRangeSet = (EventNumberRangeSet)object;
                for (long selectedEventNumber : new ArrayList<Long>(eventNumberRangeSet))
                    if (eventLog.getEventForEventNumber(selectedEventNumber) == null)
                        eventNumberRangeSet.remove(selectedEventNumber);
            }
        }
    }

    private boolean isVisibleObject(Object object) {
        if (object instanceof IEvent)
            return getModuleIdToAxisModuleIndexMap().containsKey(((IEvent)object).getModuleId());
        else if (object instanceof EventNumberRangeSet)
            return true;
        else if (object instanceof ModuleTreeItem)
            return getVisibleAxisModules().contains((ModuleTreeItem)object);
        else if (object instanceof IMessageDependency)
            return true;
        else if (object instanceof ComponentMethodBeginEntry)
            return true;
        else if (object instanceof Double)
            return true;
        return false;
    }

    /*************************************************************************************
     * INNER CLASSES
     */

    private static class AxisHeader
    {
        boolean noSubmodulesOpen = true;
        boolean allSubmodulesOpen = true;
        public int axisIndex = -1;
        public int axisCount = -1;
        public String modulePathFragment = null;
        public ModuleTreeItem module = null;
        public ArrayList<AxisHeader> children = new ArrayList<AxisHeader>();
        public int level = -1;
        public Rectangle bounds = null;
        public Rectangle labelBounds = null;
        public Rectangle[] labelElementBounds = null;
        public ModuleTreeItem[] labelElementModules = null;
        public Rectangle expandImageBounds = null;
        public Rectangle collapseImageBounds = null;
        public Rectangle closeImageBounds = null;
    }

    private static class Axis
    {
        public AxisHeader axisHeader = null;
        public IAxisRenderer axisRenderer = null;
        public int y = -1;
        public Rectangle[] labelElementBounds = null;
        public Rectangle expandImageBounds = null;
        public Rectangle collapseImageBounds = null;
        public Rectangle closeImageBounds = null;
    }

    private static class ModuleAction
    {
        public ModuleTreeItem module = null;
        public Object source = null;
        public Object identifier = null;

        public ModuleAction(ModuleTreeItem module, Object source, Object identifier) {
            this.module = module;
            this.source = source;
            this.identifier = identifier;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result
                    + ((identifier == null) ? 0 : identifier.hashCode());
            result = prime * result
                    + ((module == null) ? 0 : module.hashCode());
            result = prime * result
                    + ((source == null) ? 0 : source.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            ModuleAction other = (ModuleAction) obj;
            if (identifier == null) {
                if (other.identifier != null)
                    return false;
            } else if (!identifier.equals(other.identifier))
                return false;
            if (module == null) {
                if (other.module != null)
                    return false;
            } else if (!module.equals(other.module))
                return false;
            if (source == null) {
                if (other.source != null)
                    return false;
            } else if (!source.equals(other.source))
                return false;
            return true;
        }
    }

    /*************************************************************************************
     * CACHING
     */

    /**
     * This class is for optimizing drawing when the chart is zoomed out and
     * there's a large number of connection arrows on top of each other.
     * Most arrows tend to be vertical then, so we only need to bother with
     * drawing vertical lines if it sets new pixels over previously drawn ones
     * at that x coordinate. We exploit that x coordinates grow monotonously.
     */
    private static class VLineBuffer {
        private static class Region {
            int y1, y2;

            Region() {
            }

            Region(int y1, int y2) {
                this.y1 = y1;
                this.y2 = y2;
            }
        }

        private int currentX = -1;

        private ArrayList<Region> regions = new ArrayList<Region>();

        public boolean vlineContainsNewPixel(int x, int y1, int y2) {
            if (y1 > y2) {
                int tmp = y1;
                y1 = y2;
                y2 = tmp;
            }

            if (x != currentX) {
                // start new X
                Region r = regions.isEmpty() ? new Region() : regions.get(0);
                regions.clear();
                r.y1 = y1;
                r.y2 = y2;
                regions.add(r);
                currentX = x;
                return true;
            }

            // find an overlapping region
            int i = findOverlappingRegion(y1, y2);
            if (i == -1) {
                // no overlapping region, add this one and return
                regions.add(new Region(y1, y2));
                return true;
            }

            // existing region entirely contains this one (most frequent, fast route)
            Region r = regions.get(i);
            if (y1 >= r.y1 && y2 <= r.y2)
                return false;

            // merge it into other regions
            mergeRegion(new Region(y1, y2));
            return true;
        }

        private void mergeRegion(Region r) {
            // merge all regions into r, then add it back
            int i;
            while ((i = findOverlappingRegion(r.y1, r.y2)) != -1) {
                Region r2 = regions.remove(i);
                if (r.y1 > r2.y1) r.y1 = r2.y1;
                if (r.y2 < r2.y2) r.y2 = r2.y2;
            }
            regions.add(r);
        }

        private int findOverlappingRegion(int y1, int y2) {
            for (int i = 0; i < regions.size(); i++) {
                Region r = regions.get(i);
                if (r.y1 < y2 && r.y2 > y1)
                    return i;
            }
            return -1;
        }
    }
}

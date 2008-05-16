package org.omnetpp.sequencechart.widgets;

import java.io.Serializable;
import java.lang.reflect.Field;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.math.MathContext;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
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
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Transform;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.ScrollBar;
import org.omnetpp.common.canvas.CachingCanvas;
import org.omnetpp.common.canvas.RubberbandSupport;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.EventLogSelection;
import org.omnetpp.common.eventlog.IEventLogChangeListener;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.common.util.TimeUtils;
import org.omnetpp.common.virtualtable.IVirtualContentWidget;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.EventLogMessageEntry;
import org.omnetpp.eventlog.engine.FilteredEventLog;
import org.omnetpp.eventlog.engine.FilteredMessageDependency;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IMessageDependency;
import org.omnetpp.eventlog.engine.IMessageDependencyList;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.PtrVector;
import org.omnetpp.eventlog.engine.SequenceChartFacade;
import org.omnetpp.scave.engine.FileRunList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.sequencechart.SequenceChartPlugin;
import org.omnetpp.sequencechart.editors.SequenceChartContributor;
import org.omnetpp.sequencechart.widgets.axisorder.AxisOrderByModuleId;
import org.omnetpp.sequencechart.widgets.axisorder.AxisOrderByModuleName;
import org.omnetpp.sequencechart.widgets.axisorder.FlatAxisOrderByMinimizingCost;
import org.omnetpp.sequencechart.widgets.axisorder.HierarchicalAxisOrderByMinimizingCost;
import org.omnetpp.sequencechart.widgets.axisorder.ManualAxisOrder;
import org.omnetpp.sequencechart.widgets.axisrenderer.AxisLineRenderer;
import org.omnetpp.sequencechart.widgets.axisrenderer.AxisVectorBarRenderer;
import org.omnetpp.sequencechart.widgets.axisrenderer.IAxisRenderer;

/**
 * The sequence chart figure shows the events and the messages passed along between several modules.
 * The chart consists of a series of horizontal lines each representing a simple or compound module.
 * Message dependencies are represented by elliptic arrows pointing from the cause event to the consequence event.
 *
 * Zooming, scrolling, tooltips and event selections are also provided.
 *
 * @author andras, levy
 */
//TODO cf with ns2 trace file and cEnvir callback, and modify file format...
//TODO proper "hand" cursor - current one is not very intuitive
//TODO hierarchical sort should be able to reverse order of sorted axes of its submodules
//TODO rubberband vs. haircross, show them at once
//TODO factor out the svg export to org.omnetpp.imageexport
public class SequenceChart
	extends CachingCanvas
	implements IVirtualContentWidget<IEvent>, ISelectionProvider, IEventLogChangeListener
{
	private static final boolean debug = false;

	public static final String STATE_PROPERTY = "SequenceChartState";

	/*************************************************************************************
	 * DRAWING PARAMETERS
	 */

	private static final Color CHART_BACKGROUND_COLOR = ColorFactory.WHITE;
	private static final Color LABEL_COLOR = ColorFactory.BLACK;

	private static final Color TICK_LINE_COLOR = ColorFactory.DARK_GREY;
	private static final Color MOUSE_TICK_LINE_COLOR = ColorFactory.BLACK;
	private static final Color TICK_LABEL_COLOR = ColorFactory.BLACK;
	private static final Color INFO_BACKGROUND_COLOR = ColorFactory.LIGHT_CYAN;

	private static final Color GUTTER_BACKGROUND_COLOR = new Color(null, 255, 255, 160);
	private static final Color GUTTER_BORDER_COLOR = ColorFactory.BLACK;

    private static final Color INITIALIZATION_EVENT_BORDER_COLOR = ColorFactory.BLACK;
    private static final Color INITIALIZATION_EVENT_BACKGROUND_COLOR = ColorFactory.WHITE;
	private static final Color EVENT_BORDER_COLOR = ColorFactory.RED4;
	private static final Color EVENT_BACKGROUND_COLOR = ColorFactory.RED;
	private static final Color SELF_EVENT_BORDER_COLOR = ColorFactory.GREEN4;
	private static final Color SELF_EVENT_BACKGROUND_COLOR = ColorFactory.GREEN2;

	private static final Color EVENT_SELECTION_COLOR = ColorFactory.RED;
	private static final Color EVENT_BOOKMARK_COLOR = ColorFactory.CYAN;

	private static final Color ARROW_HEAD_COLOR = null; // defaults to line color
	private static final Color LONG_ARROW_HEAD_COLOR = ColorFactory.WHITE; // defaults to line color
	private static final Color MESSAGE_LABEL_COLOR = null; // defaults to line color

	private static final Color MESSAGE_SEND_COLOR = ColorFactory.BLUE;
	private static final Color MESSAGE_REUSE_COLOR = ColorFactory.GREEN4;

	private static final Color ZERO_SIMULATION_TIME_REGION_COLOR = ColorFactory.GREY90;

	private static final Cursor DRAG_CURSOR = new Cursor(null, SWT.CURSOR_SIZEALL);

	private static final int[] DOTTED_LINE_PATTERN = new int[] {2,2}; // 2px black, 2px gap

	private static final int ANTIALIAS_TURN_ON_AT_MSEC = 100;
	private static final int ANTIALIAS_TURN_OFF_AT_MSEC = 300;
	private static final int MOUSE_TOLERANCE = 3;

	private static final int MINIMUM_HALF_ELLIPSE_HEIGHT = 15; // vertical radius of ellipse for message arrows on one axis
	private static final int LONG_MESSAGE_ARROW_WIDTH = 40; // width for too long message lines and half ellipses
	private static final int ARROWHEAD_LENGTH = 10; // length of message arrow head
	private static final int ARROWHEAD_WIDTH = 7; // width of message arrow head
	private static final int MINIMUM_AXIS_SPACING_TO_DISPLAY_LABELS = 16; // minimum spacing of timeline label above axis
	private static final int EVENT_SELECTION_RADIUS = 10; // radius of event selection mark circle
	private static final int TICK_SPACING = 100; // space between ticks in pixels
	private static final int AXIS_OFFSET = 20;  // extra y distance before and after first and last axes
	public static final int GUTTER_HEIGHT = 17; // height of top and bottom gutter

	private static Font font = JFaceResources.getDefaultFont();

	/*************************************************************************************
	 * INTERNAL STATE
	 */

	private IEventLog eventLog; // contains the data to be displayed
	private EventLogInput eventLogInput;
	private SequenceChartFacade sequenceChartFacade; // helpful facade on eventlog

	private int fixPointViewportCoordinate; // the viewport coordinate of the coordinate system origin stored in the facade

	private double pixelPerTimelineCoordinate;
	private boolean antiAlias = true;  // antialiasing -- this gets turned on/off automatically
	private double axisSpacing = 1; // y distance between two axes, might be fractional pixels to have precise positioning for several axes
	private AxisSpacingMode axisSpacingMode = AxisSpacingMode.AUTO;

	private boolean showArrowHeads = true; // whether arrow heads are drawn or not
	private boolean showMessageNames = true;
    private boolean showSelfMessages = true; // show or hide reuse message arrows
	private boolean showReuseMessages = false; // show or hide reuse message arrows
	private boolean showEventNumbers = true;
    private boolean showZeroSimulationTimeRegions = true;
    private boolean showAxisLabels = true;
	private AxisOrderingMode axisOrderingMode = AxisOrderingMode.MODULE_ID; // specifies the ordering mode of timelines

	private HoverSupport hoverSupport;

	private boolean isDragging;
	private int dragStartX = -1, dragStartY = -1, dragDeltaX, dragDeltaY; // temporary variables for drag handling
	private long dragStartTime;

	private ArrayList<BigDecimal> ticks; // a list of simulation times drawn on the axis as tick marks
	private BigDecimal tickPrefix;

	private ArrayList<ModuleTreeItem> axisModules; // the modules which should have an axis (they must be part of a module tree!)
	private IAxisRenderer[] axisRenderers; // used to draw the axis (parallel to axisModules)
	private HashMap<Integer, IAxisRenderer> moduleIdToAxisRendererMap = new HashMap<Integer, IAxisRenderer>();
	private ManualAxisOrder manualAxisOrder = new ManualAxisOrder();
	private int[] axisModulePositions; // y order of the axis modules (in the same order as axisModules); this is a permutation of the 0..axisModule.size()-1 numbers
	private int[] axisModuleYs; // top y coordinates of axis bounding boxes
	private HashMap<Integer, Integer> moduleIdToAxisModuleIndexMap = new HashMap<Integer, Integer>();
	private boolean invalidVirtualSize = true;
	private boolean drawStuffUnderMouse = false;

    private boolean paintHasBeenFinished = false;
    private boolean internalErrorHappenedDuringPaint = false;

	private boolean followEnd = false; // when the event log changes should we follow it or not?

	/*************************************************************************************
	 * SELECTION STATE
	 */

	/**
	 * True means the chart will jump to the selection and switch input automatically when it gets notified about a selection change
	 * even if the input is different from the current one.
	 */
	private boolean followSelection = true;

	private ArrayList<SelectionListener> selectionListenerList = new ArrayList<SelectionListener>(); // SWT selection listeners
	private List<Integer> selectionEventNumbers = new ArrayList<Integer>(); // the selection
    private ListenerList selectionChangedListeners = new ListenerList(); // list of selection change listeners (type ISelectionChangedListener).
	private MenuManager menuManager;

	/*************************************************************************************
	 * PUBLIC INNER TYPES
	 */

	public enum AxisSpacingMode {
		MANUAL,
		AUTO
	}

	/**
	 * Timeline mode determines the horizontal coordinate system in the sequence chart.
	 * Simulation time and event number based means proportional to distance measured in pixels.
	 * Step means subsequent events follow in a constant distance.
	 * Nonlinear mode means distance measured in pixels is proportional to a nonlinear function of simulation time deltas.
	 */
	public enum TimelineMode {
		SIMULATION_TIME,
		EVENT_NUMBER,
		STEP,
		NONLINEAR
	}

	/**
	 * Determines the order of axis in the sequence chart.
	 */
	public enum AxisOrderingMode {
		MANUAL,
		MODULE_ID,
		MODULE_NAME,
		MINIMIZE_CROSSINGS,
		MINIMIZE_CROSSINGS_HIERARCHICALLY
	}

	/*************************************************************************************
	 * CONSTRUCTOR, GETTERS, SETTERS
	 */

	/**
     * Constructor.
     */
	public SequenceChart(Composite parent, int style) {
		super(parent, style);
		setBackground(CHART_BACKGROUND_COLOR);
    	setUpMouseHandling();

    	hoverSupport = new HoverSupport();
    	hoverSupport.setHoverSizeConstaints(700, 200);
    	hoverSupport.adapt(this, new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
				return HoverSupport.addHTMLStyleSheet(getTooltipText(x, y, outSizeConstraint));
			}
    	});

		new RubberbandSupport(this, SWT.CTRL) {
			@Override
			public void rubberBandSelectionMade(org.eclipse.swt.graphics.Rectangle r) {
				zoomToRectangle(new org.eclipse.draw2d.geometry.Rectangle(r));
			}
		};

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
					org.eclipse.swt.graphics.Rectangle r = getClientArea();

					if (pixelPerTimelineCoordinate == 0)
			            calculatePixelPerTimelineUnit(r.width);
					
					setViewportRectangle(new org.eclipse.swt.graphics.Rectangle(r.x, r.y + GUTTER_HEIGHT, r.width, r.height - GUTTER_HEIGHT * 2));
					calculateAxisSpacing();
				}
			}
		});

		addKeyListener(new KeyAdapter() {
			@Override
            public void keyPressed(KeyEvent e) {
				if (e.keyCode == SWT.F5)
					refresh();
				else if (e.keyCode == SWT.ARROW_LEFT) {
				    if (e.stateMask == 0)
				        moveSelection(-1);
				    else if (e.stateMask == SWT.CTRL) {
				        IEvent event = getSelectionEvent();
				        
				        if (event != null) {
				            event = event.getCauseEvent();

				            if (event != null)
			                    gotoClosestElement(event);
				        }
				    }
                    else if (e.stateMask == SWT.SHIFT) {
                        IEvent event = getSelectionEvent();
                        int moduleId = event.getModuleId();

                        while (event != null) {
                            event = event.getPreviousEvent();
                            
                            if (moduleId == event.getModuleId()) {
                                gotoClosestElement(event);
                                break;
                            }
                        }
                    }
				}
				else if (e.keyCode == SWT.ARROW_RIGHT) {
                    if (e.stateMask == 0)
                        moveSelection(1);
                    else if (e.stateMask == SWT.CTRL) {
                        IEvent event = getSelectionEvent();
                        
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
                        IEvent event = getSelectionEvent();
                        int moduleId = event.getModuleId();

                        while (event != null) {
                            event = event.getNextEvent();
                            
                            if (moduleId == event.getModuleId()) {
                                gotoClosestElement(event);
                                break;
                            }
                        }
                    }
				}
				else if (e.keyCode == SWT.ARROW_UP)
					scrollAxes((int)Math.floor(-axisSpacing - 1));
				else if (e.keyCode == SWT.ARROW_DOWN)
					scrollAxes((int)Math.ceil(axisSpacing + 1));
				else if (e.keyCode == SWT.PAGE_UP)
					scrollAxes(-getViewportHeight());
				else if (e.keyCode == SWT.PAGE_DOWN)
					scrollAxes(getViewportHeight());
				else if (e.keyCode == SWT.HOME)
					gotoBegin();
				else if (e.keyCode == SWT.END)
					gotoEnd();
				else if (e.keyCode == SWT.KEYPAD_ADD || e.character == '+' || e.character == '=')
					zoomIn();
				else if (e.keyCode == SWT.KEYPAD_SUBTRACT || e.character == '-')
					zoomOut();
			}
		});
	}

	/**
	 * See setFollowSelection().
	 */
	public boolean getFollowSelection() {
		return followSelection;
	}

	/**
	 * Sets whether this widget should always switch to the element which comes in
	 * the selection (=true), or stick to the input set with setInput() (=false).
	 * The proper setting typically depends on whether the widget is used in an
	 * editor (false) or in a view (true).
	 *
	 * Default is true.
	 */
	public void setFollowSelection(boolean followSelection) {
		this.followSelection = followSelection;
	}

	/**
	 * Returns chart scale, that is, the number of pixels a "timeline unit" maps to.
     *
	 * The meaning of "timeline unit" depends on the timeline mode (see enum TimelineMode).
	 */
	public double getPixelPerTimelineCoordinate() {
		return pixelPerTimelineCoordinate;
	}

	/**
	 * Set chart scale (number of pixels a "timeline unit" maps to).
	 */
	public void setPixelPerTimelineCoordinate(double pixelPerTimelineCoordinate) {
		Assert.isTrue(pixelPerTimelineCoordinate > 0);
		this.pixelPerTimelineCoordinate = pixelPerTimelineCoordinate;
		clearCanvasCacheAndRedraw();
	}

	/**
	 * Returns the pixel distance between adjacent axes in the chart.
	 */
	public double getAxisSpacing() {
		return axisSpacing;
	}

	/**
	 * Sets the pixel distance between adjacent axes in the chart.
	 */
	public void setAxisSpacing(double axisSpacing) {
		this.axisSpacing = Math.max(1, axisSpacing);
		axisModuleYs = null;
		invalidVirtualSize = true;
		clearCanvasCacheAndRedraw();
	}

	public AxisSpacingMode getAxisSpacingMode() {
		return axisSpacingMode;
	}

	public void setAxisSpacingMode(AxisSpacingMode axisSpacingMode) {
		this.axisSpacingMode = axisSpacingMode;
		calculateAxisSpacing();
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
     * Returns whether self messages are shown in the chart.
     */
    public boolean getShowSelfMessages() {
        return showSelfMessages;
    }

    /**
     * Shows/Hides self messages.
     */
    public void setShowSelfMessages(boolean showSelfMessages) {
        this.showSelfMessages = showSelfMessages;
        clearCanvasCacheAndRedraw();
    }

	/**
	 * Returns whether reuse messages are shown in the chart.
	 */
	public boolean getShowReuseMessages() {
		return showReuseMessages;
	}

	/**
	 * Shows/Hides reuse messages.
	 */
	public void setShowReuseMessages(boolean showReuseMessages) {
		this.showReuseMessages = showReuseMessages;
		clearCanvasCacheAndRedraw();
	}

	/**
	 * Returns whether event numbers are shown in the chart.
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
	 * Returns whether arrow heads are shown in the chart.
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
     * Returns whether zero simulation time regions are shown in the chart.
     */
    public void setShowZeroSimulationTimeRegions(boolean showZeroSimulationTimeRegions) {
        this.showZeroSimulationTimeRegions = showZeroSimulationTimeRegions;
        clearCanvasCacheAndRedraw();
    }

    /**
     * Shows/hides zero simulation time regions.
     */
    public boolean getShowAxisLabels() {
        return showAxisLabels;
    }

    /**
     * Returns whether zero simulation time regions are shown in the chart.
     */
    public void setShowAxisLabels(boolean showAxisLabels) {
        this.showAxisLabels = showAxisLabels;
        clearCanvasCacheAndRedraw();
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
		double[] leftRightSimulationTimes = getViewportSimulationTimeRange();
		sequenceChartFacade.setTimelineMode(timelineMode.ordinal());
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
		calculateAxisModulePositions();
		calculateAxisModuleIndices();
		// manual reordering calls paint so this must be later than setting the positions
		axisModuleYs = null;
		clearCanvasCacheAndRedraw();
	}
	
	public void showManualOrderingDialog() {
        manualAxisOrder.showManualOrderDialog(axisModules.toArray(new ModuleTreeItem[0]));
	}

	/**
	 * Returns the currently visible range of simulation times as an array of two doubles.
	 */
	public double[] getViewportSimulationTimeRange()
	{
		return new double[] {getViewportLeftSimulationTime(), getViewportRightSimulationTime()};
	}

	/**
	 * Sets the range of visible simulation times as an array of two doubles.
	 */
	public void setViewportSimulationTimeRange(double[] leftRightSimulationTimes) {
		zoomToSimulationTimeRange(leftRightSimulationTimes[0], leftRightSimulationTimes[1]);
	}

	/**
	 * Returns the smallest visible simulation time.
	 */
	public double getViewportLeftSimulationTime() {
		return getSimulationTimeForViewportCoordinate(0);
	}

	/**
	 * Returns the simulation time visible at the canvas's center.
	 */
	public double getViewportCenterSimulationTime() {
		return getSimulationTimeForViewportCoordinate(getViewportWidth() / 2);
	}

	/**
	 * Returns the biggest visible simulation time.
	 */
	public double getViewportRightSimulationTime() {
		return getSimulationTimeForViewportCoordinate(getViewportWidth());
	}

	/*************************************************************************************
	 * SCROLLING, GOTOING
	 */

	protected void relocateFixPoint(IEvent event, int fixPointViewportCoordinate) {
		this.fixPointViewportCoordinate = fixPointViewportCoordinate;
		
		if (event != null)
		    sequenceChartFacade.relocateTimelineCoordinateSystem(event);
		else
            sequenceChartFacade.undefineTimelineCoordinateSystem();

		if (eventLog != null && !eventLog.isEmpty()) {
			if (eventLog.getLastEvent().getSimulationTime().doubleValue() <= getSimulationTimeForViewportCoordinate(getViewportWidth())) {
				this.fixPointViewportCoordinate = getViewportWidth();
				sequenceChartFacade.relocateTimelineCoordinateSystem(eventLog.getLastEvent());
			}

			if (getSimulationTimeForViewportCoordinate(0) < 0) {
				this.fixPointViewportCoordinate = 0;
				sequenceChartFacade.relocateTimelineCoordinateSystem(eventLog.getFirstEvent());
			}
		}
	}

	@Override
	protected long clipX(long x) {
		// the position of the visible area is not limited to a [0, max] range
		return x;
	}

	@Override
	protected int configureHorizontalScrollBar(ScrollBar scrollBar, long virtualSize, long virtualPos, int widgetSize) {
		ScrollBar horizontalBar = getHorizontalBar();
		horizontalBar.setMinimum(0);

		long[] eventPtrRange = null;

		if (eventLog != null && !eventLogInput.isCanceled())
			eventPtrRange = getFirstLastEventForPixelRange(0, getViewportWidth());

		if (eventPtrRange != null && eventPtrRange[0] != 0 && eventPtrRange[1] != 0 &&
			(sequenceChartFacade.Event_getPreviousEvent(eventPtrRange[0]) != 0 || sequenceChartFacade.Event_getNextEvent(eventPtrRange[1]) != 0))
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

	@Override
	protected void adjustHorizontalScrollBar() {
		if (eventLog != null && !eventLogInput.isCanceled()) {
			long[] eventPtrRange = getFirstLastEventForPixelRange(0, getViewportWidth());
			long startEventPtr = eventPtrRange[0];
			long endEventPtr = eventPtrRange[1];
			double topPercentage = startEventPtr == 0 ? 0 : eventLog.getApproximatePercentageForEventNumber(sequenceChartFacade.Event_getEventNumber(startEventPtr));
			double bottomPercentage = endEventPtr == 0 ? 1 : eventLog.getApproximatePercentageForEventNumber(sequenceChartFacade.Event_getEventNumber(endEventPtr));
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
			scroll(-1);
		else if (e.detail == SWT.ARROW_DOWN)
			scroll(1);
		else if (e.detail == SWT.PAGE_UP)
			scroll(-10);
		else if (e.detail == SWT.PAGE_DOWN)
			scroll(10);
		else if (percentage == 0)
			scrollToBegin();
		else if (percentage == 1)
			scrollToEnd();
		else
			scrollToElement(eventLog.getApproximateEventAt(percentage));
	}

	@Override
	public void scrollHorizontalTo(long x) {
		fixPointViewportCoordinate -= x - getViewportLeft();
		followEnd = false;

		super.scrollHorizontalTo(x);
	}

	public void scrollToBegin() {
		if (!eventLogInput.isCanceled())
			scrollToElement(eventLog.getFirstEvent(), 0);
	}

	public void scrollToEnd() {
		if (!eventLogInput.isCanceled()) {
			scrollToElement(eventLog.getLastEvent(), getViewportWidth());
			followEnd = true;
		}
	}

	public void scrollAxes(int dy) {
		scrollVerticalTo(getViewportTop() + dy);
	}

	public void scroll(int numberOfEvents) {
		long[] eventPtrRange = getFirstLastEventForPixelRange(0, getViewportWidth());
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

		IEvent neighbourEvent = eventLog.getNeighbourEvent(sequenceChartFacade.Event_getEvent(eventPtr), numberOfEvents);

		if (neighbourEvent != null)
			scrollToElement(neighbourEvent);
	}

	public void scrollToElement(IEvent event) {
		scrollToElement(event, getViewportWidth() / 2);
	}

	public void scrollToElement(IEvent event, int viewportX) {
		int d = EVENT_SELECTION_RADIUS * 2;

		Assert.isTrue(event.getEventLog().equals(eventLog));

		long[] eventPtrRange = getFirstLastEventForPixelRange(0, getViewportWidth());
		long startEventPtr = eventPtrRange[0];
		long endEventPtr = eventPtrRange[1];
		boolean found = false;

		// look one event backward
		long previousEventPtr = sequenceChartFacade.Event_getPreviousEvent(startEventPtr);
		if (previousEventPtr != 0)
			startEventPtr = previousEventPtr;

		// and forward so that one additional event scrolling can be done with less distraction
		long nextEventPtr = sequenceChartFacade.Event_getNextEvent(endEventPtr);
		if (nextEventPtr != 0)
			endEventPtr = nextEventPtr;

		for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.Event_getNextEvent(eventPtr)) {
			if (eventPtr == event.getCPtr())
				found = true;

			if (eventPtr == endEventPtr)
				break;
		}

		if (!found) {
			relocateFixPoint(event, viewportX);
			clearCanvasCache();
		}
		else {
			long x = getViewportLeft() + getEventXViewportCoordinate(event.getCPtr());
			scrollHorizontalToRange(x - d, x + d);
		}

		long y = getViewportTop() + (isInitializationEvent(event) ? 0 : getEventYViewportCoordinate(event.getCPtr()));
		scrollVerticalToRange(y - d, y + d);
		adjustHorizontalScrollBar();

		followEnd = false;

		redraw();
	}

	public void scrollToSelectionElement() {
		IEvent event = getSelectionEvent();

		if (event != null)
			scrollToElement(event);
	}

	/**
	 * Scroll the canvas making the simulation time visible at the center.
	 */
	public void scrollToSimulationTimeWithCenter(double time) {
		scrollHorizontal(getViewportCoordinateForSimulationTime(time) - getViewportWidth() / 2);
		redraw();
	}

	/**
	 * Scroll the canvas to make the axis module visible.
	 */
	public void scrollToAxisModule(ModuleTreeItem axisModule) {
		for (int i = 0; i < axisModules.size(); i++)
			if (axisModules.get(i) == axisModule)
				scrollVerticalTo(axisModuleYs[i] - axisRenderers[i].getHeight() / 2 - getViewportHeight() / 2);
	}

	public void moveSelection(int numberOfEvents) {
		IEvent selectionEvent = getSelectionEvent();

		if (selectionEvent != null) {
			IEvent neighbourEvent = eventLog.getNeighbourEvent(selectionEvent, numberOfEvents);

			if (neighbourEvent != null)
				gotoElement(neighbourEvent);
		}
	}

	public void gotoBegin() {
		IEvent firstEvent = eventLog.getFirstEvent();

		if (firstEvent != null)
			setSelectionEvent(firstEvent);

		scrollToBegin();
	}

	public void gotoEnd() {
		IEvent lastEvent = eventLog.getLastEvent();

		if (lastEvent != null)
			setSelectionEvent(lastEvent);

		scrollToEnd();
	}

	public void gotoElement(IEvent event) {
		setSelectionEvent(event);
		scrollToElement(event);
	}

	public void gotoClosestElement(IEvent event) {
		// TODO: find the closest that matches the filter if any
		gotoElement(event);
	}

	/*************************************************************************************
	 * ZOOMING
	 */
	
	public void defaultZoom() {
        eventLogInput.runWithProgressMonitor(new Runnable() {
            public void run() {
                calculatePixelPerTimelineUnit(getViewportWidth());
                calculateVirtualSize();
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
				double time = getViewportCenterSimulationTime();
				setPixelPerTimelineCoordinate(getPixelPerTimelineCoordinate() * zoomFactor);
				calculateVirtualSize();
				clearCanvasCacheAndRedraw();
				scrollToSimulationTimeWithCenter(time);
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
				setPixelPerTimelineCoordinate(getViewportWidth() / timelineCoordinateDelta);
				calculateVirtualSize();
				clearCanvasCacheAndRedraw();
				scrollHorizontal(getViewportCoordinateForTimelineCoordinate(timelineCoordinate));
			}
		});
	}

	/**
	 * Scroll the canvas so to make start and end simulation times visible.
	 */
	public void zoomToSimulationTimeRange(final double startSimulationTime, final double endSimulationTime) {
		eventLogInput.runWithProgressMonitor(new Runnable() {
			public void run() {
				if (!Double.isNaN(endSimulationTime) && startSimulationTime != endSimulationTime) {
				    double safeEndSimulationTime = Math.min(endSimulationTime, eventLog.getLastEvent().getSimulationTime().doubleValue());
					double timelineUnitDelta = sequenceChartFacade.getTimelineCoordinateForSimulationTime(safeEndSimulationTime) - sequenceChartFacade.getTimelineCoordinateForSimulationTime(startSimulationTime);

					if (timelineUnitDelta > 0)
						setPixelPerTimelineCoordinate(getViewportWidth() / timelineUnitDelta);
				}

				scrollHorizontal(getViewportCoordinateForSimulationTime(startSimulationTime));

				invalidVirtualSize = true;
				clearCanvasCacheAndRedraw();
			}
		});
	}

	/**
	 * Scroll the canvas so to make start and end simulation times visible, but leave some pixels free on both sides.
	 */
	public void zoomToSimulationTimeRange(double startSimulationTime, double endSimulationTime, int pixelInset) {
		if (pixelInset > 0) {
			// NOTE: we can't go directly there, so here is an approximation
			for (int i = 0; i < 3; i++) {
				double newStartSimulationTime = getSimulationTimeForViewportCoordinate(getViewportCoordinateForSimulationTime(startSimulationTime) - pixelInset);
				double newEndSimulationTime = getSimulationTimeForViewportCoordinate(getViewportCoordinateForSimulationTime(endSimulationTime) + pixelInset);

				zoomToSimulationTimeRange(newStartSimulationTime, newEndSimulationTime);
			}
		}
		else
			zoomToSimulationTimeRange(startSimulationTime, endSimulationTime);
	}

	public void zoomToMessageDependency(IMessageDependency messageDependency) {
		zoomToSimulationTimeRange(messageDependency.getCauseEvent().getSimulationTime().doubleValue(), messageDependency.getConsequenceEvent().getSimulationTime().doubleValue(), (int)(getViewportWidth() * 0.1));
	}

	/**
	 * Scroll the canvas to make the value at the given simulation time visible at once.
	 */
	public void zoomToAxisValue(ModuleTreeItem axisModule, double simulationTime) {
		for (int i = 0; i < axisModules.size(); i++)
			if (axisModules.get(i) == axisModule) {
				IAxisRenderer axisRenderer = axisRenderers[i];

				if (axisRenderer instanceof AxisVectorBarRenderer) {
					AxisVectorBarRenderer axisVectorBarRenderer = (AxisVectorBarRenderer)axisRenderer;
					zoomToSimulationTimeRange(
						axisVectorBarRenderer.getSimulationTime(axisVectorBarRenderer.getIndex(simulationTime, true)),
						axisVectorBarRenderer.getSimulationTime(axisVectorBarRenderer.getIndex(simulationTime, false)),
						(int)(getViewportWidth() * 0.1));
				}
			}
	}

	/*************************************************************************************
	 * MISC & EVENT LOG NOTIFICATIONS
	 */

	/**
	 * Returns the currently displayed EventLogInput object.
	 */
	public EventLogInput getInput() {
		return eventLogInput;
	}

	/**
	 * The event log (data) to be displayed in the chart.
	 */
	public IEventLog getEventLog() {
		return eventLog;
	}

	/**
	 * Sets a new EventLogInput to be displayed.
	 */
	public void setInput(final EventLogInput input) {
		// store current settings
		if (eventLogInput != null) {
			eventLogInput.runWithProgressMonitor(new Runnable() {
				public void run() {
					eventLogInput.removeEventLogChangedListener(SequenceChart.this);
					storeState(eventLogInput.getFile());
				}
			});
		}

		// remember input
		eventLogInput = input;
		eventLog = eventLogInput == null ? null : eventLogInput.getEventLog();
		sequenceChartFacade = eventLogInput == null ? null : eventLogInput.getSequenceChartFacade();

		if (eventLogInput != null) {
			eventLogInput.runWithProgressMonitor(new Runnable() {
				public void run() {

					// clear state
					axisModules = null;
					axisModulePositions = null;
					axisModuleYs = null;
					invalidVirtualSize = true;
					clearSelection();

					// restore last known settings
					if (eventLogInput != null) {
						eventLogInput.addEventLogChangedListener(SequenceChart.this);

						if (!restoreState(eventLogInput.getFile())) {
							if (!eventLog.isEmpty() && sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() == -1) {
								sequenceChartFacade.relocateTimelineCoordinateSystem(eventLog.getFirstEvent());
								fixPointViewportCoordinate = 0;
							}

							setAxisModules(eventLogInput.getSelectedModules());
						}

						calculateAxisYs();
					}

					clearCanvasCache();
				}
			});
		}
	}

	public boolean restoreState(IResource resource) {
		PersistentResourcePropertyManager manager = new PersistentResourcePropertyManager(SequenceChartPlugin.PLUGIN_ID, getClass().getClassLoader());

		try {
			if (manager.hasProperty(resource, STATE_PROPERTY)) {
				SequenceChartState sequenceChartState = (SequenceChartState)manager.getProperty(resource, STATE_PROPERTY);
				IEvent fixPointEvent = eventLog.getEventForEventNumber(sequenceChartState.fixPointEventNumber);

				if (fixPointEvent != null) {
					setPixelPerTimelineCoordinate(sequenceChartState.pixelPerTimelineCoordinate);
					setAxisModules(eventLogInput.getSelectedModules());

					if (sequenceChartState.axisStates != null) {
						ResultFileManager resultFileManager = new ResultFileManager();

						for (int i = 0; i < sequenceChartState.axisStates.length; i++) {
							AxisState axisState = sequenceChartState.axisStates[i];

							if (axisState.vectorFileName != null) {
								ResultFile resultFile = resultFileManager.loadFile(axisState.vectorFileName);
								Run run = resultFileManager.getRunsInFile(resultFile).get(0);
								// TODO: compare it against log file's run
								FileRunList fileRunList = new FileRunList();
								fileRunList.add(resultFileManager.getFileRun(resultFile, run));
								IDList idList = resultFileManager.filterIDList(resultFileManager.getAllVectors(), fileRunList, axisState.vectorModuleFullPath, axisState.vectorName);

								if (idList.size() == 1) {
									long id = idList.get(0);
									ResultItem resultItem = resultFileManager.getItem(id);
									XYArray data = VectorFileUtil.getDataOfVector(resultFileManager, id, true);
									setAxisRenderer(getAxisModule(axisState.vectorModuleFullPath),
										new AxisVectorBarRenderer(this, axisState.vectorFileName, resultItem, data));
								}
							}
						}
					}


					// assume height to be at least this
					relocateFixPoint(fixPointEvent, sequenceChartState.fixPointViewportCoordinate);
					calculateVirtualSize();
					scrollVerticalTo(sequenceChartState.viewportTop);

					return true;
				}
			}

			return false;
		}
		catch (Exception e) {
			manager.removeProperty(resource, STATE_PROPERTY);

			throw new RuntimeException(e);
		}
	}

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
				sequenceChartState.pixelPerTimelineCoordinate = pixelPerTimelineCoordinate;

				if (axisModules != null) {
					AxisState[] axisStates = new AxisState[axisModules.size()];

					for (int i = 0; i < axisModules.size(); i++) {
						AxisState axisState = axisStates[i] = new AxisState();
						axisState.moduleFullPath = axisModules.get(i).getModuleFullPath();

						if (axisRenderers[i] instanceof AxisVectorBarRenderer) {
							AxisVectorBarRenderer renderer = (AxisVectorBarRenderer)axisRenderers[i];
							axisState.vectorFileName = renderer.getVectorFileName();
							axisState.vectorModuleFullPath = renderer.getModuleFullPath();
							axisState.vectorName = renderer.getVectorName();
						}
					}

					sequenceChartState.axisStates = axisStates;
				}

				manager.setProperty(resource, STATE_PROPERTY, sequenceChartState);
			}
		}
		catch (Exception e) {
			throw new RuntimeException(e);
		}
	}

	public void eventLogAppended() {
		eventLogChanged();
	}

    public void eventLogOverwritten() {
        eventLogChanged();
    }

    private void eventLogChanged() {
        if (!eventLog.isEmpty() && 
            (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() == -1 ||
             sequenceChartFacade.getTimelineCoordinateSystemOriginEvent() == null))
        {
            relocateFixPoint(eventLog.getFirstEvent(), 0);
        }

        if (debug)
			System.out.println("SequenceChart got notification about event log change");

		configureScrollBars();
		adjustHorizontalScrollBar();
		clearCanvasCache();

		if (followEnd)
		{
			if (debug)
				System.out.println("Scrolling to follow event log change");

            if (!eventLog.isEmpty())
                scrollToEnd();
		}
		else
			redraw();
    }

	public void eventLogFiltered() {
		eventLog = eventLogInput.getEventLog();
		int timelineCoordinateSystemOriginEventNumber = sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber();

		if (timelineCoordinateSystemOriginEventNumber != -1) {
			FilteredEventLog filteredEventLog = (FilteredEventLog)eventLogInput.getEventLog();
			IEvent closestEvent = filteredEventLog.getMatchingEventInDirection(timelineCoordinateSystemOriginEventNumber, false);

			if (closestEvent != null)
				sequenceChartFacade.relocateTimelineCoordinateSystem(closestEvent);
			else {
				closestEvent = filteredEventLog.getMatchingEventInDirection(timelineCoordinateSystemOriginEventNumber, true);

				if (closestEvent != null)
					sequenceChartFacade.relocateTimelineCoordinateSystem(closestEvent);
			}

			setAxisModules(eventLogInput.getSelectedModules());
		}

		clearCanvasCacheAndRedraw();
	}

	public void eventLogFilterRemoved() {
		eventLog = eventLogInput.getEventLog();

		if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() != -1)
			sequenceChartFacade.relocateTimelineCoordinateSystem(sequenceChartFacade.getTimelineCoordinateSystemOriginEvent());

		clearCanvasCacheAndRedraw();
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

	private ModuleTreeItem getAxisModule(String moduleFullPath) {
		for (ModuleTreeItem axisModule : axisModules)
			if (moduleFullPath.equals(axisModule.getModuleFullPath()))
				return axisModule;

		return null;
	}

	/**
	 * Sets the contributor used to build the pop-up menu in the chart.
	 */
	public void setSequenceChartContributor(SequenceChartContributor sequenceChartContributor) {
		menuManager = new MenuManager();
		sequenceChartContributor.contributeToPopupMenu(menuManager);
		setMenu(menuManager.createContextMenu(this));
	}

	public ArrayList<ModuleTreeItem> getAxisModules() {
		return axisModules;
	}

	/**
	 * Sets which modules should have axes. Items in axisModules
	 * should point to elements in the moduleTree.
	 */
	public void setAxisModules(ArrayList<ModuleTreeItem> axisModules) {
		this.axisModules = axisModules;
		this.axisRenderers = new IAxisRenderer[axisModules.size()];

		for (int i = 0; i < axisRenderers.length; i++) {
		    ModuleTreeItem axisModule = axisModules.get(i);
			IAxisRenderer axisRenderer = getAxisRenderer(axisModule);

			if (axisRenderer == null)
				axisRenderer = new AxisLineRenderer(this, axisModule);

			moduleIdToAxisRendererMap.put(axisModules.get(i).getModuleId(), axisRenderer);
			axisRenderers[i] = axisRenderer;
		}

		calculateAxisModuleIndices();
		calculateAxisSpacing();

		axisModuleYs = null;
		axisModulePositions = null;
		invalidVirtualSize = true;
		calculateAxisModulePositions();
		calculateAxisModuleIndices();
		clearCanvasCacheAndRedraw();
	}

	public IAxisRenderer getAxisRenderer(ModuleTreeItem axisModule) {
		return moduleIdToAxisRendererMap.get(axisModule.getModuleId());
	}

	/**
	 * Associates the axis renderer with the given axis.
	 */
	public void setAxisRenderer(ModuleTreeItem axisModule, IAxisRenderer axisRenderer) {
		moduleIdToAxisRendererMap.put(axisModule.getModuleId(), axisRenderer);

		int index = axisModules.indexOf(axisModule);
		if (index != -1) {
			axisRenderers[index] = axisRenderer;
			axisModuleYs = null;
			invalidVirtualSize = true;
			calculateAxisSpacing();
			clearCanvasCacheAndRedraw();
		}
	}

	/*************************************************************************************
	 * LAZY CALCULATIONS DONE NOT LATER THAN PAINT
	 */

	/**
	 * Sorts axis modules depending on timeline ordering mode.
	 */
	private void calculateAxisModulePositions() {
		eventLogInput.runWithProgressMonitor(new Runnable() {
			public void run() {
				if (axisModulePositions == null)
					axisModulePositions = new int[axisModules.size()];

				ModuleTreeItem[] axisModulesArray = axisModules.toArray(new ModuleTreeItem[0]);

				switch (axisOrderingMode) {
					case MANUAL:
						manualAxisOrder.calculateOrdering(axisModulesArray, axisModulePositions);
						break;
					case MODULE_ID:
						new AxisOrderByModuleId().calculateOrdering(axisModulesArray, axisModulePositions);
						break;
					case MODULE_NAME:
						new AxisOrderByModuleName().calculateOrdering(axisModulesArray, axisModulePositions);
						break;
					case MINIMIZE_CROSSINGS:
						new FlatAxisOrderByMinimizingCost(eventLogInput).calculateOrdering(axisModulesArray, axisModulePositions);
						break;
					case MINIMIZE_CROSSINGS_HIERARCHICALLY:
						new HierarchicalAxisOrderByMinimizingCost(eventLogInput).calculateOrdering(axisModulesArray, axisModulePositions);
						break;
					default:
						throw new RuntimeException("Unknown axis ordering mode");
				}
			}
		});
	}

	/**
	 * Calculates top y coordinates of axis bounding boxes based on height returned by each axis.
	 */
	private void calculateAxisYs() {
		axisModuleYs = new int[axisModules.size()];

		for (int i = 0; i < axisModuleYs.length; i++) {
			double y = 0;

			for (int j = 0; j < axisModuleYs.length; j++)
				if (axisModulePositions[j] < axisModulePositions[i])
					y += axisSpacing + axisRenderers[j].getHeight();

			axisModuleYs[i] = (int)Math.round((AXIS_OFFSET + axisSpacing + y));
		}
	}

	/**
	 * Calculates indices for axis modules.
	 */
	private void calculateAxisModuleIndices() {
		for (int i = 0; i < axisModules.size(); i++) {
			final int index = i;
			ModuleTreeItem axisModule = axisModules.get(i);
			axisModule.visitLeaves(new ModuleTreeItem.IModuleTreeItemVisitor() {
				public void visit(ModuleTreeItem axisModule) {
					moduleIdToAxisModuleIndexMap.put(axisModule.getModuleId(), index);
				}
			});
		}
	}

	/**
	 * Calculates initial pixelPerTimelineUnit.
	 */
	private void calculatePixelPerTimelineUnit(int viewportWidth) {
	    if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() != -1) {
    	    IEvent referenceEvent = sequenceChartFacade.getTimelineCoordinateSystemOriginEvent();
    		int distance = Math.min(20, eventLog.getApproximateNumberOfEvents());
    
    		if (distance > 1) {
    			double referenceEventTimelineCoordinate = sequenceChartFacade.getTimelineCoordinate(referenceEvent);
    			double otherEventTimelineCoordinate = sequenceChartFacade.getTimelineCoordinate(eventLog.getNeighbourEvent(referenceEvent, distance - 1));
    			double timelineCoordinateDelta = otherEventTimelineCoordinate - referenceEventTimelineCoordinate;
    			setPixelPerTimelineCoordinate(viewportWidth / timelineCoordinateDelta);
    		}
    		else
                setPixelPerTimelineCoordinate(1);
	    }
	}

	/**
	 * Calculates virtual size of the canvas. The width is an approximation while height is precise.
	 */
	private void calculateVirtualSize() {
		int height = getSumAxesHeight() + (int)Math.round(axisModules.size() * axisSpacing) + AXIS_OFFSET * 2;
		setVirtualSize(getViewportWidth() * eventLog.getApproximateNumberOfEvents(), height);

		invalidVirtualSize = false;
	}

	/**
	 * Distributes available window space among axes evenly if auto axis spacing mode is turned on.
	 */
	private void calculateAxisSpacing() {
		if (axisSpacingMode == AxisSpacingMode.AUTO) {
			if (axisModules.size() == 0)
				setAxisSpacing(0);
			else
				setAxisSpacing(Math.max(MINIMUM_AXIS_SPACING_TO_DISPLAY_LABELS, (double)(getViewportHeight() - AXIS_OFFSET * 2 - getSumAxesHeight()) / axisModules.size()));
		}
	}

	private void calculateStuff() {
		if (pixelPerTimelineCoordinate == 0)
			calculatePixelPerTimelineUnit(getViewportWidth());
		
		if (invalidVirtualSize)
			calculateVirtualSize();

		if (axisModulePositions == null)
			calculateAxisModulePositions();

		if (axisModuleYs == null)
			calculateAxisYs();

		calculateTicks(getViewportWidth());
	}

	private int getSumAxesHeight() {
		int height = 0;

		for (IAxisRenderer axisRenderer : axisRenderers)
			height += axisRenderer.getHeight();

		return height;
	}

	/*************************************************************************************
	 * DRAWING
	 */

	private Graphics createGraphics(GC gc) {
		Graphics graphics = new SWTGraphics(gc);
		graphics.setAntialias(antiAlias ? SWT.ON : SWT.OFF);
		graphics.setTextAntialias(SWT.ON);
		return graphics;
	}

	public void refresh() {
	    internalErrorHappenedDuringPaint = false;
		eventLogInput.resetCanceled();

		if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() != -1)
			sequenceChartFacade.relocateTimelineCoordinateSystem(sequenceChartFacade.getTimelineCoordinateSystemOriginEvent());

		clearCanvasCacheAndRedraw();
	}

	public void clearCanvasCacheAndRedraw() {
		clearCanvasCache();
		redraw();
	}

	@Override
	protected void paint(final GC gc) {
	    paintHasBeenFinished = false;
	    
        if (internalErrorHappenedDuringPaint)
            drawNotificationMessage(gc, "Internal error happend during painting. Try to reset zoom, position, filter, etc. and press refresh. Sorry for your inconvenience.");
        else if (eventLogInput.isCanceled())
	        drawNotificationMessage(gc,
                "Processing of a long running event log operation was cancelled, therefore the chart is incomplete and cannot be drawn.\n" +
                "Either try changing some filter parameters or select refresh from the menu. Sorry for your inconvenience.");
		else if (eventLogInput.isLongRunningOperationInProgress())
			drawNotificationMessage(gc, "Processing a long running event log operation. Please wait.");
		else
			eventLogInput.runWithProgressMonitor(new Runnable() {
				public void run() {
				    try {
    					if (eventLogInput != null)
    						calculateStuff();
    
    					SequenceChart.super.paint(gc);
    					paintHasBeenFinished = true;
    
    					if (eventLogInput != null && debug)
    						System.out.println("Read " + eventLog.getFileReader().getNumReadBytes() + " bytes, " + eventLog.getFileReader().getNumReadLines() + " lines, " + eventLog.getNumParsedEvents() + " events from " + eventLogInput.getFile().getName());
				    }
				    catch (RuntimeException e) {
				        if (eventLogInput.isEventLogChangedException(e))
				            eventLogInput.checkEventLogForChanges();
				        else {
	                        internalErrorHappenedDuringPaint = true;
				            throw e;
				        }
				    }
				}
			});
	}

	@Override
	protected void paintCachableLayer(GC gc) {
		if (eventLogInput != null) {
			Graphics graphics = createGraphics(gc);
			graphics.translate(0, GUTTER_HEIGHT);

			drawSequenceChart(graphics);

			graphics.translate(0, -GUTTER_HEIGHT);
			graphics.dispose();
		}
	}

	@Override
	protected void paintNoncachableLayer(GC gc) {
		if (eventLogInput != null) {
			Graphics graphics = createGraphics(gc);
			graphics.translate(0, GUTTER_HEIGHT);

			if (showAxisLabels)
			    drawAxisLabels(graphics);

	        drawEventBookmarks(graphics);
	        drawEventSelectionMarks(graphics);

	        graphics.translate(0, -GUTTER_HEIGHT);

	        drawGutters(graphics, getViewportHeight());
	        drawTickUnderMouse(graphics, getViewportHeight());
	        drawSimulationTimeRange(graphics, getViewportWidth());
	        drawTickPrefix(graphics);

	        graphics.translate(0, GUTTER_HEIGHT);

	        if (drawStuffUnderMouse) {
	            drawStuffUnderMouse(graphics);
	            drawStuffUnderMouse = false;
	        }

	        graphics.translate(0, -GUTTER_HEIGHT);
	        graphics.dispose();
		}
	}

	public void paintArea(Graphics graphics) {
		graphics.getClip(Rectangle.SINGLETON);
		graphics.translate(0, GUTTER_HEIGHT);

		calculateTicks(Rectangle.SINGLETON.width);
		drawSequenceChart(graphics);
		drawAxisLabels(graphics);

		graphics.translate(0, -GUTTER_HEIGHT);

        drawGutters(graphics, (int)getVirtualHeight());
        drawSimulationTimeRange(graphics, Rectangle.SINGLETON.width);
        drawTickPrefix(graphics);

        graphics.translate(0, GUTTER_HEIGHT);
    }

	protected Point getTextExtent(Graphics graphics, String string) {
// TODO factor out to the optional org.omnetpp.imageexport plugin		
//		if (graphics instanceof GraphicsSVG) {
//			java.awt.Graphics g = ((GraphicsSVG)graphics).getSVGGraphics2D();
//			java.awt.geom.Rectangle2D r = g.getFontMetrics().getStringBounds(string, g);
//
//			return new Point((int)Math.ceil(r.getWidth()), (int)Math.ceil(r.getHeight()));
//		}
//		else {
			try {
				SWTGraphics g = (SWTGraphics) graphics;
				Class<SWTGraphics> cls = SWTGraphics.class;
				Field field = cls.getDeclaredField("gc");
				field.setAccessible(true);
				GC gc = (GC)field.get(g);
				// TODO: what a hack to send the font down to GC?!
				g.drawText("", 0, 0);
				return gc.textExtent(string);
			}
			catch (Exception e) {
				throw new RuntimeException(e);
			}
//		}
	}

	protected void drawCancelMessage(Graphics graphics) {
		graphics.setForegroundColor(ColorFactory.RED4);
		graphics.setBackgroundColor(ColorFactory.WHITE);
		graphics.setFont(font);
		int x = getViewportWidth() / 2;
		int y = getViewportHeight() / 2;
		String text = "Processing of a long running event log operation was cancelled, therefore the chart is incomplete and cannot be drawn.";
		Point p = getTextExtent(graphics, text);
		graphics.fillString(text, x - p.x / 2, y - p.y);
		text = "Either try changing some filter parameters or select refresh from the menu. Sorry for your inconvenience.";
		p = getTextExtent(graphics, text);
		graphics.fillString(text, x - p.x / 2, y);
	}

	protected void drawNotificationMessage(GC gc, String text) {
	    String[] lines = text.split("\n");
        Graphics graphics = createGraphics(gc);
		graphics.setForegroundColor(ColorFactory.RED4);
		graphics.setBackgroundColor(ColorFactory.WHITE);
		graphics.setFont(font);

        int x = getViewportWidth() / 2;
        int y = getViewportHeight() / 2;

        for (int i = 0; i < lines.length; i++) {
		    String line = lines[i];
    		Point p = getTextExtent(graphics, line);
    		graphics.fillString(line, x - p.x / 2, y - (lines.length / 2 - i) * p.y);
		}

        graphics.dispose();
	}

	/**
	 * Draw range on top right.
	 */
	protected void drawSimulationTimeRange(Graphics graphics, int viewportWidth) {
		// draw simulation time range
		BigDecimal leftTick = calculateTick(0, 1);
		BigDecimal rightTick = calculateTick(viewportWidth, 1);
		BigDecimal simulationTimeRange = rightTick.subtract(leftTick);
		String timeString = "Range: " + TimeUtils.secondsToTimeString(simulationTimeRange);
		graphics.setFont(font);
		int width = getTextExtent(graphics, timeString).x;
		int x = viewportWidth - width - 3;

		graphics.setForegroundColor(TICK_LABEL_COLOR);
		graphics.setBackgroundColor(INFO_BACKGROUND_COLOR);
		graphics.fillRectangle(x - 3, GUTTER_HEIGHT, width + 6, GUTTER_HEIGHT + 1);
		graphics.setForegroundColor(GUTTER_BORDER_COLOR);
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.drawRectangle(x - 3, GUTTER_HEIGHT, width + 5, GUTTER_HEIGHT);
		graphics.setBackgroundColor(INFO_BACKGROUND_COLOR);
		drawText(graphics, timeString, x, GUTTER_HEIGHT + 2);
	}

	/**
	 * Draw tick prefix on the left side of the top gutter.
	 */
	protected void drawTickPrefix(Graphics graphics) {
		// draw tick prefix on top gutter
		String timeString = TimeUtils.secondsToTimeString(tickPrefix);
		FontData fontData = font.getFontData()[0];
		Font newFont = new Font(font.getDevice(), fontData.getName(), fontData.getHeight(), SWT.BOLD);
		graphics.setFont(newFont);
		int width = getTextExtent(graphics, timeString).x;

		graphics.setBackgroundColor(INFO_BACKGROUND_COLOR);
		graphics.fillRectangle(0, 0, width + 7, GUTTER_HEIGHT + 1);
		graphics.setForegroundColor(GUTTER_BORDER_COLOR);
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.drawRectangle(0, 0, width + 6, GUTTER_HEIGHT);

		graphics.setBackgroundColor(INFO_BACKGROUND_COLOR);
		drawText(graphics, timeString, 3, 2);
		newFont.dispose();
	}

	/**
	 * Draws the actual sequence chart part including events, messages
	 */
	protected void drawSequenceChart(Graphics graphics) {
		if (eventLog != null) {
			long startMillis = System.currentTimeMillis();

			graphics.getClip(Rectangle.SINGLETON);

			int extraClipping = (showMessageNames || showEventNumbers) ? 300 : 100;
			long[] eventPtrRange = getFirstLastEventForPixelRange(Rectangle.SINGLETON.x - extraClipping, Rectangle.SINGLETON.right() + extraClipping);
			long startEventPtr = eventPtrRange[0];
			long endEventPtr = eventPtrRange[1];

			if (debug && !eventLog.isEmpty())
				System.out.println("Redrawing events from: " + sequenceChartFacade.Event_getEventNumber(startEventPtr) + " to: " + sequenceChartFacade.Event_getEventNumber(endEventPtr));

			if (showZeroSimulationTimeRegions)
			    drawZeroSimulationTimeRegions(graphics, startEventPtr, endEventPtr);

			drawAxes(graphics, startEventPtr, endEventPtr);
	        drawMessageDependencies(graphics);
	        drawEvents(graphics, startEventPtr, endEventPtr);

	        long redrawMillis = System.currentTimeMillis() - startMillis;

	        if (debug)
	        	System.out.println("redraw(): " + redrawMillis + "ms");

	        // turn on/off anti-alias
	        if (antiAlias && redrawMillis > ANTIALIAS_TURN_OFF_AT_MSEC)
	        	antiAlias = false;
	        else if (!antiAlias && redrawMillis < ANTIALIAS_TURN_ON_AT_MSEC)
	        	antiAlias = true;
		}
	}

	private void drawZeroSimulationTimeRegions(Graphics graphics, long startEventPtr, long endEventPtr) {
		long previousEventPtr = -1;
		graphics.getClip(Rectangle.SINGLETON);
		graphics.setBackgroundColor(ZERO_SIMULATION_TIME_REGION_COLOR);

		if (startEventPtr != 0 && endEventPtr != 0) {
			// draw rectangle before the very beginning of the simulation
			int x = Rectangle.SINGLETON.x;
			if (getSimulationTimeForViewportCoordinate(x) == 0) {
				int startX = getViewportCoordinateForSimulationTime(0, true);

				if (x != startX)
					graphics.fillRectangle(x, Rectangle.SINGLETON.y, startX - x, Rectangle.SINGLETON.height);
			}

			// draw rectangles where simulation time has not elapsed between events
			for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.Event_getNextEvent(eventPtr)) {
				if (previousEventPtr != -1) {
					x = getEventXViewportCoordinate(eventPtr);
					int previousX = getEventXViewportCoordinate(previousEventPtr);
					org.omnetpp.common.engine.BigDecimal simulationTime = sequenceChartFacade.Event_getSimulationTime(eventPtr);
					org.omnetpp.common.engine.BigDecimal previousSimulationTime = sequenceChartFacade.Event_getSimulationTime(previousEventPtr);

					if (simulationTime.equals(previousSimulationTime) && x != previousX)
						graphics.fillRectangle(previousX, Rectangle.SINGLETON.y, x - previousX, Rectangle.SINGLETON.height);
				}

				previousEventPtr = eventPtr;

				if (eventPtr == endEventPtr)
					break;
			}

			// draw rectangle after the very end of the simulation
			if (sequenceChartFacade.Event_getNextEvent(endEventPtr) == 0) {
				x = Rectangle.SINGLETON.right();
				int endX = getEventXViewportCoordinate(endEventPtr);

				if (x != endX)
					graphics.fillRectangle(endX, Rectangle.SINGLETON.y, x - endX, Rectangle.SINGLETON.height);
			}
		}
		else
			graphics.fillRectangle(Rectangle.SINGLETON);
	}

	/**
	 * Draws all axes in the given simulation time range.
	 */
	private void drawAxes(Graphics graphics, long startEventPtr, long endEventPtr) {
		for (int i = 0; i < axisModules.size(); i++) {
		    ModuleTreeItem axisModule = axisModules.get(i);
		    drawAxis(graphics, startEventPtr, endEventPtr, i, axisModule);
		}
	}
	
	private void drawAxis(Graphics graphics, long startEventPtr, long endEventPtr, int index, ModuleTreeItem axisModule) {
        int y = axisModuleYs[index] - (int)getViewportTop();
        IAxisRenderer axisRenderer = axisRenderers[index];
        int dy = y;
        graphics.translate(0, dy);
        axisRenderer.drawAxis(graphics, startEventPtr, endEventPtr);
        graphics.translate(0, -dy);
	}

	/**
	 * Draws all message arrows which have visual representation in the given event range.
	 */
	private void drawMessageDependencies(Graphics graphics) {
		long[] eventPtrRange = getFirstLastEventPtrForMessageDependencies();
		long startEventPtr = eventPtrRange[0];
		long endEventPtr = eventPtrRange[1];

		if (startEventPtr != 0 && endEventPtr != 0) {
			PtrVector messageDependencies = sequenceChartFacade.getIntersectingMessageDependencies(startEventPtr, endEventPtr);

			if (debug)
				System.out.println("Drawing " + messageDependencies.size() + " message dependencies");

			VLineBuffer vlineBuffer = new VLineBuffer();
			for (int i = 0; i < messageDependencies.size(); i++)
				drawOrFitMessageDependency(messageDependencies.get(i), -1, -1, -1, graphics, vlineBuffer, startEventPtr, endEventPtr);
		}
	}

	/**
	 * Draws events for the given event range.
	 */
	private void drawEvents(Graphics graphics, long startEventPtr, long endEventPtr) {
		if (startEventPtr != 0 && endEventPtr != 0) {
			HashMap<Integer,Integer> axisYtoLastX = new HashMap<Integer, Integer>();

			for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.Event_getNextEvent(eventPtr)) {
                if (isInitializationEvent(eventPtr))
			        drawEvent(graphics, eventPtr);
			    else {
	                int x = getEventXViewportCoordinate(eventPtr);
    				int y = getEventYViewportCoordinate(eventPtr);
    				Integer lastX = axisYtoLastX.get(y);
    
    				// performance optimization: don't draw event if there's one already drawn exactly there
    				if (lastX == null || lastX.intValue() != x) {
    					axisYtoLastX.put(y, x);
    					drawEvent(graphics, eventPtr, getEventAxisModuleIndex(eventPtr), x, y);
    				}
			    }

				if (eventPtr == endEventPtr)
					break;
			}
		}
	}

	private void drawEvent(Graphics graphics, long eventPtr) {
        int x = getEventXViewportCoordinate(eventPtr);

        if (isInitializationEvent(eventPtr)) {
	        for (int i = 0; i < sequenceChartFacade.Event_getNumConsequences(eventPtr); i++) {
	            long consequenceEventPtr = sequenceChartFacade.MessageDependency_getConsequenceEvent(sequenceChartFacade.Event_getConsequence(eventPtr, i));

                if (consequenceEventPtr != 0) {
                    int y = getEventYViewportCoordinate(consequenceEventPtr);
	                drawEvent(graphics, eventPtr, getEventAxisModuleIndex(consequenceEventPtr), x, y);
                }
	        }
	    }
	    else
	        drawEvent(graphics, eventPtr, getEventAxisModuleIndex(eventPtr), x, getEventYViewportCoordinate(eventPtr));
	}

	private void drawEvent(Graphics graphics, long eventPtr, int axisModuleIndex, int x, int y) {
	    if (isInitializationEvent(eventPtr)) {
            graphics.setForegroundColor(INITIALIZATION_EVENT_BORDER_COLOR);
            graphics.setBackgroundColor(INITIALIZATION_EVENT_BACKGROUND_COLOR);
	    }
	    else if (sequenceChartFacade.Event_isSelfMessageProcessingEvent(eventPtr)) {
			graphics.setForegroundColor(SELF_EVENT_BORDER_COLOR);
			graphics.setBackgroundColor(SELF_EVENT_BACKGROUND_COLOR);
		}
		else {
			graphics.setForegroundColor(EVENT_BORDER_COLOR);
			graphics.setBackgroundColor(EVENT_BACKGROUND_COLOR);
		}

		graphics.fillOval(x - 2, y - 3, 6, 8);
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.drawOval(x - 2, y - 3, 5, 7);

		if (showEventNumbers) {
			graphics.setFont(font);
			drawText(graphics, "#" + sequenceChartFacade.Event_getEventNumber(eventPtr), x + 3, y + 3 + axisRenderers[axisModuleIndex].getHeight() / 2);
		}
	}

	/**
	 * Draws the top and bottom gutters which will display ticks.
	 */
	private void drawGutters(Graphics graphics, int viewportHeigth) {
		graphics.getClip(Rectangle.SINGLETON);
		Rectangle r = Rectangle.SINGLETON;

		// fill gutter backgrounds
		graphics.setBackgroundColor(GUTTER_BACKGROUND_COLOR);
		graphics.fillRectangle(r.x, 0, r.right(), GUTTER_HEIGHT + 1);
		graphics.fillRectangle(r.x, viewportHeigth + GUTTER_HEIGHT - 1, r.right(), GUTTER_HEIGHT + 1);

		drawTicks(graphics, viewportHeigth);

		// draw border around gutters
		graphics.setForegroundColor(GUTTER_BORDER_COLOR);
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.drawRectangle(r.x, 0, r.right() - 1, GUTTER_HEIGHT);
		graphics.drawRectangle(r.x, viewportHeigth + GUTTER_HEIGHT - 1, r.right() - 1, GUTTER_HEIGHT);
	}

	/**
	 * Draws ticks on the gutter.
	 */
	private void drawTicks(Graphics graphics, int viewportHeigth) {
		IEvent lastEvent = eventLog.getLastEvent();
		double endSimulationTime = lastEvent == null ? 0 : lastEvent.getSimulationTime().doubleValue();

		for (BigDecimal tick : ticks) {
			// BigDecimal to double conversions loose precision both in Java and C++ but we must stick to the one in C++
			// so that strange problems do not occur (think of comparing the tick's time to the last known simulation time)
			double simulationTime = Math.min(tick.doubleValue(), endSimulationTime);
			drawTick(graphics, viewportHeigth, TICK_LINE_COLOR, GUTTER_BACKGROUND_COLOR, tick, getViewportCoordinateForSimulationTime(simulationTime), false);
		}
	}

	/**
	 * Draws a tick under the mouse.
	 */
	private void drawTickUnderMouse(Graphics graphics, int viewportHeigth) {
		Point p = toControl(Display.getDefault().getCursorLocation());
		
		if (0 <= p.x && p.x < getViewportWidth() && 0 <= p.y && p.y < getViewportHeight() + GUTTER_HEIGHT * 2) {
			BigDecimal tick = calculateTick(p.x, 1);
			drawTick(graphics, viewportHeigth, MOUSE_TICK_LINE_COLOR, INFO_BACKGROUND_COLOR, tick, p.x, true);
		}
	}

	/**
	 * Draws the message arrows under the mouse with a tick line.
	 */
	private void drawStuffUnderMouse(Graphics graphics) {
		Point p = toControl(Display.getDefault().getCursorLocation());

		ArrayList<IEvent> events = new ArrayList<IEvent>();
		ArrayList<IMessageDependency> messageDependencies = new ArrayList<IMessageDependency>();
		collectStuffUnderMouse(p.x, p.y, events, messageDependencies);

		graphics.setLineWidth(2);

		// 1) if there are events under them mouse highlight them
		if (events.size() > 0) {
			for (IEvent event : events)
				drawEvent(graphics, event.getCPtr());
		}
		// 2) no events: highlight message dependencies
		else if (messageDependencies.size() >= 1) {
			long[] eventPtrRange = getFirstLastEventPtrForMessageDependencies();
			long startEventPtr = eventPtrRange[0];
			long endEventPtr = eventPtrRange[1];

			VLineBuffer vlineBuffer = new VLineBuffer();

			for (IMessageDependency messageDependency : messageDependencies)
				drawOrFitMessageDependency(messageDependency.getCPtr(), -1, -1, -1, graphics, vlineBuffer, startEventPtr, endEventPtr);
		}
		else {
			// 3) no events or message arrows: highlight axis label
			ModuleTreeItem axisModule = findAxisAt(p.y);

			if (axisModule != null) {
	            long[] eventPtrRange = getFirstLastEventPtrForMessageDependencies();
	            long startEventPtr = eventPtrRange[0];
	            long endEventPtr = eventPtrRange[1];

	            int i = getAxisModuleIndexByModuleId(axisModule.getModuleId());
				
	            drawAxisLabel(graphics, i, axisModule);
				drawAxis(graphics, startEventPtr, endEventPtr, i, axisModule);
			}
		}
	}

	/**
	 * Draws a single tick on the gutters and the chart.
	 * The tick value will be drawn on both gutters with a hair line connecting them.
	 */
	private void drawTick(Graphics graphics, int viewportHeight, Color tickColor, Color backgroundColor, BigDecimal tick, int x, boolean mouseTick) {
		String string = TimeUtils.secondsToTimeString(tick.subtract(tickPrefix));
		if (tickPrefix.doubleValue() != 0.0)
			string = "+" + string;

		graphics.setFont(font);
		int stringWidth = getTextExtent(graphics, string).x;
		int boxWidth = stringWidth + 6;
		int boxX = mouseTick ? Math.min(getViewportWidth() - boxWidth, x) : x;

		// draw background
		graphics.setBackgroundColor(backgroundColor);
		graphics.fillRectangle(boxX, 0, boxWidth, GUTTER_HEIGHT + 1);
		graphics.fillRectangle(boxX, viewportHeight + GUTTER_HEIGHT - 1, boxWidth, GUTTER_HEIGHT);

		// draw border only for mouse tick
		if (mouseTick) {
			graphics.setForegroundColor(GUTTER_BORDER_COLOR);
			graphics.setLineStyle(SWT.LINE_SOLID);
			graphics.drawRectangle(boxX, 0, boxWidth - 1, GUTTER_HEIGHT);
			graphics.drawRectangle(boxX, viewportHeight + GUTTER_HEIGHT - 1, boxWidth - 1, GUTTER_HEIGHT);
		}

		// draw tick value
		graphics.setForegroundColor(TICK_LABEL_COLOR);
		graphics.setBackgroundColor(backgroundColor);
		drawText(graphics, string, boxX + 3, 2);
		drawText(graphics, string, boxX + 3, viewportHeight + GUTTER_HEIGHT + 1);

		// draw hair line
		graphics.setLineStyle(SWT.LINE_DOT);
		graphics.setForegroundColor(tickColor);
		if (mouseTick)
			graphics.drawLine(x, GUTTER_HEIGHT, x, viewportHeight + GUTTER_HEIGHT);
		else
			graphics.drawLine(x, 0, x, viewportHeight + GUTTER_HEIGHT * 2);
	}

	/**
	 * Draws the visual representation of selections around events.
	 */
	private void drawEventSelectionMarks(Graphics graphics) {
		long[] eventPtrRange = getFirstLastEventForPixelRange(0 - EVENT_SELECTION_RADIUS, getViewportWidth() + EVENT_SELECTION_RADIUS);
		long startEventPtr = eventPtrRange[0];
		long endEventPtr = eventPtrRange[1];

		if (startEventPtr != 0 && endEventPtr != 0) {
			int startEventNumber = sequenceChartFacade.Event_getEventNumber(startEventPtr);
			int endEventNumber = sequenceChartFacade.Event_getEventNumber(endEventPtr);

			graphics.setAntialias(SWT.ON);

			// draw event selection marks
			if (selectionEventNumbers != null) {
				graphics.setLineStyle(SWT.LINE_SOLID);
			    graphics.setForegroundColor(EVENT_SELECTION_COLOR);

			    for (int selectedEventNumber : selectionEventNumbers)
			    	if (startEventNumber <= selectedEventNumber && selectedEventNumber <= endEventNumber)
			    	    drawSelectionMarks(graphics, eventLog.getEventForEventNumber(selectedEventNumber));
			}
		}
	}

	/**
	 * Draw bookmarks associated with the input file.
	 */
	private void drawEventBookmarks(Graphics graphics) {
		try {
			if (eventLogInput.getFile() != null) {
				IMarker[] markers = eventLogInput.getFile().findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);

				long[] eventPtrRange = getFirstLastEventForPixelRange(0 - EVENT_SELECTION_RADIUS, getViewportWidth() + EVENT_SELECTION_RADIUS);
				long startEventPtr = eventPtrRange[0];
				long endEventPtr = eventPtrRange[1];

				if (startEventPtr != 0 && endEventPtr != 0) {
					int startEventNumber = sequenceChartFacade.Event_getEventNumber(startEventPtr);
					int endEventNumber = sequenceChartFacade.Event_getEventNumber(endEventPtr);

					graphics.setLineStyle(SWT.LINE_SOLID);
				    graphics.setForegroundColor(EVENT_BOOKMARK_COLOR);

					for (int i = 0; i < markers.length; i++) {
						int eventNumber = markers[i].getAttribute("EventNumber", -1);

						if (startEventNumber <= eventNumber && eventNumber <= endEventNumber) {
							IEvent bookmarkedEvent = eventLog.getEventForEventNumber(eventNumber);

							if (bookmarkedEvent != null)
							    drawSelectionMarks(graphics, bookmarkedEvent);
						}
					}
				}
			}
		}
		catch (CoreException e) {
			throw new RuntimeException(e);
		}
	}
    
    private void drawSelectionMarks(Graphics graphics, IEvent event) {
        int x = getEventXViewportCoordinate(event.getCPtr());

        if (isInitializationEvent(event)) {
            long eventPtr = event.getCPtr();
            for (int i = 0; i < sequenceChartFacade.Event_getNumConsequences(eventPtr); i++) {
                long consequenceEventPtr = sequenceChartFacade.MessageDependency_getConsequenceEvent(sequenceChartFacade.Event_getConsequence(eventPtr, i));

                if (consequenceEventPtr != 0) {
                    int y = getEventYViewportCoordinate(consequenceEventPtr);
                    drawSelectionMark(graphics, x, y);
                }
            }
        }
        else {
            int y = getEventYViewportCoordinate(event.getCPtr());
            drawSelectionMark(graphics, x, y);
        }
    }
    
    private void drawSelectionMark(Graphics graphics, int x, int y) {
        graphics.drawOval(x - EVENT_SELECTION_RADIUS, y - EVENT_SELECTION_RADIUS, EVENT_SELECTION_RADIUS * 2 + 1, EVENT_SELECTION_RADIUS * 2 + 1);
    }

	/**
	 * Draws axis labels if there's enough space between axes.
	 */
	private void drawAxisLabels(Graphics graphics) {
		if (MINIMUM_AXIS_SPACING_TO_DISPLAY_LABELS <= axisSpacing) {
			for (int i = 0; i < axisModules.size(); i++) {
				ModuleTreeItem treeItem = axisModules.get(i);
				drawAxisLabel(graphics, i, treeItem);
			}
		}
	}

	private void drawAxisLabel(Graphics graphics, int index, ModuleTreeItem treeItem) {
		int y = axisModuleYs[index] - (int)getViewportTop();
		String label = treeItem.getModuleFullPath();
		graphics.setForegroundColor(LABEL_COLOR);
		graphics.setFont(font);
		drawText(graphics, label, 5, y - MINIMUM_AXIS_SPACING_TO_DISPLAY_LABELS + 1);
	}

	/**
	 * Either draws to the graphics or matches to a point a single message arrow represented by the given message dependency.
	 * It is either drawn as a straight line from the cause event to the consequence event or as a half ellipse if it is a self message.
	 * A message dependency pointing too far away in the figure is drawn by a broken dotted straight line or a dotted half ellipse.
	 * Since it is meaningless and expensive to calculate the exact shapes these are drawn differently.
	 *
	 * The line buffer is used to skip drawing message arrows where there is one already drawn. (very dense arrows)
	 */
	private boolean drawOrFitMessageDependency(long messageDependencyPtr, int fitX, int fitY, int tolerance, Graphics graphics, VLineBuffer vlineBuffer, long startEventPtr, long endEventPtr) {
		Assert.isTrue((graphics != null && vlineBuffer != null) || tolerance != -1);

		long causeEventPtr = sequenceChartFacade.MessageDependency_getCauseEvent(messageDependencyPtr);
		long consequenceEventPtr = sequenceChartFacade.MessageDependency_getConsequenceEvent(messageDependencyPtr);

		if (sequenceChartFacade.MessageDependency_getIsReuse(messageDependencyPtr) && !showReuseMessages)
			return false;

		// events may be omitted from the log
		if (causeEventPtr == 0 || consequenceEventPtr == 0)
			return false;

		// calculate pixel coordinates for message arrow endings
		int startEventNumber = sequenceChartFacade.Event_getEventNumber(startEventPtr);
		int endEventNumber = sequenceChartFacade.Event_getEventNumber(endEventPtr);
		int causeEventNumber = sequenceChartFacade.Event_getEventNumber(causeEventPtr);
		int consequenceEventNumber = sequenceChartFacade.Event_getEventNumber(consequenceEventPtr);
        int invalid = -Integer.MAX_VALUE;
        int x1 = invalid, y1 = isInitializationEvent(causeEventPtr) ? getModuleYViewportCoordinate(sequenceChartFacade.Event_getModuleId(consequenceEventPtr)) : getEventYViewportCoordinate(causeEventPtr);
        int x2 = invalid, y2 = getEventYViewportCoordinate(consequenceEventPtr);
        int fontHeight = font.getFontData()[0].getHeight();

        // calculate horizontal coordinates based on timeline coordinate limit
        double timelineCoordinateLimit = getMaximumMessageDependencyDisplayWidth() / pixelPerTimelineCoordinate;
        if (consequenceEventNumber < startEventNumber || endEventNumber < consequenceEventNumber) {
            // consequence event is out of drawn message dependency range 
        	x1 = getEventXViewportCoordinate(causeEventPtr);
        	double causeTimelineCoordinate = sequenceChartFacade.getTimelineCoordinate(causeEventPtr);
        	double consequenceTimelineCoordinate = sequenceChartFacade.getTimelineCoordinate(consequenceEventPtr, -Double.MAX_VALUE, causeTimelineCoordinate + timelineCoordinateLimit);

        	if (Double.isNaN(consequenceTimelineCoordinate) || consequenceTimelineCoordinate > causeTimelineCoordinate + timelineCoordinateLimit)
  		  		x2 = invalid;
  		  	else
  	        	x2 = getEventXViewportCoordinate(consequenceEventPtr);
        }
        else if (causeEventNumber < startEventNumber || endEventNumber < causeEventNumber) {
            // cause event is out of drawn message dependency range 
        	x2 = getEventXViewportCoordinate(consequenceEventPtr);
        	double consequenceTimelineCoordinate = sequenceChartFacade.getTimelineCoordinate(consequenceEventPtr);
        	double causeTimelineCoordinate = sequenceChartFacade.getTimelineCoordinate(causeEventPtr, consequenceTimelineCoordinate - timelineCoordinateLimit, Double.MAX_VALUE);

        	if (Double.isNaN(causeTimelineCoordinate) || causeTimelineCoordinate < consequenceTimelineCoordinate - timelineCoordinateLimit)
  		  		x1 = invalid;
  		  	else
  	        	x1 = getEventXViewportCoordinate(causeEventPtr);
        }
        else {
            // both events are inside
            x1 = getEventXViewportCoordinate(causeEventPtr);
            x2 = getEventXViewportCoordinate(consequenceEventPtr);

            double causeTimelineCoordinate = sequenceChartFacade.getTimelineCoordinate(causeEventPtr);
            double consequenceTimelineCoordinate = sequenceChartFacade.getTimelineCoordinate(consequenceEventPtr);

            if (consequenceTimelineCoordinate - causeTimelineCoordinate > timelineCoordinateLimit) {
                int viewportCenter = getViewportWidth() / 2;
                
                if (Math.abs(viewportCenter - x1) < Math.abs(viewportCenter - x2))
                    x2 = invalid;
                else
                    x1 = invalid;
            }
        }

        // at least one of the events must be in range
        Assert.isTrue(x1 != invalid || x2 != invalid);

		// line color and style depends on message kind
		if (graphics != null) {
			if (sequenceChartFacade.MessageDependency_getIsReuse(messageDependencyPtr)) {
				graphics.setForegroundColor(MESSAGE_REUSE_COLOR);
				graphics.setLineDash(DOTTED_LINE_PATTERN); // SWT.LINE_DOT style is not what we want
			}
			else {
				graphics.setForegroundColor(MESSAGE_SEND_COLOR);
				graphics.setLineStyle(SWT.LINE_SOLID);
			}
		}

		// test if self-message
		if (y1 == y2) {
		    if (!showSelfMessages)
		        return false;

			int eventNumberDelta = sequenceChartFacade.Event_getEventNumber(consequenceEventPtr) - sequenceChartFacade.Event_getEventNumber(causeEventPtr);
			int numberOfPossibleEllipseHeights = Math.max(1, (int)Math.round((axisSpacing - fontHeight) / (fontHeight + 10)));
			int halfEllipseHeight = (int)Math.max(axisSpacing * (eventNumberDelta % numberOfPossibleEllipseHeights + 1) / (numberOfPossibleEllipseHeights + 1), MINIMUM_HALF_ELLIPSE_HEIGHT);

			// test if it is a vertical line (as zero-width half ellipse)
			if (x1 == x2) {
				y2 = y1 - halfEllipseHeight;

				if (graphics != null) {
					if (vlineBuffer.vlineContainsNewPixel(x1, y2, y1))
						graphics.drawLine(x1, y1, x2, y2);

					if (showArrowHeads)
						drawArrowHead(graphics, null, x1, y1, 0, 1);

					if (showMessageNames)
						drawMessageDependencyLabel(graphics, messageDependencyPtr, x1, y1, 2, -15);
				}
				else
					return lineContainsPoint(x1, y1, x2, y2, fitX, fitY, tolerance);
			}
			else {
				int quarterEllipseWidth = LONG_MESSAGE_ARROW_WIDTH;
				boolean showArrowHeads = this.showArrowHeads;
				int xm, ym = y1 - halfEllipseHeight;

				// cause is too far away
				if (x1 == invalid) {
					x1 = x2 - quarterEllipseWidth * 2;
					xm = x1 + quarterEllipseWidth;

					// draw quarter ellipse starting with a horizontal straight line from the left
					Rectangle.SINGLETON.setLocation(x2 - quarterEllipseWidth * 2, ym);
					Rectangle.SINGLETON.setSize(quarterEllipseWidth * 2, halfEllipseHeight * 2);

					if (graphics != null) {
						graphics.drawArc(Rectangle.SINGLETON, 0, 90);
						graphics.setLineStyle(SWT.LINE_DOT);
						graphics.drawLine(x1, ym, xm, ym);

						if (sequenceChartFacade.MessageDependency_isFilteredMessageDependency(messageDependencyPtr))
							drawFilteredMessageDependencySign(graphics, x1, ym, x2, ym);
					}
					else
						return lineContainsPoint(x1, ym, xm, ym, fitX, fitY, tolerance) ||
						   halfEllipseContainsPoint(1, x1, x2, y1, halfEllipseHeight, fitX, fitY, tolerance);
				}
				// consequence is too far away
				else if (x2 == invalid) {
					x2 = x1 + quarterEllipseWidth * 2;
					xm = x1 + quarterEllipseWidth;

					// draw quarter ellipse ending in a horizontal straight line to the right
					Rectangle.SINGLETON.setLocation(x1, ym);
					Rectangle.SINGLETON.setSize(quarterEllipseWidth * 2, halfEllipseHeight * 2);

					if (graphics != null) {
						graphics.drawArc(Rectangle.SINGLETON, 90, 90);
						graphics.setLineStyle(SWT.LINE_DOT);
						graphics.drawLine(xm, ym, x2, ym);

						if (sequenceChartFacade.MessageDependency_isFilteredMessageDependency(messageDependencyPtr))
							drawFilteredMessageDependencySign(graphics, x1, ym, x2, ym);

						if (showArrowHeads)
							drawArrowHead(graphics, LONG_ARROW_HEAD_COLOR, x2, ym, 1, 0);

						showArrowHeads = false;
					}
					else
						return lineContainsPoint(xm, ym, x2, ym, fitX, fitY, tolerance) ||
						   halfEllipseContainsPoint(0, x1, x2, y1, halfEllipseHeight, fitX, fitY, tolerance);
				}
				// both events are close enough
				else {
					// draw half ellipse
					Rectangle.SINGLETON.setLocation(x1, ym);
					Rectangle.SINGLETON.setSize(x2 - x1, halfEllipseHeight * 2);

					if (graphics != null) {
						graphics.drawArc(Rectangle.SINGLETON, 0, 180);

						if (sequenceChartFacade.MessageDependency_isFilteredMessageDependency(messageDependencyPtr))
							drawFilteredMessageDependencySign(graphics, x1, ym, x2, ym);
					}
					else
						return halfEllipseContainsPoint(-1, x1, x2, y1, halfEllipseHeight, fitX, fitY, tolerance);
				}

				if (showArrowHeads) {
					// intersection of the ellipse and a circle with the arrow length centered at the end point
					// origin is in the center of the ellipse
					// mupad: solve([x^2/a^2+(r^2-(x-a)^2)/b^2=1],x,IgnoreSpecialCases)
					double a = Rectangle.SINGLETON.width / 2;
					double b = Rectangle.SINGLETON.height / 2;
					double a2 = a * a;
					double b2 = b * b;
					double r = ARROWHEAD_LENGTH;
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
					drawMessageDependencyLabel(graphics, messageDependencyPtr, (x1 + x2) / 2, y1, 0, -halfEllipseHeight - 18);
			}
		}
		else {
			int tooLongLineWidth = LONG_MESSAGE_ARROW_WIDTH;
			int y = (y2 + y1) / 2;
			Color arrowHeadFillColor = null;

			// cause is too far away
			if (x1 == invalid) {
				x1 = x2 - tooLongLineWidth * 2;

				if (graphics != null) {
					int xm = x2 - tooLongLineWidth;
					graphics.drawLine(xm, y, x2, y2);
					graphics.setLineStyle(SWT.LINE_DOT);
					graphics.drawLine(x1, y1, xm, y);
				}
			}
			// consequence is too far away
			else if (x2 == invalid) {
				x2 = x1 + tooLongLineWidth * 2;

				if (graphics != null) {
					int xm = x1 + tooLongLineWidth;
					graphics.drawLine(x1, y1, xm, y);
					graphics.setLineStyle(SWT.LINE_DOT);
					graphics.drawLine(xm, y, x2, y2);
					arrowHeadFillColor = LONG_ARROW_HEAD_COLOR;
				}
			}
			// both events are in range
			else {
				if (graphics != null) {
					if (x1 != x2 || vlineBuffer.vlineContainsNewPixel(x1, y1, y2))
						graphics.drawLine(x1, y1, x2, y2);
				}
			}

			if (graphics == null)
				return lineContainsPoint(x1, y1, x2, y2, fitX, fitY, tolerance);

			if (graphics != null && sequenceChartFacade.MessageDependency_isFilteredMessageDependency(messageDependencyPtr))
				drawFilteredMessageDependencySign(graphics, x1, y1, x2, y2);

			if (showArrowHeads)
				drawArrowHead(graphics, arrowHeadFillColor, x2, y2, x2 - x1, y2 - y1);

			if (showMessageNames) {
			    int mx = (x1 + x2) / 2;
			    int my = (y1 + y2) / 2;
			    int rowCount = Math.min(7, Math.max(1, Math.abs(y2 - y1) / fontHeight - 8));
			    int rowIndex = ((causeEventNumber + consequenceEventNumber) % rowCount) - rowCount / 2;
			    int dy = rowIndex * fontHeight;
			    int dx = y2 == y1 ? 0 : (int)((double)(x2 - x1) / (y2 - y1) * dy);
                mx += dx;
			    my += dy;
				drawMessageDependencyLabel(graphics, messageDependencyPtr, mx, my, 3, y1 < y2 ? -15 : 0);
			}
		}

		// when fitting we should have already returned
		Assert.isTrue(graphics != null);

		return false;
	}

	/**
	 * Draws a message arrow label with the corresponding message line color.
	 */
	private void drawMessageDependencyLabel(Graphics graphics, long messageDependencyPtr, int x, int y, int dx, int dy) {
		String arrowLabel = "<no label>";

		if (sequenceChartFacade.MessageDependency_isFilteredMessageDependency(messageDependencyPtr)) {
			arrowLabel = sequenceChartFacade.FilteredMessageDependency_getBeginMessageName(messageDependencyPtr) +
			 " -> " +
			 sequenceChartFacade.FilteredMessageDependency_getEndMessageName(messageDependencyPtr);

			// not to overlap with filtered sign
			x += 7;
		}
		else
			arrowLabel = sequenceChartFacade.MessageDependency_getMessageName(messageDependencyPtr);

		if (MESSAGE_LABEL_COLOR != null)
			graphics.setForegroundColor(MESSAGE_LABEL_COLOR);

		graphics.setFont(font);
		drawText(graphics, arrowLabel, x + dx, y + dy);
	}

	// FIXME This is a workaround for SWT bug https://bugs.eclipse.org/215243 
	private void drawText(Graphics g, String s, int x, int y) {
		g.drawText(s, x, y);
		// HACK clear the cairo lib internal state (on Linux)
		if("gtk".equals(SWT.getPlatform()))
				g.drawPoint(-1000000, -1000000);
	}

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
		dy /= length;
		dx /= length;
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
		double dwx = -dy / n * ARROWHEAD_WIDTH / 2;
		double dwy = dx / n * ARROWHEAD_WIDTH / 2;
		double xt = x - dx * ARROWHEAD_LENGTH / n;
		double yt = y - dy * ARROWHEAD_LENGTH / n;
		int x1 = (int)Math.round(xt - dwx);
		int y1 = (int)Math.round(yt - dwy);
		int x2 = (int)Math.round(xt + dwx);
		int y2 = (int)Math.round(yt + dwy);

		Color arrowHeadColor = ARROW_HEAD_COLOR == null ? graphics.getForegroundColor() : ARROW_HEAD_COLOR;
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.setBackgroundColor(fillColor == null ? arrowHeadColor : fillColor);
		graphics.fillPolygon(new int[] {x, y, x1, y1, x2, y2});
		graphics.setForegroundColor(arrowHeadColor);
		graphics.drawPolygon(new int[] {x, y, x1, y1, x2, y2});
	}

	/**
	 * Utility function to determine event range we need to draw.
	 * Returns an array of size 2 with the two event pointers.
	 */
	protected long[] getFirstLastEventForPixelRange(int x1, int x2) {
		if (eventLog == null || eventLog.getApproximateNumberOfEvents() == 0)
			return new long[] {0, 0};

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

	private int getMaximumMessageDependencyDisplayWidth() {
		return 3 * getViewportWidth();
	}

	private long[] getFirstLastEventPtrForMessageDependencies() {
		int width = getViewportWidth();
		int maximumWidth = getMaximumMessageDependencyDisplayWidth();
		int extraWidth = (maximumWidth - width) / 2;
		return getFirstLastEventForPixelRange(-extraWidth, extraWidth * 2);
	}
	
	private int getAxisModuleIndexByModuleId(int moduleId) {
	    if (moduleIdToAxisModuleIndexMap.containsKey(moduleId))
	        return moduleIdToAxisModuleIndexMap.get(moduleId);
	    else {
	        ModuleCreatedEntry entry = eventLog.getModuleCreatedEntry(moduleId);
	        ModuleTreeItem moduleTreeRoot = eventLogInput.getModuleTreeRoot();
	        ModuleTreeItem moduleTreeItem = moduleTreeRoot.findDescendantModule(moduleId);
	        if (moduleTreeItem == null) {
	            if (entry != null)
	                moduleTreeItem = moduleTreeRoot.addDescendantModule(entry.getParentModuleId(), entry.getModuleId(), entry.getModuleClassName(), entry.getFullName(), entry.getCompoundModule());
	            else {
	                // FIXME: this is not correct and will not be replaced automagically when the ModuleCreatedEntry is found later on
	                moduleTreeItem = new ModuleTreeItem("<unknown>", moduleTreeRoot, false);
	                moduleTreeItem.setModuleId(moduleId);
	            }
	        }
            int index = axisModules.size();
            moduleIdToAxisModuleIndexMap.put(moduleId, index);
            axisModules.add(moduleTreeItem);
            setAxisModules(axisModules);
            return index;
	    }
	}
    
    private int getModuleYViewportCoordinate(int moduleId) {
        return getModuleYViewportCoordinateByModuleIndex(getAxisModuleIndexByModuleId(moduleId));
    }

    private int getModuleYViewportCoordinateByModuleIndex(int index) {
        if (axisModuleYs == null)
            calculateAxisYs();

        return axisModuleYs[index] + axisRenderers[index].getHeight() / 2 - (int)getViewportTop();
    }

    private int getEventAxisModuleIndex(long eventPtr) {
        return getAxisModuleIndexByModuleId(sequenceChartFacade.Event_getModuleId(eventPtr));
    }
    
    private boolean isInitializationEvent(long eventPtr) {
        return sequenceChartFacade.Event_getEventNumber(eventPtr) == 0;
    }

    private boolean isInitializationEvent(IEvent event) {
        return event.getEventNumber() == 0;
    }

	public int getEventXViewportCoordinate(long eventPtr) {
		return (int)(sequenceChartFacade.Event_getTimelineCoordinate(eventPtr) * pixelPerTimelineCoordinate + fixPointViewportCoordinate);
	}

	public int getEventYViewportCoordinate(long eventPtr) {
	    Assert.isTrue(!isInitializationEvent(eventPtr));
	    return getModuleYViewportCoordinateByModuleIndex(getEventAxisModuleIndex(eventPtr));
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

		if (getTimelineMode() == TimelineMode.SIMULATION_TIME) {
			// puts ticks to constant distance from each other measured in timeline units
			int tickScale = (int)Math.ceil(Math.log10(TICK_SPACING / pixelPerTimelineCoordinate));
			BigDecimal tickSpacing = BigDecimal.valueOf(TICK_SPACING / pixelPerTimelineCoordinate);
			BigDecimal tickStart = leftSimulationTime.setScale(-tickScale, RoundingMode.FLOOR);
			BigDecimal tickEnd = rightSimulationTime.setScale(-tickScale, RoundingMode.CEILING);
			BigDecimal tickIntvl = new BigDecimal(1).scaleByPowerOfTen(tickScale);

			// use 2, 4, 6, 8, etc. if possible
			if (tickIntvl.divide(BigDecimal.valueOf(5)).compareTo(tickSpacing) > 0)
				tickIntvl = tickIntvl.divide(BigDecimal.valueOf(5));
			// use 5, 10, 15, 20, etc. if possible
			else if (tickIntvl.divide(BigDecimal.valueOf(2)).compareTo(tickSpacing) > 0)
				tickIntvl = tickIntvl.divide(BigDecimal.valueOf(2));

			for (BigDecimal tick = tickStart; tick.compareTo(tickEnd)<0; tick = tick.add(tickIntvl))
				if (tick.compareTo(BigDecimal.ZERO) >= 0)
					ticks.add(tick);
		}
		else {
			// tries to put ticks constant distance from each other measured in pixels
			int modX = fixPointViewportCoordinate % TICK_SPACING;
			int tleft = modX - TICK_SPACING;
			int tright = modX + viewportWidth + TICK_SPACING;

			IEvent lastEvent = eventLog.getLastEvent();

			if (lastEvent != null) {
				BigDecimal endSimulationTime = lastEvent.getSimulationTime().toBigDecimal();

				for (int t = tleft; t < tright; t += TICK_SPACING) {
					BigDecimal tick = calculateTick(t, TICK_SPACING / 2);

					if (tick.compareTo(BigDecimal.ZERO) >= 0 && tick.compareTo(endSimulationTime) <= 0)
						ticks.add(tick);
				}
			}
		}
	}

	/**
	 * Calculates a single tick near simulation time. The range and position is defined in terms of viewport pixels.
	 * Minimizes the number of digits to have a short tick label and returns the simulation time to be printed.
	 */
	private BigDecimal calculateTick(int x, double tickRange) {
		IEvent lastEvent = eventLog.getLastEvent();

		if (lastEvent == null)
			return BigDecimal.ZERO;

		// query the last simulation time so that times after that will never appear
		double lastSimulationTimeAsDouble = lastEvent.getSimulationTime().doubleValue();
		BigDecimal lastSimulationTime = lastEvent.getSimulationTime().toBigDecimal();

		// query the simulation time for the given coordinate
		double simulationTimeAsDouble = getSimulationTimeForViewportCoordinate(x);
		BigDecimal simulationTime = simulationTimeAsDouble == lastSimulationTimeAsDouble ? lastSimulationTime : new BigDecimal(simulationTimeAsDouble);

		// defines the range of valid simulation times for the given tick range
		double minSimulationTimeAsDouble = getSimulationTimeForViewportCoordinate(x - tickRange / 2);
		double maxSimulationTimeAsDouble = getSimulationTimeForViewportCoordinate(x + tickRange / 2);
		BigDecimal tMin = simulationTimeAsDouble == lastSimulationTimeAsDouble ? lastSimulationTime : new BigDecimal(minSimulationTimeAsDouble);
		BigDecimal tMax = simulationTimeAsDouble == lastSimulationTimeAsDouble ? lastSimulationTime : new BigDecimal(maxSimulationTimeAsDouble);

		// check some invariants
		Assert.isTrue(tMin.compareTo(simulationTime) <= 0);
		Assert.isTrue(tMax.compareTo(simulationTime) >= 0);
		Assert.isTrue(tMin.compareTo(tMax) <= 0);

		if (minSimulationTimeAsDouble == maxSimulationTimeAsDouble) {
			long[] eventPtrRange = getFirstLastEventForPixelRange(x, x);
			long startEventPtr = eventPtrRange[0];
			long endEventPtr = eventPtrRange[1];
			int count = 0;

			// find out the precise simulation time for the zero simulation time range around the given viewport pixel
			for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.Event_getNextEvent(eventPtr)) {
				count++;

				if (eventPtr == endEventPtr)
					break;
			}

			if (minSimulationTimeAsDouble == 0)
				return BigDecimal.ZERO; // the very beginning
			else if (count == 1)
				return sequenceChartFacade.Event_getSimulationTime(startEventPtr).toBigDecimal();
			else if (count == 1) {
				// find the one which is closer
				double startSimulationTime = sequenceChartFacade.Event_getSimulationTime(startEventPtr).doubleValue();
				double endSimulationTime = sequenceChartFacade.Event_getSimulationTime(endEventPtr).doubleValue();

				if (Math.abs(minSimulationTimeAsDouble - startSimulationTime) < Math.abs(endSimulationTime - minSimulationTimeAsDouble))
					return sequenceChartFacade.Event_getSimulationTime(startEventPtr).toBigDecimal();
				else
					return sequenceChartFacade.Event_getSimulationTime(endEventPtr).toBigDecimal();
			}
			else
				return sequenceChartFacade.Event_getSimulationTime(sequenceChartFacade.Event_getNextEvent(startEventPtr)).toBigDecimal();
		}
		else {
			// the idea is to round the simulation time to the shortest (in terms of digits) value
			// as long as it still fits into the range of min and max
			// number of digits
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
    public double getSimulationTimeForViewportCoordinate(int x) {
        return getSimulationTimeForViewportCoordinate(x, false);
    }

	/**
	 * Translates viewport pixel x coordinate to simulation time.
	 */
	public double getSimulationTimeForViewportCoordinate(int x, boolean upperLimit) {
		return sequenceChartFacade.getSimulationTimeForTimelineCoordinate(getTimelineCoordinateForViewportCoordinate(x), upperLimit);
	}

    /**
     * Translates viewport pixel x coordinate to simulation time lower limit.
     */
    public double getSimulationTimeForViewportCoordinate(double x) {
        return getSimulationTimeForViewportCoordinate(x, false);
    }

    /**
	 * Translates viewport pixel x coordinate to simulation time.
	 */
	public double getSimulationTimeForViewportCoordinate(double x, boolean upperLimit) {
		return sequenceChartFacade.getSimulationTimeForTimelineCoordinate(getTimelineCoordinateForViewportCoordinate(x), upperLimit);
	}

    /**
     * Translates simulation time to viewport pixel x coordinate lower limit.
     */
    public int getViewportCoordinateForSimulationTime(double t) {
        return getViewportCoordinateForSimulationTime(t, false);
    }

    /**
	 * Translates simulation time to viewport pixel x coordinate.
	 */
	public int getViewportCoordinateForSimulationTime(double t, boolean upperLimit) {
		Assert.isTrue(t >= 0);
		return (int)(Math.round(sequenceChartFacade.getTimelineCoordinateForSimulationTime(t, upperLimit) * pixelPerTimelineCoordinate) + fixPointViewportCoordinate);
	}

	/**
	 * Translates from viewport pixel x coordinate to timeline coordinate, using pixelPerTimelineCoordinate.
	 */
	public double getTimelineCoordinateForViewportCoordinate(int x) {
	    Assert.isTrue(pixelPerTimelineCoordinate > 0);
		return (x - fixPointViewportCoordinate) / pixelPerTimelineCoordinate;
	}

	/**
	 * Translates from viewport pixel x coordinate to timeline coordinate, using pixelPerTimelineCoordinate.
	 */
	public double getTimelineCoordinateForViewportCoordinate(double x) {
        Assert.isTrue(pixelPerTimelineCoordinate > 0);
		return (x - fixPointViewportCoordinate) / pixelPerTimelineCoordinate;
	}

	/**
	 * Translates timeline coordinate to viewport pixel x coordinate, using pixelPerTimelineCoordinate.
	 */
	public int getViewportCoordinateForTimelineCoordinate(double t) {
		long x = Math.round(t * pixelPerTimelineCoordinate) + fixPointViewportCoordinate;
    	return (int)x;
	}

	/*************************************************************************************
	 * MOUSE HANDLING
	 */

	/**
	 * Sets up default mouse handling.
	 */
	private void setUpMouseHandling() {
		// zoom by wheel
		addListener(SWT.MouseWheel, new Listener() {
			public void handleEvent(Event event) {
				if (!eventLogInput.isCanceled()) {
					if ((event.stateMask & SWT.CTRL)!=0) {
						for (int i = 0; i < event.count; i++)
							zoomBy(1.1);

						for (int i = 0; i < -event.count; i++)
							zoomBy(1.0 / 1.1);
					}
					else if ((event.stateMask & SWT.SHIFT)!=0)
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
                drawStuffUnderMouse = true;
                redraw();
            }
		});

		// dragging
		addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if (!eventLogInput.isCanceled()) {
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
				if (!eventLogInput.isCanceled()) {
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
            public void mouseDoubleClick(MouseEvent me) {
				if (!eventLogInput.isCanceled()) {
					ArrayList<IEvent> events = new ArrayList<IEvent>();
					ArrayList<IMessageDependency> messageDependencies = new ArrayList<IMessageDependency>();
					collectStuffUnderMouse(me.x, me.y, events, messageDependencies);
					
					if (messageDependencies.size() == 1)
						zoomToMessageDependency(messageDependencies.get(0));

                    updateSelectionEvents(events, true);
				}
			}

			@Override
            public void mouseDown(MouseEvent e) {
				if (!eventLogInput.isCanceled()) {
					setFocus();

					if (e.button == 1) {
						dragStartX = e.x;
						dragStartY = e.y;
						dragStartTime = System.currentTimeMillis();
					}
				}
			}

			@Override
            public void mouseUp(MouseEvent me) {
				if (!eventLogInput.isCanceled()) {
					if (me.button == 1) {
						if (!isDragging) {
							ArrayList<IEvent> events = new ArrayList<IEvent>();

							if ((me.stateMask & SWT.CTRL)!=0) // CTRL key extends selection
								for (Integer eventNumber : selectionEventNumbers)
									events.add(eventLog.getEventForEventNumber(eventNumber));

							collectStuffUnderMouse(me.x, me.y, events, null);
							
							updateSelectionEvents(events, false);
						}
						else if (System.currentTimeMillis() - dragStartTime < 500){
							ArrayList<IMessageDependency> messageDependencies = new ArrayList<IMessageDependency>();
							collectStuffUnderMouse(me.x, me.y, null, messageDependencies);

							if (messageDependencies.size() == 1) {
								IMessageDependency messageDependency = messageDependencies.get(0);
								gotoElement(dragDeltaX > 0 ? messageDependency.getConsequenceEvent() : messageDependency.getCauseEvent());
							}
						}
					}

					setCursor(null); // restore cursor at end of drag
					dragStartX = dragStartY = -1;
					dragStartTime = -1;
					isDragging = false;
				}
			}

			private void updateSelectionEvents(ArrayList<IEvent> events, boolean fireSelection) {
                ArrayList<Integer> eventNumbers = new ArrayList<Integer>();
                for (IEvent event : events)
                    eventNumbers.add(event.getEventNumber());

                if (eventNumbers.equals(selectionEventNumbers))
                    fireSelection(fireSelection);
                else {
                    selectionEventNumbers = eventNumbers;
                    fireSelection(fireSelection);
                    fireSelectionChanged();
                    redraw();
                }
            }
		});
	}

	/*************************************************************************************
	 * TOOLTIP
	 */

	/**
	 * Calls collectStuffUnderMouse(), and assembles a possibly multi-line
	 * tooltip text from it. Returns null if there's no text to display.
	 */
	protected String getTooltipText(int x, int y, SizeConstraint outSizeConstraint) {
		ArrayList<IEvent> events = new ArrayList<IEvent>();
		ArrayList<IMessageDependency> messageDependencies = new ArrayList<IMessageDependency>();
		collectStuffUnderMouse(x, y, events, messageDependencies);

		// 1) if there are events under them mouse, show them in the tooltip
		if (events.size() > 0) {
			String res = "";

			for (IEvent event : events) {
				if (res.length() != 0)
					res += "<br/>";

				res += getEventText(event, true, outSizeConstraint);

				if (events.size() == 1) {
					IEvent selectionEvent = getSelectionEvent();

					if (selectionEvent != null && !event.equals(selectionEvent)) {
						BigDecimal distance = event.getSimulationTime().toBigDecimal().subtract(selectionEvent.getSimulationTime().toBigDecimal());
						res += "<br/>" + "<i>Simulation time difference to selected event (#" + selectionEvent.getEventNumber() + "): " + TimeUtils.secondsToTimeString(distance) + "</i>";
					}

					for (int i = 0; i < event.getNumEventLogMessages(); i++) {
						EventLogMessageEntry eventLogMessageEntry = event.getEventLogMessage(i);

						res += "<br/><span style=\"color:rgb(127, 0, 85)\"> - " + eventLogMessageEntry.getText() + "</span>";
					}
				}
			}

			return res;
		}

		// 2) no events: show message arrows info
		if (messageDependencies.size() >= 1) {
			String res = "";
			for (IMessageDependency messageDependency : messageDependencies) {
				if (res.length() != 0)
					res += "<br/>";

				res += getMessageDependencyText(messageDependency, true, outSizeConstraint);
			}

			return res;
		}

		// 3) no events or message arrows: show axis info
		ModuleTreeItem axisModule = findAxisAt(y);
		if (axisModule != null) {
			String res = getAxisText(axisModule, true) + "<br/>";
			res += "t = " + calculateTick(x, 1).toPlainString();

			if (!eventLog.isEmpty()) {
				IEvent event = sequenceChartFacade.getLastEventNotAfterTimelineCoordinate(getTimelineCoordinateForViewportCoordinate(x));

				if (event != null)
					res += ", just after event #" + event.getEventNumber();
			}

			return res;
		}

		// absolutely nothing to say
		return null;
	}

	/**
	 * Returns a descriptive message for the ModuleTreeItem to be presented to the user.
	 */
	public String getAxisText(ModuleTreeItem axisModule, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        String moduleName = (formatted ? axisModule.getModuleFullPath() : axisModule.getModuleName());
        String moduleId = " (id = " + axisModule.getModuleId() + ")";

        return "Axis (" + axisModule.getModuleClassName() + ") " + boldStart + moduleName + boldEnd + moduleId;
	}

	/**
	 * Returns a descriptive message for the IMessageDependency to be presented to the user.
	 */
	public String getMessageDependencyText(IMessageDependency messageDependency, boolean formatted, SizeConstraint outSizeConstraint) {
		if (sequenceChartFacade.MessageDependency_isFilteredMessageDependency(messageDependency.getCPtr())) {
			FilteredMessageDependency filteredMessageDependency = (FilteredMessageDependency)messageDependency;
			BeginSendEntry beginBeginSendEntry = filteredMessageDependency.getBeginMessageDependency().getBeginSendEntry();
			BeginSendEntry endBeginSendEntry = filteredMessageDependency.getEndMessageDependency().getBeginSendEntry();
			boolean sameMessage = beginBeginSendEntry.getMessageId() == endBeginSendEntry.getMessageId();

			String result = "Filtered " + (messageDependency.getIsReuse() ? "reusing " : "sending ") + getMessageNameText(beginBeginSendEntry, formatted);
            if (!sameMessage)
                result += " -> " + getMessageNameText(endBeginSendEntry, formatted);

			if (formatted)
			    result += getMessageDependencyEventNumbersText(messageDependency) + getSimulationTimeDeltaText(messageDependency);

			result += getMessageIdText(beginBeginSendEntry, formatted);
			if (!sameMessage)
			    result += " -> " + getMessageIdText(endBeginSendEntry, formatted);
			
            if (formatted) {
                if (!sameMessage)
                    result += "First: ";

                result += getMessageDetailText(beginBeginSendEntry, outSizeConstraint);

                if (!sameMessage) {
                    result += "Last: ";
                    result += getMessageDetailText(endBeginSendEntry, outSizeConstraint);
                }
            }

            return result;
		}
		else {
			BeginSendEntry beginSendEntry = messageDependency.getBeginSendEntry();
			String result = (messageDependency.getIsReuse() ? "Reusing " : "Sending ") + getMessageNameText(beginSendEntry, formatted);

            if (formatted)
	            result += getMessageDependencyEventNumbersText(messageDependency) + getSimulationTimeDeltaText(messageDependency);
	            
		    result += getMessageIdText(beginSendEntry, formatted);

		    if (formatted)
		        result += getMessageDetailText(beginSendEntry, outSizeConstraint);

			return result;
		}
	}

    private String getMessageDetailText(BeginSendEntry beginSendEntry, SizeConstraint outSizeConstraint) {
        String detail = beginSendEntry.getDetail();

        if (detail != null) {
        	int longestLineLength = 0;
        	for (String line : detail.split("\n"))
        		longestLineLength = Math.max(longestLineLength, line.length());
        	// TODO: correct solution would be to get pre font width (monospace, 8) and consider margins too
        	outSizeConstraint.minimumWidth = longestLineLength * 8;
        	return "<br/><pre>" + detail + "</pre>";
        }
        else
            return "";
    }

    private String getMessageDependencyEventNumbersText(IMessageDependency messageDependency) {
        return " (#" + messageDependency.getCauseEventNumber() + " -> #" + messageDependency.getConsequenceEventNumber() + ")";
    }

    private String getMessageNameText(BeginSendEntry beginSendEntry, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        return "(" + beginSendEntry.getMessageClassName() + ") " + boldStart + beginSendEntry.getMessageFullName() + boldEnd;
    }

    private String getSimulationTimeDeltaText(IMessageDependency messageDependency) {
        BigDecimal causeSimulationTime = messageDependency.getCauseSimulationTime().toBigDecimal();
        BigDecimal consequenceSimulationTime = messageDependency.getConsequenceSimulationTime().toBigDecimal();
        return " dt = " + TimeUtils.secondsToTimeString(consequenceSimulationTime.subtract(causeSimulationTime));
    }

    private String getMessageIdText(BeginSendEntry beginSendEntry, boolean formatted) {
        int messageId = beginSendEntry.getMessageId();
        String result = " (id = " + messageId;

        if (formatted) {
            int messageTreeId = beginSendEntry.getMessageTreeId();
            if (messageTreeId != -1 && messageTreeId != messageId)
                result += ", tree id = " + messageTreeId;
            
            int messageEncapsulationId = beginSendEntry.getMessageEncapsulationId();
            if (messageEncapsulationId != -1 && messageEncapsulationId != messageId)
                result += ", encapsulation id = " + messageEncapsulationId;

            int messageEncapsulationTreeId = beginSendEntry.getMessageEncapsulationTreeId();
            if (messageEncapsulationTreeId != -1 && messageEncapsulationTreeId != messageEncapsulationId)
                result += ", encapsulation tree id = " + messageEncapsulationTreeId;
        }

        result += ")";
        return result;
    }

	/**
	 * Returns a descriptive message for the IEvent to be presented to the user.
	 */
	public String getEventText(IEvent event, boolean formatted, SizeConstraint outSizeConstraint) {
		String boldStart = formatted ? "<b>" : "";
		String boldEnd = formatted ? "</b>" : "";

		ModuleCreatedEntry moduleCreatedEntry = eventLog.getModuleCreatedEntry(event.getModuleId());
		String result = "Event #" + event.getEventNumber();

		if (formatted)
		    result += " at t = " + event.getSimulationTime();

		String moduleName = (formatted ? eventLogInput.getEventLogTableFacade().EventLogEntry_getModuleFullPath(moduleCreatedEntry.getCPtr()) : moduleCreatedEntry.getFullName());
		result += " in module (" + moduleCreatedEntry.getModuleClassName() + ") " +  boldStart + moduleName + boldEnd + " (id = " + event.getModuleId() + ")";

		IMessageDependency messageDependency = event.getCause();
		BeginSendEntry beginSendEntry = null;

		if (messageDependency instanceof FilteredMessageDependency)
			messageDependency = ((FilteredMessageDependency)messageDependency).getEndMessageDependency();

		if (messageDependency != null)
		    beginSendEntry = messageDependency.getBeginSendEntry();

		if (formatted && beginSendEntry != null) {
			result += " message (" + beginSendEntry.getMessageClassName() + ") " + boldStart + beginSendEntry.getMessageFullName() + boldEnd;
			result += getMessageIdText(beginSendEntry, formatted);
		}

		return result;
	}

	/**
	 * Returns the axis at the given Y coordinate (with MOUSE_TOLERANCE), or null.
	 */
	public ModuleTreeItem findAxisAt(int y) {
		if (!eventLogInput.isCanceled()) {
			y += getViewportTop() - GUTTER_HEIGHT;

			for (int i = 0; i < axisModuleYs.length; i++) {
				int height = axisRenderers[i].getHeight();
				if (axisModuleYs[i] - MOUSE_TOLERANCE <= y && y <= axisModuleYs[i] + height + MOUSE_TOLERANCE)
					return axisModules.get(i);
			}
		}

		return null;
	}

	/**
	 * Determines if there are any events (IEvent) or messages (MessageDependency)
	 * at the given mouse coordinates, and returns them in the Lists passed.
	 * Coordinates are canvas coordinates (more precisely, viewport coordinates).
	 * You can call this method from mouse click, double-click or hover event
	 * handlers.
	 *
	 * If you're interested only in messages or only in events, pass null in the
	 * events or msgs argument. This method does NOT clear the lists before filling them.
	 */
	public void collectStuffUnderMouse(final int mouseX, final int mouseY, final List<IEvent> events, final List<IMessageDependency> msgs) {
		if (!eventLogInput.isCanceled() && !eventLogInput.isLongRunningOperationInProgress()) {
			eventLogInput.runWithProgressMonitor(new Runnable() {
				public void run() {
					if (eventLog != null) {
						long startMillis = System.currentTimeMillis();

						calculateStuff();

						// determine start/end event numbers
						int width = getViewportWidth();
						long[] eventPtrRange = getFirstLastEventForPixelRange(0, width);
						long startEventPtr = eventPtrRange[0];
						long endEventPtr = eventPtrRange[1];

						// check events
			            if (events != null && startEventPtr != 0 && endEventPtr != 0) {
			            	for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.Event_getNextEvent(eventPtr)) {
			            	    int x = getEventXViewportCoordinate(eventPtr);

			            	    if (isInitializationEvent(eventPtr)) {
			                        for (int i = 0; i < sequenceChartFacade.Event_getNumConsequences(eventPtr); i++) {
			                            long consequenceEventPtr = sequenceChartFacade.MessageDependency_getConsequenceEvent(sequenceChartFacade.Event_getConsequence(eventPtr, i));
			                            
                                        if (consequenceEventPtr != 0) {
    			                            int y = getEventYViewportCoordinate(consequenceEventPtr);
    
    			                            if (eventSymbolContainsPoint(mouseX, mouseY - GUTTER_HEIGHT, x, y, MOUSE_TOLERANCE))
    		                                    events.add(sequenceChartFacade.Event_getEvent(eventPtr));
    		                            }
			                        }
			            	    }
			            	    else if (eventSymbolContainsPoint(mouseX, mouseY - GUTTER_HEIGHT, x, getEventYViewportCoordinate(eventPtr), MOUSE_TOLERANCE))
			   						events.add(sequenceChartFacade.Event_getEvent(eventPtr));

			   					if (eventPtr == endEventPtr)
				   					break;
				   			}
			            }

			            // check message arrows
			            if (msgs != null) {
			        		eventPtrRange = getFirstLastEventPtrForMessageDependencies();
			    			startEventPtr = eventPtrRange[0];
			    			endEventPtr = eventPtrRange[1];

			    			if (startEventPtr != 0 && endEventPtr != 0) {
				            	PtrVector messageDependencies = sequenceChartFacade.getIntersectingMessageDependencies(startEventPtr, endEventPtr);

				        		for (int i = 0; i < messageDependencies.size(); i++) {
				        			long messageDependencyPtr = messageDependencies.get(i);
				        			
				        			// TODO: cast ptr_t as (ptr_t)(unsigned int)
				        			// TODO: PtrVector should return Java long

				        			if (drawOrFitMessageDependency(messageDependencyPtr, mouseX, mouseY - GUTTER_HEIGHT, MOUSE_TOLERANCE, null, null, startEventPtr, endEventPtr))
				            			msgs.add(sequenceChartFacade.MessageDependency_getMessageDependency(messageDependencyPtr));
				        		}
			    			}
			            }

			            long millis = System.currentTimeMillis() - startMillis;
			            if (debug)
							System.out.println("collectStuffUnderMouse(): " + millis + "ms - " + (events == null ? "n/a" : events.size()) + " events, " + (msgs == null ? "n/a" : msgs.size()) + " msgs");
					}
				}
			});
		}
	}

	/**
	 * Utility function, to detect whether user clicked (hovered) an event in the chart
	 */
	private boolean eventSymbolContainsPoint(int x, int y, int px, int py, int tolerance) {
		return Math.abs(x - px) <= 2 + tolerance && Math.abs(y - py) <= 5 + tolerance;
	}

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
	 * SELECTION
	 */

	/**
	 * Adds an SWT selection listener which gets notified when the widget
	 * is clicked or double-clicked.
	 */
	public void addSelectionListener(SelectionListener listener) {
		selectionListenerList.add(listener);
	}

	/**
	 * Removes the given SWT selection listener.
	 */
	public void removeSelectionListener(SelectionListener listener) {
		selectionListenerList.remove(listener);
	}

	protected void fireSelection(boolean defaultSelection) {
		Event event = new Event();
		event.display = getDisplay();
		event.widget = this;
		SelectionEvent se = new SelectionEvent(event);
		for (SelectionListener listener : selectionListenerList) {
			if (defaultSelection)
				listener.widgetDefaultSelected(se);
			else
				listener.widgetSelected(se);
		}
	}

	/**
     * Add a selection change listener.
     */
	public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.add(listener);
	}

	/**
     * Remove a selection change listener.
     */
	public void removeSelectionChangedListener(ISelectionChangedListener listener) {
		selectionChangedListeners.remove(listener);
	}

	/**
     * Notifies any selection changed listeners that the viewer's selection has changed.
     * Only listeners registered at the time this method is called are notified.
     */
    protected void fireSelectionChanged() {
    	fireSelectionChanged(getSelection());
    }

    protected void fireSelectionChanged(ISelection selection) {
        Object[] listeners = selectionChangedListeners.getListeners();
        final SelectionChangedEvent event = new SelectionChangedEvent(this, selection);
        for (int i = 0; i < listeners.length; ++i) {
            final ISelectionChangedListener l = (ISelectionChangedListener) listeners[i];
            SafeRunnable.run(new SafeRunnable() {
                public void run() {
                    l.selectionChanged(event);
                }
            });
        }
    }

	/**
	 * Returns the currently "selected" events as an instance of IEventLogSelection.
	 * Selection is shown as red circles in the chart.
	 */
	public ISelection getSelection() {
		if (eventLogInput == null)
			return null;
		else
			return new EventLogSelection(eventLogInput, selectionEventNumbers);
	}

	/**
	 * Sets the currently "selected" events. The selection must be an
	 * instance of IEventLogSelection and refer to the current eventLog.
	 * Selection is displayed as red circles in the chart.
	 */
	public void setSelection(ISelection selection) {
		if (debug)
			System.out.println("SequencreChart got selection: " + selection);

		IEventLogSelection eventLogSelection = (IEventLogSelection)selection;
		EventLogInput selectionEventLogInput = eventLogSelection.getEventLogInput();

		if (eventLogInput != selectionEventLogInput) {
			if (followSelection)
				setInput(selectionEventLogInput);
			else
				throw new RuntimeException("Invalid selection");
		}

		// if new selection differs from existing one, take over its contents
		if (!eventLogSelection.getEventNumbers().equals(selectionEventNumbers)) {
			selectionEventNumbers.clear();
			selectionEventNumbers.addAll(eventLogSelection.getEventNumbers());

			// go to the time of the first event selected
			if (selectionEventNumbers.size() > 0)
				gotoElement(eventLog.getEventForEventNumber(selectionEventNumbers.get(0)));

			redraw();
			fireSelectionChanged();
		}
	}

	/**
	 * Removes all selection events.
	 */
	public void clearSelection() {
		if (selectionEventNumbers != null && selectionEventNumbers.size() != 0) {
			selectionEventNumbers.clear();

			fireSelectionChanged();
		}
	}

	/**
	 * Returns the current selection.
	 */
	public IEvent getSelectionEvent() {
		if (selectionEventNumbers != null && selectionEventNumbers.size() != 0)
			return eventLog.getEventForEventNumber(selectionEventNumbers.get(0));
		else
			return null;
	}

	public List<Integer> getSelectionEventNumbers() {
		return selectionEventNumbers;
	}

    public List<IEvent> getSelectionEvents() {
        ArrayList<IEvent> events = new ArrayList<IEvent>();
        
        for (Integer eventNumber : selectionEventNumbers)
            events.add(eventLog.getEventForEventNumber(eventNumber));
        
        return events;
    }

	public void setSelectionEvent(IEvent event) {
		selectionEventNumbers.clear();
		selectionEventNumbers.add(event.getEventNumber());
		fireSelectionChanged();
		redraw();
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
	static class VLineBuffer {
		int currentX = -1;

		static class Region {
			int y1, y2;

			Region() {
			}

			Region(int y1, int y2) {
				this.y1 = y1;
				this.y2 = y2;
			}
		}

		ArrayList<Region> regions = new ArrayList<Region>();

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

class SequenceChartState implements Serializable {
	private static final long serialVersionUID = 1L;
	public int viewportTop;
	public int fixPointEventNumber;
	public int fixPointViewportCoordinate;
	public double pixelPerTimelineCoordinate;
	public AxisState[] axisStates;
}

class AxisState implements Serializable {
	private static final long serialVersionUID = 1L;
	public String vectorFileName;
	public String moduleFullPath;
	public String vectorModuleFullPath;
	public String vectorName;
}

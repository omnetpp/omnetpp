package org.omnetpp.experimental.seqchart.widgets;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.math.MathContext;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Random;

import org.apache.commons.collections.ListUtils;
import org.apache.commons.collections.map.MultiValueMap;
import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.text.DefaultInformationControl;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.omnetpp.common.canvas.CachingCanvas;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.EventLogFacade;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IntSet;
import org.omnetpp.eventlog.engine.IntVector;
import org.omnetpp.eventlog.engine.MessageDependency;
import org.omnetpp.eventlog.engine.MessageDependencyKind;
import org.omnetpp.eventlog.engine.MessageSend;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.SequenceChartFacade;
import org.omnetpp.experimental.seqchart.editors.EventLogSelection;
import org.omnetpp.experimental.seqchart.editors.IEventLogSelection;
import org.omnetpp.experimental.seqchart.moduletree.ModuleTreeItem;
import org.omnetpp.scave.engine.XYArray;

/**
 * This is a sequence chart as a single figure.
 *
 * @author andras, levy
 */
//FIXME expressions like int x = (int)(logFacade.getEvent_i_cachedX(i) - getViewportLeft()) may overflow -- make use of XMAX!
//TODO renaming: DELIVERY->SENDING, NONDELIVERY->USAGE, isDelivery->isSending;
//               Timeline modes: Linear, Step, Compact (=nonlinear), Compact2 (CompactWithStep);
//               SortMode to OrderingMode
//TODO cf with ns2 trace file and cEnvir callbacks, and modify file format...
//TODO proper "hand" cursor - current one is not very intuitive
//TODO max number of event selection marks must be limited (e.g. max 1000)
//TODO hierarchic sort should be able to reverse order of sorted axes of its submodules
//TODO rubberband vs. haircross, show them at once
public class SequenceChart extends CachingCanvas implements ISelectionProvider {

	private static final Color LABEL_COLOR = new Color(null, 0, 0, 0);
	private static final Color TICK_LINE_COLOR = new Color(null, 160, 160, 160);
	private static final Color TICK_LABEL_COLOR = new Color(null, 0, 0, 0);
	private static final Color GUTTER_BACKGROUND_COLOR = new Color(null, 255, 255, 160);
	private static final Color GUTTER_BORDER_COLOR = new Color(null, 0, 0, 0);
	private static final Color EVENT_FG_COLOR = new Color(null,255,0,0);
	private static final Color EVENT_BG_COLOR = new Color(null,255,255,255);
	private static final Color EVENT_SEL_COLOR = new Color(null,255,0,0);
	private static final Color ARROWHEAD_COLOR = null; // defaults to line color
	private static final Color MESSAGE_LABEL_COLOR = null; // defaults to line color
	private static final Color DELIVERY_MESSAGE_COLOR = new Color(null,0,0,255);
	private static final Color NONDELIVERY_MESSAGE_COLOR = new Color(null,0,150,0);
	private static final Cursor DRAG_CURSOR = new Cursor(null, SWT.CURSOR_SIZEALL);
	private static final int[] DOTTED_LINE_PATTERN = new int[] {2,2}; // 2px black, 2px gap
	
	private static final int XMAX = Integer.MAX_VALUE / 2;
	private static final int MAX_TOOLTIP_LINES = 30;
	private static final int ANTIALIAS_TURN_ON_AT_MSEC = 100;
	private static final int ANTIALIAS_TURN_OFF_AT_MSEC = 300;
	private static final int MOUSE_TOLERANCE = 1;

	private static final int DELIVERY_SELFARROW_HEIGHT = 20; // vertical radius of ellipse for selfmsg arrows
	private static final int NONDELIVERY_SELFARROW_HEIGHT = 10; // same for non-delivery messages
	private static final int ARROWHEAD_LENGTH = 10; // length of message arrow head
	private static final int ARROWHEAD_WIDTH = 7; // width of message arrow head
	private static final int AXISLABEL_DISTANCE = 15; // distance of timeline label above axis
	private static final int EVENT_SEL_RADIUS = 10; // radius of event selection mark circle
	private static final int TICK_SPACING = 100; // space between ticks in pixels
	private static final int GUTTER_HEIGHT = 17; // height of top and bottom gutter
	
	private IEventLog eventLog; // contains the data to be displayed
	private SequenceChartFacade sequenceChartFacade; // helpful facade on eventlog
	
	private double pixelsPerTimelineUnit = -1;
	private boolean antiAlias = true;  // antialiasing -- this gets turned on/off automatically
	private boolean showArrowHeads = true; // whether arrow heads are drawn or not
	private int axisOffset = 50;  // y coord of first axis
	private int axisSpacing = -1; // y distance between two axes

	private boolean showMessageNames;
	private boolean showNonDeliveryMessages; // show or hide non-delivery message arrows
	private boolean showEventNumbers;
	private TimelineSortMode timelineSortMode = TimelineSortMode.MODULE_ID; // specifies the ordering mode of timelines
	private double nonLinearFocus = 1; // parameter for non-linear timeline transformation
	
	private double viewportLeftSimulationTime; // used to restore the visible range of simulation time
	private double viewportRightSimulationTime;
	
	private DefaultInformationControl tooltipWidget; // the current tooltip (Note: SWT's Tooltip cannot be used as it wraps lines)
	
	private int dragStartX = -1, dragStartY = -1; // temporary variables for drag handling
	private List<ModuleTreeItem> axisModules; // the modules which should have an axis (they must be part of a module tree!)
	private List<AxisGraph> axisGraphs; // used to paint the axis
	private int[] axisModulePositions; // y order of the axis modules (in the same order as axisModules); this is a permutation of the 0..axisModule.size()-1 numbers
	private int[] axisModuleYs; // top y coordinates of axis bounding boxes
	private HashMap<Integer, Integer> moduleIdToAxisYMap = new HashMap<Integer, Integer>();
	private IntSet moduleIds; // calculated from axisModules: module Ids of all modules which are submodule of an axisModule (i.e. whose events appear on the chart)
	private ArrayList<BigDecimal> ticks; // a list of simulation times painted on the axis as tick marks
	private boolean invalidVirtualSize = true;
	private boolean invalidModuleYCoordinates = true;
	
	private ArrayList<SelectionListener> selectionListenerList = new ArrayList<SelectionListener>(); // SWT selection listeners
	private List<IEvent> selectedEvents = new ArrayList<IEvent>(); // the selection
    private ListenerList selectionChangedListeners = new ListenerList(); // list of selection change listeners (type ISelectionChangedListener).

	private static Rectangle TEMP_RECT = new Rectangle();  // tmp var for local calculations (a second Rectangle.SINGLETON)
    
	public enum TimelineMode {
		LINEAR,
		STEP,
		NON_LINEAR
	}

	public enum TimelineSortMode {
		MANUAL,
		MODULE_ID,
		MODULE_NAME,
		MINIMIZE_CROSSINGS,
		MINIMIZE_CROSSINGS_HIERARCHICALLY
	}
	
	/**
     * Constructor.
     */
	public SequenceChart(Composite parent, int style) {
		super(parent, style);
    	setUpMouseHandling();
	}

	/**
	 * Returns chart scale, that is, the number of pixels a "timeline unit" maps to.
     *
	 * The meaning of "timeline unit" depends on the timeline mode (see enum TimelineMode).
	 * For LINEAR mode it is <i>second</i> (simulation time), for STEP mode it is <i>event</i>,
	 * and for NON_LINEAR mode it is calculated as a nonlinear function of simulation time.
	 */
	public double getPixelsPerTimelineUnit() {
		return pixelsPerTimelineUnit;	
	}
	
	/**
	 * Set chart scale (number of pixels a "timeline unit" maps to), 
	 * and adjusts the density of ticks. 
	 */
	public void setPixelsPerTimelineUnit(double pptu) {
		if (pixelsPerTimelineUnit != pptu)
		{
			double maxPPTU = Long.MAX_VALUE / 2 / sequenceChartFacade.getTimelineCoordinate(eventLog.getLastEvent());
			
			// set pixels per sec, and calculate tick spacing
			if (pptu <= 0)
				pptu = 1e-12;
			else if (pptu > maxPPTU)
				pptu = maxPPTU;
	
			pixelsPerTimelineUnit = pptu;
			invalidVirtualSize = true;
			invalidModuleYCoordinates = true;
			clearCanvasCacheAndRedraw();
		}
	}
	
	/**
	 * Returns the pixel distance between adjacent axes in the chart.
	 */
	public int getAxisSpacing() {
		return axisSpacing;
	}

	/**
	 * Sets the pixel distance between adjacent axes in the chart.
	 */
	public void setAxisSpacing(int axisSpacing) {
		this.axisSpacing = axisSpacing > 1 ? axisSpacing : axisSpacing == -1 ? -1 : 1;
		axisModuleYs = null;
		invalidVirtualSize = true;
		invalidModuleYCoordinates = true;
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
	 * Returns whether non-delivery messages are shown in the chart.
	 */
	public boolean getShowNonDeliveryMessages() {
		return showNonDeliveryMessages;
	}

	/**
	 * Shows/Hides non-delivery messages.
	 */
	public void setShowNonDeliveryMessages(boolean showNonDeliveryMessages) {
		this.showNonDeliveryMessages = showNonDeliveryMessages;
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
	public void setShowArrowHeads(boolean drawArrowHeads) {
		this.showArrowHeads = drawArrowHeads;
		clearCanvasCacheAndRedraw();
	}

	/**
	 * Changes the timeline mode and updates figure accordingly.
	 * Tries to show the current simulation time after changing the timeline coordinate system.
	 */
	public void setTimelineMode(TimelineMode timelineMode) {
		saveViewportSimulationTimeRange();
		sequenceChartFacade.setTimelineMode(timelineMode.ordinal());
		restoreViewportSimulationTimeRange();
	}

	/**
	 * Returns the current timeline mode.
	 */
	public TimelineMode getTimelineMode() {
		return TimelineMode.values()[sequenceChartFacade.getTimelineMode()];
	}
	
	/**
	 * Changes the timeline sort mode and updates figure accordingly.
	 */
	public void setTimelineSortMode(TimelineSortMode timelineSortMode) {
		this.timelineSortMode = timelineSortMode;
		axisModulePositions = null;
		axisModuleYs = null;
		invalidModuleYCoordinates = true;
		clearCanvasCacheAndRedraw();
	}
	
	/**
	 * Return the current timeline sort mode.
	 */
	public TimelineSortMode getTimelineSortMode() {
		return timelineSortMode;
	}
	
	/**
	 * Saves the currently visible range of simulation time.
	 */
	public void saveViewportSimulationTimeRange()
	{
		viewportLeftSimulationTime = getViewportLeftSimulationTime();
		viewportRightSimulationTime = getViewportRightSimulationTime();
	}
	
	/**
	 * Restores last saved range of visible simulation time.
	 */
	public void restoreViewportSimulationTimeRange() {
		gotoSimulationTimeRange(viewportLeftSimulationTime, viewportRightSimulationTime);
	}

	/**
	 * Returns the simulation time of the canvas's center.
	 */
	public double getViewportCenterSimulationTime() {
		int middleX = getWidth() / 2;
		return getSimulationTimeForViewportPixel(middleX);
	}
	
	/**
	 * Returns the simulation time of the canvas's left.
	 */
	public double getViewportLeftSimulationTime() {
		return getSimulationTimeForViewportPixel(0);
	}
	
	/**
	 * Returns the simulation time of the canvas's right.
	 */
	public double getViewportRightSimulationTime() {
		return getSimulationTimeForViewportPixel(getWidth());
	}
	
	/**
	 * Scroll the canvas so to make start and end simulation times visible.
	 */
	public void gotoSimulationTimeRange(double startSimulationTime, double endSimulationTime) {
		if (!Double.isNaN(endSimulationTime) && startSimulationTime != endSimulationTime) {
			double timelineUnitDelta = sequenceChartFacade.getTimelineCoordinateForSimulationTime(endSimulationTime) - sequenceChartFacade.getTimelineCoordinateForSimulationTime(startSimulationTime);
			
			if (timelineUnitDelta > 0)
				setPixelsPerTimelineUnit(getWidth() / timelineUnitDelta);
		}

		scrollHorizontalTo(getViewportLeft() + getViewportPixelForSimulationTime(startSimulationTime));

		invalidVirtualSize = true;
		invalidModuleYCoordinates = true;
		clearCanvasCacheAndRedraw();
	}
	
	/**
	 * Scroll the canvas so to make start and end simulation times visible, but leave some pixels free on both sides.
	 */
	public void gotoSimulationTimeRange(double startSimulationTime, double endSimulationTime, int pixelInset) {
		if (pixelInset > 0) {
			// NOTE: we can't go directly there, so here is an approximation
			for (int i = 0; i < 3; i++) {
				double newStartSimulationTime = getSimulationTimeForViewportPixel(getViewportPixelForSimulationTime(startSimulationTime) - pixelInset);
				double newEndSimulationTime = getSimulationTimeForViewportPixel(getViewportPixelForSimulationTime(endSimulationTime) + pixelInset);
	
				gotoSimulationTimeRange(newStartSimulationTime, newEndSimulationTime);
			}
		}
		else
			gotoSimulationTimeRange(startSimulationTime, endSimulationTime);
	}
	
	/**
	 * Scroll the canvas so as to make the simulation time visible.
	 */
	public void gotoSimulationTimeWithCenter(double time) {
		double xDouble = sequenceChartFacade.getTimelineCoordinateForSimulationTime(time) * pixelsPerTimelineUnit;
		long x = xDouble < 0 ? 0 : xDouble>Long.MAX_VALUE ? Long.MAX_VALUE : (long)xDouble;
		scrollHorizontalTo(x - getWidth()/2);
		redraw();
	}
	
	/**
	 * Scroll the canvas to make the event visible.
	 */
	public void gotoEvent(IEvent e) {
		/*
		scrollVerticalTo(e.getCachedY() - getClientArea().height / 2);
		gotoSimulationTimeWithCenter(e.getSimulationTime());
		*/
	}
	
	/**
	 * Scroll the canvas to make the axis module visible.
	 */
	public void gotoAxisModule(ModuleTreeItem axisModule) {
		for (int i = 0; i < axisModules.size(); i++)
			if (axisModules.get(i) == axisModule)
				scrollVerticalTo(axisModuleYs[i] - axisGraphs.get(i).getHeight() / 2 - getHeight() / 2);
	}
	
	/**
	 * Scroll the canvas to make the value at the given simulation time visible at once.
	 */
	public void gotoAxisValue(ModuleTreeItem axisModule, double simulationTime) {
		for (int i = 0; i < axisModules.size(); i++)
			if (axisModules.get(i) == axisModule) {
				AxisGraph axisGraph = axisGraphs.get(i);
				
				if (axisGraph instanceof AxisValueGraph) {
					AxisValueGraph axisValueGraph = (AxisValueGraph)axisGraph;
					gotoSimulationTimeRange(
						axisValueGraph.getSimulationTime(axisValueGraph.getIndex(simulationTime, true)),
						axisValueGraph.getSimulationTime(axisValueGraph.getIndex(simulationTime, false)),
						(int)(getWidth() * 0.1));
				}
			}
	}
	
	/**
	 * Sets event log and axis modules and vector data.
	 * Tries to keep the simulation time range of the canvas.
	 */
	public void setParameters(IEventLog eventLog, ArrayList<ModuleTreeItem> axisModules, ArrayList<XYArray> axisVectors) {
		boolean firstTime = (this.eventLog == null);
		
		if (!firstTime)
			saveViewportSimulationTimeRange();

		setEventLog(eventLog);
		setAxisVectors(axisVectors);
		setAxisModules(axisModules);
		
		if (!firstTime) {
			sequenceChartFacade.invalidateTimelineCoordinateSystem();
			calculatePixelPerTimelineUnit();
			restoreViewportSimulationTimeRange();
		}
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
	 * Changes pixel per timeline coordinate by zoomFactor.
	 */
	public void zoomBy(double zoomFactor) {
		double time = getViewportCenterSimulationTime();
		setPixelsPerTimelineUnit(getPixelsPerTimelineUnit() * zoomFactor);	
		calculateVirtualSize();
		calculateModuleYCoordinates();
		clearCanvasCacheAndRedraw();
		gotoSimulationTimeWithCenter(time);
	}

	/**
	 * Zoom to the given rectangle, given by pixel coordinates relative to the
	 * top-left corner of the canvas.
	 */
	public void zoomToRectangle(Rectangle r) {
		double timelineCoordinate = getTimelineCoordinateForViewportPixel(r.x);
		double timelineUnitDelta = getTimelineCoordinateForViewportPixel(r.right()) - timelineCoordinate;
		setPixelsPerTimelineUnit(getWidth() / timelineUnitDelta);
		calculateVirtualSize();
		calculateModuleYCoordinates();
		clearCanvasCacheAndRedraw();
		scrollHorizontalTo(getVirtualPixelForTimelineCoordinate(timelineCoordinate));
	}
	
	/**
	 * The event log (data) to be displayed in the chart
	 */
	public IEventLog getEventLog() {
		return eventLog;
	}

	/**
	 * Set the event log to be displayed in the chart
	 */
	private void setEventLog(IEventLog eventLog) {
		this.eventLog = eventLog;
		sequenceChartFacade = new SequenceChartFacade(eventLog);
		sequenceChartFacade.setTimelineMode(TimelineMode.STEP.ordinal());
		axisModules = null;
		axisModulePositions = null;
		axisModuleYs = null;
		moduleIds = null;
		invalidVirtualSize = true;
		invalidModuleYCoordinates = true;
		selectedEvents.clear();
		fireSelectionChanged();
		clearCanvasCacheAndRedraw();
	}
	
	/**
	 * Sets which modules should have axes. Items in axisModules
	 * should point to elements in the moduleTree. 
	 */
	public void setAxisModules(ArrayList<ModuleTreeItem> axisModules) {
		this.axisModules = axisModules;
		
		// update moduleIds
		moduleIds = new IntSet();
		for (int i=0; i<axisModules.size(); i++) {
			ModuleTreeItem treeItem = axisModules.get(i);
			// propagate y to all submodules recursively
			treeItem.visitLeaves(new ModuleTreeItem.IModuleTreeItemVisitor() {
				public void visit(ModuleTreeItem treeItem) {
					moduleIds.insert(treeItem.getModuleId());
				}
			});
		}
		
		axisModulePositions = null;
		axisModuleYs = null;
		invalidVirtualSize = true;
		invalidModuleYCoordinates = true;
		clearCanvasCacheAndRedraw();
	}
	
	/**
	 * Sets the data vectors associated with axis that will be displayed along the axis.
	 */
	private void setAxisVectors(ArrayList<XYArray> axisVectors) {
		axisGraphs = new ArrayList<AxisGraph>();

		for (XYArray axisVector : axisVectors)
			if (axisVector != null) {
				axisGraphs.add(new AxisValueGraph(this, axisVector));
			}
			else
				axisGraphs.add(new AxisGraph(this));

		axisModuleYs = null;
		invalidVirtualSize = true;
		invalidModuleYCoordinates = true;
		clearCanvasCacheAndRedraw();
	}
	
	/**
	 * Calculates axis positions sorting by module ids.
	 */
	private void sortTimelinesByModuleId() {
		Integer[] axisModulesIndices = new Integer[axisModules.size()];
		
		for (int i = 0; i < axisModulesIndices.length; i++)
			axisModulesIndices[i] = i;
		
		java.util.Arrays.sort(axisModulesIndices, new java.util.Comparator<Integer>() {
				public int compare(Integer o1, Integer o2) {
					return ((Integer)axisModules.get(o1).getModuleId()).compareTo(axisModules.get(o2).getModuleId());
				}
			});
		
		for (int i = 0; i < axisModulesIndices.length; i++)
			axisModulePositions[axisModulesIndices[i]] = i;
	}
	
	/**
	 * Calculates axis positions sorting by module names.
	 */
	private void sortTimelinesByModuleName() {
		int[] axisPositions = sortTimelinesByModuleName(axisModules.toArray(new ModuleTreeItem[0]));
		System.arraycopy(axisPositions, 0, axisModulePositions, 0, axisModulePositions.length);
	}

	/**
	 * Same as above but works on a subset of modules.
	 */
	private int[] sortTimelinesByModuleName(final ModuleTreeItem[] modules) {
		int[] axisPositions = new int[modules.length];
		Integer[] axisModulesIndexes = new Integer[modules.length];
		
		for (int i = 0; i < axisModulesIndexes.length; i++)
			axisModulesIndexes[i] = i;
		
		java.util.Arrays.sort(axisModulesIndexes, new java.util.Comparator<Integer>() {
				public int compare(Integer o1, Integer o2) {
					return modules[o1].getModuleFullPath().compareTo(modules[o2].getModuleFullPath());
				}
			});
		
		for (int i = 0; i < axisModulesIndexes.length; i++)
			axisPositions[axisModulesIndexes[i]] = i;
		
		return axisPositions;
	}
	
	/**
	 * Sorts axes by minimizing message arrows crossing timelines.
	 * Axes are sorted in a hierarchic way based on the module hierarchy.
	 */
	private void sortTimelinesHierarchicallyByMinimizingCost(IntVector axisMatrix) {
		sortTimelinesHierarchicallyByMinimizingCostRecursive(axisModules.get(0).getRootModule(), axisMatrix);
	}

	/**
	 * Recursive version for the hierarchic sort of axis timelines.
	 */
	private void sortTimelinesHierarchicallyByMinimizingCostRecursive(final ModuleTreeItem module, final IntVector axisMatrix) {
		if (!axisModules.contains(module)) {
			final int numberOfAxis = axisModules.size();
			final ModuleTreeItem[] submodules = module.getSubmodules();
			final int[] submoduleIndicesToNumberOfAxis = new int[submodules.length];
			final MultiValueMap submoduleToAxisIndexes = new MultiValueMap();
			final int[] axisPositions = new int[numberOfAxis];

			if (submodules.length == 0)
				return;

			// calculate the number of axes and the axis module indexes that a submodule represents
			for (int i = 0; i < numberOfAxis; i++) {
				ModuleTreeItem submodule = axisModules.get(i).getAncestorModuleUnder(module);
			
				if (submodule != null) {
					submoduleIndicesToNumberOfAxis[ArrayUtils.indexOf(submodules, submodule)]++;
					submoduleToAxisIndexes.put(submodule, i);
				}
			}
			
			// first sort descendants recursively
			for (ModuleTreeItem submodule : submodules)
				sortTimelinesHierarchicallyByMinimizingCostRecursive(submodule, axisMatrix);

			// sort the direct submodules of this module
			int[] submoduleIndicesToSubmodulePositions = sortTimelinesByMinimizingCost(submodules, new ICostCalculator() {
				public int calculateCost(int[] submoduleIndicesToSubmodulePositions) {
					int cost = 0;
					
					for (int i = 0; i < numberOfAxis; i++)
						axisPositions[i] = axisModulePositions[i];

					calculateAxisPositions(axisPositions, submodules, submoduleIndicesToSubmodulePositions, submoduleIndicesToNumberOfAxis, submoduleToAxisIndexes);

					// sum up cost of messages to other axis
					for (ModuleTreeItem iSubmodule : submodules)
						for (ModuleTreeItem jSubmodule : submodules)
							if (iSubmodule != jSubmodule)
								for (Object iObject : getAxisModuleIndicesForSubmodule(submoduleToAxisIndexes, iSubmodule))
									for (Object jObject : getAxisModuleIndicesForSubmodule(submoduleToAxisIndexes, jSubmodule))
										cost += SequenceChart.this.calculateCost(axisPositions, (Integer)iObject, (Integer)jObject, axisMatrix);

					return cost;
				}
			});

			// calculate axis offsets and positions for submodules
			calculateAxisPositions(axisModulePositions, submodules, submoduleIndicesToSubmodulePositions, submoduleIndicesToNumberOfAxis, submoduleToAxisIndexes);
		}
		else
			// the positions will be updated each time when returning from this recursive function
			axisModulePositions[axisModules.indexOf(module)] = 0;
	}
	
	/**
	 * Calculates axis positions by shifting axes according to the module positions and count of axes per module.
	 */
	private void calculateAxisPositions(int[] axisPositions, ModuleTreeItem[] submodules, int[] submoduleIndicesToSubmodulePositions, int[] submoduleIndicesToNumberOfAxis, MultiValueMap submoduleToAxisIndexes) {
		int[] submodulePositionsToSubmoduleIndices = new int[submodules.length];
		for (int i = 0; i < submodules.length; i++)
			submodulePositionsToSubmoduleIndices[submoduleIndicesToSubmodulePositions[i]] = i;

		int count = 0;
		int[] submoduleIndicesToSubmoduleAxisOffsets = new int[submodules.length];
		for (int submodulePosition = 0; submodulePosition < submodules.length; submodulePosition++) {
			int c = submoduleIndicesToNumberOfAxis[submodulePositionsToSubmoduleIndices[submodulePosition]];
			submoduleIndicesToSubmoduleAxisOffsets[submodulePositionsToSubmoduleIndices[submodulePosition]] = count;
			count += c;
		}

		// recalculate positions
		for (int submoduleIndex = 0; submoduleIndex < submodules.length; submoduleIndex++) {
			ModuleTreeItem submodule = submodules[submoduleIndex];

			// move axes to the offset of their module
			for (Object i : getAxisModuleIndicesForSubmodule(submoduleToAxisIndexes, submodule))
				axisPositions[(Integer)i] += submoduleIndicesToSubmoduleAxisOffsets[submoduleIndex];
		}
	}

	/**
	 * Just a convenient helper function to return an empty collection if there's no value for the given key.
	 */
	private Collection getAxisModuleIndicesForSubmodule(MultiValueMap submoduleToAxisIndexes, ModuleTreeItem submodule) {
		Collection collection = submoduleToAxisIndexes.getCollection(submodule);
		
		if (collection != null)
			return collection;
		else
			return ListUtils.EMPTY_LIST;
	}

	/**
	 * Sorts axes by minimizing message arrows crossing physical timelines.
	 * Axes are sorted in a flat way without looking at module hierarchy.
	 */
	private void sortTimelinesFlatByMinimizingCost(final IntVector axisMatrix) {
		final ModuleTreeItem[] modules = axisModules.toArray(new ModuleTreeItem[axisModules.size()]);
		final int numberOfAxis = modules.length;
		int[] axisPositions = sortTimelinesByMinimizingCost(modules,
			new ICostCalculator() {
				public int calculateCost(int[] axisPositions) {
					int cost = 0;

					// sum up cost of messages to other axis
					for (int i = 0; i < numberOfAxis; i++)
						for (int j = 0; j < numberOfAxis; j++)
							if (i != j)
								cost += SequenceChart.this.calculateCost(axisPositions, i, j, axisMatrix);
					
					return cost;
				}
			});
		System.arraycopy(axisPositions, 0, axisModulePositions, 0, axisModulePositions.length);
	}
	
	/**
	 * Cost is based on the number of crossed axis according to the positions.
	 */
	private int calculateCost(int[] axisPositions, int i, int j, IntVector axisMatrix) {
		return Math.abs(axisPositions[i] - axisPositions[j]) *
			            (axisMatrix.get(axisPositions.length * i + j) +
                         axisMatrix.get(axisPositions.length * j + i));
	}
	
	/**
	 * Helper interface to calculate the cost of a candidate ordering for the axis being ordered.
	 */
	private interface ICostCalculator {
		public int calculateCost(int[] axisPositions);
	}

	/**
	 * Sorts axes by minimizing message arrows crossing physical timelines.
	 * A message arrow costs as much as many axes it crosses. Uses simulated annealing.
	 */
	private int[] sortTimelinesByMinimizingCost(ModuleTreeItem[] modules, ICostCalculator costCalculator) {
		int cycleCount = 0;
		int noMoveCount = 0;
		int noRandomMoveCount = 0;
		int numberOfAxis = modules.length;
		int[] axisPositions = new int[numberOfAxis]; // actual positions of axis to be returned
		int[] candidateAxisPositions = new int[numberOfAxis]; // new positions of axis to be set (if better)
		int[] bestAxisPositions = new int[numberOfAxis]; // best positions choosen from a set of candidates
		Random r = new Random(0);
		double temperature = 1.0;		

		// set initial axis positions 
		System.arraycopy(sortTimelinesByModuleName(modules), 0, axisPositions, 0, numberOfAxis);
		
		while (cycleCount < 100 && (noMoveCount < numberOfAxis || noRandomMoveCount < numberOfAxis))
		{
			cycleCount++;
			
			// randomly swap axis based on temperature
			double t = temperature;
			noRandomMoveCount++;
			while (false && r.nextDouble() < t) {
				int i1 = r.nextInt(numberOfAxis);
				int i2 = r.nextInt(numberOfAxis);
				int i = axisPositions[i1];
				axisPositions[i1] = axisPositions[i2];
				axisPositions[i2] = i;
				noRandomMoveCount = 0;
				t--;
			}

			// choose an axis which we move to the best place (there are numberOfAxis possibilities)
			int selectedAxisIndex = cycleCount % numberOfAxis;
			int bestPositionOfSelectedAxis = -1;
			int costOfBestPositions = Integer.MAX_VALUE;

			// assume moving axis at index to position i while keeping the order of others and calculate cost
			for (int newPositionOfSelectedAxis = 0; newPositionOfSelectedAxis < numberOfAxis; newPositionOfSelectedAxis++) {
				if (newPositionOfSelectedAxis == -1)
					continue;

				// set up candidateAxisPositions so that the order of other axis do not change
				for (int i = 0; i < numberOfAxis; i++) {
					int pos = axisPositions[i];
					if (newPositionOfSelectedAxis <= pos && pos < axisPositions[selectedAxisIndex])
						pos++;
					if (axisPositions[selectedAxisIndex] < pos && pos <= newPositionOfSelectedAxis)
						pos--;
					candidateAxisPositions[i] = pos;
				}
				candidateAxisPositions[selectedAxisIndex] = newPositionOfSelectedAxis;

				int cost = costCalculator.calculateCost(candidateAxisPositions);
				
				// find minimum cost
				if (cost < costOfBestPositions) {
					costOfBestPositions = cost;
					bestPositionOfSelectedAxis = newPositionOfSelectedAxis;
					System.arraycopy(candidateAxisPositions, 0, bestAxisPositions, 0, numberOfAxis);
				}
			}
			
			// move selected axis into best position if applicable
			if (bestPositionOfSelectedAxis != -1) {
				System.arraycopy(bestAxisPositions, 0, axisPositions, 0, numberOfAxis);
				noMoveCount = 0;
			}
			else
				noMoveCount++;

			// decrease temperature
			temperature *= 0.9;
		}

		return axisPositions;
	}
	
	/**
	 * Sorts axis modules depending on timelineSortMode.
	 */
	private void calculateAxisPositions() {
		axisModulePositions = new int[axisModules.size()];

		switch (timelineSortMode) {
			case MODULE_ID:
				sortTimelinesByModuleId();
				break;
			case MANUAL:
				break;
			case MODULE_NAME:
				sortTimelinesByModuleName();
				break;
				/*
			case MINIMIZE_CROSSINGS:
			case MINIMIZE_CROSSINGS_HIERARCHICALLY:
				// build module id to axis map
				final IntIntMap moduleIdToAxisIdMap = new IntIntMap();
				for (int i = 0; i < axisModules.size(); i++) {
					final Integer ii = i;
					ModuleTreeItem treeItem = axisModules.get(i);
					treeItem.visitLeaves(new ModuleTreeItem.IModuleTreeItemVisitor() {
						public void visit(ModuleTreeItem treeItem) {
							moduleIdToAxisIdMap.set(treeItem.getModuleId(), ii);
						}
					});
				}

				if (timelineSortMode == TimelineSortMode.MINIMIZE_CROSSINGS)
					sortTimelinesFlatByMinimizingCost(eventLog.buildMessageCountGraph(moduleIdToAxisIdMap));
				else
					sortTimelinesHierarchicallyByMinimizingCost(eventLog.buildMessageCountGraph(moduleIdToAxisIdMap));
					
				break;
		*/
		}
	}

	/**
	 * Calculates top y coordinates of axis bounding boxes based on height returned
	 * by each axis.
	 */
	private void calculateAxisYs() {
		axisModuleYs = new int[axisModules.size()];

		for (int i = 0; i < axisModuleYs.length; i++) {
			int y = 0;

			for (int j = 0; j < axisModuleYs.length; j++)
				if (axisModulePositions[j] < axisModulePositions[i])
					y += axisSpacing + axisGraphs.get(j).getHeight();

			axisModuleYs[i] = axisOffset + y;
		}
	}
	
	/**
	 * Distributes window space among axis evenly.
	 */
	private void calculateAxisSpacing() {
		int dy = 0;
		
		for (AxisGraph axisGraph : axisGraphs)
			dy += axisGraph.getHeight();

		if (axisModules.size() == 0 || axisModules.size() == 1)
			setAxisSpacing(0);
		else
			setAxisSpacing(Math.max(AXISLABEL_DISTANCE + 1, (getHeight() - axisOffset * 2 - dy) / (axisModules.size() - 1)));
	}

	/**
	 * Calculates y coordinates for all modules
	 */
	private void calculateModuleYCoordinates() {
		// different y for each selected module
		for (int i=0; i<axisModules.size(); i++) {
			ModuleTreeItem treeItem = axisModules.get(i);
			final int y = axisModuleYs[i] + axisGraphs.get(i).getHeight() / 2;
			// propagate y to all submodules recursively
			treeItem.visitLeaves(new ModuleTreeItem.IModuleTreeItemVisitor() {
				public void visit(ModuleTreeItem treeItem) {
					moduleIdToAxisYMap.put(treeItem.getModuleId(), y);
				}
			});
		}
	}
	
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
	 * If the current pixels/sec setting doesn't look useful for the current event log,
	 * suggest a better one. Otherwise just returns the old value. The current settings
	 * are not changed.
	 */
	public double suggestPixelsPerTimelineUnit() {
		int numEvents = eventLog.getApproximateNumberOfEvents();

		// adjust pixelsPerTimelineUnit if it's way out of the range that makes sense
		if (numEvents >= 2) {
			double tStart = sequenceChartFacade.getTimelineCoordinate(eventLog.getFirstEvent());
			double tEnd = sequenceChartFacade.getTimelineCoordinate(eventLog.getLastEvent());
			double eventsPerTimelineUnit = numEvents / (tEnd - tStart);

			double minPixelsPerTimelineUnit = eventsPerTimelineUnit * 10;  // we want at least 10 pixel/event
			double maxPixelsPerTimelineUnit = eventsPerTimelineUnit * (getWidth() / 10);  // we want at least 10 events on the chart

			if (pixelsPerTimelineUnit < minPixelsPerTimelineUnit)
				return minPixelsPerTimelineUnit;
			else if (pixelsPerTimelineUnit > maxPixelsPerTimelineUnit)
				return maxPixelsPerTimelineUnit;
		}

		return pixelsPerTimelineUnit; // the current setting is fine
	}
	
	/**
	 * Calculates initial pixelPerTimelineUnit.
	 */
	private void calculatePixelPerTimelineUnit() {
		int numEvents = eventLog.getApproximateNumberOfEvents();
		if (pixelsPerTimelineUnit == -1 && numEvents > 50)
			// initial value shows the first 50 events
			pixelsPerTimelineUnit = getWidth() / sequenceChartFacade.getTimelineCoordinate(eventLog.getNeighbourEvent(eventLog.getFirstEvent(), 50));
		else
			setPixelsPerTimelineUnit(suggestPixelsPerTimelineUnit());
	}

	/**
	 * Calculates virtual size of canvas based on the last event's timeline coordinate.
	 */
	private void calculateVirtualSize() {
		IEvent lastEvent = eventLog.getLastEvent();
		long width = lastEvent==null ? 0 : (long)(sequenceChartFacade.getTimelineCoordinate(lastEvent) * getPixelsPerTimelineUnit()) + 3; // event mark should fit in
		width = Math.max(width, 600); // at least half a screen
		long height = (axisModules.size() - 1) * axisSpacing + axisOffset * 2;
		for (int i = 0; i < axisGraphs.size(); i++)
			height += axisGraphs.get(i).getHeight();
		setVirtualSize(width, height);
		invalidVirtualSize = false;
	}

	public void clearCanvasCacheAndRedraw() {
		clearCanvasCache();
		redraw();
	}

	private void calculateStuff() {
		if (pixelsPerTimelineUnit == -1)
			calculatePixelPerTimelineUnit();

		if (invalidVirtualSize)
			calculateVirtualSize();

		if (axisModulePositions == null)
			calculateAxisPositions();

		if (axisSpacing == -1)
			calculateAxisSpacing();

		if (axisModuleYs == null)
			calculateAxisYs();
		
		if (invalidModuleYCoordinates)
			calculateModuleYCoordinates();
	}
	
	@Override
	protected void beforePaint(GC gc) {
		calculateStuff();
		calculateTicks();
	}
	
	@Override
	protected void paintCachableLayer(GC gc) {
		Graphics graphics = new SWTGraphics(gc);
		paintFigure(graphics);
		graphics.dispose();
		
	}

	@Override
	protected void paintNoncachableLayer(GC gc) {
		Graphics graphics = new SWTGraphics(gc);
		paintAxisLabels(graphics);
        paintGutters(graphics);
        paintMouseTick(graphics);
        paintEventSelectionMarks(graphics);
        graphics.dispose();
	}

	/**
	 * Utility function to determine event range we need to paint. 
	 * Returns an array of size 2, or null if the eventLog is empty.
	 */
	protected long[] getFirstLastEventInPixelRange(int x1, int x2) {
		if (eventLog.getApproximateNumberOfEvents() == 0)
			return null;
		
		double tleft = Math.max(0, getTimelineCoordinateForViewportPixel(x1));
		double tright = getTimelineCoordinateForViewportPixel(x2);

		Assert.isTrue(tleft >= 0);
		IEvent startEvent = sequenceChartFacade.getLastEventNotAfterTimelineCoordinate(tleft);
		if (startEvent == null)
			startEvent = eventLog.getFirstEvent();

		Assert.isTrue(tright >= 0);
		IEvent endEvent = sequenceChartFacade.getFirstEventNotBeforeTimelineCoordinate(tright);
		if (endEvent == null)
			endEvent = eventLog.getLastEvent();

		return new long[] {startEvent.getPtr(), endEvent.getPtr()};
	}

	protected void paintFigure(Graphics graphics) {
		if (eventLog != null && eventLog.getApproximateNumberOfEvents() > 0) {
			long startMillis = System.currentTimeMillis();

			Rectangle clip = graphics.getClip(new Rectangle());

			graphics.setAntialias(antiAlias ? SWT.ON : SWT.OFF);
			graphics.setTextAntialias(SWT.ON);

			double startSimulationTime = getSimulationTimeForViewportPixel(clip.x);
			double endSimulationTime = getSimulationTimeForViewportPixel(clip.right());
			int extraClipping = (showMessageNames || showEventNumbers) ? 100 : 10;
			long[] eventPtrRange = getFirstLastEventInPixelRange(clip.x - extraClipping, clip.right() + extraClipping);
			long startEventPtr = eventPtrRange[0];
			long endEventPtr = eventPtrRange[1];
	        System.out.println("redrawing events from: " + sequenceChartFacade.Event_getEventNumber(startEventPtr) + " to: " + sequenceChartFacade.Event_getEventNumber(endEventPtr));

			ensureEventsLoaded(startEventPtr, endEventPtr);
			paintAxis(graphics, startSimulationTime, endSimulationTime);
	        paintMessageArrows(graphics, startEventPtr, endEventPtr);
	        paintEvents(graphics, startEventPtr, endEventPtr);
	
	        long repaintMillis = System.currentTimeMillis()-startMillis;
	        System.out.println("redraw(): "+repaintMillis+"ms");

	        // turn on/off anti-alias 
	        if (antiAlias && repaintMillis > ANTIALIAS_TURN_OFF_AT_MSEC)
	        	antiAlias = false;
	        else if (!antiAlias && repaintMillis < ANTIALIAS_TURN_ON_AT_MSEC)
	        	antiAlias = true;
		}
	}

	private void ensureEventsLoaded(long startEventPtr, long endEventPtr) {
		for (long eventPtr = startEventPtr; eventPtr != endEventPtr; eventPtr = sequenceChartFacade.Event_getNextEvent(eventPtr));
	}

	private void paintAxis(Graphics graphics, double startSimulationTime, double endSimulationTime) {
		for (int i = 0; i < axisModules.size(); i++) {
			int y = (int)(axisModuleYs[i] - getViewportTop());
			AxisGraph axisGraph = axisGraphs.get(i);
			int dy = y;
			graphics.translate(0, dy);
			axisGraph.paintAxis(graphics, startSimulationTime, endSimulationTime);
			graphics.translate(0, -dy);
		}
	}

	private void paintMessageArrows(Graphics graphics, long startEventPtr, long endEventPtr) {
		VLineBuffer vlineBuffer = new VLineBuffer();
		
		for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.Event_getNextEvent(eventPtr)) {
			// TODO: mark dependencies which were drawns and draw only if not yet marked
//			int numCauses = eventLogFacade.Event_getNumCauses(eventPtr);
//			for (int i = 0; i < numCauses; i++)
//				drawMessageArrow(graphics, eventLogFacade.Event_getCause(eventPtr, i), vlineBuffer);

			int numConsequences = sequenceChartFacade.Event_getNumConsequences(eventPtr);
			for (int i = 0; i < numConsequences; i++)
				drawMessageArrow(graphics, sequenceChartFacade.Event_getConsequence(eventPtr, i), vlineBuffer);

			if (eventPtr == endEventPtr)
				break;
		}
	}

	private void paintEvents(Graphics graphics, long startEventPtr, long endEventPtr) {
		graphics.setForegroundColor(EVENT_FG_COLOR);
		HashMap<Integer,Integer> axisYtoLastX = new HashMap<Integer, Integer>();

		for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.Event_getNextEvent(eventPtr)) {
			int x = getEventViewportXCoordinate(eventPtr);
			int y = getEventViewportYCoordinate(eventPtr);
			Integer lastX = axisYtoLastX.get(y);

			// performance optimization: don't paint event if there's one already drawn exactly there
			if (lastX == null || lastX.intValue() != x) {
				axisYtoLastX.put(y, x);
				
				graphics.setBackgroundColor(EVENT_FG_COLOR);
				graphics.fillOval(x - 2, y - 3, 5, 7);

				if (showEventNumbers) {
					graphics.setBackgroundColor(EVENT_BG_COLOR);
					graphics.fillText("#" + sequenceChartFacade.Event_getEventNumber(eventPtr), x + 3, y + 3);
				}
			}
			
			if (eventPtr == endEventPtr)
				break;
		}
	}
	
	private long getEventXCoordinate(long eventPtr) {
		// TODO: use cache
		return (long)(sequenceChartFacade.Event_getTimelineCoordinate(eventPtr) * pixelsPerTimelineUnit);
	}

	private long getEventYCoordinate(long eventPtr) {
		// TODO: use cache
		return moduleIdToAxisYMap.get(sequenceChartFacade.Event_getModuleId(eventPtr));
	}

	private int getEventViewportXCoordinate(long eventPtr) {
		return (int)(getEventXCoordinate(eventPtr) - getViewportLeft());
	}

	private int getEventViewportYCoordinate(long eventPtr) {
		return (int)(getEventYCoordinate(eventPtr) - getViewportTop());
	}

	private void paintGutters(Graphics graphics) {
		graphics.setBackgroundColor(GUTTER_BACKGROUND_COLOR);
		graphics.fillRectangle(0, 0, getWidth(), GUTTER_HEIGHT);
		graphics.fillRectangle(0, getHeight() - GUTTER_HEIGHT - 1, getWidth(), GUTTER_HEIGHT);
		paintTicks(graphics);
		graphics.setBackgroundColor(GUTTER_BORDER_COLOR);
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.drawRectangle(0, 0, getWidth(), GUTTER_HEIGHT);
		graphics.drawRectangle(0, getHeight() - GUTTER_HEIGHT - 1, getWidth(), GUTTER_HEIGHT);
	}

	private void paintTicks(Graphics graphics) {
		for (BigDecimal tick : ticks)
			paintTick(graphics, tick);
	}
	
	private void paintMouseTick(Graphics graphics) {
		Point p = toControl(Display.getDefault().getCursorLocation());
		
		if (0 <= p.x && p.x < getWidth() &&
			0 <= p.y && p.y < getHeight()) {
			BigDecimal t = new BigDecimal(getSimulationTimeForViewportPixel(p.x));
			paintTick(graphics, calculateTick(t, 1));
		}
	}
	
	private void paintTick(Graphics graphics, BigDecimal simulationTime) {
		int x = getViewportPixelForSimulationTime(simulationTime.doubleValue());
		graphics.setLineStyle(SWT.LINE_DOT);
		graphics.setForegroundColor(TICK_LINE_COLOR);
		graphics.drawLine(x, 0, x, getHeight());
		graphics.setForegroundColor(TICK_LABEL_COLOR);
		String str = simulationTime.toPlainString() + "s";
		graphics.setBackgroundColor(GUTTER_BACKGROUND_COLOR);
		graphics.fillText(str, x + 3, 2);
		graphics.fillText(str, x + 3, getHeight() - 16);
	}
	
	private void paintEventSelectionMarks(Graphics graphics) {
		long[] eventPtrRange = getFirstLastEventInPixelRange(0 - EVENT_SEL_RADIUS, getClientArea().width + EVENT_SEL_RADIUS);
		long startEventPtr = eventPtrRange[0];
		long endEventPtr = eventPtrRange[1];
		int startEventNumber = sequenceChartFacade.Event_getEventNumber(startEventPtr);
		int endEventNumber = sequenceChartFacade.Event_getEventNumber(endEventPtr);

		graphics.setAntialias(SWT.ON); //XXX
		
		// paint event selection marks
		if (selectedEvents != null) {
			graphics.setLineStyle(SWT.LINE_SOLID);
		    graphics.setForegroundColor(EVENT_SEL_COLOR);
			for (IEvent selectedEvent : selectedEvents) {
		    	if (startEventNumber <= selectedEvent.getEventNumber() && selectedEvent.getEventNumber() <= endEventNumber)
		    	{
		    		int x = getEventViewportXCoordinate(selectedEvent.getPtr());
		    		int y = getEventViewportYCoordinate(selectedEvent.getPtr());
		    		graphics.drawOval(x - EVENT_SEL_RADIUS, y - EVENT_SEL_RADIUS, EVENT_SEL_RADIUS * 2 + 1, EVENT_SEL_RADIUS * 2 + 1);
		    	}
			}
		} 
	}

	/**
	 * Paints axis labels if there's enough space between axes.
	 */
	private void paintAxisLabels(Graphics graphics) {
		if (AXISLABEL_DISTANCE < axisSpacing) {
			graphics.setForegroundColor(LABEL_COLOR);

			for (int i = 0; i < axisModules.size(); i++) {
				ModuleTreeItem treeItem = axisModules.get(i);
				int y = (int)(axisModuleYs[i] - getViewportTop());
				String label = treeItem.getModuleFullPath();
				graphics.drawText(label, 5, y - AXISLABEL_DISTANCE);
			}
		}
	}

	private void drawMessageArrow(Graphics graphics, long messageDependencyPtr, VLineBuffer vlineBuffer) {
		long causeEventPtr = sequenceChartFacade.MessageDependency_getCauseEvent(messageDependencyPtr);
		long consequenceEventPtr = sequenceChartFacade.MessageDependency_getConsequenceEvent(messageDependencyPtr);

		// TODO: we probably do not need this, it's check in C++ anyway
		if (causeEventPtr == 0 || consequenceEventPtr == 0)
			return;
		
        int x1 = getEventViewportXCoordinate(causeEventPtr);
        int y1 = getEventViewportYCoordinate(causeEventPtr);
        int x2 = getEventViewportXCoordinate(consequenceEventPtr);
        int y2 = getEventViewportYCoordinate(consequenceEventPtr);
		//System.out.printf("drawing %d %d %d %d \n", x1, x2, y1, y2);

        // check whether we'll need to draw an arrowhead
        boolean needArrowHead = showArrowHeads;
		if (needArrowHead) {
			// optimization: check if arrowhead is in the clipping rect (don't draw it if not)
			TEMP_RECT.setLocation(x2,y2); 
			TEMP_RECT.expand(2*ARROWHEAD_LENGTH, 2*ARROWHEAD_LENGTH);
			graphics.getClip(Rectangle.SINGLETON);
			needArrowHead = Rectangle.SINGLETON.intersects(TEMP_RECT);
		}
		
		// message name (as label on the arrow)
		String arrowLabel = null;
		if (showMessageNames) {
			switch (sequenceChartFacade.MessageDependency_getKind(messageDependencyPtr)) {
				case MessageDependencyKind.SEND:
					arrowLabel = sequenceChartFacade.MessageDependency_getCauseMessageName(messageDependencyPtr);
					break;
				case MessageDependencyKind.REUSE:
					arrowLabel = "Reuse";
					break;
				case MessageDependencyKind.FILTERED:
					arrowLabel = sequenceChartFacade.MessageDependency_getCauseMessageName(messageDependencyPtr) +
								 " -> " +
								 sequenceChartFacade.FilteredMessageDependency_getMiddleMessageName(messageDependencyPtr);
					break;
			}
		}

		// line color and style depends on message type
		boolean isDelivery = sequenceChartFacade.MessageDependency_isMessageSend(messageDependencyPtr);
		if (isDelivery) {
			graphics.setForegroundColor(DELIVERY_MESSAGE_COLOR);
			graphics.setLineStyle(SWT.LINE_SOLID);
		}
		else { 
			graphics.setForegroundColor(NONDELIVERY_MESSAGE_COLOR);
			graphics.setLineDash(DOTTED_LINE_PATTERN); // SWT.LINE_DOT style is not what we want
		}

		// check if message was sent from a method call (event module != message source module).
		// XXX This currently only works for non-delivery messages, as we don't have enough info in the log file;
		// XXX even with non-delivery messages it acts strange... 
		//if (!isDelivery && logFacade.getMessage_source_cause_module_moduleId(pos) != logFacade.getMessage_module_moduleId(pos)) {
		//	graphics.setForegroundColor(EVENT_FG_COLOR); //FIXME temporarily red
		//}
		
		// test if self-message (y1==y2) or not
		if (y1==y2) {

			int halfEllipseHeight = isDelivery ? DELIVERY_SELFARROW_HEIGHT : NONDELIVERY_SELFARROW_HEIGHT;
			
			if (x1==x2) {
				// draw vertical line (as zero-width half ellipse) 
				if (vlineBuffer.vlineContainsNewPixel(x1, y1-halfEllipseHeight, y1))
					graphics.drawLine(x1, y1, x1, y1 - halfEllipseHeight);

				if (needArrowHead)
					drawArrowHead(graphics, x1, y1, 0, 1);

				if (showMessageNames)
					drawMessageArrowLabel(graphics, arrowLabel, x1, y1, 2, -15);
			}
			else {
				// draw half ellipse
				Rectangle.SINGLETON.setLocation(x1, y1 - halfEllipseHeight);
				Rectangle.SINGLETON.setSize(x2-x1, halfEllipseHeight * 2);
				graphics.drawArc(Rectangle.SINGLETON, 0, 180);
				
				if (needArrowHead) {
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
						drawArrowHead(graphics, x2, y2, 0, 1);
					else {
						// shift solution to the coordinate system of the canvas
						x = (x1 + x2) / 2 + x;
						y = y1 + y;
						drawArrowHead(graphics, x2, y2, x2 - x, y2 - y);
					}
				}

				if (showMessageNames)
					drawMessageArrowLabel(graphics, arrowLabel, (x1 + x2) / 2, y1, 0, -halfEllipseHeight - 15);
			}
		}
		else {
			// draw straight line
			if (x1!=x2 || vlineBuffer.vlineContainsNewPixel(x1, y1, y2))
				graphics.drawLine(x1, y1, x2, y2);
			
			if (needArrowHead)
				drawArrowHead(graphics, x2, y2, x2 - x1, y2 - y1);
			
			if (showMessageNames)
				drawMessageArrowLabel(graphics, arrowLabel, (x1 + x2) / 2, (y1 + y2) / 2, 2, y1 < y2 ? -15 : 0);
		}
	}
	
	private void drawMessageArrowLabel(Graphics graphics, String label, int x, int y, int dx, int dy) {
		if (MESSAGE_LABEL_COLOR!=null)
			graphics.setForegroundColor(MESSAGE_LABEL_COLOR);
		graphics.drawText(label, x + dx, y + dy);
	}
	
	/**
	 * Draws an arrowhead.
	 * XXX what are the parameters? document!!!
	 */
	private void drawArrowHead(Graphics graphics, int x, int y, double dx, double dy) {
		double n = Math.sqrt(dx * dx + dy * dy);
		double dwx = -dy / n * ARROWHEAD_WIDTH / 2;
		double dwy = dx / n * ARROWHEAD_WIDTH / 2;
		double xt = x - dx * ARROWHEAD_LENGTH / n;
		double yt = y - dy * ARROWHEAD_LENGTH / n;
		int x1 = (int)Math.round(xt - dwx);
		int y1 = (int)Math.round(yt - dwy);
		int x2 = (int)Math.round(xt + dwx);
		int y2 = (int)Math.round(yt + dwy);

		graphics.setBackgroundColor(ARROWHEAD_COLOR!=null ? ARROWHEAD_COLOR : graphics.getForegroundColor());
		graphics.fillPolygon(new int[] {x, y, x1, y1, x2, y2});
	}

	/**
	 * Calculates and stores ticks as simulation times based on tick spacing. Tries to round tick values
	 * to have as short numbers as possible within a range of pixels.
	 */
	private void calculateTicks() {
		ticks = new ArrayList<BigDecimal>();
		org.eclipse.swt.graphics.Rectangle rect = getClientArea();
		
		if (getTimelineMode() == TimelineMode.LINEAR) {
			// puts ticks to constant distance from each other measured in timeline units
			double tleft = getSimulationTimeForViewportPixel(rect.x);
			double tright = getSimulationTimeForViewportPixel(rect.x + rect.width);
			int tickScale = (int)Math.ceil(Math.log10(TICK_SPACING / pixelsPerTimelineUnit));
			BigDecimal tickSpacing = BigDecimal.valueOf(TICK_SPACING / pixelsPerTimelineUnit);
			BigDecimal tickStart = new BigDecimal(tleft).setScale(-tickScale, RoundingMode.FLOOR);
			BigDecimal tickEnd = new BigDecimal(tright).setScale(-tickScale, RoundingMode.CEILING);
			BigDecimal tickIntvl = new BigDecimal(1).scaleByPowerOfTen(tickScale);

			// use 2, 4, 6, 8, etc. if possible
			if (tickIntvl.divide(BigDecimal.valueOf(5)).compareTo(tickSpacing) > 0)
				tickIntvl = tickIntvl.divide(BigDecimal.valueOf(5));
			// use 5, 10, 15, 20, etc. if possible
			else if (tickIntvl.divide(BigDecimal.valueOf(2)).compareTo(tickSpacing) > 0)
				tickIntvl = tickIntvl.divide(BigDecimal.valueOf(2));

			for (BigDecimal t=tickStart; t.compareTo(tickEnd)<0; t = t.add(tickIntvl))
				ticks.add(t);
		}
		else {
			// tries to put ticks constant distance from each other measured in virtual pixels
			long tleft = getViewportLeft();
			long tright = tleft + rect.width;
			int tickScale = (int)Math.ceil(Math.log10(TICK_SPACING));
			BigDecimal tickStart = new BigDecimal(tleft).setScale(-tickScale, RoundingMode.FLOOR);
			BigDecimal tickEnd = new BigDecimal(tright).setScale(-tickScale, RoundingMode.CEILING);
			BigDecimal tickIntvl = new BigDecimal(1).scaleByPowerOfTen(tickScale);
	
			for (long t = tickStart.longValue(); t < tickEnd.longValue();) {
				double simulationTime = getSimulationTimeForTimelineCoordinate(getTimelineCoordinateForVirtualPixel(t));
				BigDecimal tick = calculateTick(new BigDecimal(simulationTime), TICK_SPACING / 4);
				ticks.add(tick);
				t = t + tickIntvl.longValue();
			}
		}
	}

	/**
	 * Calculates a single tick near simulation time. The range is defined in terms of pixels.
	 * Minimizes the number of characters to have a short tick label.
	 */
	private BigDecimal calculateTick(BigDecimal simulationTime, int halfTickRange) {
		int x = getViewportPixelForSimulationTime(simulationTime.doubleValue());
		// defines the range of valid simulation times for the tick
		BigDecimal tMin = new BigDecimal(getSimulationTimeForTimelineCoordinate(Math.max(0, getTimelineCoordinateForViewportPixel(x - halfTickRange))));
		BigDecimal tMax = new BigDecimal(getSimulationTimeForViewportPixel(x + halfTickRange));
		// number of digits
		int tMinPrecision = tMin.stripTrailingZeros().precision();
		int tMaxPrecision = tMax.stripTrailingZeros().precision();
		int tDeltaPrecision = tMax.subtract(tMin).stripTrailingZeros().precision();
		int precision = Math.max(0, 1 + Math.max(tMinPrecision - tDeltaPrecision, tMaxPrecision - tDeltaPrecision));
		// estabilish initial rounding contextes
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
		
		tBestRoundedMin = calculateTickBestLastDigit(tBestRoundedMin, tMin, tMax);
		tBestRoundedMax = calculateTickBestLastDigit(tBestRoundedMax, tMin, tMax);
		
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
	

	/**
	 * Translates timeline coordinate to simulation time.
	 */
	public double getSimulationTimeForTimelineCoordinate(double t) {
		Assert.isTrue(t >= 0);
		return sequenceChartFacade.getSimulationTimeForTimelineCoordinate(t);
	}
	
	/**
	 * Translates viewport pixel x coordinate to simulation time.
	 */
	public double getSimulationTimeForViewportPixel(int x) {
		return getSimulationTimeForTimelineCoordinate(getTimelineCoordinateForViewportPixel(x));
	}
	
	/**
	 * Translates simulation time to viewport pixel x coordinate.
	 */
	public int getViewportPixelForSimulationTime(double t) {
		Assert.isTrue(t >= 0);
		return (int)((long)Math.round(sequenceChartFacade.getTimelineCoordinateForSimulationTime(t) * pixelsPerTimelineUnit) - getViewportLeft());
	}
	
	/**
	 * Translates from viewport pixel x coordinate to timeline coordinate, using on pixelsPerTimelineUnit.
	 */
	public double getTimelineCoordinateForViewportPixel(int x) {
		return (x + getViewportLeft()) / pixelsPerTimelineUnit;
	}

	/**
	 * Translates timeline coordinate to viewport pixel x coordinate, using on pixelsPerTimelineUnit.
	 * Extreme values get clipped to a reasonable interval (-XMAX, XMAX).
	 */
	public int getViewportPixelForTimelineCoordinate(double t) {
		Assert.isTrue(t >= 0);
		long x = Math.round(t * pixelsPerTimelineUnit) - getViewportLeft();
    	return (x < -XMAX) ? -XMAX : (x > XMAX) ? XMAX : (int)x;
	}

	/**
	 * Translates virtual pixel x coordinate to timeline coordinate, using on pixelsPerTimelineUnit.
	 */
	public double getTimelineCoordinateForVirtualPixel(long x) {
		Assert.isTrue(x >= 0);
		return x / pixelsPerTimelineUnit;
	}

	/**
	 * Translates timeline coordinate to virtual pixel x coordinate, using on pixelsPerTimelineUnit.
	 */
	public long getVirtualPixelForTimelineCoordinate(double t) {
		Assert.isTrue(t >= 0);
		return Math.round(t * pixelsPerTimelineUnit);
	}

	/**
	 * Sets up default mouse handling.
	 */
	private void setUpMouseHandling() { 
		addListener(SWT.MouseWheel, new Listener() {
			public void handleEvent(Event event) {
				if ((event.stateMask & SWT.CTRL)!=0) {
					for (int i = 0; i < event.count; i++)
						zoomBy(1.1);
	
					for (int i = 0; i < -event.count; i++)
						zoomBy(1.0 / 1.1);
				}
				else if ((event.stateMask & SWT.SHIFT)!=0) {
					scrollHorizontalTo(getViewportLeft() - getWidth() * event.count / 20);
				}
			}
		});
		// dragging ("hand" cursor) and tooltip
		addMouseListener(new MouseListener() {
			public void mouseDoubleClick(MouseEvent e) {}
			public void mouseDown(MouseEvent e) {
				setFocus();

				if (e.button == 1) {
					setCursor(DRAG_CURSOR);
					dragStartX = e.x;
					dragStartY = e.y;
				}

				removeTooltip();
			}
			public void mouseUp(MouseEvent e) {
				setCursor(null); // restore cursor at end of drag
				dragStartX = dragStartY = -1;
			}
    	});
		addMouseTrackListener(new MouseTrackListener() {
			public void mouseEnter(MouseEvent e) {
				redraw();
			}
			public void mouseExit(MouseEvent e) {
				redraw();
			}
			public void mouseHover(MouseEvent e) {
				if ((e.stateMask & SWT.BUTTON_MASK) == 0)
					displayTooltip(e.x, e.y);
			}
		});
		addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				removeTooltip();
				if (dragStartX != -1 && dragStartY != -1 && (e.stateMask & SWT.BUTTON_MASK) != 0 && (e.stateMask & SWT.MODIFIER_MASK) == 0)
					mouseDragged(e);
				else {
					setCursor(null); // restore cursor at end of drag (must do it here too, because we 
									 // don't get the "released" event if user releases mouse outside the canvas)
					redraw();
				}
			}

			private void mouseDragged(MouseEvent e) {
				// scroll by the amount moved since last drag call
				int dx = e.x - dragStartX;
				int dy = e.y - dragStartY;
				scrollHorizontalTo(getViewportLeft() - dx);
				scrollVerticalTo(getViewportTop() - dy);
				dragStartX = e.x;
				dragStartY = e.y;
			}
		});
		// selection handling
		addMouseListener(new MouseListener() {
			public void mouseDoubleClick(MouseEvent me) {
				ArrayList<IEvent> tmp = new ArrayList<IEvent>();
				collectStuffUnderMouse(me.x, me.y, tmp, null);
				if (eventListEquals(selectedEvents, tmp)) {
					fireSelection(true);
				}
				else {
					selectedEvents = tmp;
					fireSelection(true);
					fireSelectionChanged();
					redraw();
				}
			}
			public void mouseDown(MouseEvent me) {
				//XXX improve mouse handling: starting dragging should not deselect events!
				if (me.button==1) {
					ArrayList<IEvent> tmp = new ArrayList<IEvent>();
					if ((me.stateMask & SWT.CTRL)!=0) // CTRL key extends selection
						for (IEvent e : selectedEvents) 
							tmp.add(e);
					collectStuffUnderMouse(me.x, me.y, tmp, null);
					if (eventListEquals(selectedEvents, tmp)) {
						fireSelection(false);
					}
					else {
						selectedEvents = tmp;
						fireSelection(false);
						fireSelectionChanged();
						redraw();
					}
					
				}
			}
			public void mouseUp(MouseEvent me) {}
		});
	}

	/**
	 * Utility function, used in selection change handling
	 */
	private static boolean eventListEquals(List<IEvent> a, List<IEvent> b) {
		if (a==null || b==null)
			return a==b;
		if (a.size() != b.size())
			return false;
		for (int i=0; i<a.size(); i++)
			if (a.get(i).getEventNumber() != b.get(i).getEventNumber()) // cannot use a.get(i)==b.get(i) because SWIG return new instances every time
				return false;
		return true;
	}
	
	protected void displayTooltip(int x, int y) {
		String tooltipText = getTooltipText(x,y);
		if (tooltipText!=null) {
			tooltipWidget = new DefaultInformationControl(getShell());
			tooltipWidget.setInformation(tooltipText);
			tooltipWidget.setLocation(toDisplay(x,y+20));
			Point size = tooltipWidget.computeSizeHint();
			tooltipWidget.setSize(size.x, size.y);
			tooltipWidget.setVisible(true);
		}
	}

	protected void removeTooltip() {
		if (tooltipWidget!=null) {
			tooltipWidget.setVisible(false);
			tooltipWidget.dispose();
			tooltipWidget = null;
		}
	}
	
	/**
	 * Calls collectStuffUnderMouse(), and assembles a possibly multi-line
	 * tooltip text from it. Returns null if there's no text to display.
	 */
	protected String getTooltipText(int x, int y) {
		ArrayList<IEvent> events = new ArrayList<IEvent>();
		ArrayList<MessageDependency> msgs = new ArrayList<MessageDependency>();
		collectStuffUnderMouse(x, y, events, msgs);

		// 1) if there are events under them mouse, show them in the tooltip
		if (events.size()>0) {
			String res = "";
			int count = 0;
			for (IEvent event : events) {
				if (count++ > MAX_TOOLTIP_LINES) {
					res += "...and "+(events.size()-count)+" more";
					break;
				}
				res += getEventText(event) + "\n";
			}
			res = res.trim();
			return res;
		}
			
		// 2) no events: show message arrows info
		if (msgs.size()>=1) {
			String res = "";
			int count = 0;
			for (MessageDependency msg : msgs) {
				// truncate tooltip
				if (count++ > MAX_TOOLTIP_LINES) {
					res += "...and "+(msgs.size()-count)+" more";
					break;
				}
				// add message
				res += getMessageText(msg) + "\n"; 
			}
			res = res.trim();
			return res;
		}

		// 3) no events or message arrows: show axis info
		ModuleTreeItem axisModule = findAxisAt(y);
		if (axisModule!=null) {
			String res = getAxisText(axisModule)+"\n";
			double t = getSimulationTimeForViewportPixel(x);
			res += String.format("t = %gs", t);
// TODO: resurrect
//			IEvent event = eventLog.getLastEventBefore(t);
//			if (event!=null)
//				res += ", just after event #"+event.getEventNumber(); 
			return res;
		}

		// absolutely nothing to say
		return null;
	}

	/**
	 * Returns a descriptive message for the ModuleTreeItem to be presented to the user.
	 */
	public String getAxisText(ModuleTreeItem axisModule) {
		return "axis "+axisModule.getModuleFullPath();
	}

	/**
	 * Returns a descriptive message for the MessageDependency to be presented to the user.
	 */
	public String getMessageText(MessageDependency msg) {
		BeginSendEntry beginSendEntry = msg.getCauseBeginSendEntry();
		String result = "message (" + beginSendEntry.getMessageClassName() + ") " + beginSendEntry.getMessageFullName()
			+ "  (" + (msg instanceof MessageSend ? "sending" : "usage")  //TODO also: "selfmsg"
			+ ", #" + msg.getCauseEventNumber() + " -> #" + msg.getConsequenceEventNumber() + ")";
			
		return result;
	}

	/**
	 * Returns a descriptive message for the IEvent to be presented to the user.
	 */
	public String getEventText(IEvent event) {
		BeginSendEntry beginSendEntry = event.getCause().getCauseBeginSendEntry();
		ModuleCreatedEntry moduleCreatedEntry = eventLog.getModuleCreatedEntry(event.getModuleId());
		String result = "event #" + event.getEventNumber() + " at t=" + event.getSimulationTime()
			+ "  at (" + moduleCreatedEntry.getModuleClassName() + ") "
			+ moduleCreatedEntry.getFullName()
			+ " (id=" + event.getModuleId() + ")"
			+ "  message (" + beginSendEntry.getMessageClassName() + ") "
			+ beginSendEntry.getMessageFullName();
		return result;
	}

	/**
	 * Returns the axis at the given Y coordinate (with MOUSE_TOLERANCE), or null. 
	 */
	public ModuleTreeItem findAxisAt(long y) {
		y += getViewportTop();
		for (int i=0; i<axisModuleYs.length; i++) {
			int height = axisGraphs.get(i).getHeight();
			if (axisModuleYs[i] - MOUSE_TOLERANCE <= y && y <= axisModuleYs[i] + height + MOUSE_TOLERANCE)
				return axisModules.get(i);
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
	public void collectStuffUnderMouse(int mouseX, int mouseY, List<IEvent> events, List<MessageDependency> msgs) {
		if (eventLog!=null) {
			long startMillis = System.currentTimeMillis();
		
			// determine start/end event numbers
			long[] eventPtrRange = getFirstLastEventInPixelRange(0, getClientArea().width);
			long startEventPtr = eventPtrRange[0];
			long endEventPtr = eventPtrRange[1];
			int startEventNumber = sequenceChartFacade.Event_getEventNumber(startEventPtr);
			int endEventNumber = sequenceChartFacade.Event_getEventNumber(endEventPtr);

			// check events
            if (events != null) {
            	for (long eventPtr = startEventPtr;; eventPtr = sequenceChartFacade.Event_getNextEvent(eventPtr)) {
					if (eventSymbolContainsPoint(mouseX, mouseY, getEventViewportXCoordinate(eventPtr), getEventViewportYCoordinate(eventPtr), MOUSE_TOLERANCE))
   						events.add(sequenceChartFacade.Event_getEvent(eventPtr));
   					
   					if (eventPtr == endEventPtr)
	   					break;
	   			}
            }

/* TODO: resurrect
            // check message arrows
            if (msgs != null) {
    			// collect msgs
            	IntVector msgsIndices = eventLog.getMessagesIntersecting(startEventNumber, endEventNumber, moduleIds, showNonDeliveryMessages); 
        		//System.out.printf("interval: #%d, #%d, %d msgs to check\n",startEventNumber, endEventNumber, msgsIndices.size());
            	for (int i=0; i<msgsIndices.size(); i++) {
            		int pos = msgsIndices.get(i);
            		if (messageArrowContainsPoint(pos, mouseX, mouseY, MOUSE_TOLERANCE))
            			msgs.add(eventLog.getMessage(pos));
            	}
            }
            long millis = System.currentTimeMillis()-startMillis;
            //System.out.println("collectStuffUnderMouse(): "+millis+"ms - "+(events==null ? "n/a" : events.size())+" events, "+(msgs==null ? "n/a" : msgs.size())+" msgs");
*/
		}
	}

	/**
	 * Utility function, to detect whether use clicked (hovered) an event in the chart
	 */
	private boolean eventSymbolContainsPoint(int x, int y, int px, int py, int tolerance) {
		return Math.abs(x-px) <= 2+tolerance && Math.abs(y-py) <= 5+tolerance;
	}

	private boolean messageArrowContainsPoint(long messageDependencyPtr, int px, int py, int tolerance) {
		long causeEventPtr = sequenceChartFacade.MessageDependency_getCauseEvent(messageDependencyPtr);
		long consequenceEventPtr = sequenceChartFacade.MessageDependency_getConsequenceEvent(messageDependencyPtr);

		// TODO: we probably do not need this, it's check in C++ anyway
		if (causeEventPtr == 0 || consequenceEventPtr == 0)
			return false;
		
        int x1 = getEventViewportXCoordinate(causeEventPtr);
        int y1 = getEventViewportYCoordinate(causeEventPtr);
        int x2 = getEventViewportXCoordinate(consequenceEventPtr);
        int y2 = getEventViewportYCoordinate(consequenceEventPtr);

		//System.out.printf("checking %d %d %d %d\n", x1, x2, y1, y2);
		if (y1==y2) {
			int height = sequenceChartFacade.MessageDependency_isMessageSend(messageDependencyPtr) ? DELIVERY_SELFARROW_HEIGHT : NONDELIVERY_SELFARROW_HEIGHT;
			return halfEllipseContainsPoint(x1, x2, y1, height, px, py, tolerance);
		}
		else
			return lineContainsPoint(x1, y1, x2, y2, px, py, tolerance);
	}
	
	private boolean halfEllipseContainsPoint(int x1, int x2, int y, int height, int px, int py, int tolerance) {
		tolerance++;

		Rectangle.SINGLETON.setSize(0, 0);
		Rectangle.SINGLETON.setLocation(x1, y);
		Rectangle.SINGLETON.union(x2, y-height);
		Rectangle.SINGLETON.expand(tolerance, tolerance);
		if (!Rectangle.SINGLETON.contains(px, py))
			return false;

		int x = (x1+x2) / 2;
		int rx = Math.abs(x1-x2) / 2;
		int ry = height;

        if (rx == 0)
        	return true;
		
		int dxnorm = (x - px) * ry / rx;
		int dy = y - py;
		int distSquare = dxnorm*dxnorm + dy*dy;
		return distSquare < (ry+tolerance)*(ry+tolerance) && distSquare > (ry-tolerance)*(ry-tolerance); 
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
        Object[] listeners = selectionChangedListeners.getListeners();
        final SelectionChangedEvent event = new SelectionChangedEvent(this, getSelection());        
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
		return new EventLogSelection(eventLog, selectedEvents);
	}

	/**
	 * Sets the currently "selected" events. The selection must be an
	 * instance of IEventLogSelection and refer to the current eventLog, 
	 * otherwise the call will be ignored. Selection is displayed as red
	 * circles in the graph.
	 */
	public void setSelection(ISelection newSelection) {
		System.out.println("SeqChartFigure got selection: "+newSelection);
		
		if (!(newSelection instanceof IEventLogSelection))
			return; // wrong selection type
		IEventLogSelection newEventLogSelection = (IEventLogSelection)newSelection;
		if (newEventLogSelection.getEventLog() != eventLog)
			return;  // wrong -- refers to another eventLog

		// if new selection differs from existing one, take over its contents
		if (!eventListEquals(newEventLogSelection.getEvents(), selectedEvents)) {
			selectedEvents.clear();
			for (IEvent e : newEventLogSelection.getEvents()) 
				selectedEvents.add(e);

			// go to the time of the first event selected
			if (selectedEvents.size()>0) {
				gotoEvent(selectedEvents.get(0));
			}

			redraw();
		}
	}
	
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
			Region() {} 
			Region(int y1, int y2) {this.y1=y1; this.y2=y2;} 
		}
		ArrayList<Region> regions = new ArrayList<Region>();

		public boolean vlineContainsNewPixel(int x, int y1, int y2) {
			if (y1>y2) {
				int tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			if (x!=currentX) {
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
			if (i==-1) {
				// no overlapping region, add this one and return
				regions.add(new Region(y1, y2));
				return true;
			}

			// existing region entirely contains this one (most frequent, fast route)
			Region r = regions.get(i);
			if (y1>=r.y1 && y2<=r.y2)
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
				if (r.y1>r2.y1) r.y1 = r2.y1; 
				if (r.y2<r2.y2) r.y2 = r2.y2;
			}
			regions.add(r);
		}
		private int findOverlappingRegion(int y1, int y2) {
			for (int i=0; i<regions.size(); i++) {
				Region r = regions.get(i);
				if (r.y1 < y2 && r.y2 > y1)
					return i;
			}
			return -1;
		}
	}
}

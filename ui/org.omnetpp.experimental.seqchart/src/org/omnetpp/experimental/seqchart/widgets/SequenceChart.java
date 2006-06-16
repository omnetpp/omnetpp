package org.omnetpp.experimental.seqchart.widgets;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.math.MathContext;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Random;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.Graphics;
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
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.omnetpp.experimental.seqchart.editors.EventLogSelection;
import org.omnetpp.experimental.seqchart.editors.IEventLogSelection;
import org.omnetpp.experimental.seqchart.moduletree.ModuleTreeItem;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;
import org.omnetpp.scave.engine.IntIntMap;
import org.omnetpp.scave.engine.IntSet;
import org.omnetpp.scave.engine.IntVector;
import org.omnetpp.scave.engine.JavaFriendlyEventLogFacade;
import org.omnetpp.scave.engine.MessageEntry;
import org.omnetpp.scave.engine.XYArray;

/**
 * This is a sequence chart as a single figure.
 *
 * @author andras, levy
 */
//FIXME expressions like int x = (int)(logFacade.getEvent_i_cachedX(i) - getViewportLeft()) may overflow -- make use of XMAX!
//TODO improve mouse handling; support wheel too!
//FIXME ensure consistency of internal data structure when doing set() operations!!!!
//TODO renaming: DELIVERY->SENDING, NONDELIVERY->USAGE, isDelivery->isSending;
//               Timeline modes: Linear, Step, Compact (=nonlinear), Compact2 (CompactWithStep);
//               SortMode to OrderingMode
//TODO cf with ns2 trace file and cEnvir callbacks, and modify file format...
//TODO proper "hand" cursor - current one is not very intuitive
//TODO max number of event selection marks must be limited (e.g. max 1000)
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
	
	private EventLog eventLog; // contains the data to be displayed
	private JavaFriendlyEventLogFacade logFacade; // helpful facade on eventlog
	
	private double pixelsPerTimelineUnit = -1;
	private boolean antiAlias = true;  // antialiasing -- this gets turned on/off automatically
	private boolean showArrowHeads = true; // whether arrow heads are drawn or not
	private int axisOffset = 50;  // y coord of first axis
	private int axisSpacing = -1; // y distance between two axes

	private boolean showMessageNames;
	private boolean showNonDeliveryMessages; // show or hide non-delivery message arrows
	private boolean showEventNumbers;
	private TimelineMode timelineMode = TimelineMode.NON_LINEAR; // specifies timeline x coordinate transformation, see enum
	private TimelineSortMode timelineSortMode = TimelineSortMode.MODULE_ID; // specifies the ordering mode of timelines
	private double nonLinearFocus = 1; // parameter for non-linear timeline transformation
	
	private double viewportLeftSimulationTime; // used to restore the visible range of simulation time
	private double viewportRightSimulationTime;
	
	private DefaultInformationControl tooltipWidget; // the current tooltip (Note: SWT's Tooltip cannot be used as it wraps lines)
	
	private int dragStartX = -1, dragStartY = -1; // temporary variables for drag handling
	private List<ModuleTreeItem> axisModules; // the modules which should have an axis (they must be part of a module tree!)
	private List<AxisGraph> axisGraphs; // used to paint the axis
	private Integer[] axisModulePositions; // y order of the axis modules (in the same order as axisModules); this is a permutation of the 0..axisModule.size()-1 numbers
	private Integer[] axisModuleYs; // top y coordinates of axis bounding boxes
	private IntSet moduleIds; // calculated from axisModules: module Ids of all modules which are submodule of an axisModule (i.e. whose events appear on the chart)
	private ArrayList<BigDecimal> ticks; // a list of simulation times painted on the axis as tick marks
	private boolean invalidVirtualSize = true;
	private boolean invalidEventCoordinates = true;
	private boolean invalidTimelineCoordinates = true;
	
	private ArrayList<SelectionListener> selectionListenerList = new ArrayList<SelectionListener>(); // SWT selection listeners
	private List<EventEntry> selectionEvents = new ArrayList<EventEntry>(); // the selection
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
			double maxPPTU = Long.MAX_VALUE / 2 / eventLog.getLastEvent().getTimelineCoordinate();
			
			// set pixels per sec, and calculate tick spacing
			if (pptu <= 0)
				pptu = 1e-12;
			else if (pptu > maxPPTU)
				pptu = maxPPTU;
	
			pixelsPerTimelineUnit = pptu;
			invalidVirtualSize = true;
			invalidEventCoordinates = true;
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
		this.axisSpacing = axisSpacing > 20 ? axisSpacing : axisSpacing == -1 ? -1 : 20;
		axisModuleYs = null;
		invalidVirtualSize = true;
		invalidEventCoordinates = true;
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
		this.timelineMode = timelineMode;
		calculateTimelineCoordinates();
		restoreViewportSimulationTimeRange();
	}

	/**
	 * Returns the current timeline mode.
	 */
	public TimelineMode getTimelineMode() {
		return timelineMode;
	}
	
	/**
	 * Changes the timeline sort mode and updates figure accordingly.
	 */
	public void setTimelineSortMode(TimelineSortMode timelineSortMode) {
		this.timelineSortMode = timelineSortMode;
		axisModulePositions = null;
		axisModuleYs = null;
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
		return pixelToSimulationTime(middleX);
	}
	
	/**
	 * Returns the simulation time of the canvas's left.
	 */
	public double getViewportLeftSimulationTime() {
		return pixelToSimulationTime(0);
	}
	
	/**
	 * Returns the simulation time of the canvas's right.
	 */
	public double getViewportRightSimulationTime() {
		return pixelToSimulationTime(getWidth());
	}
	
	/**
	 * Scroll the canvas so to make start and end simulation times visible.
	 */
	public void gotoSimulationTimeRange(double startSimulationTime, double endSimulationTime) {
		if (!Double.isNaN(endSimulationTime) && startSimulationTime != endSimulationTime) {
			double timelineUnitDelta = simulationTimeToTimelineCoordinate(endSimulationTime) - simulationTimeToTimelineCoordinate(startSimulationTime);
			
			if (timelineUnitDelta > 0)
				setPixelsPerTimelineUnit(getWidth() / timelineUnitDelta);
		}

		scrollHorizontalTo(getViewportLeft() + simulationTimeToPixel(startSimulationTime));

		invalidVirtualSize = true;
		invalidEventCoordinates = true;
		clearCanvasCacheAndRedraw();
	}
	
	/**
	 * Scroll the canvas so to make start and end simulation times visible, but leave some pixels free on both sides.
	 */
	public void gotoSimulationTimeRange(double startSimulationTime, double endSimulationTime, int pixelInset) {
		if (pixelInset > 0) {
			// NOTE: we can't go directly there, so here is an approximation
			for (int i = 0; i < 3; i++) {
				double newStartSimulationTime = pixelToSimulationTime(simulationTimeToPixel(startSimulationTime) - pixelInset);
				double newEndSimulationTime = pixelToSimulationTime(simulationTimeToPixel(endSimulationTime) + pixelInset);
	
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
		double xDouble = simulationTimeToTimelineCoordinate(time) * pixelsPerTimelineUnit;
		long x = xDouble < 0 ? 0 : xDouble>Long.MAX_VALUE ? Long.MAX_VALUE : (long)xDouble;
		scrollHorizontalTo(x - getWidth()/2);
		redraw();
	}
	
	/**
	 * Scroll the canvas to make the event visible.
	 */
	public void gotoEvent(EventEntry e) {
		scrollVerticalTo(e.getCachedY() - getClientArea().height / 2);
		gotoSimulationTimeWithCenter(e.getSimulationTime());
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
	public void setParameters(EventLog eventLog, ArrayList<ModuleTreeItem> axisModules, ArrayList<XYArray> axisVectors) {
		boolean firstTime = (this.eventLog == null);
		
		if (!firstTime)
			saveViewportSimulationTimeRange();

		setEventLog(eventLog);
		setAxisVectors(axisVectors);
		setAxisModules(axisModules);
		
		if (!firstTime) {
			calculateTimelineCoordinates();
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
		calculateEventCoordinates();
		clearCanvasCacheAndRedraw();
		gotoSimulationTimeWithCenter(time);
	}

	/**
	 * Zoom to the given rectangle, given by pixel coordinates relative to the
	 * top-left corner of the canvas.
	 */
	public void zoomToRectangle(Rectangle r) {
		double timelineCoordinate = pixelToTimelineCoordinate(r.x);
		double timelineUnitDelta = pixelToTimelineCoordinate(r.right()) - timelineCoordinate;
		setPixelsPerTimelineUnit(getWidth() / timelineUnitDelta);
		calculateVirtualSize();
		calculateEventCoordinates();
		clearCanvasCacheAndRedraw();
		scrollHorizontalTo(timelineCoordinateToVirtualPixel(timelineCoordinate));
	}
	
	/**
	 * The event log (data) to be displayed in the chart
	 */
	public EventLog getEventLog() {
		return eventLog;
	}

	/**
	 * Set the event log to be displayed in the chart
	 */
	private void setEventLog(EventLog eventLog) {
		this.eventLog = eventLog;
		logFacade = new JavaFriendlyEventLogFacade(eventLog);
		axisModules = null;
		axisModulePositions = null;
		axisModuleYs = null;
		moduleIds = null;
		invalidVirtualSize = true;
		invalidEventCoordinates = true;
		invalidTimelineCoordinates = true;
		selectionEvents.clear();
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
	}
	
	/**
	 * Calculates Y coordinates of axis sorting by module ids.
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
	 * Calculates Y coordinates of axis sorting by module names.
	 */
	private void sortTimelinesByModuleName() {
		Integer[] axisModulesIndexes = new Integer[axisModules.size()];
		
		for (int i = 0; i < axisModulesIndexes.length; i++)
			axisModulesIndexes[i] = i;
		
		java.util.Arrays.sort(axisModulesIndexes, new java.util.Comparator<Integer>() {
				public int compare(Integer o1, Integer o2) {
					return axisModules.get(o1).getModuleFullPath().compareTo(axisModules.get(o2).getModuleFullPath());
				}
			});
		
		for (int i = 0; i < axisModulesIndexes.length; i++)
			axisModulePositions[axisModulesIndexes[i]] = i;
	}

	/**
	 * Sorts axis by minimizing message arrows crossing timelines.
	 * A message arrow costs as much as many axis it crosses. Uses simulated annealing.
	 */
	private void sortTimelinesByMinimizingCost(IntVector axisMatrix) {
		int cycleCount = 0;
		int noMoveCount = 0;
		int noRandomMoveCount = 0;
		int numberOfAxis = axisModules.size();
		int[] axisPositions = new int[numberOfAxis]; // actual positions of axis to be returned
		int[] candidateAxisPositions = new int[numberOfAxis]; // new positions of axis to be set (if better)
		int[] bestAxisPositions = new int[numberOfAxis]; // best positions choosen from a set of candidates
		int[] possibleNewPositionsForSelectedAxis = new int[numberOfAxis]; // a list of possible new positions for a selected axis
		Random r = new Random(0);
		double temperature = 5.0;

		// set initial axis positions 
		sortTimelinesByModuleName();
		for (int i = 0; i < numberOfAxis; i++)
			axisPositions[i] = axisModulePositions[i];
		
		while (cycleCount < 100 && (noMoveCount < numberOfAxis || noRandomMoveCount < numberOfAxis))
		{
			cycleCount++;
			
			// randomly choose an axis which we move to the best place (there are numberOfAxis possibilities)
			//int selectedAxisIndex = r.nextInt(numberOfAxis);
			int selectedAxisIndex = cycleCount % numberOfAxis;
			ModuleTreeItem selectedAxisModule = axisModules.get(selectedAxisIndex);
			int bestPositionOfSelectedAxis = -1;
			int costOfBestPositions = Integer.MAX_VALUE;
			
			// find out possible new positions for selected axis
			for (int newPositionCandidate = 0; newPositionCandidate < numberOfAxis; newPositionCandidate++) {
				possibleNewPositionsForSelectedAxis[newPositionCandidate] = newPositionCandidate;

				// do not allow to move axis to a place where none of its neighbour axis have the same
				// parent module in hierarhical mode
				if (timelineSortMode == TimelineSortMode.MINIMIZE_CROSSINGS_HIERARCHICALLY)
				{
					ModuleTreeItem previousAxisModule = null;
					ModuleTreeItem nextAxisModule = null;
					
					// find axis module that would be right before the selected axis module at new position
					if (newPositionCandidate > 0)
						for (int i = 0; i < numberOfAxis; i++)
							if (axisPositions[i] == newPositionCandidate - 1) {
								previousAxisModule = axisModules.get(i);
								break;
							}

					// find axis module that would be right after the selected axis module at new position
					if (newPositionCandidate < numberOfAxis - 1)
						for (int i = 0; i < numberOfAxis; i++)
							if (axisPositions[i] == newPositionCandidate) {
								nextAxisModule = axisModules.get(i);
								break;
							}

					if ((previousAxisModule == null || previousAxisModule.getParentModule() != selectedAxisModule.getParentModule()) &&
					    (nextAxisModule == null || nextAxisModule.getParentModule() != selectedAxisModule.getParentModule()))
						possibleNewPositionsForSelectedAxis[newPositionCandidate] = -1;
				}
			}

			// assume moving axis at index to position i while keeping the order of others and calculate cost
			for (int newPositionOfSelectedAxis : possibleNewPositionsForSelectedAxis) {
				if (newPositionOfSelectedAxis == -1)
					continue;

				int cost = 0;
				
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

				// sum up cost of messages to other axis
				for (int i = 0; i < numberOfAxis; i++)
					for (int j = 0; j < numberOfAxis; j++)
						cost += Math.abs(candidateAxisPositions[i] - candidateAxisPositions[j]) *
								(axisMatrix.get(numberOfAxis * i + j) +
								 axisMatrix.get(numberOfAxis * j + i));
				
				// find minimum cost
				if (cost < costOfBestPositions) {
					costOfBestPositions = cost;
					bestPositionOfSelectedAxis = newPositionOfSelectedAxis;
					System.arraycopy(candidateAxisPositions, 0, bestAxisPositions, 0, numberOfAxis);
				}
			}
			
			// move selected axis into best position if applicable
			if (bestPositionOfSelectedAxis != -1 && selectedAxisIndex != bestPositionOfSelectedAxis) {
				System.arraycopy(bestAxisPositions, 0, axisPositions, 0, numberOfAxis);
				noMoveCount = 0;
			}
			else
				noMoveCount++;

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
			temperature *= 0.9;
		}

		for (int i = 0; i < numberOfAxis; i++)
			axisModulePositions[i] = axisPositions[i];
	}
	
	/**
	 * Sorts axis modules depending on timelineSortMode.
	 */
	private void calculateAxisPositions() {
		axisModulePositions = new Integer[axisModules.size()];

		switch (timelineSortMode) {
			case MODULE_ID:
				sortTimelinesByModuleId();
				break;
			case MANUAL:
				break;
			case MODULE_NAME:
				sortTimelinesByModuleName();
				break;
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

				sortTimelinesByMinimizingCost(eventLog.buildMessageCountGraph(moduleIdToAxisIdMap));
				break;
		}
	}

	/**
	 * Calculates top y coordinates of axis bounding boxes based on height returned
	 * by each axis.
	 */
	private void calculateAxisYs() {
		axisModuleYs = new Integer[axisModules.size()];
		
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

		setAxisSpacing((getHeight() - axisOffset * 2 - dy) / (axisModules.size() - 1));
	}
	
	/**
	 * Calculates (x,y) coordinates for all events, based on axes settings and timeline coordinates
	 */
	private void calculateEventCoordinates() {
		// different y for each selected module
		final HashMap<Integer, Integer> moduleIdToAxisYMap = new HashMap<Integer, Integer>();
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
		
        long startMillis = System.currentTimeMillis();
        for (int i=0; i<logFacade.getNumEvents(); i++) {
			long x = Math.round(logFacade.getEvent_i_timelineCoordinate(i) * pixelsPerTimelineUnit);
			long y = moduleIdToAxisYMap.get(logFacade.getEvent_i_module_moduleId(i));
			logFacade.setEvent_cachedX(i, x);
			logFacade.setEvent_cachedY(i, y);
        }
        System.out.println("calculateEventCoordinates: "+(System.currentTimeMillis()-startMillis)+"ms");
        invalidEventCoordinates = false;
	}
	
	/**
	 * Adds an SWT selection listener which gets notified when the widget
	 * is clicked or double-clicked.
	 */
	public void addSelectionListener (SelectionListener listener) {
		selectionListenerList.add(listener);
	}

	/**
	 * Removes the given SWT selection listener.
	 */
	public void removeSelectionListener (SelectionListener listener) {
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
		int numEvents = eventLog.getNumEvents();

		// adjust pixelsPerTimelineUnit if it's way out of the range that makes sense
		if (numEvents >= 2) {
			double tStart = eventLog.getFirstEvent().getTimelineCoordinate();
			double tEnd = eventLog.getEvent(numEvents-1).getTimelineCoordinate();
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
		int numEvents = eventLog.getNumEvents();
		if (pixelsPerTimelineUnit == -1 && numEvents > 50)
			// initial value shows the first 50 events
			pixelsPerTimelineUnit = getWidth() / eventLog.getEvent(50).getTimelineCoordinate();
		else
			setPixelsPerTimelineUnit(suggestPixelsPerTimelineUnit());
	}

	/**
	 * Calculates virtual size of canvas based on the last event's timeline coordinate.
	 */
	private void calculateVirtualSize() {
		EventEntry lastEvent = eventLog.getLastEvent();
		long width = lastEvent==null ? 0 : (long)(lastEvent.getTimelineCoordinate() * getPixelsPerTimelineUnit()) + 3; // event mark should fit in
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
		if (invalidTimelineCoordinates)
			calculateTimelineCoordinates();
		
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
		
		if (invalidEventCoordinates)
			calculateEventCoordinates();
	}
	
	@Override
	protected void beforePaint() {
		calculateStuff();
		calculateTicks();
	}
	
	@Override
	protected void paintCachableLayer(Graphics graphics) {
		paintFigure(graphics);
	}

	@Override
	protected void paintNoncachableLayer(Graphics graphics) {
		paintAxisLabels(graphics);
        paintGutters(graphics);
        paintMouseTick(graphics);
        paintEventSelectionMarks(graphics);
	}

	/**
	 * Utility function to determine event range we need to paint. 
	 * Returns an array of size 2, or null if the eventLog is empty.
	 */
	protected int[] getFirstLastEventIndicesInRange(int x1, int x2) {
		if (eventLog.getNumEvents()==0)
			return null;
		
		double tleft = pixelToTimelineCoordinate(x1);
		double tright = pixelToTimelineCoordinate(x2);
		EventEntry startEvent = eventLog.getLastEventBeforeByTimelineCoordinate(tleft);
		if (startEvent==null)
			startEvent = eventLog.getFirstEvent();
		EventEntry endEvent = eventLog.getFirstEventAfterByTimelineCoordinate(tright);
		if (endEvent==null)
			endEvent = eventLog.getLastEvent();
		int startEventIndex = (startEvent!=null) ? eventLog.findEvent(startEvent) : 0;
		int endEventIndex = (endEvent!=null) ? eventLog.findEvent(endEvent) : eventLog.getNumEvents(); 
		return new int[] {startEventIndex, endEventIndex};
	}	

	protected void paintFigure(Graphics graphics) {
		if (eventLog!=null && eventLog.getNumEvents()>0) {
			long startMillis = System.currentTimeMillis();

			Rectangle clip = graphics.getClip(new Rectangle());

			graphics.setAntialias(antiAlias ? SWT.ON : SWT.OFF);
			graphics.setTextAntialias(SWT.ON);

			double startSimulationTime = pixelToSimulationTime(clip.x);
			double endSimulationTime = pixelToSimulationTime(clip.right());
			
			for (int i=0; i<axisModules.size(); i++) {
				int y = (int)(axisModuleYs[i] - getViewportTop());
				AxisGraph axisGraph = axisGraphs.get(i);
				int dy = y;
				graphics.translate(0, dy);
				axisGraph.paintAxis(graphics, startSimulationTime, endSimulationTime);
				graphics.translate(0, -dy);
			}

			int extraClipping = (showMessageNames || showEventNumbers) ? 100 : 10;
			int[] eventIndexRange = getFirstLastEventIndicesInRange(clip.x - extraClipping, clip.right() + extraClipping);
			int startEventIndex = eventIndexRange[0];
			int endEventIndex = eventIndexRange[1];
	        //System.out.println("redrawing events (index) from: " + startEventIndex + " to: " + endEventIndex);
			int startEventNumber = logFacade.getEvent_i_eventNumber(startEventIndex);
			int endEventNumber = logFacade.getEvent_i_eventNumber(endEventIndex);
			
	        // paint arrows
	        IntVector msgIndices = eventLog.getMessagesIntersecting(startEventNumber, endEventNumber, moduleIds, showNonDeliveryMessages); 
	        //System.out.println(""+msgIndices.size()+" msgs to draw");
	        VLineBuffer vlineBuffer = new VLineBuffer();
	        for (int i=0; i<msgIndices.size(); i++) {
	        	int pos = msgIndices.get(i);
	            drawMessageArrow(graphics, pos, vlineBuffer);
	        }
	        msgIndices.delete();
	        //System.out.println("draw msgs: "+(System.currentTimeMillis()-startMillis)+"ms");
	       
			// paint events
	        graphics.setForegroundColor(EVENT_FG_COLOR);
	        HashMap<Integer,Integer> axisYtoLastX = new HashMap<Integer, Integer>();
	        for (int i=startEventIndex; i<=endEventIndex; i++) {
				int x = (int)(logFacade.getEvent_i_cachedX(i) - getViewportLeft());
				int y = (int)(logFacade.getEvent_i_cachedY(i) - getViewportTop());

				// performance optimization: don't paint event if there's one already drawn exactly there
				if (!Integer.valueOf(x).equals(axisYtoLastX.get(y))) {
					axisYtoLastX.put(y,x);
					
					graphics.setBackgroundColor(EVENT_FG_COLOR);
					graphics.fillOval(x-2, y-3, 5, 7);
	
					if (showEventNumbers) {
						graphics.setBackgroundColor(EVENT_BG_COLOR);
						graphics.fillText("#"+logFacade.getEvent_i_eventNumber(i), x + 3, y + 3);
					}
				}
	        }           	
	
	        long repaintMillis = System.currentTimeMillis()-startMillis;
	        System.out.println("redraw(): "+repaintMillis+"ms");

	        // turn on/off anti-alias 
	        if (antiAlias && repaintMillis > ANTIALIAS_TURN_OFF_AT_MSEC)
	        	antiAlias = false;
	        else if (!antiAlias && repaintMillis < ANTIALIAS_TURN_ON_AT_MSEC)
	        	antiAlias = true;
		}
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
			BigDecimal t = new BigDecimal(pixelToSimulationTime(p.x));
			paintTick(graphics, calculateTick(t, 1));
		}
	}
	
	private void paintTick(Graphics graphics, BigDecimal simulationTime) {
		int x = simulationTimeToPixel(simulationTime.doubleValue());
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
		int[] eventIndexRange = getFirstLastEventIndicesInRange(0 - EVENT_SEL_RADIUS, getClientArea().width + EVENT_SEL_RADIUS);
		int startEventIndex = eventIndexRange[0];
		int endEventIndex = eventIndexRange[1];
		int startEventNumber = logFacade.getEvent_i_eventNumber(startEventIndex);
		int endEventNumber = logFacade.getEvent_i_eventNumber(endEventIndex);
		
		// paint event selection marks
		if (selectionEvents != null) {
			graphics.setLineStyle(SWT.LINE_SOLID);
		    graphics.setForegroundColor(EVENT_SEL_COLOR);
			for (EventEntry sel : selectionEvents) {
		    	if (startEventNumber<=sel.getEventNumber() && sel.getEventNumber()<=endEventNumber)
		    	{
		    		int x = (int)(sel.getCachedX()-getViewportLeft());
		    		int y = (int)(sel.getCachedY()-getViewportTop());
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
			for (int i=0; i<axisModules.size(); i++) {
				ModuleTreeItem treeItem = axisModules.get(i);
				int y = (int)(axisModuleYs[i] - getViewportTop());
				String label = treeItem.getModuleFullPath();
				graphics.drawText(label, 5, y - AXISLABEL_DISTANCE);
			}
		}
	}

	private void drawMessageArrow(Graphics graphics, int pos, VLineBuffer vlineBuffer) {
        int x1 = (int)(logFacade.getMessage_source_cachedX(pos) - getViewportLeft());
        int y1 = (int)(logFacade.getMessage_source_cachedY(pos) - getViewportTop());
        int x2 = (int)(logFacade.getMessage_target_cachedX(pos) - getViewportLeft());
        int y2 = (int)(logFacade.getMessage_target_cachedY(pos) - getViewportTop());
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
		if (showMessageNames)
			arrowLabel = logFacade.getMessage_messageName(pos);

		// line color and style depends on message type
		boolean isDelivery = logFacade.getMessage_isDelivery(pos);
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
		
		if (timelineMode == TimelineMode.LINEAR) {
			// puts ticks to constant distance from each other measured in timeline units
			double tleft = pixelToSimulationTime(rect.x);
			double tright = pixelToSimulationTime(rect.x + rect.width);
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
				double simulationTime = timelineCoordinateToSimulationTime(virtualPixelToTimelineCoordinate(t));
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
		int x = simulationTimeToPixel(simulationTime.doubleValue());
		// defines the range of valid simulation times for the tick
		BigDecimal tMin = new BigDecimal(pixelToSimulationTime(x - halfTickRange));
		BigDecimal tMax = new BigDecimal(pixelToSimulationTime(x + halfTickRange));
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
	 * Calculates timeline coordinates for all events. It might be a non-linear transformation
	 * of simulation time, event number, etc.
	 */
	private void calculateTimelineCoordinates() {
		double previousSimulationTime = 0;
		double previousTimelineCoordinate = 0;
		int size = logFacade.getNumEvents();
		nonLinearFocus = logFacade.getEvent_i_simulationTime(size - 1) / size / 10;

		for (int i=0; i<size; i++) {
        	double simulationTime = logFacade.getEvent_i_simulationTime(i);

        	switch (timelineMode)
        	{
	        	case LINEAR:
	        		logFacade.setEvent_i_timelineCoordinate(i, simulationTime);
	        		break;
	        	case STEP:
	        		logFacade.setEvent_i_timelineCoordinate(i, i);
	        		break;
	        	case NON_LINEAR:
	        		double timelineCoordinate = previousTimelineCoordinate + Math.atan((simulationTime - previousSimulationTime) / nonLinearFocus) / Math.PI * 2;
	        		logFacade.setEvent_i_timelineCoordinate(i, timelineCoordinate);
	        		previousTimelineCoordinate = timelineCoordinate;
	        		break;
        	}
        	
        	previousSimulationTime = simulationTime;
    	}

		invalidTimelineCoordinates = false;
	}

	/**
	 * Translates from simulation time to timeline coordinate.
	 */
	public double simulationTimeToTimelineCoordinate(double simulationTime) {		
    	switch (timelineMode)
    	{
        	case LINEAR:
        		return simulationTime;
        	case STEP:
        	case NON_LINEAR:
        		int pos = eventLog.getLastEventPositionBefore(simulationTime);
        		double eventSimulationTime;
        		double eventTimelineCoordinate;
        		
        		// if before the first event
        		if (pos == -1) {
        			eventSimulationTime = 0;
        			eventTimelineCoordinate = 0;
        		}
        		else {
        			eventSimulationTime = logFacade.getEvent_i_simulationTime(pos);
        			eventTimelineCoordinate = logFacade.getEvent_i_timelineCoordinate(pos);
    			}

        		// after the last event simulationTime and timelineCoordinate are proportional
        		if (pos == eventLog.getNumEvents() - 1)
        			return eventTimelineCoordinate + simulationTime - eventSimulationTime;

    			// linear approximation between two enclosing events
        		double simulationTimeDelta = logFacade.getEvent_i_simulationTime(pos + 1) - eventSimulationTime;
        		double timelineCoordinateDelta = logFacade.getEvent_i_timelineCoordinate(pos + 1) - eventTimelineCoordinate;
           		
        		if (simulationTimeDelta == 0) //XXX this can happen in STEP mode when pos==-1, and 1st event is at timeline zero. perhaps getLastEventPositionBeforeByTimelineCoordinate() should check "<=" not "<" ?
        			return eventTimelineCoordinate;
        		Assert.isTrue(simulationTimeDelta > 0);

        		return eventTimelineCoordinate + timelineCoordinateDelta * (simulationTime - eventSimulationTime) / simulationTimeDelta;
        	default:
        		throw new RuntimeException("Unknown timeline mode");
    	}
	}
	
	/**
	 * Translates from timeline coordinate to simulation time.
	 */
	public double timelineCoordinateToSimulationTime(double timelineCoordinate) {
    	switch (timelineMode)
    	{
        	case LINEAR:
        		return timelineCoordinate;
        	case STEP:
        	case NON_LINEAR:
        		int pos = eventLog.getLastEventPositionBeforeByTimelineCoordinate(timelineCoordinate);
        		double eventSimulationTime;
        		double eventTimelineCoordinate;
        		
           		// if before the first event
        		if (pos == -1) {
        			eventSimulationTime = 0;
        			eventTimelineCoordinate = 0;
        		}
        		else {
        			eventSimulationTime = logFacade.getEvent_i_simulationTime(pos);
        			eventTimelineCoordinate = logFacade.getEvent_i_timelineCoordinate(pos);
        		}

        		// after the last event simulationTime and timelineCoordinate are proportional
        		if (pos == eventLog.getNumEvents() - 1)
        			return eventSimulationTime + timelineCoordinate - eventTimelineCoordinate;

    			// linear approximation between two enclosing events
        		double simulationTimeDelta = logFacade.getEvent_i_simulationTime(pos + 1) - eventSimulationTime;
        		double timelineCoordinateDelta = logFacade.getEvent_i_timelineCoordinate(pos + 1) - eventTimelineCoordinate;

        		if (timelineCoordinateDelta == 0) //XXX this can happen in STEP mode when pos==-1, and 1st event is at timeline zero. perhaps getLastEventPositionBeforeByTimelineCoordinate() should check "<=" not "<" ?
        			return eventSimulationTime;
        		Assert.isTrue(timelineCoordinateDelta > 0);
        		
        		return eventSimulationTime + simulationTimeDelta * (timelineCoordinate - eventTimelineCoordinate) / timelineCoordinateDelta;
        	default:
        		throw new RuntimeException("Unknown timeline mode");
    	}
	}
	
	/**
	 * Translates from pixel x coordinate to seconds.
	 */
	public double pixelToSimulationTime(int x) {
		return timelineCoordinateToSimulationTime(pixelToTimelineCoordinate(x));
	}
	
	/**
	 * Translates from seconds to pixel x coordinate.
	 */
	public int simulationTimeToPixel(double t) {
		return (int)((long)Math.round(simulationTimeToTimelineCoordinate(t) * pixelsPerTimelineUnit) - getViewportLeft());
	}
	
	/**
	 * Translates from pixel x coordinate to timeline coordinate, using on pixelsPerTimelineUnit.
	 */
	public double pixelToTimelineCoordinate(int x) {
		return (x + getViewportLeft()) / pixelsPerTimelineUnit;
	}

	/**
	 * Translates timeline coordinate to pixel x coordinate, using on pixelsPerTimelineUnit.
	 * Extreme values get clipped to a reasonable interval (-XMAX, XMAX).
	 */
	public int timelineCoordinateToPixel(double t) {
		long x = Math.round(t * pixelsPerTimelineUnit) - getViewportLeft();
    	return (x < -XMAX) ? -XMAX : (x > XMAX) ? XMAX : (int)x;
	}

	/**
	 * Translates from virtual pixel x coordinate to timeline coordinate, using on pixelsPerTimelineUnit.
	 */
	public double virtualPixelToTimelineCoordinate(long x) {
		return x / pixelsPerTimelineUnit;
	}

	/**
	 * Translates timeline coordinate to pixel x coordinate, using on pixelsPerTimelineUnit.
	 */
	public long timelineCoordinateToVirtualPixel(double t) {
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
				ArrayList<EventEntry> tmp = new ArrayList<EventEntry>();
				collectStuffUnderMouse(me.x, me.y, tmp, null);
				if (eventListEquals(selectionEvents, tmp)) {
					fireSelection(true);
				}
				else {
					selectionEvents = tmp;
					fireSelection(true);
					fireSelectionChanged();
					redraw();
				}
			}
			public void mouseDown(MouseEvent me) {
				//XXX improve mouse handling: starting dragging should not deselect events!
				if (me.button==1) {
					ArrayList<EventEntry> tmp = new ArrayList<EventEntry>();
					if ((me.stateMask & SWT.CTRL)!=0) // CTRL key extends selection
						for (EventEntry e : selectionEvents) 
							tmp.add(e);
					collectStuffUnderMouse(me.x, me.y, tmp, null);
					if (eventListEquals(selectionEvents, tmp)) {
						fireSelection(false);
					}
					else {
						selectionEvents = tmp;
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
	private static boolean eventListEquals(List<EventEntry> a, List<EventEntry> b) {
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
		ArrayList<EventEntry> events = new ArrayList<EventEntry>();
		ArrayList<MessageEntry> msgs = new ArrayList<MessageEntry>();
		collectStuffUnderMouse(x, y, events, msgs);

		// 1) if there are events under them mouse, show them in the tooltip
		if (events.size()>0) {
			String res = "";
			int count = 0;
			for (EventEntry event : events) {
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
			for (MessageEntry msg : msgs) {
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
			double t = pixelToSimulationTime(x);
			res += String.format("t = %gs", t);
			EventEntry event = eventLog.getLastEventBefore(t);
			if (event!=null)
				res += ", just after event #"+event.getEventNumber(); 
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
	 * Returns a descriptive message for the MessageEntry to be presented to the user.
	 */
	public String getMessageText(MessageEntry msg) {
		String res = "message ("+msg.getMessageClassName()+") "+msg.getMessageName()
			+"  ("+ (msg.getIsDelivery() ? "sending" : "usage")  //TODO also: "selfmsg"
			+", #"+msg.getSource().getEventNumber()+" -> #"+msg.getTarget().getEventNumber()+")";
		return res;
	}

	/**
	 * Returns a descriptive message for the EventEntry to be presented to the user.
	 */
	public String getEventText(EventEntry event) {
		String res = "event #"+event.getEventNumber()+" at t="+event.getSimulationTime()
			+"  at ("+event.getCause().getModule().getModuleClassName()+")"
			+event.getCause().getModule().getModuleFullPath()
			+" (id="+event.getCause().getModule().getModuleId()+")"
			+"  message ("+event.getCause().getMessageClassName()+")"
			+event.getCause().getMessageName();
		return res;
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
	 * Determines if there are any events (EventEntry) or messages (MessageEntry)
	 * at the given mouse coordinates, and returns them in the Lists passed. 
	 * Coordinates are canvas coordinates (more precisely, viewport coordinates).
	 * You can call this method from mouse click, double-click or hover event 
	 * handlers. 
	 * 
	 * If you're interested only in messages or only in events, pass null in the
	 * events or msgs argument. This method does NOT clear the lists before filling them.
	 */
	public void collectStuffUnderMouse(int mouseX, int mouseY, List<EventEntry> events, List<MessageEntry> msgs) {
		if (eventLog!=null) {
			long startMillis = System.currentTimeMillis();
		
			// determine start/end event numbers
			int[] eventIndexRange = getFirstLastEventIndicesInRange(0, getClientArea().width);
			int startEventIndex = eventIndexRange[0];
			int endEventIndex = eventIndexRange[1];
			int startEventNumber = logFacade.getEvent_i_eventNumber(startEventIndex);
			int endEventNumber = logFacade.getEvent_i_eventNumber(endEventIndex);

			// check events
            if (events != null) {
            	for (int i=startEventIndex; i<=endEventIndex; i++)
   				if (eventSymbolContainsPoint(mouseX, mouseY, (int)(logFacade.getEvent_i_cachedX(i)-getViewportLeft()), (int)(logFacade.getEvent_i_cachedY(i)-getViewportTop()), MOUSE_TOLERANCE))
   					events.add(eventLog.getEvent(i));
            }

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
		}
	}

	/**
	 * Utility function, to detect whether use clicked (hovered) an event in the chart
	 */
	private boolean eventSymbolContainsPoint(int x, int y, int px, int py, int tolerance) {
		return Math.abs(x-px) <= 2+tolerance && Math.abs(y-py) <= 5+tolerance;
	}

	private boolean messageArrowContainsPoint(int pos, int px, int py, int tolerance) {
        int x1 = (int)(logFacade.getMessage_source_cachedX(pos) - getViewportLeft());
        int y1 = (int)(logFacade.getMessage_source_cachedY(pos) - getViewportTop());
        int x2 = (int)(logFacade.getMessage_target_cachedX(pos) - getViewportLeft());
        int y2 = (int)(logFacade.getMessage_target_cachedY(pos) - getViewportTop());
		//System.out.printf("checking %d %d %d %d\n", x1, x2, y1, y2);
		if (y1==y2) {
			int height = logFacade.getMessage_isDelivery(pos) ? DELIVERY_SELFARROW_HEIGHT : NONDELIVERY_SELFARROW_HEIGHT;
			return halfEllipseContainsPoint(x1, x2, y1, height, px, py, tolerance);
		}
		else {
			return lineContainsPoint(x1, y1, x2, y2, px, py, tolerance);
		}
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
		return new EventLogSelection(eventLog, selectionEvents);
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
		if (!eventListEquals(newEventLogSelection.getEvents(), selectionEvents)) {
			selectionEvents.clear();
			for (EventEntry e : newEventLogSelection.getEvents()) 
				selectionEvents.add(e);

			// go to the time of the first event selected
			if (selectionEvents.size()>0) {
				gotoEvent(selectionEvents.get(0));
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

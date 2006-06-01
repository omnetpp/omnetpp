package org.omnetpp.experimental.seqchart.editors;


import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.MouseMotionListener;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.ToolTip;
import org.omnetpp.experimental.seqchart.moduletree.ModuleTreeItem;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;
import org.omnetpp.scave.engine.JavaFriendlyEventLogFacade;
import org.omnetpp.scave.engine.MessageEntry;

/**
 * This is a sequence chart as a single figure.
 *
 * @author andras
 */
//TODO limit pixelsPerTimelineCoordinate to a range that makes sense (for the current eventLog)
//TODO draw arrowheads
//TODO instead of (in addition to) gotoSimulationTime(), we need gotoEvent() as well, which would do vertical scrolling too
//XXX Performanece note: perf log is line drawing. Coordicate calculations etc
//    take much less time (to verify, comment out body of drawMessageArrow()).
//    Solution: draw into an off-screen image, and use that during repaints!
//TODO non-delivery events should be drawn in dotted line, and maybe lower ellipse than others?
//TODO refine tooltips: if there's an event in the hover, don't print msgs in detail, only this "and 5 message arrows"
//FIXME scrollbar breaks badly when chart size exceeds ~4,000,000 pixels (this means only ~0.1s resolution ticks on an 1000s trace!!! not enough!)
//FIXME msg arrows that intersect the chart area but don't start or end there are not displayed (BUG!)
//FIXME redraw chart with antialias while user is idle? hints: new SafeRunnable(); or:
//		canvas.getDisplay().syncExec(new Runnable() {
//			public void run() { ... }
//		};

public class SeqChartFigure extends Figure implements ISelectionProvider {

	private final Color ARROW_COLOR = new Color(null, 0, 0, 0);
	private final Color EVENT_FG_COLOR = new Color(null,255,0,0);
	private final Color EVENT_BG_COLOR = new Color(null,255,255,255);
	private final Color MSG_COLOR = new Color(null,0,0,255);
	private final Color DELIVERY_MSG_COLOR = new Color(null,0,255,0);
	private final Cursor DRAGCURSOR = new Cursor(null, SWT.CURSOR_SIZEALL);
	private static final int XMAX = 10000;
	private static final int ANTIALIAS_TURN_ON_AT_MSEC = 100;
	private static final int ANTIALIAS_TURN_OFF_AT_MSEC = 300;
	private static final int MOUSE_TOLERANCE = 1;

	protected EventLog eventLog; // contains the data to be displayed
	protected JavaFriendlyEventLogFacade logFacade; // helpful facade on eventlog
	
	protected double pixelsPerTimelineCoordinate = 2;
	protected int tickScale = 2; // -1 means step=0.1
	private boolean antiAlias = true;  // antialiasing -- this gets turned on/off automatically
	private final int axisOffset = 30;  // y coord of first axis
	private final int axisSpacing = 50; // y distance between two axes
	private final int selfArrowHeight = 20; // vertical radius of ellipse for self arrows
	private final int arrowHeadLength = 10; // length of message arrow head
	private final int arrowHeadWideness = 7; // wideness of message arrow head
	private final int labelDistance = 35; // distance of timeline label from timeline
	private final int eventRadius = 10; // radius of event circle

	private boolean showNonDeliveryMessages; // show or hide non delivery message arrows
	private boolean showEventNumbers;
	private TimelineMode timelineMode = TimelineMode.LINEAR; // specifies timeline x coordinate transformation, see enum
	private double nonLinearFocus = 0.1; // parameter for non linear timeline transformation
	
	private Canvas canvas;  // our host widget (reference needed for tooltip creation)
	private ScrollPane scrollPane; // parent scrollPane
	private ToolTip swtTooltip; // the current tooltip
	
	private int dragStartX, dragStartY; // temporary variables for drag handling
	private List<ModuleTreeItem> axisModules; // the modules which should have an axis (they must be part of a module tree!)

	List<Integer> selectedModuleIds; // a list simple module ids which have timelines assigned (calculated by flattening axisModules)

	private ArrayList<SelectionListener> selectionListenerList = new ArrayList<SelectionListener>(); // SWT selection listeners

	private List<EventEntry> selectionEvents = new ArrayList<EventEntry>(); // the selection
    private ListenerList selectionChangedListeners = new ListenerList(); // list of selection change listeners (type ISelectionChangedListener).

	public enum TimelineMode {
		LINEAR,
		STEP_BY_STEP,
		NON_LINEAR
	}
	
	/**
     * Constructor.
	 * We need to know the surrounding scroll pane to be able to scroll here and there.
     */
	public SeqChartFigure(Canvas canvas, ScrollPane scrollPane) {
		this.canvas = canvas;
		this.scrollPane = scrollPane;

    	// add mouse handling
    	setUpMouseHandling();
	}

	/**
	 * Returns chart scale (number of pixels a 1-second interval maps to).
	 */
	public double getPixelsPerTimelineCoordinate() {
		return pixelsPerTimelineCoordinate;	
	}
	
	/**
	 * Set chart scale (number of pixels a 1-second interval maps to), 
	 * and adjusts the density of ticks.
	 */
	public void setPixelsPerTimelineCoordinate(double pp) {
		if (pixelsPerTimelineCoordinate == pp)
			 return; // nothing to do
		
		// set pixels per sec, and recalculate tick spacing
		if (pp <= 0)
			pp = 1e-12;
		pixelsPerTimelineCoordinate = pp;
		int labelWidthNeeded = 50; // pixels
		tickScale = (int)Math.ceil(Math.log10(labelWidthNeeded / pp));

		System.out.println("pixels per timeline coordinate: "+pixelsPerTimelineCoordinate);
	}

	/**
	 * Shows/Hides non delivery messages.
	 */
	public void setShowNonDeliveryMessages(boolean showNonDeliveryMessages) {
		this.showNonDeliveryMessages = showNonDeliveryMessages;
		repaint();
	}
	
	public void setShowEventNumbers(boolean showEventNumbers) {
		this.showEventNumbers = showEventNumbers;
		repaint();
	}
	
	public void setTimelineMode(TimelineMode timelineMode) {
		double time = currentSimulationTime();
		this.timelineMode = timelineMode;
		recalculateTimelineCoordinates();
		setPixelsPerTimelineCoordinate(suggestPixelsPerTimelineCoordinate());
		recalculatePreferredSize();
		gotoSimulationTime(time);
	}
	
	/**
	 * Returns the simulation time of the canvas's center.
	 */
	public double currentSimulationTime() {
		int middleX = scrollPane.getViewport().getBounds().width/2;
		return pixelToSimulationTime(middleX);
	}
	
	/**
	 * Scroll the canvas so as to make the simulation time visible 
	 */
	public void gotoSimulationTime(double time) {
		double xDouble = simulationTimeToTimelineCoordinate(time) * pixelsPerTimelineCoordinate;
		int x = xDouble < 0 ? 0 : xDouble>Integer.MAX_VALUE ? Integer.MAX_VALUE : (int)xDouble;
		scrollPane.scrollHorizontalTo(x - scrollPane.getViewport().getBounds().width/2);
		repaint();
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
		zoomBy(1.1 / 1.5);
	}

	/**
	 * Changes pixel per timeline coordinate by zoomFactor.
	 */
	private void zoomBy(double zoomFactor) {
		double time = currentSimulationTime();
		setPixelsPerTimelineCoordinate(getPixelsPerTimelineCoordinate() * zoomFactor);	
		recalculatePreferredSize();
		gotoSimulationTime(time);
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
	public void setEventLog(EventLog eventLog) {
		this.eventLog = eventLog;
		this.logFacade = new JavaFriendlyEventLogFacade(eventLog); 
		double time = currentSimulationTime();
		
		// transform simulation times to timeline coordinate system
		recalculateTimelineCoordinates();

		// adapt our zoom level to the current eventLog
		setPixelsPerTimelineCoordinate(suggestPixelsPerTimelineCoordinate());
		recalculatePreferredSize();
		gotoSimulationTime(time);
		
		// notify listeners
		fireSelectionChanged();
	}

	/**
	 * Sets which modules should have axes. Items in axisModules
	 * should point to elements in the moduleTree. 
	 */
	public void setAxisModules(ArrayList<ModuleTreeItem> axisModules) {
		this.axisModules = axisModules;
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
		event.display = canvas.getDisplay();
		event.widget = canvas;
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
	public double suggestPixelsPerTimelineCoordinate() {
		// adjust pixelsPerTimelineCoordinate if it's way out of the range that makes sense
		int numEvents = eventLog.getNumEvents();
		if (numEvents>=2) {
			double tStart = eventLog.getEvent(0).getTimelineCoordinate();
			double tEnd = eventLog.getEvent(numEvents-1).getTimelineCoordinate();
			double eventPerSec = numEvents / (tEnd - tStart);

			int chartWidthPixels = scrollPane.getViewport().getBounds().width;
			if (chartWidthPixels<=0) chartWidthPixels = 800;  // may be 0 on startup

			double minPixelsPerTimelineCoordinate = eventPerSec * 10;  // we want at least 10 pixel/event
			double maxPixelsPerTimelineCoordinate = eventPerSec * (chartWidthPixels/10);  // we want at least 10 events on the chart

			if (pixelsPerTimelineCoordinate < minPixelsPerTimelineCoordinate)
				return minPixelsPerTimelineCoordinate;
			else if (pixelsPerTimelineCoordinate > maxPixelsPerTimelineCoordinate)
				return maxPixelsPerTimelineCoordinate;
		}
		return pixelsPerTimelineCoordinate; // the current setting is fine
	}

	private void recalculatePreferredSize() {
		EventEntry lastEvent = eventLog.getEvent(eventLog.getNumEvents()-1);
		int width = lastEvent==null ? 0 : (int)(lastEvent.getTimelineCoordinate() * getPixelsPerTimelineCoordinate()) + 3;
		width = Math.max(width, 600); // at least half a screen
		int height = eventLog.getNumModules()*axisSpacing;
		setPreferredSize(width, height);
	}

	/**
	 * Overridden org.eclipse.draw2d.Figure method.
	 */
	@Override
	protected void paintFigure(Graphics graphics) {
		if (eventLog!=null) {
			long startMillis = System.currentTimeMillis();
			
			// paint axes
			final HashMap<Integer,Integer> moduleIdToAxisYMap = new HashMap<Integer, Integer>();

			// different y for each selected module
			for (int i=0; i<axisModules.size(); i++) {
				ModuleTreeItem treeItem = axisModules.get(i);
				final int y = getBounds().y + axisOffset + i*axisSpacing;
				// propagate y to all submodules recursively
				treeItem.visitLeaves(new ModuleTreeItem.IModuleTreeItemVisitor() {
					public void visit(ModuleTreeItem treeItem) {
						moduleIdToAxisYMap.put(treeItem.getModuleId(), y);
					}
				});
				drawLinearAxis(graphics, y, treeItem.getModuleFullPath());
			}

			graphics.setAntialias(antiAlias ? SWT.ON : SWT.OFF);
			graphics.setTextAntialias(SWT.ON);
			
			// paint events
			Rectangle rect = graphics.getClip(new Rectangle());
			double tleft = pixelToSimulationTime(rect.x);
			double tright = pixelToSimulationTime(rect.right());
			EventEntry startEvent = eventLog.getLastEventBefore(tleft);
			EventEntry endEvent = eventLog.getFirstEventAfter(tright);
			int startEventIndex = (startEvent!=null) ? eventLog.findEvent(startEvent) : 0;
			int endEventIndex = (endEvent!=null) ? eventLog.findEvent(endEvent) : eventLog.getNumEvents(); 

			int startEventNumber = (startEvent!=null) ? startEvent.getEventNumber() : 0;
			int endEventNumber = (endEvent!=null) ? endEvent.getEventNumber() : Integer.MAX_VALUE;
            
            // calculate event coordinates (we'll paint them after the arrows)
            for (int i=startEventIndex; i<endEventIndex; i++) {
   				int x = timelineCoordinateToPixel(logFacade.getEvent_i_timelineCoordinate(i));
   				int y = moduleIdToAxisYMap.get(logFacade.getEvent_i_module_moduleId(i));
   				logFacade.setEvent_cachedX(i, x);
   				logFacade.setEvent_cachedY(i, y);
            }

            // paint arrows
            for (int i=startEventIndex; i<endEventIndex; i++) {
   				int eventX = logFacade.getEvent_i_cachedX(i);
   				int eventY = logFacade.getEvent_i_cachedY(i);

   				// paint forward arrows for this event
            	int numConsequences = logFacade.getEvent_i_numConsequences(i);
            	for (int k=0; k<numConsequences; k++) {
            		boolean isDelivery = logFacade.getEvent_i_consequences_k_isDelivery(i,k);
            		if ((isDelivery || showNonDeliveryMessages) &&
            			logFacade.getEvent_i_consequences_k_hasTarget(i, k) &&
            			logFacade.getEvent_i_consequences_k_target_isInFilteredSubset(i, k, eventLog))
            		{
            			// we have to calculate target event's coords if it's beyond endEventNumber
            			graphics.setForegroundColor(isDelivery ? DELIVERY_MSG_COLOR : MSG_COLOR); 
                		if (logFacade.getEvent_i_consequences_k_target_eventNumber(i, k) < endEventNumber) {
                			// both source and target event in the visible chart area, and already painted
                			drawMessageArrow(graphics,
                					eventX,
                					eventY,
                					logFacade.getEvent_i_consequences_k_target_cachedX(i, k),
                					logFacade.getEvent_i_consequences_k_target_cachedY(i, k));
                		}
                		else {
                			// target is outside the repaint region (on the far right)
               				double targetXDouble = timelineCoordinateToPixelDouble(logFacade.getEvent_i_consequences_k_target_timelineCoordinatea(i, k));
               				int targetX = targetXDouble > XMAX ? XMAX : (int)targetXDouble;
               				int targetY = moduleIdToAxisYMap.get(logFacade.getEvent_i_consequences_k_target_cause_module_moduleId(i, k));
               				logFacade.setEvent_i_consequences_k_target_cachedX(i, k, targetX);
               				logFacade.setEvent_i_consequences_k_target_cachedY(i, k, targetY);

               				drawMessageArrow(graphics, eventX, eventY, targetX, targetY);
                		}
            		}
            	}

            	// paint backward arrows that we didn't paint as forward arrows
            	int numCauses = logFacade.getEvent_i_numCauses(i);
            	for (int k=0; k<numCauses; k++) {
            		boolean isDelivery = logFacade.getEvent_i_causes_k_isDelivery(i,k);
            		if (logFacade.getEvent_i_causes_k_source_eventNumber(i, k) < startEventNumber) {
            			// source event is outside the repaint region (on the far left)
                		if ((isDelivery || showNonDeliveryMessages) &&
                			logFacade.getEvent_i_causes_k_hasSource(i, k) &&
                    		logFacade.getEvent_i_causes_k_source_isInFilteredSubset(i, k, eventLog))
                		{
                			double srcXDouble = timelineCoordinateToPixelDouble(logFacade.getEvent_i_causes_k_source_timelineCoordinate(i, k));
               				int srcY = moduleIdToAxisYMap.get(logFacade.getEvent_i_causes_k_source_cause_module_moduleId(i, k));
               				int srcX = srcXDouble < -XMAX ? -XMAX : (int)srcXDouble;
               				logFacade.setEvent_i_causes_k_source_cachedX(i, k, srcX);
               				logFacade.setEvent_i_causes_k_source_cachedY(i, k, srcY);
               				graphics.setForegroundColor(isDelivery ? DELIVERY_MSG_COLOR : MSG_COLOR); 
               				drawMessageArrow(graphics, srcX, srcY, eventX, eventY);
                		}
            		}
            	}
            }

            System.out.println("draw msgs: "+(System.currentTimeMillis()-startMillis)+"ms");
           
			// paint events
            graphics.setForegroundColor(EVENT_FG_COLOR); 
            graphics.setBackgroundColor(EVENT_BG_COLOR);
            for (int i=startEventIndex; i<endEventIndex; i++) {
   				int x = logFacade.getEvent_i_cachedX(i);
   				int y = logFacade.getEvent_i_cachedY(i);

   	            graphics.setBackgroundColor(EVENT_FG_COLOR);
   				graphics.fillOval(x-2, y-3, 5, 7);

   				if (showEventNumbers) {
	   	            graphics.setBackgroundColor(EVENT_BG_COLOR);
	   	            graphics.fillText("#"+i, x-10, y - labelDistance);
   				}
            }           	

            // paint event selection marks
            if (selectionEvents != null) {
            	for (EventEntry sel : selectionEvents) {
                	if (startEventNumber<=sel.getEventNumber() &&
                		sel.getEventNumber()<endEventNumber)
                	{
	            		int x = sel.getCachedX();
	            		int y = sel.getCachedY();
	            		graphics.drawOval(x - eventRadius, y - eventRadius, eventRadius * 2 + 1, eventRadius * 2 + 1);
                	}
            	}
            }
            
            // turn on/off anti-alias 
            long repaintMillis = System.currentTimeMillis()-startMillis;
            System.out.println("repaint(): "+repaintMillis+"ms");
            if (antiAlias && repaintMillis > ANTIALIAS_TURN_OFF_AT_MSEC)
            	antiAlias = false;
            else if (!antiAlias && repaintMillis < ANTIALIAS_TURN_ON_AT_MSEC)
            	antiAlias = true;
            //XXX also: turn it off also during painting if it's going to take too long 
		}
	}
	
	private void drawMessageArrow(Graphics graphics, int x1, int y1, int x2, int y2) {
		Rectangle.SINGLETON.setLocation(x2 - selfArrowHeight, y2 - selfArrowHeight);
		Rectangle.SINGLETON.setSize(selfArrowHeight * 2, selfArrowHeight * 2);
		boolean arrowHeadInClipping = !graphics.getClip(Rectangle.SINGLETON).isEmpty();
		
		if (y1==y2) {
			if (x1==x2) {
				// draw vertical line (as zero-width half ellipse) 
				graphics.drawLine(x1, y1, x1, y1 - selfArrowHeight);

				if (arrowHeadInClipping)
					drawArrowHead(graphics, x1, y1, 0, 1);
			}
			else {
				// draw half ellipse
				Rectangle.SINGLETON.setLocation(x1, y1 - selfArrowHeight);
				Rectangle.SINGLETON.setSize(x2-x1, selfArrowHeight * 2);
				graphics.drawArc(Rectangle.SINGLETON, 0, 180);
				
				if (arrowHeadInClipping) {
					// intersection of the ellipse and a circle with the arrow length centered at the end point
					// origin is in the center of the ellipse
					// mupad: solve([x^2/a^2+(r^2-(x-a)^2)/b^2=1],x,IgnoreSpecialCases)
					double a = Rectangle.SINGLETON.width / 2;
					double b = Rectangle.SINGLETON.height / 2;
					double a2 = a * a;
					double b2 = b * b;
					double r = arrowHeadLength;
					double r2 = r *r;
					double x = a * (-Math.sqrt(a2 * r2 + b2 * b2 - b2 * r2) + a2) / (a2 - b2);
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
			}
		} else {
			//XXX some attempt do do manual clipping...
			//graphics.clipRect(Rectangle.SINGLETON);
			//if (Rectangle.SINGLETON.y > y1 && Rectangle.SINGLETON.y > y2)
			//	return;
			//if (Rectangle.SINGLETON.bottom() < y1 && Rectangle.SINGLETON.bottom() < y2)
			//	return;
			graphics.drawLine(x1, y1, x2, y2);
			
			if (arrowHeadInClipping)
				drawArrowHead(graphics, x2, y2, x2 - x1, y2 - y1);
		}
	}
	
	private void drawArrowHead(Graphics graphics, int x, int y, double dx, double dy)
	{
		double n = Math.sqrt(dx * dx + dy * dy);
		double dwx = -dy / n * arrowHeadWideness / 2;
		double dwy = dx / n * arrowHeadWideness / 2;
		double xt = x - dx * arrowHeadLength / n;
		double yt = y - dy * arrowHeadLength / n;
		int x1 = (int)Math.round(xt - dwx);
		int y1 = (int)Math.round(yt - dwy);
		int x2 = (int)Math.round(xt + dwx);
		int y2 = (int)Math.round(yt + dwy);

		graphics.setBackgroundColor(ARROW_COLOR);
		graphics.fillPolygon(new int[] {x, y, x1, y1, x2, y2});
	}

	/**
	 * Draws the axis, according to the current pixelsPerTimelineCoordinate and tickInterval
	 * settings.
	 */
	private void drawLinearAxis(Graphics graphics, int y, String label) {
		Rectangle rect = graphics.getClip(new Rectangle());
		Rectangle bounds = getBounds();
		rect.intersect(bounds); // although looks like Clip is already set up like this

		// draw axis label; may it should be "sticky" on the screen?
		
		graphics.drawText(label, scrollPane.getViewport().getBounds().x+5, y - labelDistance);
		
		// draw axis
		graphics.drawLine(rect.x, y, rect.right(), y);

		double tleft = pixelToSimulationTime(rect.x);
		double tright = pixelToSimulationTime(rect.right());

		// draw ticks and labels
		BigDecimal tickStart = new BigDecimal(tleft).setScale(-tickScale, RoundingMode.FLOOR);
		BigDecimal tickEnd = new BigDecimal(tright).setScale(-tickScale, RoundingMode.CEILING);
		BigDecimal tickIntvl = new BigDecimal(1).scaleByPowerOfTen(tickScale);
		//System.out.println(tickStart+" - "+tickEnd+ " step "+tickIntvl);

		for (BigDecimal t=tickStart; t.compareTo(tickEnd)<0; t = t.add(tickIntvl)) {
			int x = simulationTimeToPixel(t.doubleValue());
			graphics.drawLine(x, y-2, x, y+2);
			graphics.drawText(t.toPlainString() + "s", x, y+3);
		}
	}

	/**
	 * Calculates timeline coordinates for all events. It might be a non linear transformation
	 * of simulation time, event number, etc.
	 *
	 */
	private void recalculateTimelineCoordinates()
	{
		double previousSimulationTime = 0;
		double previousTimelineCoordinate = 0;
		int size = logFacade.getNumEvents();

		for (int i=0; i<size; i++) {
        	double simulationTime = logFacade.getEvent_i_simulationTime(i);

        	switch (timelineMode)
        	{
	        	case LINEAR:
	        		logFacade.setEvent_i_timelineCoordinate(i, simulationTime);
	        		break;
	        	case STEP_BY_STEP:
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

		nonLinearFocus = previousSimulationTime / size;
	}

	/**
	 * Translates from simulation time to timeline coordinate.
	 */
	private double simulationTimeToTimelineCoordinate(double simulationTime)
	{		
    	switch (timelineMode)
    	{
        	case LINEAR:
        		return simulationTime;
        	case STEP_BY_STEP:
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
        		return eventTimelineCoordinate + timelineCoordinateDelta * (simulationTime - eventSimulationTime) / simulationTimeDelta;
        	default:
        		throw new RuntimeException("Unknown timeline mode");
    	}
	}
	
	/**
	 * Translates from timeline coordinate to simulation time.
	 */
	private double timelineCoordinateToSimulationTime(double timelineCoordinate)
	{
    	switch (timelineMode)
    	{
        	case LINEAR:
        		return timelineCoordinate;
        	case STEP_BY_STEP:
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
        		return eventSimulationTime + simulationTimeDelta * (timelineCoordinate - eventTimelineCoordinate) / timelineCoordinateDelta;
        	default:
        		throw new RuntimeException("Unknown timeline mode");
    	}
	}
	
	/**
	 * Translates from pixel x coordinate to seconds, using on pixelsPerTimelineCoordinate.
	 */
	private double pixelToSimulationTime(int x) {
		return timelineCoordinateToSimulationTime((x-getBounds().x) / pixelsPerTimelineCoordinate);
	}
	
	/**
	 * Translates from seconds to pixel x coordinate, using on pixelsPerTimelineCoordinate.
	 */
	private int simulationTimeToPixel(double t) {
		return (int)Math.round(simulationTimeToTimelineCoordinate(t) * pixelsPerTimelineCoordinate) + getBounds().x;
	}

	/**
	 * Translates timeline coordinate to pixel x coordinate, using on pixelsPerTimelineCoordinate.
	 */
	private int timelineCoordinateToPixel(double t) {
		return (int)Math.round(t * pixelsPerTimelineCoordinate) + getBounds().x;
	}

	/**
	 * Same as timelineCoordinateToPixel(), but doesn't convert to int; to be used where "int" may overflow
	 */
	private double timelineCoordinateToPixelDouble(double t) {
		return t * pixelsPerTimelineCoordinate + getBounds().x;
	}
	
	/**
	 * Sets up default mouse handling.
	 */
	private void setUpMouseHandling() {
		// dragging ("hand" cursor) and tooltip
		addMouseListener(new MouseListener() {
			public void mouseDoubleClicked(MouseEvent me) {}
			public void mousePressed(MouseEvent me) {
				dragStartX = me.x;
				dragStartY = me.y;
				removeTooltip();
			}
			public void mouseReleased(MouseEvent me) { 
				setCursor(null); // restore cursor at end of drag
			}
    	});
		addMouseMotionListener(new MouseMotionListener.Stub() {
			public void mouseDragged(MouseEvent me) {
				// display drag cursor if not already displayed
				if (getCursor() == null) {
					setCursor(DRAGCURSOR);
				}
				// scroll by the amount moved since last drag call
				int dx = me.x - dragStartX;
				int dy = me.y - dragStartY;
				scrollPane.scrollHorizontalTo(-getBounds().x-dx);
				scrollPane.scrollVerticalTo(-getBounds().y-dy);
				dragStartX = me.x;
				dragStartY = me.y;
			}
			public void mouseHover(MouseEvent me) {
				displayTooltip(me.x, me.y);
			}
			public void mouseMoved(MouseEvent me) {
				removeTooltip();
				setCursor(null); // restore cursor at end of drag (must do it here too, because we 
								 // don't get the "released" event if user releases mouse outside the canvas)
			}
		});
		
		// selection handling
		addMouseListener(new MouseListener() {
			public void mouseDoubleClicked(MouseEvent me) {
				ArrayList<EventEntry> tmp = new ArrayList<EventEntry>();
				collectStuffUnderMouse(me.x, me.y, tmp, null);
				if (eventListEquals(selectionEvents, tmp)) {
					fireSelection(true);
				}
				else {
					selectionEvents = tmp;
					fireSelection(true);
					fireSelectionChanged();
					repaint();
				}
			}
			public void mousePressed(MouseEvent me) {
				if (me.button==1) {
					ArrayList<EventEntry> tmp = new ArrayList<EventEntry>();
					if ((me.getState() & SWT.CTRL)!=0) // CTRL key extends selection
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
						repaint();
					}
					
				}
			}
			public void mouseReleased(MouseEvent me) {}
		});
		
	}

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
			swtTooltip = new ToolTip(canvas.getShell(), SWT.NONE);
			swtTooltip.setMessage(tooltipText);
			swtTooltip.setLocation(canvas.toDisplay(x,y+20));
			swtTooltip.setVisible(true);
			swtTooltip.setAutoHide(false);
		}
	}

	protected void removeTooltip() {
		if (swtTooltip!=null) {
			swtTooltip.setVisible(false);
			swtTooltip.dispose();
			swtTooltip = null;
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

		if (events.isEmpty() && msgs.isEmpty())
			return null;
		String res = "";
		if (events.size()+msgs.size()>1) {
			if (events.size()>0 && msgs.size()>0)
				res = "Multiple hits ("+events.size()+" events, "+msgs.size()+" messages):\n";
			else if (events.size()>0)
				res = "Multiple hits ("+events.size()+" events):\n";
			else
				res = "Multiple hits ("+msgs.size()+" messages):\n";
		}

		for (EventEntry event : events) {
			res += "Event #"+event.getEventNumber()
	    		+" at t="+event.getSimulationTime()
	    		+"\n    at module ("+event.getCause().getModule().getModuleClassName()+")"
	    		+event.getCause().getModule().getModuleFullPath()
	    		+" (id="+event.getCause().getModule().getModuleId()+")"
	    		+"\n    message ("+event.getCause().getMessageClassName()+")"
	    		+event.getCause().getMessageName()+"\n";
		}

		for (MessageEntry msg : msgs) {
			res += "Message ("+msg.getMessageClassName()+") "
				+msg.getMessageName()+"\n";
		}
		//XXX truncate to, say, 20 lines (chop off the rest, and add "...")
		return res;
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
		
			JavaFriendlyEventLogFacade logFacade = new JavaFriendlyEventLogFacade(eventLog); 

			// paint events
			Rectangle rect = scrollPane.getViewport().getBounds();
			double tleft = pixelToSimulationTime(rect.x);
			double tright = pixelToSimulationTime(rect.right());
			EventEntry startEvent = eventLog.getLastEventBefore(tleft);
			EventEntry endEvent = eventLog.getFirstEventAfter(tright);
			int startEventIndex = (startEvent!=null) ? eventLog.findEvent(startEvent) : 0;
			int endEventIndex = (endEvent!=null) ? eventLog.findEvent(endEvent) : eventLog.getNumEvents(); 

			int startEventNumber = (startEvent!=null) ? startEvent.getEventNumber() : 0;

			// check events
            if (events != null) {
            	for (int i=startEventIndex; i<endEventIndex; i++)
   				if (eventSymbolContainsPoint(mouseX, mouseY, logFacade.getEvent_i_cachedX(i), logFacade.getEvent_i_cachedY(i), MOUSE_TOLERANCE))
   					events.add(eventLog.getEvent(i));
            }

            // check message arrows
            if (msgs != null) {
            	for (int i=startEventIndex; i<endEventIndex; i++) {
	            		int eventX = logFacade.getEvent_i_cachedX(i);
	            		int eventY = logFacade.getEvent_i_cachedY(i);
	
	            		// check forward arrows for this event
	            		int numConsequences = logFacade.getEvent_i_numConsequences(i);
	            		for (int k=0; k<numConsequences; k++) {
	            			if (logFacade.getEvent_i_consequences_k_hasTarget(i, k) &&
	            					logFacade.getEvent_i_consequences_k_target_isInFilteredSubset(i, k, eventLog)) {
	            				if (messageArrowContainsPoint(
	            						eventX,
	            						eventY,
	            						logFacade.getEvent_i_consequences_k_target_cachedX(i, k),
	            						logFacade.getEvent_i_consequences_k_target_cachedY(i, k),
	            						mouseX, 
	            						mouseY,
	            						MOUSE_TOLERANCE))
	            					msgs.add(eventLog.getEvent(i).getConsequences().get(k));
	            			}
	            		}
	
	            		// check backward arrows that we didn't check as forward arrows
	            		int numCauses = logFacade.getEvent_i_numCauses(i);
	            		for (int k=0; k<numCauses; k++) {
	            			if (logFacade.getEvent_i_causes_k_source_eventNumber(i, k) < startEventNumber) {
	            				if (logFacade.getEvent_i_causes_k_hasSource(i, k) &&
	            						logFacade.getEvent_i_causes_k_source_isInFilteredSubset(i, k, eventLog)) {
	            					if (messageArrowContainsPoint(
	            							logFacade.getEvent_i_causes_k_source_cachedX(i, k),
	            							logFacade.getEvent_i_causes_k_source_cachedY(i, k),
	            							eventX,
	            							eventY,
	            							mouseX, 
	            							mouseY,
	            							MOUSE_TOLERANCE))
	            						msgs.add(eventLog.getEvent(i).getConsequences().get(k));
	            				}
	            			}
	            		}
//                	}
            	}
            }

            long millis = System.currentTimeMillis()-startMillis;
            System.out.println("collectStuffUnderMouse(): "+millis+"ms");
		}
	}

	/**
	 * Utility function, to detect whether use clicked (hovered) an event in the the chart
	 */
	private boolean eventSymbolContainsPoint(int x, int y, int px, int py, int tolerance) {
		return Math.abs(x-px) <= 2+tolerance && Math.abs(y-py) <= 5+tolerance;
	}

	private boolean messageArrowContainsPoint(int x1, int y1, int x2, int y2, int px, int py, int tolerance) {
		if (y1==y2)
			return halfEllipseContainsPoint(x1, x2, y1, selfArrowHeight, px, py, tolerance); 
		else
			return lineContainsPoint(x1, y1, x2, y2, px, py, tolerance); 
	}
	
	private boolean halfEllipseContainsPoint(int x1, int x2, int y, int height, int px, int py, int tolerance) {
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
			if (selectionEvents.size()>0)
				gotoSimulationTime(selectionEvents.get(0).getSimulationTime());

			repaint();
		}
	}
}

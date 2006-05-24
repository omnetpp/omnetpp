package org.omnetpp.experimental.seqchart.editors;


import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.HashMap;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.MouseMotionListener;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.ToolTip;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;
import org.omnetpp.scave.engine.JavaFriendlyEventLogFacade;
import org.omnetpp.scave.engine.MessageEntry;

/**
 * This is a sequence chart as a single figure.
 *
 * @author andras
 */
//TODO limit pixelsPersec to a range that makes sense (for the current eventLog)
//TODO draw arrowheads
//TODO merge/hide axes
//FIXME scrollbar breaks badly when char size exceeds ~4,000,000 pixels (this means only ~0.1s resolution ticks on an 1000s trace!!! not enough!)
//FIXME msg arrows that intersect the chart area but don't start or end there are not displayed (BUG!)
//FIXME redraw chart with antialias while user is idle? hints: new SafeRunnable(); or:
//		canvas.getDisplay().syncExec(new Runnable() {
//			public void run() { ... }
//		};

public class SeqChartFigure extends Figure {

	private final Color EVENT_FG_COLOR = new Color(null,255,0,0);
	private final Color EVENT_BG_COLOR = new Color(null,255,0,0);
	private final Color MSG_COLOR = new Color(null,0,255,0);
	private final Color DELIVERY_MSG_COLOR = new Color(null,0,0,255);
	private final Cursor DRAGCURSOR = new Cursor(null, SWT.CURSOR_SIZEALL);
	private static final int XMAX = 10000;
	private static final int ANTIALIAS_TURN_ON_AT_MSEC = 100;
	private static final int ANTIALIAS_TURN_OFF_AT_MSEC = 300;
	private static final int MOUSE_TOLERANCE = 1;

	private ScrollPane scrollPane; // parent scrollPane
	
	protected EventLog eventLog;
	
	protected double pixelsPerSec = 2;
	protected int tickScale = 2; // -1 means step=0.1
	private boolean antiAlias = true;

	private int dragStartX, dragStartY; // temporary variables for drag handling
	
	private Canvas canvas;  // needed for tooltip creation
	private ToolTip swtTooltip;
	
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
	public double getPixelsPerSec() {
		return pixelsPerSec;	
	}
	
	/**
	 * Set chart scale (number of pixels a 1-second interval maps to), 
	 * and adjusts the density of ticks.
	 */
	public void setPixelsPerSec(double pps) {
		if (pixelsPerSec == pps)
			 return; // nothing to do
		
		// we want to center around the time currently displayed
		int middleX = scrollPane.getViewport().getBounds().width/2;
		double currentTime = pixelToTime(middleX);
		
		// set pixels per sec, and recalculate tick spacing
		if (pps<=0)
			pps = 1e-12;
		pixelsPerSec = pps;
		int labelWidthNeeded = 50; // pixels
		tickScale = (int)Math.ceil(Math.log10(labelWidthNeeded/pps));

		System.out.println("pixels per sec: "+pixelsPerSec);
        
		// we are of different size now
		recalculatePreferredSize();

		// restore the time originally displayed
		gotoTime(currentTime);  // this includes repaint()
	}

	/**
	 * Scroll the canvas so as to make currentTime visible 
	 */
	public void gotoTime(double time) {
		double xDouble = time * pixelsPerSec;
		int x = xDouble < 0 ? 0 : xDouble>Integer.MAX_VALUE ? Integer.MAX_VALUE : (int)xDouble;
		scrollPane.scrollHorizontalTo(x - scrollPane.getViewport().getBounds().width/2);
		repaint();
	}

	/**
	 * Increases pixels per second. 
	 */
	public void zoomIn() {
		setPixelsPerSec(getPixelsPerSec() * 1.5);
	}

	/**
	 * Decreases pixels per second. 
	 */
	public void zoomOut() {
		setPixelsPerSec(getPixelsPerSec() / 1.5);	
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

		// adapt our zoom level to the current eventLog
		setPixelsPerSec(suggestPixelsPerSec());

		// refresh chart. We may end up doing this twice, since it's also called from 
		// setPixelsPerSec(), but it does no harm
		recalculatePreferredSize(); 
		repaint();
	}

	/**
	 * If the current pixels/sec setting doesn't look useful for the current event log,
	 * suggest a better one. Otherwise just returns the old value. The current settings
	 * are not changed.
	 */
	public double suggestPixelsPerSec() {
		// adjust pixelsPerSec if it's way out of the range that makes sense
		int numEvents = eventLog.getNumEvents();
		if (numEvents>=2) {
			double tStart = eventLog.getEvent(0).getSimulationTime();
			double tEnd = eventLog.getEvent(numEvents-1).getSimulationTime();
			double eventPerSec = numEvents / (tEnd - tStart);

			int chartWidthPixels = scrollPane.getViewport().getBounds().width;
			if (chartWidthPixels<=0) chartWidthPixels = 800;  // may be 0 on startup

			double minPixelsPerSec = eventPerSec * 10;  // we want at least 10 pixel/event
			double maxPixelsPerSec = eventPerSec * (chartWidthPixels/10);  // we want at least 10 events on the chart

			if (pixelsPerSec < minPixelsPerSec)
				return minPixelsPerSec;
			else if (pixelsPerSec > maxPixelsPerSec)
				return maxPixelsPerSec;
		}
		return pixelsPerSec; // the current setting is fine
	}

	private void recalculatePreferredSize() {
		EventEntry lastEvent = eventLog.getEvent(eventLog.getNumEvents()-1);
		int width = lastEvent==null ? 0 : (int)(lastEvent.getSimulationTime()*getPixelsPerSec());
		width = Math.max(width, 600); // at least half a screen
		int height = eventLog.getNumModules()*50;
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
			JavaFriendlyEventLogFacade logFacade = new JavaFriendlyEventLogFacade(eventLog); 
			HashMap<Integer,Integer> moduleIdToAxisYMap = new HashMap<Integer, Integer>();
			for (int i=0; i<eventLog.getNumModules(); i++) {
				int y = bounds.y+30+i*50;
				moduleIdToAxisYMap.put(eventLog.getModule(i).getModuleId(), y);
				drawLinearAxis(graphics, y, eventLog.getModule(i).getModuleFullPath());
			}

			graphics.setAntialias(antiAlias ? SWT.ON : SWT.OFF);
			graphics.setTextAntialias(SWT.ON);
			
			// paint events
			Rectangle rect = graphics.getClip(new Rectangle());
			double tleft = pixelToTime(rect.x);
			double tright = pixelToTime(rect.right());
			EventEntry startEvent = eventLog.getLastEventBefore(tleft);
			EventEntry endEvent = eventLog.getFirstEventAfter(tright);
			int startEventIndex = (startEvent!=null) ? eventLog.findEvent(startEvent) : 0;
			int endEventIndex = (endEvent!=null) ? eventLog.findEvent(endEvent) : eventLog.getNumEvents(); 

			int startEventNumber = (startEvent!=null) ? startEvent.getEventNumber() : 0;
			int endEventNumber = (endEvent!=null) ? endEvent.getEventNumber() : Integer.MAX_VALUE;
            
            // calculate event coordinates (we'll paint them after the arrows)
            for (int i=startEventIndex; i<endEventIndex; i++) {
   				int x = timeToPixel(logFacade.getEvent_i_simulationTime(i));
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
            		if (logFacade.getEvent_i_consequences_k_hasTarget(i, k) &&
            			logFacade.getEvent_i_consequences_k_target_isInFilteredSubset(i, k, eventLog)) {
            			// we have to calculate target event's coords if it's beyond endEventNumber
            			graphics.setForegroundColor(logFacade.getEvent_i_consequences_k_isDelivery(i,k) ? DELIVERY_MSG_COLOR : MSG_COLOR); 
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
               				double targetTime = logFacade.getEvent_i_consequences_k_target_simulationTime(i, k);
               				double targetXDouble = timeToPixelDouble(targetTime);
               				int targetX = targetXDouble > XMAX ? XMAX : (int)targetXDouble;
               				int targetY = moduleIdToAxisYMap.get(logFacade.getEvent_i_consequences_k_target_module_moduleId(i, k));
               				logFacade.setEvent_i_consequences_k_target_cachedX(i, k, targetX);
               				logFacade.setEvent_i_consequences_k_target_cachedY(i, k, targetY);

               				drawMessageArrow(graphics, eventX, eventY, targetX, targetY);
                		}
            		}
            	}

            	// paint backward arrows that we didn't paint as forward arrows
            	int numCauses = logFacade.getEvent_i_numCauses(i);
            	for (int k=0; k<numCauses; k++) {
            		if (logFacade.getEvent_i_causes_k_source_eventNumber(i, k) < startEventNumber) {
            			// source event is outside the repaint region (on the far left)
                		if (logFacade.getEvent_i_causes_k_hasSource(i, k) &&
                    		logFacade.getEvent_i_causes_k_source_isInFilteredSubset(i, k, eventLog)) {
               				double srcXDouble = timeToPixelDouble(logFacade.getEvent_i_causes_k_source_simulationTime(i, k));
               				int srcY = moduleIdToAxisYMap.get(logFacade.getEvent_i_causes_k_source_module_moduleId(i, k));
               				int srcX = srcXDouble < -XMAX ? -XMAX : (int)srcXDouble;
               				logFacade.setEvent_i_causes_k_source_cachedX(i, k, srcX);
               				logFacade.setEvent_i_causes_k_source_cachedY(i, k, srcY);
               				graphics.setForegroundColor(logFacade.getEvent_i_causes_k_isDelivery(i,k) ? DELIVERY_MSG_COLOR : MSG_COLOR); 
               				drawMessageArrow(graphics, srcX, srcY, eventX, eventY);
                		}
            		}
            	}
            }

			// paint events
            graphics.setForegroundColor(EVENT_FG_COLOR); 
            graphics.setBackgroundColor(EVENT_BG_COLOR);
            for (int i=startEventIndex; i<endEventIndex; i++) {
   				int x = logFacade.getEvent_i_cachedX(i);
   				int y = logFacade.getEvent_i_cachedY(i);
            	graphics.fillOval(x-2, y-3, 5, 7);
            	
            	if (logFacade.getEvent_i_isSelected(i)) {
                	graphics.drawOval(x-10, y-10, 21, 21);
            	}
            }           	

            // turn on/off anti-alias 
            long repaintMillis = System.currentTimeMillis()-startMillis;
            //System.out.println("repaint(): "+repaintMillis+"ms");
            if (antiAlias && repaintMillis > ANTIALIAS_TURN_OFF_AT_MSEC)
            	antiAlias = false;
            else if (!antiAlias && repaintMillis < ANTIALIAS_TURN_ON_AT_MSEC)
            	antiAlias = true;
            //XXX also: turn it off also during painting if it's going to take too long 
		}
	}

	private void drawMessageArrow(Graphics graphics, int x1, int y1, int x2, int y2) {
		if (y1==y2) {
			Rectangle.SINGLETON.setLocation(x1, y1-10);
			Rectangle.SINGLETON.setSize(x2-x1, 20);
			graphics.drawArc(Rectangle.SINGLETON, 0, 180);
		} else {
			graphics.drawLine(x1, y1, x2, y2);
		}
	}

	/**
	 * Draws the axis, according to the current pixelsPerSec and tickInterval
	 * settings.
	 */
	private void drawLinearAxis(Graphics graphics, int y, String label) {
		Rectangle rect = graphics.getClip(new Rectangle());
		Rectangle bounds = getBounds();
		rect.intersect(bounds); // although looks like Clip is already set up like this

		// draw axis label; may it should be "sticky" on the screen?
		graphics.drawText(label, bounds.x, y-20); //XXX refine

		// draw axis
		graphics.drawLine(rect.x, y, rect.right(), y);

		double tleft = pixelToTime(rect.x);
		double tright = pixelToTime(rect.right());

		// draw ticks and labels
		BigDecimal tickStart = new BigDecimal(tleft).setScale(-tickScale, RoundingMode.FLOOR);
		BigDecimal tickEnd = new BigDecimal(tright).setScale(-tickScale, RoundingMode.CEILING);
		BigDecimal tickIntvl = new BigDecimal(1).scaleByPowerOfTen(tickScale);
		//System.out.println(tickStart+" - "+tickEnd+ " step "+tickIntvl);

		for (BigDecimal t=tickStart; t.compareTo(tickEnd)<0; t = t.add(tickIntvl)) {
			int x = timeToPixel(t.doubleValue());
			graphics.drawLine(x, y-2, x, y+2);
			graphics.drawText(t.toPlainString()+"s", x, y+3);
		}
	}

	/**
	 * Translates from pixel x coordinate to seconds, using on pixelsPerSec.
	 */
	private double pixelToTime(int x) {
		return (x-getBounds().x) / pixelsPerSec;
	}

	/**
	 * Translates from seconds to pixel x coordinate, using on pixelsPerSec.
	 */
	private int timeToPixel(double t) {
		return (int)Math.round(t * pixelsPerSec) + getBounds().x;
	}

	/**
	 * Same as timeToPixel(), but doesn't convert to int; to be used where "int" may overflow
	 */
	private double timeToPixelDouble(double t) {
		return t * pixelsPerSec + getBounds().x;
	}
	
	/**
	 * Sets up dragging and tooltip.
	 */
	private void setUpMouseHandling() {
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
			}
		});
	}

	protected void displayTooltip(int x, int y) {
		String tooltipText = getTooltipText(x,y);
		if (tooltipText!=null) {
			swtTooltip = new ToolTip(canvas.getShell(), SWT.NONE);
			swtTooltip.setMessage(tooltipText);
			swtTooltip.setLocation(canvas.toDisplay(x,y+20));
			swtTooltip.setVisible(true);
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

		String res = "";
		for (EventEntry event : events) {
			res += "Event #"+event.getEventNumber()
	    		+" at t="+event.getSimulationTime()
	    		+", module ("+event.getModule().getModuleClassName()+")"
	    		+event.getModule().getModuleFullPath()
	    		+" (id="+event.getModule().getModuleId()+"),"
	    		+" message ("+event.getCause().getMessageClassName()+")"
	    		+event.getCause().getMessageName()+"\n";
		}
		for (MessageEntry msg : msgs) {
			res += "Message ("+msg.getMessageClassName()+") "
				+msg.getMessageName()+"\n";
		}
		return res.equals("") ? null : res;
	}

	/**
	 * Determines if there are any events (EventEntry) or messages (MessageEntry)
	 * at the given mouse coordinates, and returns them in the ArrayLists passed. 
	 * Coordinates are canvas coordinates (more precisely, viewport coordinates).
	 * You can call this method from mouse click, double-click or hover event 
	 * handlers.
	 */
	public void collectStuffUnderMouse(int mouseX, int mouseY, ArrayList<EventEntry> events, ArrayList<MessageEntry> msgs) {
		if (eventLog!=null) {
			long startMillis = System.currentTimeMillis();
		
			JavaFriendlyEventLogFacade logFacade = new JavaFriendlyEventLogFacade(eventLog); 

			// paint events
			Rectangle rect = scrollPane.getViewport().getBounds();
			double tleft = pixelToTime(rect.x);
			double tright = pixelToTime(rect.right());
			EventEntry startEvent = eventLog.getLastEventBefore(tleft);
			EventEntry endEvent = eventLog.getFirstEventAfter(tright);
			int startEventIndex = (startEvent!=null) ? eventLog.findEvent(startEvent) : 0;
			int endEventIndex = (endEvent!=null) ? eventLog.findEvent(endEvent) : eventLog.getNumEvents(); 

			int startEventNumber = (startEvent!=null) ? startEvent.getEventNumber() : 0;
            
            // check events
            for (int i=startEventIndex; i<endEventIndex; i++)
   				if (eventSymbolContainsPoint(mouseX, mouseY, logFacade.getEvent_i_cachedX(i), logFacade.getEvent_i_cachedY(i), MOUSE_TOLERANCE))
   					events.add(eventLog.getEvent(i));

            // check message arrows
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
            }

            long millis = System.currentTimeMillis()-startMillis;
            System.out.println("collectStuff(): "+millis+"ms");
		}
	}

	/**
	 * Utility function, to detect whether use clicked (hovered) an event in the the chart
	 */
	private boolean eventSymbolContainsPoint(int x, int y, int px, int py, int tolerance) {
		return Math.abs(x-px) <= 2+tolerance && Math.abs(y-py) <= 3+tolerance;
	}

	private boolean messageArrowContainsPoint(int x1, int y1, int x2, int y2, int px, int py, int tolerance) {
		if (y1==y2)
			return halfEllipseContainsPoint(x1, x2, y1, 10, px, py, tolerance); 
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

}

package org.omnetpp.experimental.seqchart.editors;


import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.HashMap;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;
import org.omnetpp.scave.engine.MessageEntries;
import org.omnetpp.scave.engine.MessageEntry;

/**
 * This one uses figures for arrows. 
 *
 * @author andras
 */
public class SeqChartFigure2 extends Figure {
	
	protected double pixelsPerSec = 2;
	protected int tickScale = 2; // -1 means step=0.1
	protected EventLog eventLog;

	public SeqChartFigure2() {
		setLayoutManager(new XYLayout());
	}
	
	public double getPixelsPerSec() {
		return pixelsPerSec;	
	}
	
	/**
	 * Also adjusts tickScale which controls density of ticks
	 */
	public void setPixelsPerSec(double pps) {
		if (pps<=0)
			pps = 1e-12;
		pixelsPerSec = pps;
		int labelWidthNeeded = 50; // pixels
		tickScale = (int)Math.ceil(Math.log10(labelWidthNeeded/pps));

		System.out.println("pixels per sec: "+pixelsPerSec);
        
		recalculatePreferredSize();
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
	
	public EventLog getEventLog() {
		return eventLog;
	}

	public void setEventLog(EventLog eventLog) {
		this.eventLog = eventLog;
		recalculatePreferredSize();
		
		recreateFigures();
	}

	private void recreateFigures() {
		removeAll();

		// add figures for events
		Color red = new Color(null,255,0,0);
		for (int i=0; i<eventLog.getNumEvents(); i++) {
        	EventEntry event = eventLog.getEvent(i);
    		int x = timeToPixel(event.getSimulationTime());
    		int y = getBounds().y+60;
			Figure f = new Figure();
			f.setBackgroundColor(red);
			f.setOpaque(true);
			add(f);
			getLayoutManager().setConstraint(f, new Rectangle(x,y,5,5));
		}

//		// paint events
//		Rectangle rect = graphics.getClip(new Rectangle());
//		double tleft = pixelToTime(rect.x);
//		double tright = pixelToTime(rect.right());
//		EventEntry startEvent = eventLog.getFirstEventAfter(tleft);
//		EventEntry endEvent = eventLog.getFirstEventAfter(tright);
//		int startEventIndex = (startEvent!=null) ? eventLog.findEvent(startEvent) : 0;
//		int endEventIndex = (endEvent!=null) ? eventLog.findEvent(endEvent) : eventLog.getNumEvents(); 
//		int startEventNumber = (startEvent!=null) ? startEvent.getEventNumber() : 0;
//        
//        graphics.setForegroundColor(new Color(null,255,0,0));
//        graphics.setBackgroundColor(new Color(null,170,0,0));
//        for (int i=startEventIndex; i<endEventIndex; i++) {
//        	EventEntry event = eventLog.getEvent(i);
//			Point p = getEventCoords(event, moduleIdToAxisMap);
//        	graphics.fillOval(p.x-2, p.y-2, 5, 5);
//        	
//        	// paint forward arrows for this event
//        	MessageEntries consequences = event.getConsequences();
//        	for (int j=0; j<consequences.size(); j++)
//    			drawMessageArrow(consequences.get(j), moduleIdToAxisMap, graphics);
//
//        	// paint backward arrows that wouldn't be painted otherwise
//        	MessageEntries causes = event.getCauses();
//        	for (int j=0; j<causes.size(); j++)
//        		if (causes.get(j).getSource().getEventNumber() < startEventNumber)
//        			drawMessageArrow(causes.get(j), moduleIdToAxisMap, graphics);
//        	//FIXME this is not quite good enough -- we have to draw all messages where 
//        	//the arrow is at least partially on the screen (ie maybe src<tstart and target>tend!)
//        	
//        }
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
			// paint axes
			HashMap<Integer,Integer> moduleIdToAxisMap = new HashMap<Integer, Integer>();
			for (int i=0; i<eventLog.getNumModules(); i++) {
				moduleIdToAxisMap.put(eventLog.getModule(i).getModuleId(), i);
				drawLinearAxis(graphics, bounds.y+30+i*50, eventLog.getModule(i).getModuleFullName());
			}

			//XXX events and arrows are to become figures
		}
	}

	private void drawMessageArrow(MessageEntry msg, HashMap<Integer, Integer> moduleIdToAxisMap, Graphics graphics) {
		Point p1 = getEventCoords(msg.getSource(), moduleIdToAxisMap);
		Point p2 = getEventCoords(msg.getTarget(), moduleIdToAxisMap);
		if (p1.y==p2.y) {
			drawArc(graphics, p1, p2);
		} else {
			graphics.drawLine(p1, p2);
		}
	}

	private void drawArc(Graphics graphics, Point p1, Point p2) {
		Rectangle rect = new Rectangle(p1.x, p1.y-10, p2.x-p1.x, 20);
		//rect.expand(rect.width, 0);
		graphics.drawArc(rect, 0, 180);
	}

	private Point getEventCoords(EventEntry event, HashMap<Integer,Integer> moduleIdToAxisMap) {
		int x = timeToPixel(event.getSimulationTime());
		int axis = moduleIdToAxisMap.get(event.getModule().getModuleId());
		int y = bounds.y+30+axis*50;
		return new Point(x,y);
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
}

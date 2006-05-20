package org.omnetpp.experimental.seqchart.editors;


import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.HashMap;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;
import org.omnetpp.scave.engine.MessageEntries;

public class SeqChartFigure extends Figure {
	
	protected double pixelsPerSec = 2;
	protected int tickScale = 2; // -1 means step=0.1
	protected EventLog eventLog;


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
		int labelWidthNeeded = 30; // pixels
		tickScale = (int)Math.ceil(Math.log10(labelWidthNeeded/pps));

		System.out.println("pixels per sec: "+pixelsPerSec);
        
		repaint();
	}

	/**
	 * Increases pixels per second. 
	 */
	public void zoomIn() {
		setPixelsPerSec(getPixelsPerSec() * 1.5);	
	}

	/**
	 * Increases pixels per second. 
	 */
	public void zoomOut() {
		setPixelsPerSec(getPixelsPerSec() / 1.5);	
	}
	
	public EventLog getEventLog() {
		return eventLog;
	}

	public void setEventLog(EventLog eventLog) {
		this.eventLog = eventLog;
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

			// paint events
			Rectangle rect = graphics.getClip(new Rectangle());
			double tleft = pixelToTime(rect.x);
			double tright = pixelToTime(rect.right());
			int startEventIndex = 0;
			int endEventIndex = eventLog.getNumEvents();
            EventEntry startEvent = eventLog.getFirstEventAfter(tleft);
            if (startEvent!=null)
            	startEventIndex = eventLog.findEvent(startEvent);
            EventEntry endEvent = eventLog.getFirstEventAfter(tright);
            if (endEvent!=null)
            	endEventIndex = eventLog.findEvent(endEvent);
            
            graphics.setForegroundColor(new Color(null,255,0,0));
            for (int i=startEventIndex; i<endEventIndex; i++) {
            	EventEntry event = eventLog.getEvent(i);
    			Point p = getEventCoords(event, moduleIdToAxisMap);
            	graphics.drawOval(p.x-2, p.y-2, 5, 5);
            	
            	// paint forward arrows for this event
            	MessageEntries consequences = event.getConsequences();
            	for (int j=0; j<consequences.size(); j++) {
        			Point p1 = getEventCoords(consequences.get(j).getSource(), moduleIdToAxisMap);
        			Point p2 = getEventCoords(consequences.get(j).getTarget(), moduleIdToAxisMap);
        			if (p1.y==p2.y)
        				graphics.drawArc(p1.x, p1.y-10, p2.x-p1.x, 10, 60, 60);
        			else
        				graphics.drawLine(p1, p2);
            	}
            }
		}
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

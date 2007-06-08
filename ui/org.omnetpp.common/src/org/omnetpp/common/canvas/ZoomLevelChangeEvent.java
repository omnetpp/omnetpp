package org.omnetpp.common.canvas;

/**
 * Event object holding data of a zoom level change.
 *
 * @author tomi
 */
public class ZoomLevelChangeEvent {
	ZoomableCachingCanvas source;
	private int direction;   // SWT.HORIZONTAL, SWT.VERTICAL
	private double oldLevel;
	private double newLevel;
	
	public ZoomLevelChangeEvent(ZoomableCachingCanvas source, int direction, double oldLevel, double newLevel) {
		this.source = source;
		this.direction = direction;
		this.oldLevel = oldLevel;
		this.newLevel = newLevel;
	}
	
	public ZoomableCachingCanvas getSource() {
		return source;
	}
	
	public int getDirection() {
		return direction;
	}
	
	public double getOldZoomLevel() {
		return oldLevel;
	}
	
	public double getNewZoomLevel() {
		return newLevel;
	}
}

package org.omnetpp.common.canvas;

/**
 * Interface for notifying listeners of zoom level changes.
 *
 * @author tomi
 */
public interface IZoomLevelChangeListener {
	public void zoomLevelChanged(ZoomLevelChangeEvent event);
}

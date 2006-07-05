package org.omnetpp.figures;

import org.eclipse.draw2d.Layer;

/**
 * Interface to mark and support multiple layers in a the figure. This is used to provide
 * decoration layers to child figures.
 * @author rhornig
 */
public interface LayerSupport {
	enum LayerID { Background, BackgroundDecoration, Default, FrontDecoration, Connection, ToolTip } 

    /**
     * @param layerId
     * @return the requested layer or <code>null</code> if does not exist
     */
    public Layer getLayer(Object layerId);

}

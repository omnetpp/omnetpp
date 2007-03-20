package org.omnetpp.figures;

import org.eclipse.draw2d.Layer;

/**
 * Interface to mark and support multiple layers in a the figure. This is used to provide
 * decoration layers for child figures.
 * @author rhornig
 */
public interface ILayerSupport {
	enum LayerID { BACKGROUND, BACKGROUND_DECORATION, DEFAULT, FRONT_DECORATION, CONNECTION, MESSAGE } 

    /**
     * @param layerId
     * @return the requested layer or <code>null</code> if does not exist
     */
    public Layer getLayer(LayerID layerId);

}

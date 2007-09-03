package org.omnetpp.figures.misc;

import org.eclipse.draw2d.Layer;

/**
 * Interface to mark and support multiple layers in a the figure. This is used to provide
 * decoration layers for child figures.
 *
 * @author rhornig
 */
public interface ILayerSupport {
	enum LayerID { BACKGROUND, BACKGROUND_DECORATION, DEFAULT, FRONT_DECORATION, CONNECTION, MESSAGE }

    /**
     * Returns the layer with the given id, or null if it does not exist
     */
    public Layer getLayer(LayerID layerId);

}

package org.omnetpp.ned.editor.graph.figures.properties;

import org.eclipse.draw2d.Layer;

/**
 * @author rhornig
 * Interface to mark and support multiple layers in a the figure. This if is used to provide
 * decoration layers to child figures.
 */
public interface LayerSupport {
    public final static String BACK_DECORATION_LAYER = "Backgorund decoration layer";
    public final static String DEFAULT_LAYER = "Normal layer";
    public final static String FRONT_DECORATION_LAYER = "Foregorund decoration layer";

    /**
     * @param layerId
     * @return the requested layer or <code>null</code> if does not exist
     */
    public Layer getLayer(Object layerId);

}

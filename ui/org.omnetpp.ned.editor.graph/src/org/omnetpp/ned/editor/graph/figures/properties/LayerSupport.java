package org.omnetpp.ned.editor.graph.figures.properties;

import org.eclipse.draw2d.Layer;

/**
 * @author rhornig
 * Interface to mark and support multiple layers in a the figure. This is used to provide
 * decoration layers to child figures.
 */
public interface LayerSupport {
	enum LayerID { Background, BackgroundDecoration, Default, FrontDecoration, Connection, ToolTip } 
//    public final static String BACKGROUND_LAYER = "Background layer";
//    public final static String BACK_DECORATION_LAYER = "Background decoration layer";
//    public final static String DEFAULT_LAYER = "Normal layer";
//    public final static String FRONT_DECORATION_LAYER = "Foreground decoration layer";

    /**
     * @param layerId
     * @return the requested layer or <code>null</code> if does not exist
     */
    public Layer getLayer(Object layerId);

}

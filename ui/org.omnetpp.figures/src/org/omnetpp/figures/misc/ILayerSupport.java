/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.misc;

import org.eclipse.draw2d.Layer;

/**
 * Interface to mark and support multiple layers in a the figure. This is used to provide
 * decoration layers for child figures.
 *
 * @author rhornig
 */
public interface ILayerSupport {
    public Layer getBackgroundLayer();
    public Layer getBackgroundDecorationLayer();
    public Layer getSubmoduleLayer();
    public Layer getForegroundDecorationLayer();
    public Layer getConnectionLayer();
    public Layer getMessageLayer();
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.swt.widgets.Control;

/**
 * Interface to produce information that will be displayed in a tooltip-like
 * floating information window , for a given mouse position over a widget.
 * What the information window will contain is determined by the returned
 * HoverInfo object.
 *
 * For use with HoverSupport; see HoverSupport.adapt().
 *
 * @author Andras
 */
public interface IHoverInfoProvider {
    /**
     * Return a tooltip, or null if there is no tooltip to be shown.
     * The (x,y) coordinates are relative to the widget.
     *
     * @param control   the control which is hovered over
     * @param x         the x coordinate of the mouse cursor
     * @param y         the y coordinate of the mouse cursor
     */
    HoverInfo getHoverFor(Control control, int x, int y);
}

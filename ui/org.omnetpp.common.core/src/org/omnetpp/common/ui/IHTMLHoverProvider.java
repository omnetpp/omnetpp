/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.swt.widgets.Control;

/**
 * Produces an HTML page to display as hover info (tooltip), for a given mouse
 * position over a widget. For use with HoverSupport.
 *
 * @author Andras
 */
public interface IHTMLHoverProvider {
	/**
	 * Return a tooltip, or null if there is no tooltip to be shown.
	 * The (x,y) coordinates are relative to the widget.
	 *
	 * @param control 	the control which is hovered over
	 * @param x 		the x coordinate of the mouse cursor
	 * @param y			the y coordinate of the mouse cursor
	 */
	HTMLHoverInfo getHTMLHoverFor(Control control, int x, int y);
}

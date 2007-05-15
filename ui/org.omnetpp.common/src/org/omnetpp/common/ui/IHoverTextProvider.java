package org.omnetpp.common.ui;

import org.eclipse.swt.widgets.Control;

/**
 * Produces a string to display as hover info (tooltip), for a given mouse
 * position over a widget. For use with HoverSupport.
 * 
 * @author Andras
 */
public interface IHoverTextProvider {
	/**
	 * Return a tooltip text, or null if there is no tooltip to be shown.
	 * The (x,y) coordinates are relative to the widget.
	 */
	String getHoverTextFor(Control control, int x, int y);
}

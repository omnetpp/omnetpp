package org.omnetpp.common.ui;

import org.eclipse.swt.widgets.Control;

/**
 * For use with TooltipSupport.
 * @author Andras
 */
public interface ITooltipTextProvider {
	/**
	 * Return a tooltip text, or null if there is no tooltip to be shown.
	 */
	String getTooltipFor(Control control, int x, int y);
}

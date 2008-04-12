package org.omnetpp.scave.charting;

import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;

/**
 * Chart tooltip
 * @author Andras
 */
class Tooltip {
	/**
	 * 
	 */

	public Tooltip(final ChartCanvas canvas) {
		HoverSupport hoverSupport = new HoverSupport();
		hoverSupport.setHoverSizeConstaints(600, 400);
		hoverSupport.adapt(canvas, new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
				String html = canvas.getHoverHtmlText(x, y, outSizeConstraint);
				return html != null ? HoverSupport.addHTMLStyleSheet(html) : null;
			}
		});
	}
}
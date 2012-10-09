/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.ui.HTMLHoverInfo;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHTMLHoverProvider;

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
		hoverSupport.adapt(canvas, new IHTMLHoverProvider() {
			public HTMLHoverInfo getHTMLHoverFor(Control control, int x, int y) {
				String html = canvas.getHoverHtmlText(x, y);
				return html != null ? new HTMLHoverInfo(HoverSupport.addHTMLStyleSheet(html)) : null;
			}
		});
	}
}
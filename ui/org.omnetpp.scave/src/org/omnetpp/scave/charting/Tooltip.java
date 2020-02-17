/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverInfoProvider;

/**
 * Chart tooltip
 * @author Andras
 */
class Tooltip {
    /**
     *
     */

    public Tooltip(final PlotViewerBase canvas) {
        HoverSupport hoverSupport = new HoverSupport();
        hoverSupport.setHoverSizeConstaints(600, 400);
        hoverSupport.adapt(canvas, new IHoverInfoProvider() {
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                String html = canvas.getHoverHtmlText(x, y);
                return html != null ? new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(html)) : null;
            }
        });
    }
}
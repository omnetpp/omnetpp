/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;

/**
 * A figure representing a range around submodules. Used typically in wireless simulation
 * to denote transmission/interference range
 *
 * @author rhornig
 */
public class RangeFigure extends Ellipse {
    public static final int ALPHA = 64;

    // TODO: eclipse bug ID 199710
    @Override
    protected void outlineShape(Graphics graphics) {
        Rectangle r = Rectangle.SINGLETON;
        r.setBounds(getBounds());
        r.width--;
        r.height--;
    if (lineWidth % 2 == 0) {
        r.width--;
        r.height--;
    }
    float t = lineWidth / 2.0f;
    r.translate((int)t, (int)t);
    float s = (lineWidth - 1) / 2.0f;
    r.resize((int)(-t - s), (int)(-t -s));
    graphics.setClip(getBounds().getExpanded(1, 1));
    graphics.setLineWidthFloat(lineWidth*(float)graphics.getAbsoluteScale());
        graphics.setAlpha(ALPHA);
        graphics.drawOval(r);
    }

    @Override
    protected void fillShape(Graphics graphics) {
        Rectangle r = Rectangle.SINGLETON;
        r.setBounds(getBounds());
        r.width--;
        r.height--;
        int t = lineWidth;
        r.translate(t, t);
        int s = (lineWidth - 1);
        r.resize(-t - s, -t - s);
        graphics.setAlpha(ALPHA);
        graphics.fillOval(r);
    }
}

package org.omnetpp.figures;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;

/**
 * A figure representing a range around submodules. Used typically in wireless simulation
 * to denote transmission/interference range
 *
 * @author rhornig
 */
public class RangeFigure extends Ellipse {

    @Override
    public void paint(Graphics graphics) {
        graphics.pushState();
        // set antialiasing on content and child/derived figures
        if (NedFileFigure.antialias != SWT.DEFAULT)
            graphics.setAntialias(NedFileFigure.antialias);
        super.paint(graphics);
        graphics.popState();
    }

    // TODO: eclipse bug ID 199710
    @Override
    protected void outlineShape(Graphics graphics) {
    	Rectangle r = Rectangle.SINGLETON;
    	r.setBounds(getBounds());
    	r.width--;
    	r.height--;
    	int t = lineWidth / 2;
    	r.translate(t, t);
    	int s = (lineWidth - 1) / 2;
    	r.resize(-t - s, -t -s);
    	graphics.drawOval(r);
    }
}

package org.omnetpp.figures;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;

/**
 * A figure representing a range around submodules. Used tipically in wireless simulation
 * to denote transmission/interference range
 *
 * @author rhornig
 */
public class RangeFigure extends Ellipse {

    @Override
    public void paint(Graphics graphics) {
        graphics.pushState();
        // set antialiasing on contenet and child/derived figures
        if (NedFileFigure.antialias != SWT.DEFAULT)
            graphics.setAntialias(NedFileFigure.antialias);
        super.paint(graphics);
        graphics.popState();
    }
}

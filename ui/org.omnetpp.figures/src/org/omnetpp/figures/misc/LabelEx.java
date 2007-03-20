package org.omnetpp.figures.misc;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.swt.SWT;

/**
 * @author rhornig
 * Fixes some issues with antialiased fonts
 */
public class LabelEx extends Label {
    
    @Override
    public void paint(Graphics graphics) {
//        graphics.setTextAntialias(SWT.OFF);
        graphics.drawRectangle(getBounds());
        super.paint(graphics);
    }
    
    @Override
    protected Dimension calculateTextSize() {
        Dimension d = super.calculateTextSize();
        // XXX this is a hack to return 2px wider size because antialised fonts are wider
        // This should be rather handled correctly however by FigureUtilities.getTextExtent
        return d.expand(2,0);
    }
    
}

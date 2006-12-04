package org.omnetpp.figures;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.swt.SWT;

/**
 * @author rhornig
 * Top level figure corresponding to a NED file
 */
public class NedFileFigure extends Layer {
    protected int antialias = SWT.ON;

    public NedFileFigure() {
        super();
        ToolbarLayout fl = new ToolbarLayout();
        fl.setStretchMinorAxis(false);
        fl.setSpacing(20);
        setLayoutManager(fl);
        setBorder(new MarginBorder(5));
    }

    /**
     * Sets the antialaiasing mode. can be SWT.DEFAULT, SWT.ON, SWT.OFF
     * @param antialias
     */
    public void setAntialias(int antialias) {
        this.antialias = antialias;
    }

    @Override
    public void paint(Graphics graphics) {
        // set antialiasing on derived figures
        if(antialias != SWT.DEFAULT)
            graphics.setAntialias(antialias);
        super.paint(graphics);
    }
    
    
}

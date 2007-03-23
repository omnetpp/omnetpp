package org.omnetpp.figures;

import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.swt.SWT;

/**
 * @author rhornig
 * Top level figure corresponding to a NED file
 */
public class NedFileFigure extends Layer {
    // global switch for antialiasing
    public static int antialias = SWT.ON;

    public NedFileFigure() {
        super();
        ToolbarLayout fl = new ToolbarLayout();
        fl.setStretchMinorAxis(false);
        fl.setSpacing(20);
        setLayoutManager(fl);
        setBorder(new MarginBorder(5));
    }

}

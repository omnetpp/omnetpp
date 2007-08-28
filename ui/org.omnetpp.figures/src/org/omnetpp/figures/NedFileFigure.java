package org.omnetpp.figures;

import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.swt.SWT;
import org.omnetpp.common.color.ColorFactory;

/**
 * Top level figure corresponding to a NED file
 *
 * @author rhornig
 */
public class NedFileFigure extends Layer {
    // global switch for antialiasing
    public static int antialias = SWT.ON;
    
    protected String problemMessage;

    public NedFileFigure() {
        super();
        ToolbarLayout fl = new ToolbarLayout();
        fl.setStretchMinorAxis(false);
        fl.setSpacing(20);
        setLayoutManager(fl);
        setProblemMessage(null);
    }
	
    public void setProblemMessage(String problemMessage) {
    	this.problemMessage = problemMessage;
    	setBorder(problemMessage == null ? new MarginBorder(10) : new LineBorder(ColorFactory.RED, 10));
		invalidate();
    }
}

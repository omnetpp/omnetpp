package org.omnetpp.figures;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;

/**
 * It is a generic 'Module' (anything that can have connections)
 * @author rhornig
 */
public class ModuleFigure extends Figure {
    
    @Override
    public void paint(Graphics graphics) {
    	// Enable antialiasing on derived figures
        graphics.setAntialias(SWT.ON);
        super.paint(graphics);
    }

    /**
     * temporary debug function to display figure hierarchy
     * @param f
     * @param indent
     */
    static public void debugPrintFigureHierarchy(Figure f, String indent) {
    	System.out.println(indent+f.getClass().getSimpleName()+"@"+f.hashCode()+"  "+f.getBounds()+" opaque="+f.isOpaque());
    	for (Object child : f.getChildren()) {
    		debugPrintFigureHierarchy((Figure)child, indent+"    ");
    	}
    }
}
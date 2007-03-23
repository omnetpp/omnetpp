package org.omnetpp.figures;

import org.eclipse.draw2d.Figure;
import org.omnetpp.common.displaymodel.IDisplayString;

/**
 * It is a generic 'Module' (anything that can have a display string and a connection)
 * @author rhornig
 */
abstract public class ModuleFigure extends Figure {
    protected String figureName = ""; 
	protected IDisplayString lastDisplayString;

    /**
     * Returns the lastly set displaysting
     * @return
     */
    public IDisplayString getLastDisplayString() {
    	return lastDisplayString;
    }
    
    /**
     * Stro
     * @param dps
     */
    abstract public void setDisplayString(IDisplayString dps);
    
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

    @Override
    public String toString() {
        return getClass().getSimpleName()+" "+figureName;
    }
}
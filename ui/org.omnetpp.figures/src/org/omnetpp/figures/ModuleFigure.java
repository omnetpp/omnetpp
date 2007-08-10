package org.omnetpp.figures;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.IFigure;
import org.omnetpp.common.displaymodel.IDisplayString;

/**
 * It is a generic 'Module' (anything that can have a display string and a connection)
 * @author rhornig
 */
abstract public class ModuleFigure extends Figure {
    protected String figureName = "";
	protected IDisplayString lastDisplayString;

    /**
     * Returns the most recently set display sting
     */
    public IDisplayString getLastDisplayString() {
    	return lastDisplayString;
    }

    /**
     * Stores the display string, and updates the figure accordingly.
     * @param dps
     */
    abstract public void setDisplayString(IDisplayString dps);

    /**
     * temporary debug function to display figure hierarchy
     * @param f
     * @param indent
     */
    static public void debugPrintFigureHierarchy(IFigure f, String indent) {
    	System.out.println(indent+f+"@"+f.hashCode()+"  "+f.getBounds()+" parent="+f.getParent());
    	for (Object child : f.getChildren()) {
    		debugPrintFigureHierarchy((IFigure)child, indent+"    ");
    	}
    }

    @Override
    public String toString() {
        return getClass().getSimpleName()+" "+figureName;
    }
}
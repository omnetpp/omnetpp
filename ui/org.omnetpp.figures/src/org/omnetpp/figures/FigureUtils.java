package org.omnetpp.figures;

import org.eclipse.draw2d.IFigure;

/**
 * A collection of figure-related utility functions
 * 
 * @author Andras
 */
public class FigureUtils {

    /**
     * Finds the root figure for any figure
     */
	static public IFigure getRootFigure(IFigure anyFigure) {
    	IFigure f = anyFigure;
    	while (f.getParent() != null)
    		f = f.getParent();
    	return f;
    }

    static public void debugPrintRootFigureHierarchy(IFigure anyFigure) {
    	debugPrintFigureHierarchy(getRootFigure(anyFigure), "");
    }

	/**
     * Debug function to display the figure hierarchy
     */
    static public void debugPrintFigureHierarchy(IFigure f, String indent) {
    	System.out.println(indent + f.getClass().getSimpleName() + " @"+f.hashCode() + "  " + f.getBounds());
    	for (Object child : f.getChildren()) {
    		debugPrintFigureHierarchy((IFigure)child, indent+".   ");
    	}
    }
}

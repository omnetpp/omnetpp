package org.omnetpp.figures;

import org.eclipse.core.resources.IMarker;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.IFigure;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;

/**
 * It is a generic 'Module' (anything that can have a display string and a connection)
 *
 * @author rhornig
 */
abstract public class ModuleFigure extends Figure {
	protected static final Image ICON_ERROR = ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_ERROR);
	protected static final Image ICON_WARNING = ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_WARNING);
	protected static final Image ICON_INFO = null;

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

	protected static Image getProblemImageFor(int severity) {
		Image image;
    	switch (severity) {
	    	case -1: image = null; break;
	    	case IMarker.SEVERITY_ERROR: image = ICON_ERROR; break;
	    	case IMarker.SEVERITY_WARNING: image = ICON_WARNING; break;
	    	case IMarker.SEVERITY_INFO: image = ICON_INFO; break;
	    	default: throw new RuntimeException("invalid severity value"); 
    	}
		return image;
	}
    
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
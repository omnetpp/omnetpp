package org.omnetpp.figures;

import org.eclipse.core.resources.IMarker;
import org.eclipse.draw2d.Figure;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.displaymodel.IDisplayString;

/**
 * It is the common base to all selectable figures in the editor.
 * (anything that can have a display string).
 * It can be annotated by an error marker.
 *
 * @author rhornig
 */
abstract public class NedFigure extends Figure {
	protected static final Image ICON_ERROR = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_ERROR_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_ERROR);
	protected static final Image ICON_WARNING = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_WARN_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_WARNING);
	protected static final Image ICON_INFO = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_INFO_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_INFO);

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

    @Override
    public String toString() {
        return getClass().getSimpleName()+" "+figureName;
    }
}
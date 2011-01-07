/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.core.runtime.Platform;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.FigureUtilities;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.TextUtilities;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.displaymodel.IDisplayString;


/**
 * Common base for ModuleTypeFigure, CompoundModuleFigure and ConnectionTypeFigure.
 *
 * @author rhornig
 */
abstract public class NedTypeFigure extends Figure implements IProblemDecorationSupport, ITooltipTextProvider {

	// This is a global hack to increase the text size returned by the textExtents function
	// on Windows, because it is too small if anti aliased fonts are used. Once 
	// textExtent works correctly it can be removed
	static {
		TextUtilities.INSTANCE = new TextUtilities() {
		    public Dimension getStringExtents(String s, Font f) {
		        Dimension stringExtents = FigureUtilities.getStringExtents(s, f);
				return Platform.OS_WIN32.equals(Platform.getOS()) ? stringExtents.expand(2,0) : stringExtents;
		    }

		    public Dimension getTextExtents(String s, Font f) {
		        Dimension textExtents = FigureUtilities.getTextExtents(s, f);
				return Platform.OS_WIN32.equals(Platform.getOS()) ? textExtents.expand(2,0) : textExtents;
		    }
		};
	}
	
	// FIXME move it to ImageFactory
	protected static final Image ICON_ERROR = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_ERROR_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_ERROR);
	protected static final Image ICON_WARNING = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_WARN_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_WARNING);
	protected static final Image ICON_INFO = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_INFO_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_INFO);

    protected Label nameFigure = new Label();
    protected ImageFigure problemMarkerFigure = new ImageFigure(); // FIXME create in on demand
    protected ITooltipTextProvider problemMarkerTextProvider;
    protected String tooltipText;

    private boolean isInterface; 
    private boolean isInnerType; 

    public NedTypeFigure() {
        // left align everything inside the figure
        nameFigure.setLabelAlignment(PositionConstants.LEFT);
        // text is on the east side of the icon
        nameFigure.setTextAlignment(PositionConstants.EAST);
        // and placed at the middle vertically
        nameFigure.setTextPlacement(PositionConstants.MIDDLE);
    }

    /**
     * Stores the display string, and updates the figure accordingly.
     * @param dps
     */
    abstract public void setDisplayString(IDisplayString dps);

	// FIXME move it to ImageFactory
    public static Image getProblemImageFor(int severity) {
		Image image;
    	switch (severity) {
	    	case -1: image = null; break;
	    	case SEVERITY_ERROR: image = ICON_ERROR; break;
	    	case SEVERITY_WARNING: image = ICON_WARNING; break;
	    	case SEVERITY_INFO: image = ICON_INFO; break;
	    	default: throw new RuntimeException("invalid severity value");
    	}
		return image;
	}

    
    public boolean isInterface() {
        return isInterface;
    }

    public void setInterface(boolean isInterface) {
        this.isInterface = isInterface;
    }

    public boolean isInnerType() {
        return isInnerType;
    }

    public void setInnerType(boolean isInnerType) {
        this.isInnerType = isInnerType;
    }

    public void setTooltipText(String tttext) {
    	tooltipText = tttext;
    }

	public String getTooltipText(int x, int y) {
		// if there is a problem marker and an associated tooltip text provider
		// and the cursor is over the marker, delegate to the problem marker text provider
		if (problemMarkerTextProvider != null && problemMarkerFigure != null) {
			Rectangle markerBounds = problemMarkerFigure.getBounds().getCopy();
			translateToAbsolute(markerBounds);
			if (markerBounds.contains(x, y)) {
				String text = problemMarkerTextProvider.getTooltipText(x, y);
				if (text != null)
					return text;
			}
		}
		return tooltipText;
	}

    public void setProblemDecoration(int maxSeverity, ITooltipTextProvider textProvider) {
        Image image = NedTypeFigure.getProblemImageFor(maxSeverity);
        if (image != null)
            problemMarkerFigure.setImage(image);
        problemMarkerFigure.setVisible(image != null);

        problemMarkerTextProvider = textProvider;
        repaint();
    }

    public void setName(String text) {
        nameFigure.setText(text);
    }

    public String getName() {
        return nameFigure.getText();
    }

    @Override
    public String toString() {
        return getClass().getSimpleName()+" "+nameFigure.getText();
    }
}
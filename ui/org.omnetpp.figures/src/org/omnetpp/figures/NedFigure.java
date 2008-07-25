package org.omnetpp.figures;

import org.eclipse.core.resources.IMarker;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.misc.IDirectEditSupport;
import org.omnetpp.figures.misc.LabelCellEditorLocator;

/**
 * It is the common base to all selectable figures in the editor.
 * Anything that can have a display string, name.
 * It can be annotated by an error marker and supports name changes by direct edit operation.
 *
 * @author rhornig
 */
abstract public class NedFigure extends Figure implements IDirectEditSupport {
	protected static final Image ICON_ERROR = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_ERROR_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_ERROR);
	protected static final Image ICON_WARNING = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_WARN_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_WARNING);
	protected static final Image ICON_INFO = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_INFO_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_INFO);

    private String tmpName;
    protected Label nameFigure = new Label();
    protected TooltipFigure tooltipFigure;
    protected ImageFigure problemMarkerFigure = new ImageFigure();
    protected TooltipFigure problemMarkerTooltipFigure;

    public NedFigure() {
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

	// Direct edit support
    public CellEditorLocator getDirectEditCellEditorLocator() {
        return new LabelCellEditorLocator(nameFigure);
    }

    public void showLabelUnderCellEditor(boolean visible) {
        // HACK to hide the text part only of the label
        if (!visible) {
            tmpName = nameFigure.getText();
            nameFigure.setText("");
        }
        else {
            if (StringUtils.isEmpty(nameFigure.getText()))
                nameFigure.setText(tmpName);
        }
        invalidate();
        validate();
    }

    protected void setTooltipText(String tttext) {
        if (StringUtils.isEmpty(tttext)) {
            setToolTip(null);
            tooltipFigure = null;
        }
        else {
            tooltipFigure = new TooltipFigure();
            setToolTip(tooltipFigure);
            tooltipFigure.setText(tttext);
            invalidate();
        }
    }

    /**
     * Display a "problem" image decoration on the submodule.
     * @param maxSeverity  any of the IMarker.SEVERITY_xxx constants, or -1 for none
     * @param textProvider callback to get the text to be displayed as a tooltip on hover event 
     */
    public void setProblemDecoration(int maxSeverity, TooltipFigure.ITextProvider textProvider) {
        Image image = NedFigure.getProblemImageFor(maxSeverity);
        if (image != null)
            problemMarkerFigure.setImage(image);
        problemMarkerFigure.setVisible(image != null);

        // set a tooltip text
        if (textProvider == null) {
            problemMarkerFigure.setToolTip(null);
            problemMarkerTooltipFigure = null;
        }
        else {
            problemMarkerTooltipFigure = new TooltipFigure();
            problemMarkerFigure.setToolTip(problemMarkerTooltipFigure);
            problemMarkerTooltipFigure.setTextProvider(textProvider);
        }
        invalidate();
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
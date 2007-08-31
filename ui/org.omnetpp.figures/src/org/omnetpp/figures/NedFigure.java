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
 * (anything that can have a display string).
 * It can be annotated by an error marker. And supports name changes by direct edit operation.
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

    public NedFigure() {
        nameFigure.setTextAlignment(PositionConstants.EAST);
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

    public String getName() {
        return nameFigure.getText();
    }

    public void showLabelUnderCellEditor(boolean visible) {
        // HACK to hide the text part only of the label
        if (!visible) {
            tmpName = nameFigure.getText();
            nameFigure.setText("");
        }
        else {
            if ("".equals(nameFigure.getText()))
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
     * @param severity  any of the IMarker.SEVERITY_xxx constants, or -1 for none
     */
    public void setProblemDecoration(int severity) {
        Image image = NedFigure.getProblemImageFor(severity); //TODO subclass this AND ModuleFigure from a common NedFigure!
        if (image != null)
            problemMarkerFigure.setImage(image);
        problemMarkerFigure.setVisible(image != null);
        invalidate(); //XXX needed?
    }

    public void setName(String text) {
        nameFigure.setText(text);
    }

    @Override
    public String toString() {
        return getClass().getSimpleName()+" "+nameFigure.getText();
    }
}
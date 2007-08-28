package org.omnetpp.figures;

import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;

import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.misc.IDirectEditSupport;
import org.omnetpp.figures.misc.LabelCellEditorLocator;

/**
 * Figure to represent top level components like simple modules, channels,
 * channel interfaces and module interfaces; EXCEPT compound modules which
 * have their own figure class.
 *
 * @author rhornig
 */
public class NedTypeFigure extends Label implements IDirectEditSupport {

    private String tmpName;
    private TooltipFigure tooltipFigure;
    private ImageFigure problemMarkerFigure = new ImageFigure();

    public NedTypeFigure() {
        setLayoutManager(new XYLayout());
        add(problemMarkerFigure, new Rectangle(-1,0,16,16)); //XXX hardcoded image size; need better positioning!
    }

    protected void setShape(Image img,
            String shape, int shapeWidth, int shapeHeight,
            Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {
        // handle defaults. if no size is given and no icon is present use a default icon
        if (img == null)
            img = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);

        setIcon(img);
    }

    /**
	 * Adjusts the image properties using a displayString object (except the location
	 * and size)
	 */
	public void setDisplayString(IDisplayString dps) {

        // shape support
        String imgSize = dps.getAsString(IDisplayString.Prop.IMAGESIZE);
        Image img = ImageFactory.getImage(
        		dps.getAsString(IDisplayString.Prop.IMAGE),
        		imgSize,
        		ColorFactory.asRGB(dps.getAsString(IDisplayString.Prop.IMAGECOLOR)),
        		dps.getAsInt(IDisplayString.Prop.IMAGECOLORPERCENTAGE,0));

        // set the figure properties
        setShape(img,
        		dps.getAsString(IDisplayString.Prop.SHAPE),
        		dps.getSize(null).width,
        		dps.getSize(null).height,
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.FILLCOL)),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.BORDERCOL)),
        		dps.getAsInt(IDisplayString.Prop.BORDERWIDTH, -1));

        invalidate();
	}

    public void setTooltipText(String tttext) {
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

    public CellEditorLocator getDirectEditCellEditorLocator() {
        return new LabelCellEditorLocator(this);
    }

    public String getDirectEditText() {
        return getText();
    }

    public void setDirectEditTextVisible(boolean visible) {
        // HACK to hide the text part only of the label
        if (!visible) {
            tmpName = getText();
            setText("");
        }
        else {
            if ("".equals(getText()))
                setText(tmpName);
        }
    }

    @Override
	public String toString() {
	    return getClass().getSimpleName()+" "+getText();
	}

    /**
     * Display a "problem" image decoration on the submodule.
     * @param severity  any of the IMarker.SEVERITY_xxx constants, or -1 for none
     */
    public void setProblemDecoration(int severity) {
    	Image image = ModuleFigure.getProblemImageFor(severity); //TODO subclass this AND ModuleFigure from a common NedFigure!
    	if (image != null)
    		problemMarkerFigure.setImage(image);
    	problemMarkerFigure.setVisible(image != null);
    	invalidate(); //XXX needed?
    }

}


package org.omnetpp.figures;

import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;

import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;

/**
 * Figure to represent type elements like simple modules, channels,
 * channel interfaces and module interfaces; EXCEPT compound modules which
 * have their own figure class.
 *
 * @author rhornig
 */
public class NedTypeFigure extends NedFigure {

    public NedTypeFigure() {
        setLayoutManager(new XYLayout());
        add(nameFigure, new Rectangle(0,0,-1,-1));
        add(problemMarkerFigure, new Rectangle(-1,0,16,16));
    }

    protected void setShape(Image img,
            String shape, int shapeWidth, int shapeHeight,
            Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {
        // handle defaults. if no size is given and no icon is present use a default icon
        if (img == null)
            img = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);

        nameFigure.setIcon(img);
    }

    /**
	 * Adjusts the image properties using a displayString object (except the location
	 * and size)
	 */
	@Override
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

}


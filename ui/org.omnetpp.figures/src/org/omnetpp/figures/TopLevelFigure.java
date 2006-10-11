package org.omnetpp.figures;

import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;

/**
 * @author rhornig
 * Figure to represent top level components like SImpleModule, CHannel,CHannel IF and Module IF
 */
public class TopLevelFigure extends Label {

    protected void setShape(Image img, 
            String shape, int shapeWidth, int shapeHeight, 
            Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {
        // handle defaults. if no size is given and no icon is present use a default icon
        if(shapeWidth <= 0 && shapeHeight <= 0 && img == null)
            img = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);
        
        setIcon(img);
    }

    /**
	 * Adjusts the image properties using a displayString object (except the location and size)
	 * @param dps
	 */
	public void setDisplayString(IDisplayString dps) {

        // shape support
        String imgSize = dps.getAsStringDef(IDisplayString.Prop.IMAGESIZE);
        Image img = ImageFactory.getImage(
        		dps.getAsStringDef(IDisplayString.Prop.IMAGE), 
        		imgSize,
        		ColorFactory.asRGB(dps.getAsStringDef(IDisplayString.Prop.IMAGECOLOR)),
        		dps.getAsIntDef(IDisplayString.Prop.IMAGECOLORPCT,0));

        // set the figure properties
        setShape(img, 
        		dps.getAsStringDef(IDisplayString.Prop.SHAPE), 
        		dps.getSize(null).width, 
        		dps.getSize(null).height,
        		ColorFactory.asColor(dps.getAsStringDef(IDisplayString.Prop.FILLCOL)),
        		ColorFactory.asColor(dps.getAsStringDef(IDisplayString.Prop.BORDERCOL)),
        		dps.getAsIntDef(IDisplayString.Prop.BORDERWIDTH, -1));



        invalidate();
	}
}

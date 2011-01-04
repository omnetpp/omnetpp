/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;


/**
 * A figure representing a compound module. Also displays a caption with the name and the default
 * image (if any) and an area where inner types can be added.
 *
 * @author rhornig
 */
public class CompoundModuleTypeFigure extends CompoundModuleFigure {


    public CompoundModuleTypeFigure() {
        super();
        // position the error marker above the problemMarker figure
        Layer nameHelperLayer = new Layer();
        nameHelperLayer.setLayoutManager(new XYLayout());
        nameHelperLayer.add(nameFigure, new Rectangle(0,0,-1,-1));
        nameHelperLayer.add(problemMarkerFigure, new Rectangle(-1,0,16,16));
        add(nameHelperLayer, 0); // place at the beginning
    }
    
	protected void setDefaultShape(Image img, String shape, int shapeWidth, int shapeHeight, Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {
        if (img == null)
            img = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);
		nameFigure.setIcon(img);
	}

	@Override
	public void setDisplayString(IDisplayString dps) {
		super.setDisplayString(dps);
		// setup the figure's properties
        // set the icon showing the default representation in the titlebar
        Image img = ImageFactory.getImage(
        		dps.getAsString(IDisplayString.Prop.IMAGE),
        		dps.getAsString(IDisplayString.Prop.IMAGE_SIZE),
        		ColorFactory.asRGB(dps.getAsString(IDisplayString.Prop.IMAGE_COLOR)),
        		dps.getAsInt(IDisplayString.Prop.IMAGE_COLOR_PERCENTAGE,0));
        setDefaultShape(img,
        		dps.getAsString(IDisplayString.Prop.SHAPE),
        		dps.getAsInt(IDisplayString.Prop.SHAPE_WIDTH, -1),
        		dps.getAsInt(IDisplayString.Prop.SHAPE_HEIGHT, -1),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.SHAPE_FILL_COLOR)),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.SHAPE_BORDER_COLOR)),
        		dps.getAsInt(IDisplayString.Prop.SHAPE_BORDER_WIDTH, -1));

        repaint();
	}
}

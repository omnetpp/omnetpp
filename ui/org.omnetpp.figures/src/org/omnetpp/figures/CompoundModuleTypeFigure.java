/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Insets;
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
// TODO border handling must be moved to the  CompoundModuleFigure class
// TODO CompoundModuleTypeFigure should be a composit? instead of inheriting from CompoundModuleFigure
public class CompoundModuleTypeFigure extends CompoundModuleFigure {
    protected IFigure innerTypeContainer;

    /**
     * Used at the left inner types compartment.
     */
    class InnerTypesBorder extends MarginBorder {
        InnerTypesBorder(int t, int l, int b, int r) {
            super(t, l, b, r);
        }

        @Override
        public void paint(IFigure f, Graphics g, Insets i) {
            Rectangle r = getPaintRectangle(f, i);
            g.setForegroundColor(ColorConstants.buttonDarker);
            int x = r.x + insets.left/4;
            g.drawLine(x, r.y, x, r.bottom());
        }
    }

    public CompoundModuleTypeFigure() {
        super();
        // position the error marker above the problemMarker figure
        Layer nameHelperLayer = new Layer();
        nameHelperLayer.setLayoutManager(new XYLayout());
        nameHelperLayer.add(nameFigure, new Rectangle(0,0,-1,-1));
        nameHelperLayer.add(problemMarkerFigure, new Rectangle(-1,0,16,16));
        add(nameHelperLayer,0);

        // create the container for the inner types
        innerTypeContainer = new Figure();
        ToolbarLayout typesLayout = new ToolbarLayout();
        typesLayout.setStretchMinorAxis(false);
        typesLayout.setSpacing(5);
        innerTypeContainer.setBorder(new InnerTypesBorder(0, 20, 0, 0 ));
        innerTypeContainer.setLayoutManager(typesLayout);
        add(innerTypeContainer,1);

        // add a compound module border / titlebar (FIXME: shoud be rather drawn in CompoundModuleFigure)
        mainContainer.setBorder(new CompoundModuleLineBorder());
    }

    public IFigure getInnerTypeContainer() {
        // this is the figure which is used to add inner types
        return innerTypeContainer;
    }

    /**
     * Helper function to return the current border
     */
    public CompoundModuleLineBorder getCompoundModuleBorder() {
    	return (CompoundModuleLineBorder)mainContainer.getBorder();
    }

    @Override
	protected void setBackground(Image img, String arrange,
			Color backgroundColor, Color borderColor, int borderWidth) {
    	super.setBackground(img, arrange, backgroundColor, borderColor,borderWidth);
    	// super has already set the module border color/width. fields so we set it on the border
		getCompoundModuleBorder().setColor(moduleBorderColor);
		getCompoundModuleBorder().setWidth(moduleBorderWidth);
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

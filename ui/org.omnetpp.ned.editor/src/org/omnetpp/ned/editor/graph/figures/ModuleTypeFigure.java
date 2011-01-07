/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.NedTypeFigure;
import org.omnetpp.ned.editor.graph.misc.IDirectEditSupport;
import org.omnetpp.ned.editor.graph.misc.LabelCellEditorLocator;

/**
 * Figure for a simple module or module interface NED type 
 *
 * @author andras
 */
public class ModuleTypeFigure extends NedTypeFigure implements IDirectEditSupport {
    protected ModuleTypeIconFigure iconFigure = new ModuleTypeIconFigure();
    protected String tmpName;

    public ModuleTypeFigure() {
        setLayoutManager(new XYLayout());
        add(nameFigure); 
        add(iconFigure);
        add(problemMarkerFigure, new Rectangle(-1, 0, 16, 16));
    }
    
    @Override
    protected boolean useLocalCoordinates() {
        return true;
    }

    /**
	 * Adjusts the figure properties using a displayString object
	 */
	public void setDisplayString(IDisplayString displayString) {
	    if (isInnerType())
	        iconFigure.setTargetSize(24, 24);  // "s" icons
	    else 
	        iconFigure.setTargetSize(40, 40); // normal icons
	    
        // image
        String imageSize = isInnerType() ? "s" : "n";
        Image image = ImageFactory.getImage(
                displayString.getAsString(IDisplayString.Prop.IMAGE),
                imageSize,
                ColorFactory.asRGB(displayString.getAsString(IDisplayString.Prop.IMAGE_COLOR)),
                displayString.getAsInt(IDisplayString.Prop.IMAGE_COLOR_PERCENTAGE,0));
        iconFigure.setImage(image);

        // shape
        Dimension size = displayString.getSize(1.0f);
        boolean widthExist = displayString.containsProperty(IDisplayString.Prop.SHAPE_WIDTH);
        boolean heightExist = displayString.containsProperty(IDisplayString.Prop.SHAPE_HEIGHT);

        // if one of the dimensions is missing use the other dimension instead
        if (!widthExist && heightExist)
            size.width = size.height;
        else if (widthExist && !heightExist)
            size.height = size.width;

        String shape = displayString.getAsString(IDisplayString.Prop.SHAPE);
        if (!displayString.containsTag(IDisplayString.Tag.b))
            shape = null;
        iconFigure.setShape(shape,
                size.width,
                size.height,
                ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.SHAPE_FILL_COLOR), ColorFactory.RED),
                ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.SHAPE_BORDER_COLOR), ColorFactory.RED),
                displayString.getAsInt(IDisplayString.Prop.SHAPE_BORDER_WIDTH, -1));

        // decoration image 
        iconFigure.setDecorationImage(
                ImageFactory.getImage(
                        displayString.getAsString(IDisplayString.Prop.IMAGE2),
                        null,
                        ColorFactory.asRGB(displayString.getAsString(IDisplayString.Prop.IMAGE2_COLOR)),
                        displayString.getAsInt(IDisplayString.Prop.IMAGE2_COLOR_PERCENTAGE,0)));

        setConstraint(iconFigure, new Rectangle(0, 0, -1, -1));
        setConstraint(nameFigure, new Rectangle(iconFigure.getPreferredSize().width+3, 2, -1, iconFigure.getPreferredSize().height));
        
        invalidateTree();
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
	
}

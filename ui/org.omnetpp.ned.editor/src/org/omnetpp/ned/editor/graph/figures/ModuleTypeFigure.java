/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.core.resources.IProject;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.NedEditorPlugin;

/**
 * Figure for a simple module or module interface NED type. Provides an icon in addition to the name
 * of the module.
 *
 * @author andras
 */
public class ModuleTypeFigure extends NedTypeFigure {
    // default icons
    protected static final Image IMG_DEFAULT_SIMPLE = NedEditorPlugin.getCachedImage("icons/type/simple.png");
    protected static final Image IMG_DEFAULT_SIMPLE_S = NedEditorPlugin.getCachedImage("icons/type/simple_s.png");
    protected static final Image IMG_DEFAULT_INTERFACE = NedEditorPlugin.getCachedImage("icons/type/interface.png");
    protected static final Image IMG_DEFAULT_INTERFACE_S = NedEditorPlugin.getCachedImage("icons/type/interface_s.png");

    protected NedIconFigure iconFigure = new NedIconFigure();

    public ModuleTypeFigure() {
        super();
        titleArea.add(iconFigure, 0); // the 0 index is needed so the problem image will be displayed above iconFigure (z-ordering)
    }

    /**
     * Adjusts the figure properties using a displayString object
     */
    public void setDisplayString(IDisplayString displayString, IProject project, float scale, float iconScale) {
        if (isInnerType())
            iconFigure.setTargetSize(24, 24);  // "s" icons
        else
            iconFigure.setTargetSize(40, 40); // normal icons

        // shape
        Dimension size = displayString.getSize().toPixels(scale);
        String shape = displayString.getAsString(IDisplayString.Prop.SHAPE);
        if (!displayString.containsTag(IDisplayString.Tag.b))
            shape = null;
        iconFigure.setShape(shape,
                size.width,
                size.height,
                ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.SHAPE_FILL_COLOR), ColorFactory.RED),
                ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.SHAPE_BORDER_COLOR), ColorFactory.RED),
                displayString.getAsInt(IDisplayString.Prop.SHAPE_BORDER_WIDTH, -1));

        // image
        String imageSize = isInnerType() ? "s" : "n";
        Image image = ImageFactory.of(project).getImage(
                displayString.getAsString(IDisplayString.Prop.IMAGE),
                imageSize,
                ColorFactory.asRGB(displayString.getAsString(IDisplayString.Prop.IMAGE_COLOR)),
                displayString.getAsInt(IDisplayString.Prop.IMAGE_COLOR_PERCENTAGE,0));

        // default image
        if (image == null && StringUtils.isEmpty(shape))
            image = getDefaultImage();

        iconFigure.setImage(image);

        // decoration image
        iconFigure.setDecorationImage(
                ImageFactory.of(project).getImage(
                        displayString.getAsString(IDisplayString.Prop.IMAGE2),
                        null,
                        ColorFactory.asRGB(displayString.getAsString(IDisplayString.Prop.IMAGE2_COLOR)),
                        displayString.getAsInt(IDisplayString.Prop.IMAGE2_COLOR_PERCENTAGE,0)));

        titleArea.setConstraint(iconFigure, new Rectangle(0, 0, -1, -1));
        titleArea.setConstraint(nameFigure, new Rectangle(iconFigure.getPreferredSize().width+3, 0, -1, iconFigure.getPreferredSize().height));

        invalidateTree();
    }

    /**
     * Return an image representing the type if no icon or shape is specified in display string
     */
    protected Image getDefaultImage() {
        Image image = isInterface() ?
                (isInnerType() ? IMG_DEFAULT_INTERFACE_S : IMG_DEFAULT_INTERFACE) :
                    (isInnerType() ? IMG_DEFAULT_SIMPLE_S : IMG_DEFAULT_SIMPLE);
        return image;
    }

}

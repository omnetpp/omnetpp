/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import static org.omnetpp.figures.CompoundModuleFigure.BORDER_SNAP_WIDTH;

import org.eclipse.core.resources.IProject;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.ned.editor.NedEditorPlugin;

/**
 * A figure representing a compound module type (or network). Displays a caption with the name and the default
 * image (if any) and an area where inner types can be added plus the area used to display submodules
 * (inner types compartment is inserted on demand from the TypesEditPart)
 *
 * @author rhornig
 */
public class CompoundModuleTypeFigure extends ModuleTypeFigure implements HandleBounds {
    protected static final Image IMG_DEFAULT_COMPOUND = NedEditorPlugin.getCachedImage("icons/type/compound.png");
    protected static final Image IMG_DEFAULT_COMPOUND_S = NedEditorPlugin.getCachedImage("icons/type/compound_s.png");
    protected static final Image IMG_DEFAULT_NETWORK = NedEditorPlugin.getCachedImage("icons/type/network.png");
    protected static final Image IMG_DEFAULT_NETWORK_S = NedEditorPlugin.getCachedImage("icons/type/network_s.png");

    protected boolean isNetwork;
    protected CompoundModuleFigure submoduleArea = new CompoundModuleFigure();

    /**
     * A figure that contains the inner types inside a compound module type
     *
     */
    public static class InnerTypesFigure extends Figure {
        /**
         * A border that draws a line at the left side of the figure
         */
        public static class InnerTypesBorder extends MarginBorder {
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

        public InnerTypesFigure() {
            setBorder(new InnerTypesBorder(0, 20, 0, 0 ));
            ToolbarLayout typesLayout = new ToolbarLayout();
            typesLayout.setStretchMinorAxis(false);
            typesLayout.setSpacing(5);
            setLayoutManager(typesLayout);
        }
    }

    // ----------------------------------------------------------
    public CompoundModuleTypeFigure() {
        super();
        add(submoduleArea);
    }

    public CompoundModuleFigure getSubmoduleArea() {
        return submoduleArea;
    }

    public boolean isNetwork() {
        return isNetwork;
    }

    public void setNetwork(boolean isNetwork) {
        this.isNetwork = isNetwork;
    }

    @Override
    protected Image getDefaultImage() {
        Image image = isNetwork() ?
                (isInnerType() ? IMG_DEFAULT_NETWORK_S : IMG_DEFAULT_NETWORK) :
                    (isInnerType() ? IMG_DEFAULT_COMPOUND_S : IMG_DEFAULT_COMPOUND);
        return image;
    }

    @Override
    public void setDisplayString(IDisplayString displayString, IProject project, float scale) {
        // set the properties for the submodules compartment
        getSubmoduleArea().setDisplayString(displayString, scale);
        // set the properties for the icon in the title
        super.setDisplayString(displayString, project, scale);
    }

    /**
     * Returns whether the point is on the border area, where dragging and selection
     * and connection start/end is possible. Border are includes the title of the compound
     * module but EXCLUDES the types area where inner types are shown.
     * Coordinates are viewport relative.
     */
    public boolean isOnBorder(int x, int y) {
        // translate the mouse coordinates to be relative to the root figure
        Point mouse = new Point(x,y);
        translateToRelative(mouse);

        int borderWidth = submoduleArea.getBorder().getWidth();
        Rectangle submoduleAreaRectInner = getHandleBounds().shrink(borderWidth, borderWidth).shrink(2*BORDER_SNAP_WIDTH, 2*BORDER_SNAP_WIDTH);
        Rectangle titleAreaOuter = titleArea.getBounds().getCopy();
        translateToParent(titleAreaOuter);  // because we are using local coordinates

        // either on the title area or on the border of compound module figure (submodule area)
        return titleAreaOuter.contains(mouse) ||
                (getHandleBounds().contains(mouse) && !submoduleAreaRectInner.contains(mouse));
    }

    public Rectangle getHandleBounds() {
        Rectangle clientArea = submoduleArea.getHandleBounds();
        translateToParent(clientArea);  // because we are using local coordinates
        return clientArea;
    }

}

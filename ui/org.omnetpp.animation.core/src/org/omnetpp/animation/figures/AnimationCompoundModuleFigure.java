/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.figures;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.common.image.ImageFactory;

/**
 * TODO: subclass from WindowFigure
 *
 * @author levy
 */
public class AnimationCompoundModuleFigure extends Figure {
    protected static final int TITLE_HEIGHT = 20;

    private AnimationController animationController;

    private IFigure compoundModuleFigure;

    private int moduleId;

    private Label title;

    private ImageFigure closer;

    public AnimationCompoundModuleFigure(AnimationController animationController, IFigure compoundModuleFigure, int moduleId, String moduleName) {
        this.animationController = animationController;
        this.compoundModuleFigure = compoundModuleFigure;
        this.moduleId = moduleId;
        this.title = new Label(moduleName);
        this.closer = new ImageFigure(ImageFactory.global().getImage(ImageFactory.DEFAULT_ERROR));
        setLayoutManager(new XYLayout());
        add(title);
        setConstraint(title, new Rectangle(4, 2, -1, -1));
        add(closer);
        closer.setSize(bounds.width, bounds.height);
        closer.addMouseListener(new MouseListener() {
            public void mousePressed(MouseEvent me) {
                removeFigure();
                AnimationCompoundModuleFigure.this.animationController.clearInternalState();
            }

            public void mouseReleased(MouseEvent me) {
            }

            public void mouseDoubleClicked(MouseEvent me) {
            }
        });
        setCompoundModuleFigure(compoundModuleFigure);
    }

    public int getModuleId() {
        return moduleId;
    }

    public void setCompoundModuleFigure(IFigure compoundModuleFigure) {
        if (getChildren().contains(this.compoundModuleFigure))
            remove(this.compoundModuleFigure);
        if (compoundModuleFigure != null) {
            add(compoundModuleFigure);
            setConstraint(compoundModuleFigure, new Rectangle(0, TITLE_HEIGHT, -1, -1));
        }
        this.compoundModuleFigure = compoundModuleFigure;
    }

    @Override
    public void paint(Graphics graphics) {
        org.eclipse.swt.graphics.Rectangle imageBounds = closer.getImage().getBounds();
        int y = (TITLE_HEIGHT - imageBounds.height) / 2;
        setConstraint(closer, new Rectangle(getSize().width - imageBounds.width - y, y, -1, -1));
        Rectangle headerRectangle = getClientArea();
        headerRectangle.height = TITLE_HEIGHT;
        graphics.setBackgroundColor(Display.getDefault().getSystemColor(SWT.COLOR_TITLE_BACKGROUND));
        graphics.fillRectangle(headerRectangle);
        graphics.drawRectangle(headerRectangle.x, headerRectangle.y, headerRectangle.width - 1, headerRectangle.height);
        super.paint(graphics);
    }

    private void removeFigure() {
        animationController.stopAnimation();
        long eventNumber = animationController.getCurrentEventNumber();
        animationController.getAnimationCanvas().removeShownCompoundModule(moduleId);
        getParent().remove(this);
        animationController.gotoEventNumber(eventNumber);
    }
}

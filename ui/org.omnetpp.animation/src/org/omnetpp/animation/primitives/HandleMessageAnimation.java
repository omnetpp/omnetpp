/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.omnetpp.animation.AnimationPlugin;
import org.omnetpp.animation.editors.AnimationContributor;
import org.omnetpp.animation.figures.SelectableImageFigure;
import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.animation.widgets.AnimationPosition;

public class HandleMessageAnimation extends AbstractAnimationPrimitive {
    protected static Image image = new Image(null, AnimationPlugin.getImageDescriptor(AnimationContributor.IMAGE_HANDLE_MESSAGE).getImageData());

    protected long eventNumber;

	protected int moduleId;

    protected SelectableImageFigure imageFigure;

	public HandleMessageAnimation(AnimationController animationController, long eventNumber, int moduleId) {
		super(animationController);
		this.eventNumber = eventNumber;
		this.moduleId = moduleId;
        createFigures();
    }

    private void createFigures() {
        Rectangle bounds = image.getBounds();
        imageFigure = new SelectableImageFigure(image);
        imageFigure.setSize(bounds.width, bounds.height);
        imageFigure.addMouseListener(new MouseListener() {
            public void mouseDoubleClicked(MouseEvent me) {
            }

            public void mousePressed(MouseEvent me) {
            }

            public void mouseReleased(MouseEvent me) {
                animationController.getAnimationCanvas().setSelectedElement(imageFigure, eventNumber);
            }
        });
    }

	@Override
	public void activate() {
	    super.activate();
	    if (moduleId != 1)
	        getDecorationLayer().add(imageFigure);
	}

	@Override
	public void deactivate() {
        super.deactivate();
        if (moduleId != 1)
            getDecorationLayer().remove(imageFigure);
	}

	@Override
	public void refreshAnimation(AnimationPosition animtionPosition) {
	    if (moduleId != 1) {
            Point location = getSubmoduleFigure(moduleId).getBounds().getLocation();
            Dimension size = imageFigure.getSize();
            imageFigure.setLocation(location.translate(0, -size.height));
	    }
	}

    protected Layer getDecorationLayer() {
        return getEnclosingCompoundModuleFigure(moduleId).getForegroundDecorationLayer();
    }
}

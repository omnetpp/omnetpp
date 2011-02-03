/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
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
import org.omnetpp.common.eventlog.EventLogMessage;

public class SendSelfMessageAnimation extends AbstractAnimationPrimitive {
    protected static Image image = new Image(null, AnimationPlugin.getImageDescriptor(AnimationContributor.IMAGE_SEND_SELF_MESSAGE). getImageData());

    protected int moduleId;

    protected EventLogMessage message;

    protected ImageFigure imageFigure;

    public SendSelfMessageAnimation(AnimationController animationEnvironment, int moduleId, EventLogMessage message) {
		super(animationEnvironment);
        this.message = message;
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
                animationController.getAnimationCanvas().setSelectedElement(imageFigure, message);
            }
        });
    }

	@Override
	public void activate() {
        super.activate();
	    getDecorationLayer().add(imageFigure);
	}

	@Override
	public void deactivate() {
        super.deactivate();
        getDecorationLayer().remove(imageFigure);
	}

	@Override
	public void refreshAnimation(AnimationPosition animtionPosition) {
        Point location = getSubmoduleFigure(moduleId).getBounds().getLocation();
        Dimension size = imageFigure.getSize();
        imageFigure.setLocation(location.translate(-size.width, -size.height));
        imageFigure.setToolTip(new Label("Timer expires at " + message.getArrivalTime() + "s, after " + message.getArrivalTime().subtract(animationController.getSimulationTime()) + "s"));
        double alpha = (animationController.getAnimationTime() - getBeginAnimationTime()) / getAnimationTimeDuration();
        // TODO: add a small line segment that shrinks as time goes by
	}

    protected Layer getDecorationLayer() {
        return getEnclosingCompoundModuleFigure(moduleId).getForegroundDecorationLayer();
    }
}

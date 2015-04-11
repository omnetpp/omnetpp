/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.omnetpp.animation.AnimationCorePlugin;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.editors.AnimationContributor;
import org.omnetpp.animation.figures.Completion;
import org.omnetpp.animation.figures.SelectableImageFigure;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.MessageModel;

public class SendSelfMessageAnimation extends AbstractAnimationPrimitive {
    protected static Image image = new Image(null, AnimationCorePlugin.getImageDescriptor(AnimationContributor.IMAGE_SEND_SELF_MESSAGE).getImageData());

    protected int moduleId;

    protected MessageModel message;

    protected SelectableImageFigure imageFigure;

    protected Completion messageSendCompletion;

    protected double messageHighlightAnimationDuration;

    public SendSelfMessageAnimation(AnimationController animationController, int moduleId, MessageModel message, double messageHighlightAnimationDuration) {
		super(animationController);
        this.message = message;
		this.moduleId = moduleId;
		this.messageHighlightAnimationDuration = messageHighlightAnimationDuration;
        createFigures();
	}

    private void createFigures() {
        Rectangle bounds = image.getBounds();
        MouseListener mouseListener = new MouseListener() {
            public void mouseDoubleClicked(MouseEvent me) {
            }

            public void mousePressed(MouseEvent me) {
            }

            public void mouseReleased(MouseEvent me) {
                animationController.getAnimationCanvas().setSelectedElement(imageFigure, message);
            }
        };
        imageFigure = new SelectableImageFigure(image);
        imageFigure.setSize(bounds.width, bounds.height);
        imageFigure.addMouseListener(mouseListener);
        messageSendCompletion = new Completion(true, true, true, 0, ColorFactory.BLACK);
        messageSendCompletion.setSize(bounds.width - 7, bounds.height - 7);
        messageSendCompletion.addMouseListener(mouseListener);
    }

	@Override
	public void activate() {
        super.activate();
	    Layer decorationLayer = getDecorationLayer();
        decorationLayer.add(imageFigure);
        decorationLayer.add(messageSendCompletion);
	}

	@Override
	public void deactivate() {
        super.deactivate();
        Layer decorationLayer = getDecorationLayer();
        decorationLayer.remove(imageFigure);
        decorationLayer.remove(messageSendCompletion);
	}

	@Override
	public void refreshAnimation(AnimationPosition animtionPosition) {
        boolean visible = animationController.getCurrentAnimationTime() < getOriginRelativeEndAnimationTime() - messageHighlightAnimationDuration ||
                          2 * animationController.getCurrentRealTime() - Math.floor(2 * animationController.getCurrentRealTime()) < 0.5;
        imageFigure.setVisible(visible);
        messageSendCompletion.setVisible(visible);
        if (visible) {
            Dimension size = imageFigure.getSize();
            Point location = getSubmoduleFigure(moduleId).getBounds().getLocation().translate(-size.width, -size.height);
            Label tooltip = new Label("Timer " + message.getName() + " expires at " + message.getArrivalTime() + "s, after " + message.getArrivalTime().subtract(animationController.getCurrentSimulationTime()) + "s");
            imageFigure.setLocation(location);
            imageFigure.setToolTip(tooltip);
            BigDecimal simulationTimeDuration = getSimulationTimeDuration();
            double completion = simulationTimeDuration.equals(BigDecimal.getZero()) ? 0 : 1 - animationController.getCurrentSimulationTime().subtract(getBeginSimulationTime()).doubleValue() / simulationTimeDuration.doubleValue();
            messageSendCompletion.setLocation(location.translate(3, 4));
            messageSendCompletion.setAlpha(64);
            messageSendCompletion.setCompletion(completion);
            messageSendCompletion.setToolTip(tooltip);
        }
	}

    protected Layer getDecorationLayer() {
        return getEnclosingCompoundModuleFigure(moduleId).getForegroundDecorationLayer();
    }
}

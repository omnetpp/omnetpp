/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.ModuleModel;
import org.omnetpp.common.util.Timer;
import org.omnetpp.figures.SubmoduleFigure;

public class BubbleAnimation extends AbstractInstantaneousAnimation {
	protected int moduleId;

	protected RoundedRectangle background;

	protected Label label;

	protected Timer bubbleTimer;

	protected String text;

	public BubbleAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime, String text, int moduleId) {
		super(animationController, eventNumber, simulationTime);
		this.background = new RoundedRectangle();
		this.text = text;
		this.label = new Label(text);
		this.moduleId = moduleId;
		this.bubbleTimer = new Timer(3000, false, false) {
			@Override
            public void run() {
				if (background.getParent() != null)
					removeBubble();
				getTimerQueue().removeTimer(bubbleTimer);
			}
		};
	}

	@Override
	public void activate() {
        super.activate();
        // NOTE: do not show the bubble if we are jumping around
        if (animationController.isRunning() || animationController.getCurrentAnimationPosition().equals(getBeginAnimationPosition())) {
            // NOTE: avoids a previously scheduled timer to kick in
            getTimerQueue().removeTimer(bubbleTimer);
			addBubble();
        }
	}

	@Override
	public void deactivate() {
        super.deactivate();
        if (label.getParent() != null) {
		    if (animationController.isRunning() || animationController.getCurrentAnimationPosition().equals(getEndAnimationPosition())) {
    			bubbleTimer.reset();
    			getTimerQueue().addTimer(bubbleTimer);
		    }
		    else
                removeBubble();
        }
	}

	protected ModuleModel getModule() {
		return animationController.getSimulation().getModuleById(moduleId);
	}

	protected void addFigure(IFigure figure) {
		getRootFigure().add(figure);
	}

	protected void removeFigure(IFigure figure) {
		getRootFigure().remove(figure);
	}

    protected void addBubble() {
        addFigure(background);
        addFigure(label);
    }

    protected void removeBubble() {
        removeFigure(background);
        removeFigure(label);
    }

	@Override
	public void refreshAnimation(AnimationPosition animationPosition) {
        ModuleModel module = getModule();
        SubmoduleFigure moduleFigure = (SubmoduleFigure)animationController.getFigure(module, SubmoduleFigure.class);
        Dimension size = new Dimension(10 * text.length(), 20);
        Rectangle r = new Rectangle(moduleFigure.getLocation().translate(0, -20), size);
        LayoutManager layoutManager = getLayoutManager();
        layoutManager.setConstraint(background, r);
        layoutManager.setConstraint(label, r);
	}
}

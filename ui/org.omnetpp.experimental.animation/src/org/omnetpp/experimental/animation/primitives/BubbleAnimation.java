/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.controller.Timer;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class BubbleAnimation extends AbstractAnimationPrimitive {
	protected int moduleId;

	protected RoundedRectangle background;

	protected Label label;

	protected Timer bubbleTimer;

	protected String text;

	public BubbleAnimation(ReplayAnimationController animationController,
						   AnimationPosition animationPosition,
						   String text,
						   int moduleId) {
		super(animationController, animationPosition);
		this.background = new RoundedRectangle();
		this.text = text;
		this.label = new Label(text);
		this.moduleId = moduleId;
		this.bubbleTimer = new Timer(3000, false, false) {
			public void run() {
				if (background.getParent() != null) {
					removeFigure(background);
					removeFigure(label);
				}

				getTimerQueue().removeTimer(bubbleTimer);
			}
		};
	}

	@Override
	public void redo() {
		IRuntimeModule module = getModule();
		IRuntimeModule parentModule = module.getParentModule();

		if (parentModule == animationEnvironment.getSimulation().getRootModule()) {
			addFigure(background);
			addFigure(label);

			ModuleFigure moduleFigure = (ModuleFigure)animationEnvironment.getFigure(module);
			Rectangle r = new Rectangle(moduleFigure.getLocation(), new Dimension(10 * text.length(), 20));
			setConstraint(background, r);
			setConstraint(label, r);
		}
	}

	@Override
	public void undo() {
		IRuntimeModule module = getModule();
		IRuntimeModule parentModule = module.getParentModule();

		if (parentModule == animationEnvironment.getSimulation().getRootModule()) {
			bubbleTimer.reset();
			getTimerQueue().addTimer(bubbleTimer);
		}
	}

	protected IRuntimeModule getModule() {
		return animationEnvironment.getSimulation().getModuleByID(moduleId);
	}

	//FIXME to be replaced by specific calls
	protected IFigure addFigure(IFigure figure) {
		getRootFigure().add(figure);

		return figure;
	}

	//FIXME to be replaced by specific calls
	protected IFigure removeFigure(IFigure figure) {
		getRootFigure().remove(figure);

		return figure;
	}
}

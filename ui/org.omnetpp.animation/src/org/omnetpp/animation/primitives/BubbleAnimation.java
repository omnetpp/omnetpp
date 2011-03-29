/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

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
import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.common.eventlog.EventLogModule;
import org.omnetpp.common.util.Timer;
import org.omnetpp.figures.SubmoduleFigure;

public class BubbleAnimation extends AbstractAnimationPrimitive {
	protected int moduleId;

	protected RoundedRectangle background;

	protected Label label;

	protected Timer bubbleTimer;

	protected String text;

	public BubbleAnimation(AnimationController animationController, String text, int moduleId) {
		super(animationController);
		this.background = new RoundedRectangle();
		this.text = text;
		this.label = new Label(text);
		this.moduleId = moduleId;
		this.bubbleTimer = new Timer(3000, false, false) {
			@Override
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
	public void activate() {
        super.activate();
		EventLogModule module = getModule();
		EventLogModule parentModule = module.getParentModule();

		if (parentModule == animationController.getAnimationSimulation().getRootModule()) {
			addFigure(background);
			addFigure(label);

			SubmoduleFigure moduleFigure = (SubmoduleFigure)animationController.getFigure(module, SubmoduleFigure.class);
			Dimension size = new Dimension(10 * text.length(), 20);
            Rectangle r = new Rectangle(moduleFigure.getLocation().translate(0, -20), size);
            LayoutManager layoutManager = getLayoutManager();
            layoutManager.setConstraint(background, r);
            layoutManager.setConstraint(label, r);
		}
	}

	@Override
	public void deactivate() {
        super.deactivate();
		EventLogModule module = getModule();
		EventLogModule parentModule = module.getParentModule();

		if (parentModule == animationController.getAnimationSimulation().getRootModule()) {
			bubbleTimer.reset();
			getTimerQueue().addTimer(bubbleTimer);
		}
	}

	protected EventLogModule getModule() {
		return animationController.getAnimationSimulation().getModuleById(moduleId);
	}

	protected void addFigure(IFigure figure) {
		getRootFigure().add(figure);
	}

	protected void removeFigure(IFigure figure) {
		getRootFigure().remove(figure);
	}
}

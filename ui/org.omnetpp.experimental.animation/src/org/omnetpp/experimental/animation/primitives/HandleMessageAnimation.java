/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.simulation.model.IRuntimeMessage;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class HandleMessageAnimation extends AbstractAnimationPrimitive {
	protected IRuntimeMessage message;

	protected Ellipse ellipse;

	protected int moduleId;

	public HandleMessageAnimation(ReplayAnimationController animationController,
								  AnimationPosition animationPosition,
								  int moduleId,
								  IRuntimeMessage message) {
		super(animationController, animationPosition);
		this.moduleId = moduleId;
		this.message = message;
		this.ellipse = new Ellipse();
		this.ellipse.setBackgroundColor(new Color(null, 0, 128, 0));
	}

	@Override
	public double getEndAnimationTime() {
		return getBeginAnimationTime();
// FIXME: this does not return the correct value during loading		return animationEnvironment.getAnimationTimeForEventNumber(eventNumber + 1);
	}

	@Override
	public void redo() {
//		if (getModule().getParentModule() == animationEnvironment.getSimulation().getRootModule())  //FIXME 
//			addFigure(ellipse);
	}

	@Override
	public void undo() {
//		if (getModule().getParentModule() == animationEnvironment.getSimulation().getRootModule())  //FIXME 
//			removeFigure(ellipse);
	}

	@Override
	public void animateAt(AnimationPosition animtionPosition) {
		IRuntimeModule module = getModule();
		if (module.getParentModule()==animationEnvironment.getSimulation().getRootModule()) { //FIXME
			ModuleFigure moduleFigure = (ModuleFigure)animationEnvironment.getFigure(module);
			setConstraint(ellipse, new Rectangle(moduleFigure.getLocation().x + 3, moduleFigure.getLocation().y + 3, 7, 7));
		}
	}

	public IRuntimeMessage getMessage() {
		return message;
	}

	protected IRuntimeModule getModule() {
		return animationEnvironment.getSimulation().getModuleByID(moduleId);
	}
}

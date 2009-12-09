/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class ScheduleSelfMessageAnimation extends AbstractAnimationPrimitive {
	protected double endSimulationTime;

	public ScheduleSelfMessageAnimation(ReplayAnimationController animationController,
										AnimationPosition animationPosition,
										double endSimulationTime) {
		super(animationController, animationPosition);
		this.endSimulationTime = endSimulationTime;
	}

	@Override
	public double getEndSimulationTime() {
		return endSimulationTime;
	}

	@Override
	public double getEndAnimationTime() {
		return animationEnvironment.getAnimationTimeForSimulationTime(endSimulationTime);
	}

	@Override
	public void redo() {
		// TODO:
	}

	@Override
	public void undo() {
		// TODO:
	}

	@Override
	public void animateAt(AnimationPosition animtionPosition) {
		// TODO:
	}
}

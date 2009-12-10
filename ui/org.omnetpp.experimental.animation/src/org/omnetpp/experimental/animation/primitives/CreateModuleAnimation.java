/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;
import org.omnetpp.experimental.animation.replay.model.ReplaySimulation;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned.model.DisplayString;

public class CreateModuleAnimation extends AbstractInfiniteAnimation {
	protected ReplayModule module;

	protected int parentModuleId;

	public CreateModuleAnimation(ReplayAnimationController animationController,
								 AnimationPosition animationPosition,
								 ReplayModule module,
								 int parentModuleId) {
		super(animationController, animationPosition);
		this.module = module;
		this.parentModuleId = parentModuleId;
	}

	@Override
	public void redo() {
		ReplayModule parentModule = getParentModule();
		if (parentModule == getSimulation().getRootModule()) { //FIXME
			SubmoduleFigure submoduleFigure = new SubmoduleFigure();
			submoduleFigure.addMouseListener(new MouseListener() {
				public void mouseDoubleClicked(MouseEvent me) {
					// TODO: open new canvas here
				}

				public void mousePressed(MouseEvent me) {
				}

				public void mouseReleased(MouseEvent me) {
				}
			});
			animationEnvironment.setFigure(module, submoduleFigure);
			getCompoundModuleFigure(parentModule).addSubmoduleFigure(submoduleFigure);
			submoduleFigure.setDisplayString(new DisplayString(""));
			submoduleFigure.setName(getReplayModule().getFullName());

		}

		if (parentModule != null)
			parentModule.addSubmodule(getReplayModule());

		getReplaySimulation().addModule(getReplayModule());
	}

	@Override
	public void undo() {
		ReplayModule parentModule = getParentModule();
		if (parentModule != null) {
			SubmoduleFigure submoduleFigure = (SubmoduleFigure)animationEnvironment.getFigure(module);
			getCompoundModuleFigure(parentModule).removeSubmoduleFigure(submoduleFigure);
		}


		if (parentModule != null)
			parentModule.removeSubmodule(getReplayModule());

		animationEnvironment.setFigure(module, null); //XXX move inside if???
		getReplaySimulation().removeModule(module.getId());
	}

	protected ReplaySimulation getReplaySimulation() {
		return (ReplaySimulation)animationEnvironment.getSimulation();
	}

	protected ReplayModule getReplayModule() {
		return (ReplayModule)module;
	}

	protected ReplayModule getParentModule() {
		return getReplaySimulation().getModuleByID(parentModuleId);
	}
}

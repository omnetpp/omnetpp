package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;
import org.omnetpp.experimental.animation.replay.model.ReplaySimulation;
import org.omnetpp.figures.SubmoduleFigure;

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
			SubmoduleFigure figure = new SubmoduleFigure();
			animationEnvironment.setFigure(module, figure);
			getCompoundModuleFigure(parentModule).addSubmoduleFigure(figure);
			figure.setDisplayString(new DisplayString(null, null, ""));
			figure.setName(getReplayModule().getFullName());

			parentModule.addSubmodule(getReplayModule());
		}

		getReplaySimulation().addModule(getReplayModule());
	}

	@Override
	public void undo() {
		ReplayModule parentModule = getParentModule();
		if (parentModule != null) {
			SubmoduleFigure moduleFigure = (SubmoduleFigure)animationEnvironment.getFigure(module);
			getCompoundModuleFigure(parentModule).removeSubmoduleFigure(moduleFigure);
			parentModule.removeSubmodule(getReplayModule());
		}

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

package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.layout.SubmoduleConstraint;

public class SetModuleDisplayStringAnimation extends AbstractAnimationPrimitive {
	private int moduleId;
	
	private IDisplayString displayString;
	
	private IDisplayString oldDisplayString; // FIXME: this is a temproray hack to be able to undo changes
	
	public SetModuleDisplayStringAnimation(ReplayAnimationController animationController,
									 long eventNumber,
									 double simulationTime,
									 long animationNumber,
									 int moduleId,
									 IDisplayString displayString) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.moduleId = moduleId;
		this.displayString = displayString;
		//System.out.println("SetModuleDisplayStringAnimation: "+displayString);
	}
	
	@Override
	public double getEndAnimationTime() {
		return Double.MAX_VALUE;
	}
	
	public void redo() {
		IRuntimeModule module = animationEnvironment.getSimulation().getModuleByID(moduleId);
		if (module.getParentModule() == animationEnvironment.getSimulation().getRootModule()) { //FIXME
			SubmoduleFigure moduleFigure = (SubmoduleFigure)animationEnvironment.getFigure(module);
			SubmoduleConstraint submoduleConstraint = new SubmoduleConstraint(displayString);
			submoduleConstraint.setVectorName(module.getFullPath());
			submoduleConstraint.setVectorSize(module.getSize());
			submoduleConstraint.setVectorIndex(module.getIndex());
			
			oldDisplayString = moduleFigure.getLastDisplayString();

			moduleFigure.setConstraint(submoduleConstraint);
			moduleFigure.setDisplayString(displayString);
		}
	}

	public void undo() {
		IRuntimeModule module = animationEnvironment.getSimulation().getModuleByID(moduleId);
		if (module.getParentModule() == animationEnvironment.getSimulation().getRootModule()) { //FIXME
			SubmoduleFigure moduleFigure = (SubmoduleFigure)animationEnvironment.getFigure(module);
			moduleFigure.setDisplayString(oldDisplayString); // FIXME: this is a temproray hack to be able to undo changes
		}
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		// void
	}
}

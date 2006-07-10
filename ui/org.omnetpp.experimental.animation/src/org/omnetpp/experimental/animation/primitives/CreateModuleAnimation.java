package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.figures.SubmoduleFigure;

public class CreateModuleAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;
	
	private SubmoduleFigure moduleFigure;

	public CreateModuleAnimation(ReplayAnimationController animationController,
								 double beginSimulationTime,
								 IRuntimeModule module) {
		super(animationController, beginSimulationTime);
		this.module = module;
		this.moduleFigure = new SubmoduleFigure();
	}
	
	public void gotoSimulationTime(double simulationTime) {
		if (simulationTime >= beginSimulationTime) {
			animationController.setFigure(module, moduleFigure);
			animationController.setFigure(new GateId(module.getId(), 0), new GateAnchor(moduleFigure, "in"));
			animationController.setFigure(new GateId(module.getId(), 1), new GateAnchor(moduleFigure, "out"));
			showFigure(moduleFigure);
		}
		else {
			hideFigure(moduleFigure);
		}
	}
}

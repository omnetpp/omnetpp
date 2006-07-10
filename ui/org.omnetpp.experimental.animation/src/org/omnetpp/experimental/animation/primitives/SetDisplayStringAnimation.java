package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;

public class SetDisplayStringAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;
	
	private String displayString;
	
	public SetDisplayStringAnimation(ReplayAnimationController animationController,
								 double beginSimulationTime,
								 IRuntimeModule module,
								 String displayString) {
		super(animationController, beginSimulationTime);
		this.module = module;
		this.displayString = displayString;
	}
	
	public void gotoSimulationTime(double simulationTime) {
		if (simulationTime >= beginSimulationTime) {
			SubmoduleFigure moduleFigure = (SubmoduleFigure)animationController.getFigure(module);
			moduleFigure.setDisplayString(new DisplayString(null, null, displayString));
			System.out.println(displayString);
		}
		else {
			// TODO: or what to do?
		}
	}
}

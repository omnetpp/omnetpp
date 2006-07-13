package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;

public class SetModuleDisplayStringAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;
	
	private DisplayString displayString;
	
	public SetModuleDisplayStringAnimation(ReplayAnimationController animationController,
									 long eventNumber,
									 double simulationTime,
									 long animationNumber,
									 IRuntimeModule module,
									 String displayString) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.module = module;
		this.displayString = new DisplayString(null, null, displayString);
		//System.out.println(displayString);
	}
	
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		if (simulationTime >= beginSimulationTime) {
			SubmoduleFigure moduleFigure = (SubmoduleFigure)animationEnvironment.getFigure(module);
			moduleFigure.setDisplayString(displayString);
		}
		else {
			// TODO: or what to do?
		}
	}
}

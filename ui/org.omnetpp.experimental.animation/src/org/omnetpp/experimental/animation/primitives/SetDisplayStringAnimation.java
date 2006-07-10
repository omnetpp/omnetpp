package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;

public class SetDisplayStringAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;
	
	private DisplayString displayString;
	
	public SetDisplayStringAnimation(ReplayAnimationController animationController,
									 long eventNumber,
									 double beginSimulationTime,
									 IRuntimeModule module,
									 String displayString) {
		super(animationController, eventNumber, beginSimulationTime);
		this.module = module;
		this.displayString = new DisplayString(null, null, displayString);
		//System.out.println(displayString);
	}
	
	public void animateAt(long eventNumber, double simulationTime) {
		if (simulationTime >= beginSimulationTime) {
			SubmoduleFigure moduleFigure = (SubmoduleFigure)animationController.getFigure(module);
			moduleFigure.setDisplayString(displayString);
		}
		else {
			// TODO: or what to do?
		}
	}
}

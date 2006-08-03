package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.simulation.model.IRuntimeMessage;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ModuleFigure;

public class HandleMessageAnimation extends AbstractAnimationPrimitive {
	private IRuntimeMessage message;
	
	private Ellipse ellipse;

	private int moduleId;
	
	public HandleMessageAnimation(ReplayAnimationController animationController,
								  long eventNumber,
								  double simulationTime,
								  long animationNumber,
								  int moduleId,
								  IRuntimeMessage message) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.moduleId = moduleId;
		this.message = message;
		this.ellipse = new Ellipse();
		this.ellipse.setBackgroundColor(new Color(null, 0, 128, 0));
	}
	
	@Override
	public double getEndSimulationTime() {
		return animationEnvironment.getSimulationTimeForEventNumber(eventNumber + 1);
	}
	
	@Override
	public double getEndAnimationTime() {
		return animationEnvironment.getAnimationTimeForEventNumber(eventNumber + 1);
	}
	
	public void redo() {
		if (ellipse.getParent()!=getRootFigure())  //FIXME 
			addFigure(ellipse);
	}

	public void undo() {
		if (ellipse.getParent()==getRootFigure())  //FIXME 
			removeFigure(ellipse);
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		IRuntimeModule module = animationEnvironment.getSimulation().getModuleByID(moduleId);
		if (module.getParentModule()==animationEnvironment.getSimulation().getRootModule()) { //FIXME
			ModuleFigure moduleFigure = (ModuleFigure)animationEnvironment.getFigure(module);
			setConstraint(ellipse, new Rectangle(moduleFigure.getLocation().x + 3, moduleFigure.getLocation().y + 3, 7, 7));
		}
	}
}

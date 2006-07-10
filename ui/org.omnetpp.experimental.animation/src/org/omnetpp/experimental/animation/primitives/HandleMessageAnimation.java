package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.experimental.animation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ModuleFigure;

public class HandleMessageAnimation extends AbstractAnimationPrimitive {
	private int eventNumber;
	
	private IRuntimeMessage message;
	
	private Ellipse ellipse;
	
	public HandleMessageAnimation(ReplayAnimationController animationController, double beginSimulationTime, int eventNumber, IRuntimeMessage message) {
		super(animationController, beginSimulationTime);
		this.eventNumber = eventNumber;
		this.message = message;
		this.ellipse = new Ellipse();
		this.ellipse.setBackgroundColor(new Color(null, 0, 128, 0));
	}
	
	public int getEventNumber() {
		return eventNumber;
	}
	
	public void gotoSimulationTime(double simulationTime) {
		if (beginSimulationTime == simulationTime) {
			ModuleFigure moduleFigure = (ModuleFigure)animationController.getFigure(animationController.getSimulation().getModuleByID(message.getArrivalModuleId()));
			setConstraint(ellipse, new Rectangle(moduleFigure.getLocation().x + 3, moduleFigure.getLocation().y + 3, 7, 7));
			showFigure(ellipse);
		}
		else
			hideFigure(ellipse);
	}
}

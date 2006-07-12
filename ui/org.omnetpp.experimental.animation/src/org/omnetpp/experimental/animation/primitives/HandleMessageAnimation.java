package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.experimental.animation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ModuleFigure;

public class HandleMessageAnimation extends AbstractAnimationPrimitive {
	private IRuntimeMessage message;
	
	private Ellipse ellipse;

	private IRuntimeModule module;
	
	public HandleMessageAnimation(ReplayAnimationController animationController,
								  long eventNumber,
								  double simulationTime,
								  long animationNumber,
								  IRuntimeModule module,
								  IRuntimeMessage message) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.module = module;
		this.message = message;
		this.ellipse = new Ellipse();
		this.ellipse.setBackgroundColor(new Color(null, 0, 128, 0));
	}
	
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		if (this.eventNumber == eventNumber) {
			ModuleFigure moduleFigure = (ModuleFigure)animationEnvironment.getFigure(module);
			
			if (moduleFigure != null) {
				setConstraint(ellipse, new Rectangle(moduleFigure.getLocation().x + 3, moduleFigure.getLocation().y + 3, 7, 7));
				showFigure(ellipse);
			}
		}
		else
			hideFigure(ellipse);
	}
}

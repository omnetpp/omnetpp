package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.figures.SubmoduleFigure;

public class CreateModuleAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;
	
	private Point location;
	
	private SubmoduleFigure moduleFigure;

	public CreateModuleAnimation(ReplayAnimationController animationController,
								 double beginSimulationTime,
								 IRuntimeModule module,
								 Point location) {
		super(animationController, beginSimulationTime);
		this.module = module;
		this.location = location;
		this.moduleFigure = new SubmoduleFigure();
	}
	
	public void gotoSimulationTime(double simulationTime) {
		if (simulationTime >= beginSimulationTime) {
			animationController.setFigure(module, moduleFigure);
			animationController.setFigure(new GateId(module.getId(), 0), new GateAnchor(moduleFigure, "in"));
			animationController.setFigure(new GateId(module.getId(), 1), new GateAnchor(moduleFigure, "out"));
			showFigure(moduleFigure);
			setConstraint(moduleFigure, new Rectangle(location.x, location.y, -1, -1));
		}
		else {
			hideFigure(moduleFigure);
		}
	}
}

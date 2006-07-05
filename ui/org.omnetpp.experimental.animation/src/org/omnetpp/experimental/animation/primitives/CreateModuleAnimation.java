package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.experimental.animation.controller.IAnimationController;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.figures.ModuleFigure;

public class CreateModuleAnimation extends AbstractAnimationPrimitive {
	private double creationSimulationTime;
	
	private int moduleId;
	
	private Point location;
	
	private ModuleFigure moduleFigure;

	public CreateModuleAnimation(IAnimationController controller,
								 double creationSimulationTime,
								 int moduleId,
								 Point location) {
		super(controller);
		this.creationSimulationTime = creationSimulationTime;
		this.moduleId = moduleId;
		this.location = location;
		this.moduleFigure = new CompoundModuleFigure();
	}
	
	public void gotoSimulationTime(double t) {
		if (t >= creationSimulationTime) {
			controller.setFigure(new GateId(moduleId, 0), new GateAnchor(moduleFigure, "in"));
			controller.setFigure(new GateId(moduleId, 1), new GateAnchor(moduleFigure, "out"));

			if (moduleId == 2)
				setConstraint(moduleFigure, new Rectangle(new Point(location.x + (int)(t * 30), location.y), new Dimension(200, 200)));
			else
				setConstraint(moduleFigure, new Rectangle(location, new Dimension(200, 200)));

			showFigure(moduleFigure);
		}
		else {
			hideFigure(moduleFigure);
		}
	}
}

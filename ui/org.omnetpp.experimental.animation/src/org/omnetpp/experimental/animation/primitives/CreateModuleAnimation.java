package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.figures.SubmoduleFigure;

public class CreateModuleAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;
	
	private Point location;
	
	private SubmoduleFigure moduleFigure;

	public CreateModuleAnimation(ReplayAnimationController controller,
								 double beginSimulationTime,
								 IRuntimeModule module,
								 Point location) {
		super(controller, beginSimulationTime);
		this.module = module;
		this.location = location;
		this.moduleFigure = new SubmoduleFigure();
	}
	
	public void gotoSimulationTime(double t) {
		if (t >= beginSimulationTime) {
			animationController.setFigure(new GateId(module.getId(), 0), new GateAnchor(moduleFigure, "in"));
			animationController.setFigure(new GateId(module.getId(), 1), new GateAnchor(moduleFigure, "out"));

			showFigure(moduleFigure);

			// TODO: remove this hackish stuff
			moduleFigure.setDisplayString(new DisplayString(null, null, "i=block/cogwheel"));
			if (module.getId() == 2)
				setConstraint(moduleFigure, new Rectangle(location.x + (int)(t * 30), location.y, -1, -1));
			else
				setConstraint(moduleFigure, new Rectangle(location.x, location.y, -1, -1));

		}
		else {
			hideFigure(moduleFigure);
		}
	}
}

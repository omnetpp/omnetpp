package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.experimental.animation.controller.IAnimationController;
import org.omnetpp.experimental.animation.editors.TimerQueue;

public abstract class AbstractAnimationPrimitive implements IAnimationPrimitive {
	protected IAnimationController controller;
	
	public AbstractAnimationPrimitive(IAnimationController controller) {
		this.controller = controller;
	}

	public void gotoSimulationTime(double t) {
		
	}
	
	public void animateSimulationTimeRange(double t0, double t1, double animationTime) {
		
	}
	
	protected TimerQueue getTimerQueue() {
		return controller.getTimerQueue();
	}

	protected IFigure getRootFigure() {
		return controller.getCanvas().getRootFigure();
	}

	protected LayoutManager getLayoutManager() {
		return getRootFigure().getLayoutManager();
	}

	protected void removeFigure(IFigure figure) {
		getRootFigure().remove(figure);
	}

	protected void addFigure(IFigure figure) {
		getRootFigure().add(figure);
	}

	protected void showFigure(IFigure figure) {
		if (figure.getParent() == null)
			addFigure(figure);
	}

	protected void hideFigure(IFigure figure) {
		if (figure.getParent() != null)
			removeFigure(figure);
	}

	protected void setConstraint(IFigure figure, Rectangle constraint) {
		getLayoutManager().setConstraint(figure, constraint);
	}
}
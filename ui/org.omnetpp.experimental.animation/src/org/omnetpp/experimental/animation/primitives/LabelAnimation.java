package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.experimental.animation.controller.IAnimationController;

public class LabelAnimation implements IAnimationPrimitive {
	private double tStart;
	private double tEnd;
	private Point pStart;
	private Point pEnd;
	
	private IFigure label;
	
	private IAnimationController controller;

	public LabelAnimation(IAnimationController controller, String text, double tStart, double tEnd, Point pStart, Point pEnd) {
		this.controller = controller;
		this.tStart = tStart;
		this.tEnd = tEnd;
		this.pStart = pStart;
		this.pEnd = pEnd;
		this.label = new Label(text);
		this.label = new RectangleFigure();
		label.setBackgroundColor(new Color(null, 0, 0, 0));
	}
	
	public void animateSimulationTimeRange(double t0, double t1, double animationTime) {
	}

	public void gotoSimulationTime(double t) {
		if (tStart <= t && t <= tEnd) {
			Point p1 = pStart.getCopy();
			Point p2 = pEnd.getCopy();
			double alpha = (t - tStart) / (tEnd - tStart);
			p1.scale(1 - alpha);
			p2.scale(alpha);
			p1.translate(p2);

			setConstraint(label, new Rectangle(p1, new Dimension(50, 20)));
			ensureFigure(label);
		}
		else
			desureFigure(label);
	}
	
	private IFigure getRootFigure() {
		return controller.getCanvas().getRootFigure();
	}

	private LayoutManager getLayoutManager() {
		return getRootFigure().getLayoutManager();
	}

	private void setConstraint(IFigure figure, Rectangle constraint) {
		getLayoutManager().setConstraint(figure, constraint);
	}

	private void removeFigure(IFigure figure) {
		getRootFigure().remove(figure);
	}

	private void addFigure(IFigure figure) {
		getRootFigure().add(figure);
	}

	private void ensureFigure(IFigure figure) {
		if (figure.getParent() == null)
			addFigure(figure);
	}

	private void desureFigure(IFigure figure) {
		if (figure.getParent() != null)
			removeFigure(figure);
	}
}

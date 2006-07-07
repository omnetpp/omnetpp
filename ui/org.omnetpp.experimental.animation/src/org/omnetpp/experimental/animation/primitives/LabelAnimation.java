package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.experimental.animation.controller.Timer;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class LabelAnimation extends AbstractAnimationPrimitive {
	private double endSimulationTime;
	
	private Point startPosition;
	
	private Point endPosition;
	
	private IFigure label;
	
	private Timer changeColorTimer;

	public LabelAnimation(ReplayAnimationController animationController,
						  String text,
						  double beginSimulationTime,
						  double endSimulationTime,
						  Point startPosition,
						  Point endPosition) {
		super(animationController, beginSimulationTime);
		this.endSimulationTime = endSimulationTime;
		this.startPosition = startPosition;
		this.endPosition = endPosition;
		this.label = new Label(text);
		this.changeColorTimer = new Timer(10, true, false) {
			public void run() {
				label.setForegroundColor(new Color(null, (int)(changeColorTimer.getNumberOfExecutions() % 255), 0, 0));
			}
		};
	}
	
	public void gotoSimulationTime(double simulationTime) {
		if (beginSimulationTime <= simulationTime && simulationTime <= endSimulationTime) {
			Point p1 = startPosition.getCopy();
			Point p2 = endPosition.getCopy();
			double alpha = (simulationTime - beginSimulationTime) / (endSimulationTime - beginSimulationTime);
			Point p = new Point((1 - alpha) * p1.x + alpha * p2.x, (1 - alpha) * p1.y + alpha * p2.y);
			setConstraint(label, new Rectangle(p, new Dimension(50, 20)));
			showFigure(label);
		}
		else
			hideFigure(label);
	}

	protected void addFigure(IFigure figure) {
		super.addFigure(figure);
		getTimerQueue().addTimer(changeColorTimer);
	}

	protected void removeFigure(IFigure figure) {
		super.removeFigure(figure);
		getTimerQueue().removeTimer(changeColorTimer);
	}
}

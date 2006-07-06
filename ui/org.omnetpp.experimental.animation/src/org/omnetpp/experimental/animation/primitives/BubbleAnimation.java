package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.experimental.animation.controller.Timer;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class BubbleAnimation extends AbstractAnimationPrimitive {
	private Point location;
	
	private RoundedRectangle background;

	private Label label;
	
	private Timer bubbleTimer;
	
	public BubbleAnimation(ReplayAnimationController controller,
						  String text,
						  double beginSimulationTime,
						  Point location) {
		super(controller, beginSimulationTime);
		this.background = new RoundedRectangle();
		this.label = new Label(text);
		this.location = location;
		this.bubbleTimer = new Timer(3000, false, false) {
			public void run() {
				hide();
				// to avoid to show it again
				shown = true;
			}
		};
	}
	
	public void gotoSimulationTime(double t) {
		if ((beginSimulationTime < t && !shown) || beginSimulationTime == t) {
			Rectangle r = new Rectangle(location, new Dimension(50, 20));
			setConstraint(background, r);
			setConstraint(label, r);
			show();
		}
		else
			hide();
	}
	
	protected void show() {
		if (!shown) {
			showFigure(background);
			showFigure(label);
			getTimerQueue().addTimer(bubbleTimer);
		}
	}
	
	protected void hide() {
		if (shown) {
			hideFigure(background);
			hideFigure(label);
			getTimerQueue().removeTimer(bubbleTimer);
		}
	}
}

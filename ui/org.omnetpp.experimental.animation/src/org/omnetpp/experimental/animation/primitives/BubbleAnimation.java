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
	
	private boolean expired;
	
	public BubbleAnimation(ReplayAnimationController animationController,
						   long eventNumber,
						   double beginSimulationTime,
						   String text,
						   Point location) {
		super(animationController, eventNumber, beginSimulationTime);
		this.background = new RoundedRectangle();
		this.label = new Label(text);
		this.location = location;
		this.bubbleTimer = new Timer(3000, false, false) {
			public void run() {
				expired = true;
				hide();
			}
		};
	}
	
	public void animateAt(long eventNumber, double simulationTime) {
		if (beginSimulationTime <= simulationTime) {
			if (!expired) {
				Rectangle r = new Rectangle(location, new Dimension(50, 20));
				setConstraint(background, r);
				setConstraint(label, r);
				show();
			}
		}
		else {
			expired = false;
			hide();
		}
	}
	
	protected void show() {
		if (!shown && !expired) {
			showFigure(background);
			showFigure(label);
			bubbleTimer.reset();
			getTimerQueue().addTimer(bubbleTimer);
		}
		
		super.show();
	}
	
	protected void hide() {
		if (shown) {
			hideFigure(background);
			hideFigure(label);
			getTimerQueue().removeTimer(bubbleTimer);
		}
		
		super.hide();
	}
}

package org.omnetpp.experimental.animation.controller;

import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public interface IAnimationController {
	public void animateBack();

	public void animatePlay();
	
	public void animateExpress();

	public void animateFast();

	public void animateStop();
	
	public void gotoBegin();

	public void gotoEnd();
	
	public AnimationCanvas getCanvas();

	public void gotoSimulationTime(double t);
}

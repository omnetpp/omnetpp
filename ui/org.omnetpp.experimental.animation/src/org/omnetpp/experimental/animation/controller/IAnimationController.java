package org.omnetpp.experimental.animation.controller;

import org.omnetpp.experimental.animation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public interface IAnimationController {
	public void animateBack();

	public void animateStep();
	
	public void animatePlay();
	
	public void animateExpress();

	public void animateFast();

	public void animateStop();
	
	public void gotoBegin();

	public void gotoEnd();
	
	public void gotoSimulationTime(double simulationTime);
	
	public void setSpeed(int speed);

	public IRuntimeSimulation getSimulation();

	public AnimationCanvas getCanvas();

	public TimerQueue getTimerQueue();

	public Object getFigure(Object key);
	
	public void setFigure(Object key, Object value);
}

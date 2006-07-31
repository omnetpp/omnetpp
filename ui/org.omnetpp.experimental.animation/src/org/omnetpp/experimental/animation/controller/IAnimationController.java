package org.omnetpp.experimental.animation.controller;

import org.omnetpp.experimental.animation.replay.ReplayAnimationController.AnimationMode;

/**
 * IAnimationController is the public interface of the animation controllers.
 */
public interface IAnimationController {
	public void init();
	
	public void shutdown();
	
	public void animateBack();
	
	public void animateStep();
	
	public void animatePlay();
	
	public void animateFast();

	public void animateExpress();
	
	public void animateStop();

	public void gotoBegin();

	public void gotoEnd();

	public void gotoEventNumber(long eventNumber);
	
	public void gotoSimulationTime(double simulationTime);

	public void gotoAnimationNumber(long animationNumber);

	public void gotoAnimationTime(double animationTime);
	
	public void gotoLivePosition();
	
	public boolean isAtLivePosition();

	public AnimationMode getAnimationMode();
	
	public void setAnimationMode(AnimationMode mode);

	public long getEventNumber();

	public double getSimulationTime();

	public long getAnimationNumber();

	public double getAnimationTime();

	public void addAnimationListener(IAnimationListener editor);

	public void setRealTimeToAnimationTimeScale(double scale);
}

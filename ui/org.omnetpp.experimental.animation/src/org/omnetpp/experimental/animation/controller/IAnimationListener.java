package org.omnetpp.experimental.animation.controller;

public interface IAnimationListener {
	public void replaySimulationTimeChanged(double simulationTime);
	
	public void replayEventNumberChanged(long eventNumber);

	public void replayAnimationNumberChanged(long animationNumber);

	public void replayAnimationTimeChanged(double animationTime);

	public void liveSimulationTimeChanged(double simulationTime);
	
	public void liveEventNumberChanged(long eventNumber);

	public void liveAnimationNumberChanged(long animationNumber);

	public void liveAnimationTimeChanged(double animationTime);
}

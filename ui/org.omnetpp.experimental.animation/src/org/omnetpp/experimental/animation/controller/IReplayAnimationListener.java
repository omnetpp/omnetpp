package org.omnetpp.experimental.animation.controller;

public interface IReplayAnimationListener {
	public void controllerStateChanged();
	
	public void replayPositionChanged(long eventNumber, double simulationTime, long animationNumber, double animationTime);
}

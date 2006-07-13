package org.omnetpp.experimental.animation.controller;

public interface IAnimationListener {
	public void replayPositionChanged(long eventNumber, double simulationTime, long animationNumber, double animationTime);

	public void livePositionChanged(long eventNumber, double simulationTime, long animationNumber, double animationTime);
}

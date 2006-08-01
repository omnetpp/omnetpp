package org.omnetpp.experimental.animation.controller;

public interface ILiveAnimationListener extends IReplayAnimationListener {
	public void livePositionChanged(long eventNumber, double simulationTime, long animationNumber, double animationTime);
}

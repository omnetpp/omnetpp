package org.omnetpp.experimental.animation.controller;

public interface IReplayAnimationListener {
	public void controllerStateChanged();
	
	public void replayPositionChanged(AnimationPosition animationPosition);
}

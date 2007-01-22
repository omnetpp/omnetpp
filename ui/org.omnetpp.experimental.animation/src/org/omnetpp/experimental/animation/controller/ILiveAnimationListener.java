package org.omnetpp.experimental.animation.controller;

public interface ILiveAnimationListener extends IReplayAnimationListener {
	public void livePositionChanged(AnimationPosition animationPosition);
}

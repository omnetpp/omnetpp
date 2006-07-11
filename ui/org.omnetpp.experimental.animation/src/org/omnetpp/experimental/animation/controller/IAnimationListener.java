package org.omnetpp.experimental.animation.controller;

public interface IAnimationListener {
	public void replaySimulationTimeChanged(double simulationTime);
	
	public void replayEventNumberChanged(long eventNumber);

	public void liveSimulationTimeChanged(double simulationTime);
	
	public void liveEventNumberChanged(long eventNumber);
}

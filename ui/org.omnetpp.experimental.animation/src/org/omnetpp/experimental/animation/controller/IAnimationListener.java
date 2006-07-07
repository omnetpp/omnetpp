package org.omnetpp.experimental.animation.controller;

public interface IAnimationListener {
	public void simulationTimeChanged(double simulationTime);
	
	public void eventNumberChanged(int eventNumber);
}

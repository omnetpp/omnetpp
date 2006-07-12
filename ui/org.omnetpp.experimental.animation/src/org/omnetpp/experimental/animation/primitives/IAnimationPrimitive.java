package org.omnetpp.experimental.animation.primitives;

/**
 * IAnimationPrimitive is an atomic animation element managed by the IAnimationController.
 */
public interface IAnimationPrimitive {
	/**
	 * Returns the event number when this animation primitive begins.
	 */
	public long getEventNumber();

	/**
	 * Returns the simulation time when this animation primitive begins.
	 * BeginSimulationTime and EndSimulationTime may be equal.
	 */
	public double getBeginSimulationTime();

	/**
	 * Returns the simulation time when this animation primitive ends.
	 * BeginSimulationTime and EndSimulationTime may be equal.
	 */
	public double getEndSimulationTime();
	
	/**
	 * Returns the animation number when this animation primitive begins.
	 */
	public long getAnimationNumber();

	/**
	 * Updates figures according to the given animation state.
	 */
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime);
}

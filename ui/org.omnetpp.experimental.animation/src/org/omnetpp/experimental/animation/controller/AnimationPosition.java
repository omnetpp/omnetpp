package org.omnetpp.experimental.animation.controller;

/**
 * An animation position consist of the following tuple:
 *  - event number
 *  - simulation time
 *  - animation number
 *  - animation time
 *  
 * The position may be partially filled when some of its elements are -1. Elements which have values must be consistent.
 * If none of the elements are set, then the position is said to be invalid.
 */
public class AnimationPosition implements Comparable {
	protected long eventNumber;
	
	protected double simulationTime;
	
	protected long animationNumber;
	
	protected double animationTime;

	public AnimationPosition() {
		this(-1, -1, -1, -1);
	}
	
	public AnimationPosition(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		setAnimationPosition(eventNumber, simulationTime, animationNumber, animationTime);
	}

	public AnimationPosition(AnimationPosition animationPosition) {
		assign(animationPosition);
	}

	public void assign(AnimationPosition animationPosition) {
		setAnimationPosition(animationPosition.eventNumber, animationPosition.simulationTime, animationPosition.animationNumber, animationPosition.animationTime);
	}

	public void setAnimationPosition(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		this.eventNumber = eventNumber;
		this.simulationTime = simulationTime;
		this.animationNumber = animationNumber;
		this.animationTime = animationTime;
	}

	public void invalidate() {
		setAnimationPosition(-1, -1, -1, -1);
	}

	public AnimationPosition copy() {
		return new AnimationPosition(this);
	}

	public long getAnimationNumber() {
		return animationNumber;
	}

	public long getEventNumber() {
		return eventNumber;
	}

	public void setEventNumber(long eventNumber) {
		this.eventNumber = eventNumber;
	}

	public double getSimulationTime() {
		return simulationTime;
	}

	public void setSimulationTime(double simulationTime) {
		this.simulationTime = simulationTime;
	}

	public void setAnimationNumber(long animationNumber) {
		this.animationNumber = animationNumber;
	}

	public double getAnimationTime() {
		return animationTime;
	}

	public void setAnimationTime(double animationTime) {
		this.animationTime = animationTime;
	}
	
	public boolean isValid() {
		return eventNumber != -1 &&
			simulationTime != -1 &&
			animationNumber != -1 &&
			animationTime != -1;
	}
	
	public boolean isPartiallyValid() {
		return eventNumber != -1 ||
			simulationTime != -1 ||
			animationNumber != -1 ||
			animationTime != -1;
	}

	/**
	 * Unspecified values are treated as being equal.
	 */
	public int compareTo(Object o) {
		if (this == o)
			return 0;
		final AnimationPosition other = (AnimationPosition) o;
		return
			compareTo(eventNumber, other.eventNumber) + 
			compareTo(simulationTime, other.simulationTime) +
			compareTo(animationNumber, other.animationNumber) + 
			compareTo(animationTime, other.animationTime);
	}

	protected int compareTo(long l1, long l2) {
		if (l1 == -1 || l2 == -1)
			return 0;
		else
			return (int)Math.signum(l1 - l2);
	}

	protected int compareTo(double d1, double d2) {
		if (d1 == -1 || d2 == -1)
			return 0;
		else
			return (int)Math.signum(d1 - d2);
	}

	@Override
	public int hashCode() {
		final int PRIME = 31;
		int result = 1;
		result = PRIME * result + (int) (animationNumber ^ (animationNumber >>> 32));
		long temp;
		temp = Double.doubleToLongBits(animationTime);
		result = PRIME * result + (int) (temp ^ (temp >>> 32));
		result = PRIME * result + (int) (eventNumber ^ (eventNumber >>> 32));
		temp = Double.doubleToLongBits(simulationTime);
		result = PRIME * result + (int) (temp ^ (temp >>> 32));
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		final AnimationPosition other = (AnimationPosition) obj;
		if (animationNumber != other.animationNumber)
			return false;
		if (Double.doubleToLongBits(animationTime) != Double.doubleToLongBits(other.animationTime))
			return false;
		if (eventNumber != other.eventNumber)
			return false;
		if (Double.doubleToLongBits(simulationTime) != Double.doubleToLongBits(other.simulationTime))
			return false;
		return true;
	}
	
	@Override
	public String toString() {
		return "event number: " + eventNumber + ", simulation time: " + simulationTime + ", animation number: " + animationNumber + ", animationTime: " + animationTime;
	}
}

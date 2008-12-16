/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.controller;

/**
 * This is an immutable class. An animation position consist of the following tuple:
 *  - event number
 *  - simulation time
 *  - animation number
 *  - animation time
 *  
 * The position may be partially filled when some of its elements are -1. Elements which have values must be greater or equal to 0
 * and be consistent. If none of the elements are set, then the position is said to be invalid.
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
		setAnimationPosition(animationPosition.eventNumber, animationPosition.simulationTime, animationPosition.animationNumber, animationPosition.animationTime);
	}

	protected void setAnimationPosition(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		setEventNumber(eventNumber);
		setSimulationTime(simulationTime);
		setAnimationNumber(animationNumber);
		setAnimationTime(animationTime);
	}

	public long getEventNumber() {
		return eventNumber;
	}

	protected void setEventNumber(long eventNumber) {
		if (eventNumber != -1 && eventNumber < 0)
			throw new RuntimeException("Invalid event number");
		
		this.eventNumber = eventNumber;
	}

	public double getSimulationTime() {
		return simulationTime;
	}

	protected void setSimulationTime(double simulationTime) {
		if (simulationTime != -1 && simulationTime < 0)
			throw new RuntimeException("Invalid event number");
		
		this.simulationTime = simulationTime;
	}

	public long getAnimationNumber() {
		return animationNumber;
	}

	protected void setAnimationNumber(long animationNumber) {
		if (animationNumber != -1 && animationNumber < 0)
			throw new RuntimeException("Invalid event number");
		
		this.animationNumber = animationNumber;
	}

	public double getAnimationTime() {
		return animationTime;
	}

	protected void setAnimationTime(double animationTime) {
		if (animationTime != -1 && animationTime < 0)
			throw new RuntimeException("Invalid event number");
		
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

package org.omnetpp.experimental.animation.controller;

public class AnimationPosition {
	protected long eventNumber;
	
	protected double simulationTime;
	
	protected long animationNumber;
	
	protected double animationTime;

	public AnimationPosition() {
		this(-1, -1, -1, -1);
	}
	
	public AnimationPosition(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		this.eventNumber = eventNumber;
		this.simulationTime = simulationTime;
		this.animationNumber = animationNumber;
		this.animationTime = animationTime;
	}

	public AnimationPosition(AnimationPosition animationPosition) {
		this.eventNumber = animationPosition.eventNumber;
		this.simulationTime = animationPosition.simulationTime;
		this.animationNumber = animationPosition.animationNumber;
		this.animationTime = animationPosition.animationTime;
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
	
	public boolean isValid() {
		return eventNumber >= 0 &&
			simulationTime >= 0 &&
			animationNumber >= 0 &&
			animationTime >= 0;
	}
}

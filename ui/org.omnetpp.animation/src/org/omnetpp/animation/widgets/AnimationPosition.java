/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.widgets;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.StringUtils;

/**
 * An animation position designates a single moment in the animation process. It consist of the following tuple:
 *  - event number
 *  - simulation time
 *  - animation time
 *  - relative animation time
 *
 * The position may be partially valid by setting only some of its elements.
 * If none of the elements are set, then the position is said to be completely invalid.
 * The animation position is determined by its animation time and all other parts are used for
 */
public class AnimationPosition implements Comparable<AnimationPosition> {
    /**
     * The event number of the event that is immediately preceding this animation position.
     * -1 if the event is not yet known.
     */
	protected long eventNumber;

    /**
     * The simulation time that corresponds to this animation position.
     */
	protected BigDecimal simulationTime;

	/**
	 * The animation time relative to the first animation time moment of the corresponding simulation time.
	 */
	protected double relativeAnimationTime;

    /**
     * The animation time uniquely identifies this animation position by itself.
     */
    protected double animationTime;

	public AnimationPosition() {
		this(-1, null, -1, -1);
	}

	public AnimationPosition(long eventNumber, BigDecimal simulationTime, double relativeAnimationTime, double animationTime) {
		setAnimationPosition(eventNumber, simulationTime, relativeAnimationTime, animationTime);
	}

	public AnimationPosition(AnimationPosition animationPosition) {
		setAnimationPosition(animationPosition.eventNumber, animationPosition.simulationTime, animationPosition.relativeAnimationTime, animationPosition.animationTime);
	}

	public void setAnimationPosition(long eventNumber, BigDecimal simulationTime, double relativeAnimationTime, double animationTime) {
		setEventNumber(eventNumber);
		setSimulationTime(simulationTime);
		setRelativeAnimationTime(relativeAnimationTime);
		setAnimationTime(animationTime);
	}

	public long getEventNumber() {
		return eventNumber;
	}

	public void setEventNumber(long eventNumber) {
		this.eventNumber = eventNumber;
	}

	public BigDecimal getSimulationTime() {
		return simulationTime;
	}

	public void setSimulationTime(BigDecimal simulationTime) {
		this.simulationTime = simulationTime;
	}

	public double getRelativeAnimationTime() {
        return relativeAnimationTime;
    }

	public void setRelativeAnimationTime(double relativeAnimationTime) {
        this.relativeAnimationTime = relativeAnimationTime;
    }

	public double getAnimationTime() {
		return animationTime;
	}

	public void setAnimationTime(double animationTime) {
		this.animationTime = animationTime;
	}

	public boolean isValid() {
		return eventNumber != -1 && simulationTime != null && relativeAnimationTime != -1 && animationTime != -1;
	}

	/**
	 * Unspecified values are ignored.
	 */
	public int compareTo(AnimationPosition other) {
		if (this == other)
			return 0;
		// TODO: shouldn't we rely on animation time as the primary comparison value?
        else if (eventNumber != -1 && other.eventNumber != -1 && eventNumber != other.eventNumber)
            return (int)Math.signum(eventNumber - other.eventNumber);
        else if (simulationTime != null && other.simulationTime != null && !simulationTime.equals(other.simulationTime))
            return simulationTime.less(other.simulationTime) ? -1 : 1;
        else if (animationTime != -1 && other.animationTime != -1)
            return (int)Math.signum(animationTime - other.animationTime);
		else
		    throw new RuntimeException("Cannot compare the provided partially filled animation positions");
	}

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        long temp;
        temp = Double.doubleToLongBits(animationTime);
        result = prime * result + (int) (temp ^ (temp >>> 32));
        result = prime * result + (int) (eventNumber ^ (eventNumber >>> 32));
        temp = Double.doubleToLongBits(relativeAnimationTime);
        result = prime * result + (int) (temp ^ (temp >>> 32));
        result = prime * result + ((simulationTime == null) ? 0 : simulationTime.hashCode());
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
        AnimationPosition other = (AnimationPosition) obj;
        if (Double.doubleToLongBits(animationTime) != Double.doubleToLongBits(other.animationTime))
            return false;
        if (eventNumber != other.eventNumber)
            return false;
        if (Double.doubleToLongBits(relativeAnimationTime) != Double.doubleToLongBits(other.relativeAnimationTime))
            return false;
        if (simulationTime == null) {
            if (other.simulationTime != null)
                return false;
        }
        else if (!simulationTime.equals(other.simulationTime))
            return false;
        return true;
    }

    @Override
	public String toString() {
		return "[#" + eventNumber + ", " + simulationTime + "s, " + relativeAnimationTime + ", " + animationTime + "]";
	}

    public String unparse() {
        return eventNumber + ":" + simulationTime + ":" + relativeAnimationTime + ":" + animationTime;
    }

    public static AnimationPosition parse(String value) {
        AnimationPosition animationPosition = new AnimationPosition();
        String[] parts = StringUtils.split(value, ':');
        if (parts.length == 4) {
            try {
                animationPosition.eventNumber = Long.parseLong(parts[0]);
                animationPosition.simulationTime = BigDecimal.parse(parts[1]);
                animationPosition.relativeAnimationTime = Double.parseDouble(parts[2]);
                animationPosition.animationTime = Double.parseDouble(parts[3]);
                return animationPosition;
            }
            catch (Exception e) {
            }
        }
        return animationPosition;
    }
}

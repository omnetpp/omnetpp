/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.animation.widgets;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.StringUtils;

/**
 * <p>
 * A completely specified animation position designates a single moment in the
 * animation. The animation is pretty much like a movie that flows from the
 * begin to the end with the current animation speed. Animation positions have a
 * natural ordering based on the moments they designate in the animation. An
 * animation position is said to be less than another if it designates a moment
 * before the moment designated by the other animation position.
 * </p>
 * <p>
 * An absolute animation time would be sufficient to specify an animation
 * position by simply measuring the distance from the very begin of the
 * animation. Unfortunately expressing animation positions this way would make
 * impossible to compute animation positions lazily and would prevent handling
 * huge animations effectively.
 * </p>
 * <p>
 * A relative animation time helps to achieve the above goals by specifying
 * animation positions relative to another moment called the origin within the
 * same animation. One such moment might be the very first moment of the
 * animation of a single event. Another might be the first moment of an
 * animation frame. An animation frame is a coherent part of the whole animation
 * that belongs to the very same simulation time.
 * <p>
 * An animation position consist of the following values:
 * </p>
 * <ul>
 * <li>An event number or <code>-1</code> if not yet set.</li>
 * <li>A simulation time or <code>null</code> if not yet set.</li>
 * <li>A double value specifying the frame relative animation time or
 * <code>Double.NaN</code> if not yet set.</li>
 * <li>A double value specifying the origin relative animation time or
 * <code>Double.NaN</code> if not yet set.</li>
 * </ul>
 * <p>
 * An animation position may be partially set and may only become complete
 * later. If none of the elements are set then the position is said to be
 * completely unspecified. If all of the elements are set then the position is
 * said to be completely specified. An animation position cannot change its
 * designated moment in the animation. This means that it always designates the
 * very same moment even if some parts have not yet been set.
 * </p>
 * <p>
 * The <b>begin animation position</b> is the first moment of the network setup
 * animation specified as follows:
 * </p>
 * <ul>
 * <li>Event number is <code>-1</code>.</li>
 * <li>Simulation time is <code>-1</code>.</li>
 * <li>Frame relative animation time is <code>0</code> (the first moment in the
 * animation frame).</li>
 * <li>Origin relative animation time is a negative value equal to the first
 * animation frame's size (assuming initialize as being the origin).</li>
 * </ul>
 * <p>
 * The <b>initialize animation position</b> is the first moment of the
 * initialization animation specified as follows:
 * </p>
 * <ul>
 * <li>Event number is <code>0</code>.</li>
 * <li>Simulation time is to <code>0</code>.</li>
 * <li>Frame relative animation time is <code>0</code> (the first moment in the
 * animation frame starting with the initialize).</li>
 * <li>Origin relative animation time is <code>0</code> (assuming initialize as
 * being the origin).</li>
 * </ul>
 * <p>
 * The <b>first event animation position</b> is the first moment of the first
 * event's animation specified as follows:
 * </p>
 * <ul>
 * <li>Event number is the event number of the first event.</li>
 * <li>Simulation time is the simulation time of the first event.</li>
 * <li>Frame relative animation time is either <code>0</code> (the first moment
 * in the animation frame starting with the first event) or some non-zero value
 * (the animation frame starts with initialize and expands into the first
 * event).</li>
 * <li>Origin relative animation time is <code>0</code> (assuming the first
 * event as being the origin).</li>
 * </ul>
 * <p>
 * The <b>end animation position</b> is the last moment of the last event's
 * animation specified as follows:
 * </p>
 * <ul>
 * <li>Event number is the event number of the last event.</li>
 * <li>Simulation time is the simulation time of the last event.</li>
 * <li>Frame relative animation time is the animation frame's size.</li>
 * <li>Origin relative animation time is <code>0</code> (assuming the last event
 * as being the origin).</li>
 * </ul>
 *
 * @author levy
 */
public class AnimationPosition implements Comparable<AnimationPosition> {
    /**
     * The event number of the event that is immediately preceding this
     * animation position. The value <code>-1</code> means that it is not yet
     * set or there is no such event (i.e. during network setup). The value
     * <code>0</code> means initialize. The event numbers monotonically increase
     * along the animation. This means that if an animation position designates
     * a moment later in the animation then it will also have greater than or
     * equal event number.
     */
    protected Long eventNumber;

    /**
     * The simulation time that corresponds to this animation position. There is
     * a range of animation times that correspond to the very same simulation
     * time. This range is called the animation frame. The value
     * <code>null</code> means that it is not yet set. This means that if an
     * animation position designates a moment later in the animation then it
     * will also have greater than or equal simulation time.
     */
    protected BigDecimal simulationTime;

    /**
     * The animation time relative to the beginning of the animation frame. See
     * above for the definition of animation frame. The value must be greater
     * than or equal to zero. The value <code>Double.NaN</code> means that it is
     * not yet set.
     */
    protected double frameRelativeAnimationTime;

    /**
     * The animation time relative to the origin of the animation. The origin is
     * specified with an event number and it refers to the first animation
     * moment of that event. The value may be any finite double number. The
     * value <code>Double.NaN</code> means that it is not yet set.
     */
    protected double originRelativeAnimationTime;

    public AnimationPosition() {
        setAnimationPosition(null, null, Double.NaN, Double.NaN);
    }

    public AnimationPosition(long eventNumber, BigDecimal simulationTime, double frameRelativeAnimationTime, double originRelativeAnimationTime) {
        setAnimationPosition(eventNumber, simulationTime, frameRelativeAnimationTime, originRelativeAnimationTime);
    }

    public AnimationPosition(Long eventNumber, BigDecimal simulationTime, double frameRelativeAnimationTime, double originRelativeAnimationTime) {
        setAnimationPosition(eventNumber, simulationTime, frameRelativeAnimationTime, originRelativeAnimationTime);
    }

    public AnimationPosition(AnimationPosition animationPosition) {
        setAnimationPosition(animationPosition.eventNumber, animationPosition.simulationTime, animationPosition.frameRelativeAnimationTime, animationPosition.originRelativeAnimationTime);
    }

    public void setAnimationPosition(Long eventNumber, BigDecimal simulationTime, double frameRelativeAnimationTime, double originRelativeAnimationTime) {
        setEventNumber(eventNumber);
        setSimulationTime(simulationTime);
        setFrameRelativeAnimationTime(frameRelativeAnimationTime);
        setOriginRelativeAnimationTime(originRelativeAnimationTime);
    }

    public long getEventNumber() {
        return eventNumber;
    }

    public void setEventNumber(long eventNumber) {
        Assert.isTrue(this.eventNumber == null);
        this.eventNumber = eventNumber;
    }

    public void setEventNumber(Long eventNumber) {
        Assert.isTrue(this.eventNumber == null);
        this.eventNumber = eventNumber;
    }

    public BigDecimal getSimulationTime() {
        return simulationTime;
    }

    public void setSimulationTime(BigDecimal simulationTime) {
        Assert.isTrue(this.simulationTime == null);
        this.simulationTime = simulationTime;
    }

    public double getFrameRelativeAnimationTime() {
        return frameRelativeAnimationTime;
    }

    public void setFrameRelativeAnimationTime(double frameRelativeAnimationTime) {
        Assert.isTrue(Double.isNaN(this.frameRelativeAnimationTime));
        this.frameRelativeAnimationTime = frameRelativeAnimationTime;
    }

    public double getOriginRelativeAnimationTime() {
        return originRelativeAnimationTime;
    }

    public void setOriginRelativeAnimationTime(double originRelativeAnimationTime) {
        Assert.isTrue(Double.isNaN(this.originRelativeAnimationTime));
        this.originRelativeAnimationTime = originRelativeAnimationTime;
    }

    public boolean isCompletelyUnspecified() {
        return eventNumber == null && simulationTime == null && Double.isNaN(frameRelativeAnimationTime) && Double.isNaN(originRelativeAnimationTime);
    }

    public boolean isCompletelySpecified() {
        return eventNumber != null && simulationTime != null && !Double.isNaN(frameRelativeAnimationTime) && !Double.isNaN(originRelativeAnimationTime);
    }

    public int compareTo(AnimationPosition other) {
        if (this == other)
            return 0;
        else if (eventNumber != null && other.eventNumber != null && !eventNumber.equals(other.eventNumber))
            return (int)Math.signum(eventNumber - other.eventNumber);
        else if (simulationTime != null && other.simulationTime != null && !simulationTime.equals(other.simulationTime))
            return simulationTime.less(other.simulationTime) ? -1 : 1;
        else if (!Double.isNaN(originRelativeAnimationTime) && !Double.isNaN(other.originRelativeAnimationTime))
            return (int)Math.signum(originRelativeAnimationTime - other.originRelativeAnimationTime);
        else
            throw new RuntimeException("Cannot compare the provided partially specified animation positions: " + this + " & " + other);
    }


    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((eventNumber == null) ? 0 : eventNumber.hashCode());
        long temp;
        temp = Double.doubleToLongBits(frameRelativeAnimationTime);
        result = prime * result + (int) (temp ^ (temp >>> 32));
        temp = Double.doubleToLongBits(originRelativeAnimationTime);
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
        if (eventNumber == null) {
            if (other.eventNumber != null)
                return false;
        }
        else if (!eventNumber.equals(other.eventNumber))
            return false;
        if (Double.doubleToLongBits(frameRelativeAnimationTime) != Double.doubleToLongBits(other.frameRelativeAnimationTime))
            return false;
        if (Double.doubleToLongBits(originRelativeAnimationTime) != Double.doubleToLongBits(other.originRelativeAnimationTime))
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
        return "[#" + eventNumber + ", " + simulationTime + "s, " + frameRelativeAnimationTime + ", " + originRelativeAnimationTime + "]";
    }

    public String unparse() {
        return eventNumber + ":" + simulationTime + ":" + frameRelativeAnimationTime + ":" + originRelativeAnimationTime;
    }

    public static AnimationPosition parse(String value) throws NumberFormatException {
        String[] parts = StringUtils.split(value, ':');
        if (parts.length == 4) {
            AnimationPosition animationPosition = new AnimationPosition();
            animationPosition.eventNumber = Long.parseLong(parts[0]);
            animationPosition.simulationTime = BigDecimal.parse(parts[1]);
            animationPosition.frameRelativeAnimationTime = Double.parseDouble(parts[2]);
            animationPosition.originRelativeAnimationTime = Double.parseDouble(parts[3]);
            return animationPosition;
        }
        else
            throw new RuntimeException("Cannot parse animation position from: " + value);
    }
}

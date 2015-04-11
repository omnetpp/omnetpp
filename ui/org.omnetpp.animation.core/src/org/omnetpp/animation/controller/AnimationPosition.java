/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.animation.controller;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.StringUtils;

/**
 * <p>
 * A completely specified animation position designates a single moment in the
 * animation. The animation is pretty much like a movie that flows from the
 * beginning to the end with the current animation speed. Animation positions
 * have a natural ordering based on the moments they designate in the animation.
 * An animation position is said to be less than another if it designates a
 * moment before the moment designated by the other animation position.
 * </p>
 * <p>
 * An absolute animation time, which is simply measuring the distance from the
 * very beginning of the animation, would be sufficient to specify an animation
 * position. Unfortunately, expressing animation positions this way would make
 * it impossible to compute animation positions lazily, and would prevent
 * handling huge animations effectively.
 * </p>
 * <p>
 * The concept of relative animation time helps to achieve the above goals by
 * specifying animation positions relative to another moment (called the origin)
 * within the same animation. One such moment is the very first moment of the
 * animation of a single simulation event.
 * </p>
 * <p>
 * Another important concept is the animation frame. The whole animation is
 * basically a list of interleaved constant and interpolation frames. A constant
 * frame belongs to the very same simulation time and event number and it is
 * identified by them. An animation frame cannot span across multiple events,
 * but there may be multiple animation frames between two subsequent events.
 * Within constant frames the event number and the simulation time are constant.
 * Within interpolation frames the event number is constant, but the simulation
 * time is proportional to the animation time. A constant frame may also be
 * assigned to a simulation time where in fact no event occurred.
 * </p>
 * <p>
 * An animation position consist of the following values:
 * </p>
 * <ul>
 * <li>An event number or <code>null</code> if not yet set.</li>
 * <li>A simulation time or <code>null</code> if not yet set.</li>
 * <li>A double value specifying the frame-relative animation time or
 * <code>null</code> if not yet set.</li>
 * <li>A double value specifying the origin-relative animation time or
 * <code>null</code> if not yet set.</li>
 * </ul>
 * <p>
 * An animation position may be partially set and may only become complete
 * later. If none of the elements are set then the position is said to be
 * completely unspecified. If all of the elements are set then the position is
 * said to be completely specified.
 * </p>
 * <p>
 * An animation position is not allowed to change its designated moment in the
 * animation while the program is running. This means that it always designates
 * the very same moment even if some parts have not yet been set. It is almost
 * like an immutable data structure, but unfortunately it is difficult to
 * enforce this constraint.
 * </p>
 * <p>
 * Note that there are strict relationships between the parts of an animation
 * position and also where the animation time coordinate system origin is. The
 * following examples assume certain coordinate system origins to simplify the
 * descriptions. Other origins are also possible but they would only change the
 * origin-relative animation time.
 * </p>
 * <p>
 * The <b>begin animation position</b> is the first moment of the initialize
 * animation specified as follows (assuming initialize as being the origin):
 * </p>
 * <ul>
 * <li>Event number is <code>0</code>.</li>
 * <li>Simulation time is <code>0</code>.</li>
 * <li>frame-relative animation time is <code>0</code> (the first moment in the
 * animation frame).</li>
 * <li>Origin-relative animation time is <code>0</code>.</li>
 * </ul>
 * <p>
 * The <b>first event animation position</b> is the first moment of the first
 * event's animation specified as follows (assuming the first event as being the
 * origin):
 * </p>
 * <ul>
 * <li>Event number is the event number of the first event.</li>
 * <li>Simulation time is the simulation time of the first event.</li>
 * <li>frame-relative animation time is <code>0</code> (the first moment in the
 * animation frame starting with the first event).</li>
 * <li>Origin-relative animation time is <code>0</code>.</li>
 * </ul>
 * <p>
 * The <b>end animation position</b> is the last moment of the last event's
 * animation specified as follows (assuming the last event as being the origin):
 * </p>
 * <ul>
 * <li>Event number is the event number of the last event.</li>
 * <li>Simulation time is the simulation time of the last event.</li>
 * <li>frame-relative animation time is the animation frame's size.</li>
 * <li>Origin-relative animation time is the animation frame's size.</li>
 * </ul>
 *
 * @author levy
 */
public class AnimationPosition implements Comparable<AnimationPosition> {
    /**
     * The event number of the event that is immediately preceding this
     * animation position. The value <code>null</code> means that it is not yet
     * set. The value <code>0</code> means initialize. The event numbers
     * monotonically increase along the animation. This means that if an
     * animation position designates a moment later in the animation then it
     * will also have greater than or equal event number.
     */
    protected Long eventNumber;

    /**
     * The simulation time that corresponds to this animation position. There is
     * a range of animation times that correspond to the very same simulation
     * time. This range is called the constant animation frame. The value
     * <code>null</code> means that it is not yet set. The simulation times
     * monotonically increase along the animation. This means that if an
     * animation position designates a moment later in the animation then it
     * will also have greater than or equal simulation time.
     */
    protected BigDecimal simulationTime;

    /**
     * The animation time relative to the beginning of the animation frame. See
     * above for the definition of animation frame. The value must be greater
     * than or equal to zero. The value <code>null</code> means that it is not
     * yet set. The frame-relative animation times do not increase monotonically
     * along the animation. They are also independent of the selected animation
     * time origin.
     */
    protected Double frameRelativeAnimationTime;

    /**
     * The animation time relative to the origin of the animation. The origin is
     * specified with an event number and it refers to the first animation
     * moment of that event. The value may be any finite double number. The
     * value <code>null</code> means that it is not yet set. The origin-relative
     * animation times monotonically increase along the animation. This means
     * that if an animation position designates a moment later in the animation
     * then it will also have greater than or equal origin-relative animation
     * time.
     */
    protected Double originRelativeAnimationTime;

    public AnimationPosition() {
    }

    public AnimationPosition(Long eventNumber, BigDecimal simulationTime, Double frameRelativeAnimationTime, Double originRelativeAnimationTime) {
        setAnimationPosition(eventNumber, simulationTime, frameRelativeAnimationTime, originRelativeAnimationTime);
    }

    public AnimationPosition(AnimationPosition animationPosition) {
        setAnimationPosition(animationPosition.eventNumber, animationPosition.simulationTime, animationPosition.frameRelativeAnimationTime, animationPosition.originRelativeAnimationTime);
    }

    public void setAnimationPosition(Long eventNumber, BigDecimal simulationTime, Double frameRelativeAnimationTime, Double originRelativeAnimationTime) {
        setEventNumber(eventNumber);
        setSimulationTime(simulationTime);
        setFrameRelativeAnimationTime(frameRelativeAnimationTime);
        setOriginRelativeAnimationTime(originRelativeAnimationTime);
    }

    public Long getEventNumber() {
        return eventNumber;
    }

    public void setEventNumber(Long eventNumber) {
        Assert.isTrue(eventNumber == null || eventNumber >= 0);
        Assert.isTrue(this.eventNumber == null || this.eventNumber.equals(eventNumber));
        this.eventNumber = eventNumber;
    }

    public BigDecimal getSimulationTime() {
        return simulationTime;
    }

    public void setSimulationTime(BigDecimal simulationTime) {
        Assert.isTrue(this.simulationTime == null || this.simulationTime.equals(simulationTime));
        this.simulationTime = simulationTime;
    }

    public Double getFrameRelativeAnimationTime() {
        return frameRelativeAnimationTime;
    }

    public void setFrameRelativeAnimationTime(Double frameRelativeAnimationTime) {
        Assert.isTrue(this.frameRelativeAnimationTime == null || this.frameRelativeAnimationTime.equals(frameRelativeAnimationTime));
        this.frameRelativeAnimationTime = frameRelativeAnimationTime;
    }

    public Double getOriginRelativeAnimationTime() {
        return originRelativeAnimationTime;
    }

    public void setOriginRelativeAnimationTime(Double originRelativeAnimationTime) {
        // we must allow this change because the animation time coordinate system origin might change
        // Assert.isTrue(this.originRelativeAnimationTime == null || this.originRelativeAnimationTime.equals(originRelativeAnimationTime));
        Assert.isTrue(originRelativeAnimationTime == null || !Double.isNaN(originRelativeAnimationTime));
        this.originRelativeAnimationTime = originRelativeAnimationTime;
    }

    public boolean isCompletelyUnspecified() {
        return eventNumber == null && simulationTime == null && frameRelativeAnimationTime == null && originRelativeAnimationTime == null;
    }

    public boolean isCompletelySpecified() {
        return eventNumber != null && simulationTime != null && frameRelativeAnimationTime != null && originRelativeAnimationTime != null;
    }

    public boolean isPartiallySpecified() {
        return !isCompletelyUnspecified() && !isCompletelySpecified();
    }

    public int compareTo(AnimationPosition other) {
        if (this == other)
            return 0;
        else if (eventNumber != null && other.eventNumber != null && !eventNumber.equals(other.eventNumber))
            return (int)Math.signum(eventNumber - other.eventNumber);
        else if (simulationTime != null && other.simulationTime != null && !simulationTime.equals(other.simulationTime))
            return simulationTime.less(other.simulationTime) ? -1 : 1;
        else if (frameRelativeAnimationTime != null && other.frameRelativeAnimationTime != null)
            // TODO: this was part of the condition here, but I don't know why... should I delete it?
            // && eventNumber.equals(other.eventNumber) && simulationTime.equals(other.simulationTime) && frameRelativeAnimationTime < other.frameRelativeAnimationTime)
            return (int)Math.signum(frameRelativeAnimationTime - other.frameRelativeAnimationTime);
        else if (originRelativeAnimationTime != null && other.originRelativeAnimationTime != null)
            return (int)Math.signum(originRelativeAnimationTime - other.originRelativeAnimationTime);
        else
            throw new RuntimeException("Cannot compare the provided partially specified animation positions: " + this + " & " + other);
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((eventNumber == null) ? 0 : eventNumber.hashCode());
        result = prime * result + ((frameRelativeAnimationTime == null) ? 0 : frameRelativeAnimationTime.hashCode());
        result = prime * result + ((originRelativeAnimationTime == null) ? 0 : originRelativeAnimationTime.hashCode());
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
        if (frameRelativeAnimationTime == null) {
            if (other.frameRelativeAnimationTime != null)
                return false;
        }
        else if (!frameRelativeAnimationTime.equals(other.frameRelativeAnimationTime))
            return false;
        if (originRelativeAnimationTime == null) {
            if (other.originRelativeAnimationTime != null)
                return false;
        }
        else if (!originRelativeAnimationTime.equals(other.originRelativeAnimationTime))
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

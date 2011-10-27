package org.omnetpp.animation.controller;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.util.BinarySearchUtils;
import org.omnetpp.common.util.BinarySearchUtils.BoundKind;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

/**
 * This class provides a mapping between animation times and event numbers. The
 * animation time between two subsequent events is determined by a linear
 * interpolation of their simulation times. If the simulation times are equal
 * for the two events, then the mapping is not bijective.
 *
 * @author levy
 */
public class AnimationCoordinateSystem {
    private EventLogInput eventLogInput;

    private double nonLinearFocus;

    private double nonLinearMinimumAnimationTimeDelta;

    private ArrayList<AnimationPosition> animationPositions;

    private Map<Long, Double> eventNumberToAnimationTime = new HashMap<Long, Double>();

    public AnimationCoordinateSystem(EventLogInput eventLogInput, ArrayList<AnimationPosition> animationPositions) {
        this.eventLogInput = eventLogInput;
        this.animationPositions = animationPositions;
        this.nonLinearFocus = calculateNonLinearFocus();
        this.nonLinearMinimumAnimationTimeDelta = 0.1;
    }

    public int getSize() {
        return animationPositions.size();
    }

    public double getAnimationTime(IEvent event) {
        long eventNumber = event.getEventNumber();
        Double animationTime = eventNumberToAnimationTime.get(eventNumber);
        if (animationTime == null) {
            AnimationPosition eventAnimationPosition = new AnimationPosition(eventNumber, event.getSimulationTime(), 0.0, null);
            int index = (int)BinarySearchUtils.binarySearch(animationPositions, eventAnimationPosition, BinarySearchUtils.BoundKind.LOWER_BOUND);
            if (index < 0 || animationPositions.size() <= index)
                animationTime = 0.0;
            else {
                AnimationPosition foundAnimationPosition = animationPositions.get(index);
                if (foundAnimationPosition.getEventNumber() == eventNumber)
                    animationTime = foundAnimationPosition.getOriginRelativeAnimationTime();
                else {
                    if (index == 0)
                        animationTime = 0.0;
                    else
                        animationTime = animationPositions.get(index - 1).getOriginRelativeAnimationTime();
                }
            }
            eventNumberToAnimationTime.put(eventNumber, animationTime);
        }
        return animationTime;
    }

    public double getAnimationTimeForSimulationTime(BigDecimal simulationTime) {
        AnimationPosition simulationTimeAnimationPosition = new AnimationPosition(null, simulationTime, 0.0, null);
        int index = (int)BinarySearchUtils.binarySearch(animationPositions, simulationTimeAnimationPosition, BinarySearchUtils.BoundKind.LOWER_BOUND);
        if (index == 0)
            return animationPositions.get(index).getOriginRelativeAnimationTime();
        else {
            AnimationPosition beginAnimationPosition = animationPositions.get(index - 1);
            AnimationPosition endAnimationPosition = animationPositions.get(index);
            Assert.isTrue(beginAnimationPosition.getSimulationTime().lessOrEqual(simulationTime) && simulationTime.lessOrEqual(endAnimationPosition.getSimulationTime()));
            double ratio = simulationTime.subtract(beginAnimationPosition.getSimulationTime()).doubleValue() / endAnimationPosition.getSimulationTime().subtract(beginAnimationPosition.getSimulationTime()).doubleValue();
            if (Double.isNaN(ratio)) // all times are equal
                ratio = 0;
            double beginAnimationTime = beginAnimationPosition.getOriginRelativeAnimationTime();
            double endAnimationTime = endAnimationPosition.getOriginRelativeAnimationTime();
            // make sure we are precise at the boundaries and never return invalid values due to double arithmetic
            if (ratio == 0.0)
                return beginAnimationTime;
            else if (ratio == 1.0)
                return endAnimationTime;
            else {
                double animationTime = beginAnimationTime + (endAnimationTime - beginAnimationTime) * ratio;
                if (animationTime < beginAnimationTime)
                    return beginAnimationTime;
                else if (animationTime > endAnimationTime)
                    return endAnimationTime;
                else
                    return animationTime;
            }
        }
    }

    public IEvent getLastEventNotAfterSimulationTime(BigDecimal simulationTime) {
        return eventLogInput.getEventLog().getLastEventNotAfterSimulationTime(simulationTime);
    }

    public BigDecimal getSimulationTimeForAnimationTime(double animationTime) {
        AnimationPosition simulationTimeAnimationPosition = new AnimationPosition(null, null, null, animationTime);
        int index = (int)BinarySearchUtils.binarySearch(animationPositions, simulationTimeAnimationPosition, BinarySearchUtils.BoundKind.LOWER_BOUND);
        if (index == 0)
            return animationPositions.get(index).getSimulationTime();
        else {
            AnimationPosition beginAnimationPosition = animationPositions.get(index - 1);
            AnimationPosition endAnimationPosition = animationPositions.get(index);
            Assert.isTrue(beginAnimationPosition.getOriginRelativeAnimationTime() <= animationTime && animationTime <= endAnimationPosition.getOriginRelativeAnimationTime());
            double ratio = (animationTime - beginAnimationPosition.getOriginRelativeAnimationTime()) / (endAnimationPosition.getOriginRelativeAnimationTime() - beginAnimationPosition.getOriginRelativeAnimationTime());
            if (Double.isNaN(ratio)) // all times are equal
                ratio = 0;
            BigDecimal beginSimulationTime = beginAnimationPosition.getSimulationTime();
            BigDecimal endSimulationTime = endAnimationPosition.getSimulationTime();
            // make sure we are precise at the boundaries and never return invalid values due to double arithmetic
            if (ratio == 0.0)
                return beginSimulationTime;
            else if (ratio == 1.0)
                return endSimulationTime;
            else {
                BigDecimal simulationTime = beginSimulationTime.add(new BigDecimal(endSimulationTime.subtract(beginSimulationTime).doubleValue() * ratio));
                if (simulationTime.less(beginSimulationTime))
                    return beginSimulationTime;
                else if (simulationTime.greater(endSimulationTime))
                    return endSimulationTime;
                else
                    return simulationTime;
            }
        }
    }

    public IEvent getLastEventNotAfterAnimationTime(final double animationTime) {
        final IEventLog eventLog = eventLogInput.getEventLog();
        long eventLogSize = eventLog.getLastEvent().getEventNumber() + 1;
        BinarySearchUtils.KeyComparator keyComparator = new BinarySearchUtils.KeyComparator() {
            public int compareTo(long eventNumber) {
                return (int)Math.signum(animationTime - getAnimationTime(eventLog.getEventForEventNumber(eventNumber)));
            }
        };
        long eventNumber = BinarySearchUtils.binarySearch(keyComparator, eventLogSize, BoundKind.UPPER_BOUND);
        if (eventNumber > eventLog.getLastEvent().getEventNumber())
            return eventLog.getLastEvent();
        else
            return eventLog.getEventForEventNumber(eventNumber - 1);
    }

    public double getAnimationTimeDelta(double simulationTimeDelta) {
        // step mode
        return 1;
//        TODO: revive: linear mode
//        if (simulationTimeDelta == 0)
//            return 1;
//        else
//            return simulationTimeDelta;
//        TODO: revive: non linear mode
//        return nonLinearMinimumAnimationTimeDelta + (1 - nonLinearMinimumAnimationTimeDelta) * Math.atan(Math.abs(simulationTimeDelta) / nonLinearFocus) / Math.PI * 2;
    }

    private double calculateNonLinearFocus() {
        IEventLog eventLog = eventLogInput.getEventLog();
        if (!eventLog.isEmpty()) {
            double lastEventSimulationTime = eventLog.getLastEvent().getSimulationTime().doubleValue();
            double firstEventSimulationTime = eventLog.getFirstEvent().getSimulationTime().doubleValue();
            double totalSimulationTimeDelta = lastEventSimulationTime - firstEventSimulationTime;
            if (totalSimulationTimeDelta == 0)
                totalSimulationTimeDelta = firstEventSimulationTime;
            if (totalSimulationTimeDelta == 0)
                return 1;
            else
                return totalSimulationTimeDelta / eventLog.getApproximateNumberOfEvents() / 10;
        }
        else
            return 1;
    }
}

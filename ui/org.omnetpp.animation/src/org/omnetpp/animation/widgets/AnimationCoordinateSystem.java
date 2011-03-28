package org.omnetpp.animation.widgets;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

/**
 *
 * @author levy
 */
// TODO: binary search
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

    public double getAnimationTime(IEvent event) {
        long eventNumber = event.getEventNumber();
        Double animationTime = eventNumberToAnimationTime.get(eventNumber);
        if (animationTime == null) {
            // TODO: binary search
            for (int i = 0; i < animationPositions.size(); i++) {
                AnimationPosition animationPosition = animationPositions.get(i);
                if (animationPosition.getEventNumber() == eventNumber) {
                    animationTime = animationPosition.getOriginRelativeAnimationTime();
                    break;
                }
                else if (animationPosition.getEventNumber() > eventNumber) {
                    if (i == 0)
                        animationTime = 0.0;
                    else
                        animationTime = animationPositions.get(i - 1).getOriginRelativeAnimationTime();
                    break;
                }
            }
            eventNumberToAnimationTime.put(eventNumber, animationTime);
        }
        return animationTime;
    }

    public double getAnimationTimeForSimulationTime(BigDecimal simulationTime) {
        // TODO: binary search
        for (int i = 0; i < animationPositions.size() - 1; i++) {
            AnimationPosition beginAnimationPosition = animationPositions.get(i);
            AnimationPosition endAnimationPosition = animationPositions.get(i + 1);
            if (beginAnimationPosition.getSimulationTime().lessOrEqual(simulationTime) && simulationTime.lessOrEqual(endAnimationPosition.getSimulationTime())) {
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
        throw new RuntimeException("Invalid animation time");
    }

    public IEvent getLastEventNotAfterSimulationTime(BigDecimal simulationTime) {
        return eventLogInput.getEventLog().getLastEventNotAfterSimulationTime(simulationTime);
    }

    public BigDecimal getSimulationTimeForAnimationTime(double animationTime) {
        // TODO: binary search
        for (int i = 0; i < animationPositions.size() - 1; i++) {
            AnimationPosition beginAnimationPosition = animationPositions.get(i);
            AnimationPosition endAnimationPosition = animationPositions.get(i + 1);
            if (beginAnimationPosition.getOriginRelativeAnimationTime() <= animationTime && animationTime <= endAnimationPosition.getOriginRelativeAnimationTime()) {
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
        throw new RuntimeException("Invalid animation time");
    }

    public IEvent getLastEventNotAfterAnimationTime(double animationTime) {
        IEventLog eventLog = eventLogInput.getEventLog();
        IEvent event = eventLog.getFirstEvent();
        // TODO: binary search
        while (event != null && getAnimationTime(event) <= animationTime)
            event = event.getNextEvent();
        if (event == null)
            return eventLog.getLastEvent();
        else
            return event.getPreviousEvent();
    }

    public double getAnimationTimeDelta(double simulationTimeDelta) {
        return 1;
//        if (simulationTimeDelta == 0)
//            return 1;
//        else
//            return simulationTimeDelta;
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

package org.omnetpp.eventlog;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Map;
import java.util.Random;
import java.util.TreeMap;
import java.util.TreeSet;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.BeginSendEntry;
import org.omnetpp.eventlog.entry.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.entry.ComponentMethodEndEntry;

public class SequenceChartFacade extends EventLogFacade
{
    protected boolean separateEventLogEntries; // separate entries within events
    protected int timelineCoordinateSystemVersion = -1; // a counter incremented each time the timeline coordinate system is relocated
    protected long timelineCoordinateSystemOriginEventNumber = -1; // -1 means undefined, otherwise the event number of the timeline coordinate system origin
    protected BigDecimal timelineCoordinateSystemOriginSimulationTime = null; // simtime_nil means undefined
    protected long timelineCoordinateRangeStartEventNumber = -1; // -1 means undefined, the beginning of the continuous event range which has timeline coordinates assigned
    protected long timelineCoordinateRangeEndEventNumber = -1; // -1 means undefined, the end of the continuous event range which has timeline coordinates assigned
    protected TimelineMode timelineMode = TimelineMode.NONLINEAR;
    protected double nonLinearFocus = -1; // a useful constant for the nonlinear transformation between simulation time and timeline coordinate
    protected double nonLinearMinimumTimelineCoordinateDelta = 0.1; // minimum timeline coordinate difference between two events

    protected static final class RefObject<T>
    {
        public T argValue;

        public RefObject(T refArg) {
            argValue = refArg;
        }
    }

    public SequenceChartFacade(IEventLog eventLog) {
        super(eventLog);
        clearInternalState();
        setTimelineMode(TimelineMode.NONLINEAR);
    }

    public final double calculateNonLinearFocus() {
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

    @Override
    public void synchronize(int change) {
        if (change != FileReader.FileChange.UNCHANGED) {
            IEvent event;
            switch (change) {
                case FileReader.FileChange.OVERWRITTEN:
                    super.synchronize(change);
                    clearInternalState();
                    break;
                case FileReader.FileChange.APPENDED:
                    super.synchronize(change);
                    event = timelineCoordinateSystemOriginEventNumber != -1 ? eventLog.getEventForEventNumber(timelineCoordinateSystemOriginEventNumber, MatchKind.EXACT, true) : null;
                    if (event != null)
                        relocateTimelineCoordinateSystem(event);
                    else
                        undefineTimelineCoordinateSystem();
                    break;
                default:
                    throw new RuntimeException("Unknown file change");
            }
        }
    }

    public final TimelineMode getTimelineMode() {
        return timelineMode;
    }

    public final void setTimelineMode(TimelineMode timelineMode) {
        this.timelineMode = timelineMode;
        if (timelineCoordinateSystemOriginEventNumber != -1)
            relocateTimelineCoordinateSystem(eventLog.getEventForEventNumber(timelineCoordinateSystemOriginEventNumber));
    }

    public final boolean getSeparateEventLogEntries() {
        return separateEventLogEntries;
    }

    public final void setSeparateEventLogEntries(boolean separateEventLogEntries) {
        this.separateEventLogEntries = separateEventLogEntries;
    }

    public final double getNonLinearMinimumTimelineCoordinateDelta() {
        return nonLinearMinimumTimelineCoordinateDelta;
    }

    public final void setNonLinearMinimumTimelineCoordinateDelta(double value) {
        Assert.isTrue(value >= 0);
        nonLinearMinimumTimelineCoordinateDelta = value;
    }

    public final double getNonLinearFocus() {
        if (nonLinearFocus == -1)
            nonLinearFocus = calculateNonLinearFocus();
        return nonLinearFocus;
    }

    public final void setNonLinearFocus(double nonLinearFocus) {
        Assert.isTrue(nonLinearFocus >= 0);
        this.nonLinearFocus = nonLinearFocus;
    }

    public final IEvent getTimelineCoordinateSystemOriginEvent() {
        return eventLog.getEventForEventNumber(timelineCoordinateSystemOriginEventNumber);
    }

    public final long getTimelineCoordinateSystemOriginEventNumber() {
        return timelineCoordinateSystemOriginEventNumber;
    }

    public final void undefineTimelineCoordinateSystem() {
        timelineCoordinateSystemVersion++;
        timelineCoordinateSystemOriginEventNumber = timelineCoordinateRangeStartEventNumber = timelineCoordinateRangeEndEventNumber = -1;
        timelineCoordinateSystemOriginSimulationTime = BigDecimal.getMinusOne();
    }

    public final void relocateTimelineCoordinateSystem(IEvent event) {
        Assert.isTrue(event != null);
        timelineCoordinateSystemVersion++;
        timelineCoordinateSystemOriginEventNumber = timelineCoordinateRangeStartEventNumber = timelineCoordinateRangeEndEventNumber = event.getEventNumber();
        timelineCoordinateSystemOriginSimulationTime = event.getSimulationTime();
        event.setCachedTimelineCoordinateBegin(0);
        event.setCachedTimelineCoordinateEnd(getTimelineCoordinateDelta(event));
        event.setCachedTimelineCoordinateSystemVersion(timelineCoordinateSystemVersion);
    }

    /**
     * Calculates the timeline coordinate for the given event. Returns value from the cache if already there or calculates it
     * while taking care about the calculation limits.
     */
    public final double getTimelineCoordinateDelta(double simulationTimeDelta) {
        Assert.isTrue(getNonLinearFocus() > 0);

        if (timelineMode == TimelineMode.STEP)
            return 1;
        else
            return nonLinearMinimumTimelineCoordinateDelta + (1 - nonLinearMinimumTimelineCoordinateDelta) * Math.atan(Math.abs(simulationTimeDelta / getNonLinearFocus())) / Math.PI * 2;
    }

    public final double getTimelineCoordinateDelta(IEvent event) {
        if (!separateEventLogEntries || timelineMode == TimelineMode.SIMULATION_TIME || timelineMode == TimelineMode.EVENT_NUMBER)
            return 0;
        else if (timelineMode == TimelineMode.STEP || timelineMode == TimelineMode.NONLINEAR) {
            double timelineCoordinateDelta = 0;
            for (int i = 0; i < event.getNumEventLogEntries(); i++) {
                EventLogEntry eventLogEntry = event.getEventLogEntry(i);
                if (eventLogEntry instanceof ComponentMethodBeginEntry)
                    timelineCoordinateDelta += timelineMode == TimelineMode.STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
                else if (eventLogEntry instanceof ComponentMethodEndEntry)
                    timelineCoordinateDelta += timelineMode == TimelineMode.STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
                else if (eventLogEntry instanceof BeginSendEntry)
                    timelineCoordinateDelta += timelineMode == TimelineMode.STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
            }
            return timelineCoordinateDelta;
        }
        else
            throw new RuntimeException("Unknown timeline mode");
    }

    public final double getTimelineCoordinateBegin(IEvent event) {
        return getTimelineCoordinateBegin(event, -Double.MAX_VALUE, Double.MAX_VALUE);
    }

    public final double getTimelineCoordinateBegin(IEvent event, double lowerTimelineCoordinateCalculationLimit) {
        return getTimelineCoordinateBegin(event, lowerTimelineCoordinateCalculationLimit, Double.MAX_VALUE);
    }

    public final double getTimelineCoordinateBegin(IEvent event, double lowerTimelineCoordinateCalculationLimit, double upperTimelineCoordinateCalculationLimit) {
        Assert.isTrue(event != null);
        Assert.isTrue(event.getEventLog() == eventLog);
        Assert.isTrue(timelineCoordinateSystemVersion != -1);
        Assert.isTrue(timelineCoordinateSystemOriginEventNumber != -1);
        if (this.timelineCoordinateSystemVersion > event.getCachedTimelineCoordinateSystemVersion()) {
            double timelineCoordinateBegin;
            switch (timelineMode) {
                case SIMULATION_TIME:
                    timelineCoordinateBegin = event.getSimulationTime().subtract(timelineCoordinateSystemOriginSimulationTime).doubleValue();
                    break;
                case EVENT_NUMBER:
                    timelineCoordinateBegin = event.getEventNumber() - timelineCoordinateSystemOriginEventNumber;
                    break;
                case STEP:
                case NONLINEAR: {
                    IEvent previousEvent = null;
                    // do we go forward from end or backward from start of known range
                    boolean forward = event.getEventNumber() > timelineCoordinateRangeEndEventNumber;
                    IEvent currentEvent = eventLog.getEventForEventNumber(forward ? timelineCoordinateRangeEndEventNumber : timelineCoordinateRangeStartEventNumber);
                    Assert.isTrue(event.getEventNumber() < timelineCoordinateRangeStartEventNumber || timelineCoordinateRangeEndEventNumber < event.getEventNumber());
                    // LONG RUNNING OPERATION
                    // does a linear search towards the event to calculate the non linear timeline coordinate
                    do {
                        eventLog.progress();
                        Assert.isTrue(currentEvent != null);
                        previousEvent = currentEvent;
                        currentEvent = forward ? currentEvent.getNextEvent() : currentEvent.getPreviousEvent();
                        Assert.isTrue(currentEvent != null);

                        BigDecimal previousSimulationTime = previousEvent.getSimulationTime();
                        double previousTimelineCoordinateBegin = previousEvent.getCachedTimelineCoordinateBegin();
                        BigDecimal simulationTime = currentEvent.getSimulationTime();
                        double timelineCoordinateDelta = getTimelineCoordinateDelta(simulationTime.subtract(previousSimulationTime).doubleValue()) + getTimelineCoordinateDelta(forward ? previousEvent : currentEvent);

                        if (forward) {
                            timelineCoordinateBegin = previousTimelineCoordinateBegin + timelineCoordinateDelta;
                            if (timelineCoordinateBegin > upperTimelineCoordinateCalculationLimit)
                                return Double.NaN;
                        }
                        else {
                            timelineCoordinateBegin = previousTimelineCoordinateBegin - timelineCoordinateDelta;
                            if (timelineCoordinateBegin < lowerTimelineCoordinateCalculationLimit)
                                return Double.NaN;
                        }
                        currentEvent.setCachedTimelineCoordinateBegin(timelineCoordinateBegin);
                        currentEvent.setCachedTimelineCoordinateEnd(timelineCoordinateBegin + getTimelineCoordinateDelta(currentEvent));
                        currentEvent.setCachedTimelineCoordinateSystemVersion(timelineCoordinateSystemVersion);
                    } while (currentEvent != event);
                    if (forward)
                        timelineCoordinateRangeEndEventNumber = event.getEventNumber();
                    else
                        timelineCoordinateRangeStartEventNumber = event.getEventNumber();
                    break;
                }
                default:
                    throw new RuntimeException("Unknown timeline mode");
            }
            event.setCachedTimelineCoordinateBegin(timelineCoordinateBegin);
            event.setCachedTimelineCoordinateEnd(timelineCoordinateBegin + getTimelineCoordinateDelta(event));
            event.setCachedTimelineCoordinateSystemVersion(timelineCoordinateSystemVersion);
        }
        return event.getCachedTimelineCoordinateBegin();
    }

    public final double getTimelineCoordinateEnd(IEvent event) {
        return getTimelineCoordinateEnd(event, -Double.MAX_VALUE, Double.MAX_VALUE);
    }

    public final double getTimelineCoordinateEnd(IEvent event, double lowerTimelineCoordinateCalculationLimit) {
        return getTimelineCoordinateEnd(event, lowerTimelineCoordinateCalculationLimit, Double.MAX_VALUE);
    }

    public final double getTimelineCoordinateEnd(IEvent event, double lowerTimelineCoordinateCalculationLimit, double upperTimelineCoordinateCalculationLimit) {
        return getTimelineCoordinateDelta(event) + getTimelineCoordinateBegin(event, lowerTimelineCoordinateCalculationLimit, upperTimelineCoordinateCalculationLimit);
    }

    public final double getCachedTimelineCoordinateBegin(IEvent event) {
        Assert.isTrue(event != null);
        Assert.isTrue(timelineCoordinateSystemVersion != -1);
        Assert.isTrue(timelineCoordinateSystemOriginEventNumber != -1);
        if (this.timelineCoordinateSystemVersion > event.getCachedTimelineCoordinateSystemVersion())
            return -1;
        else
            return event.getCachedTimelineCoordinateBegin();
    }

    public final double getCachedTimelineCoordinateEnd(IEvent event) {
        Assert.isTrue(event != null);
        Assert.isTrue(timelineCoordinateSystemVersion != -1);
        Assert.isTrue(timelineCoordinateSystemOriginEventNumber != -1);
        if (this.timelineCoordinateSystemVersion > event.getCachedTimelineCoordinateSystemVersion())
            return -1;
        else
            return event.getCachedTimelineCoordinateEnd();
    }

    public double EventLogEntry_getTimelineCoordinate(EventLogEntry eventLogEntry) {
        IEvent event = eventLogEntry.getEvent();
        if (event.getEventLog() != eventLog) {
            // TODO: this may be slow
            event = eventLog.getEventForEventNumber(event.getEventNumber());
            Assert.isTrue(event != null);
        }
        if (!separateEventLogEntries || timelineMode == TimelineMode.SIMULATION_TIME || timelineMode == TimelineMode.EVENT_NUMBER)
            return getTimelineCoordinateBegin(event);
        else if (timelineMode == TimelineMode.STEP || timelineMode == TimelineMode.NONLINEAR) {
            double timelineCoordinateDelta = timelineMode == TimelineMode.STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
            for (int i = 0; i < event.getNumEventLogEntries(); i++) {
                EventLogEntry currentEventLogEntry = event.getEventLogEntry(i);
                // TODO: factor with getTimelineCoordinateDelta
                if (eventLogEntry == currentEventLogEntry)
                    return getTimelineCoordinateBegin(event) + timelineCoordinateDelta;
                else if (currentEventLogEntry instanceof ComponentMethodBeginEntry)
                    timelineCoordinateDelta += timelineMode == TimelineMode.STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
                else if (currentEventLogEntry instanceof ComponentMethodEndEntry)
                    timelineCoordinateDelta += timelineMode == TimelineMode.STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
                else if (currentEventLogEntry instanceof BeginSendEntry)
                    timelineCoordinateDelta += timelineMode == TimelineMode.STEP ? 1 : nonLinearMinimumTimelineCoordinateDelta;
            }
            throw new RuntimeException("EventLogEntry not found");
        }
        else
            throw new RuntimeException("Unknown timeline mode");
    }

    public EventLogEntry EventLogEntry_getNextComponentMethodEntry(EventLogEntry eventLogEntry) {
        IEvent event = eventLogEntry.getEvent();
        for (int i = eventLogEntry.getEntryIndex() + 1; i < event.getNumEventLogEntries(); i++) {
            EventLogEntry currentEventLogEntry = event.getEventLogEntry(i);
            if (currentEventLogEntry instanceof ComponentMethodBeginEntry || currentEventLogEntry instanceof ComponentMethodEndEntry)
                return currentEventLogEntry;
        }
        return null;
    }

    public final IEvent getEventForNonLinearTimelineCoordinate(double timelineCoordinate, RefObject<Boolean> forward) {
        Assert.isTrue(timelineCoordinateSystemOriginEventNumber != -1);
        IEvent timelineCoordinateRangeStartEvent = eventLog.getEventForEventNumber(timelineCoordinateRangeStartEventNumber);
        IEvent timelineCoordinateRangeEndEvent = eventLog.getEventForEventNumber(timelineCoordinateRangeEndEventNumber);
        IEvent currentEvent;
        Assert.isTrue(timelineCoordinateRangeStartEvent != null && timelineCoordinateRangeEndEvent != null);
        if (timelineCoordinate <= getTimelineCoordinateBegin(timelineCoordinateRangeStartEvent)) {
            forward.argValue = false;
            currentEvent = timelineCoordinateRangeStartEvent;
        }
        else if (getTimelineCoordinateBegin(timelineCoordinateRangeEndEvent) <= timelineCoordinate) {
            forward.argValue = true;
            currentEvent = timelineCoordinateRangeEndEvent;
        }
        else {
            forward.argValue = true;
            currentEvent = timelineCoordinateRangeStartEvent;
        }
        // LONG RUNNING OPERATION
        // does a linear search towards requested non linear timeline coordinate
        while (currentEvent != null && (forward.argValue ? getTimelineCoordinateBegin(currentEvent) < timelineCoordinate : timelineCoordinate <= getTimelineCoordinateBegin(currentEvent))) {
            eventLog.progress();
            currentEvent = forward.argValue ? currentEvent.getNextEvent() : currentEvent.getPreviousEvent();
        }
        return currentEvent;
    }

    public final IEvent getLastEventNotAfterTimelineCoordinate(double timelineCoordinate) {
        if (eventLog.isEmpty())
            return null;
        switch (timelineMode) {
            case SIMULATION_TIME:
                return eventLog.getLastEventNotAfterSimulationTime(getSimulationTimeForTimelineCoordinate(timelineCoordinate));
            case EVENT_NUMBER: {
                long eventNumber = (long)Math.floor(timelineCoordinate) + timelineCoordinateSystemOriginEventNumber;
                if (eventNumber < 0)
                    return null;
                else
                    return eventLog.getLastEventNotAfterEventNumber(eventNumber);
            }
            case STEP:
            case NONLINEAR: {
                RefObject<Boolean> tempRef_forward = new RefObject<Boolean>(null);
                IEvent currentEvent = getEventForNonLinearTimelineCoordinate(timelineCoordinate, tempRef_forward);
                boolean forward = tempRef_forward.argValue;
                currentEvent = forward ? (currentEvent != null ? currentEvent.getPreviousEvent() : eventLog.getLastEvent()) : currentEvent;
                Assert.isTrue(currentEvent == null || getTimelineCoordinateBegin(currentEvent) <= timelineCoordinate);
                return currentEvent;
            }
            default:
                throw new RuntimeException("Unknown timeline mode");
        }
    }

    public final IEvent getFirstEventNotBeforeTimelineCoordinate(double timelineCoordinate) {
        if (eventLog.isEmpty())
            return null;
        switch (timelineMode) {
            case SIMULATION_TIME:
                return eventLog.getFirstEventNotBeforeSimulationTime(getSimulationTimeForTimelineCoordinate(timelineCoordinate));
            case EVENT_NUMBER: {
                long eventNumber = (long)Math.floor(timelineCoordinate) + timelineCoordinateSystemOriginEventNumber;
                if (eventNumber < 0)
                    return eventLog.getFirstEvent();
                else
                    return eventLog.getFirstEventNotBeforeEventNumber(eventNumber);
            }
            case STEP:
            case NONLINEAR: {
                RefObject<Boolean> tempRef_forward = new RefObject<Boolean>(null);
                IEvent currentEvent = getEventForNonLinearTimelineCoordinate(timelineCoordinate, tempRef_forward);
                boolean forward = tempRef_forward.argValue;
                currentEvent = forward ? currentEvent : (currentEvent != null ? currentEvent.getNextEvent() : eventLog.getFirstEvent());

                Assert.isTrue(currentEvent == null || getTimelineCoordinateBegin(currentEvent) >= timelineCoordinate);
                return currentEvent;
            }
            default:
                throw new RuntimeException("Unknown timeline mode");
        }
    }

    /**
     * Timeline coordinate can be given in the range (-infinity, +infinity).
     * Simulation time will be in the range [0, lastEventSimulationTime].
     */
    public final BigDecimal getSimulationTimeForTimelineCoordinate(double timelineCoordinate) {
        return getSimulationTimeForTimelineCoordinate(timelineCoordinate, false);
    }

    public final BigDecimal getSimulationTimeForTimelineCoordinate(double timelineCoordinate, boolean upperLimit) {
        Assert.isTrue(!Double.isNaN(timelineCoordinate));
        if (eventLog.isEmpty())
            return BigDecimal.getZero();
        BigDecimal simulationTime;
        switch (timelineMode) {
            case SIMULATION_TIME: {
                BigDecimal lastEventSimulationTime = eventLog.getLastEvent().getSimulationTime();
                simulationTime = bigDecimalMax(BigDecimal.getZero(), bigDecimalMin(lastEventSimulationTime, new BigDecimal(timelineCoordinate).add(timelineCoordinateSystemOriginSimulationTime)));
                break;
            }
            case EVENT_NUMBER:
            case STEP:
            case NONLINEAR: {
                IEvent event = getLastEventNotAfterTimelineCoordinate(timelineCoordinate);
                IEvent nextEvent;
                BigDecimal eventSimulationTime = BigDecimal.getMinusOne();
                BigDecimal nextEventSimulationTime = BigDecimal.getMinusOne();
                BigDecimal simulationTimeDelta = BigDecimal.getZero();
                double eventTimelineCoordinateBegin = Double.NaN;
                double eventTimelineCoordinateEnd = Double.NaN;
                double nextEventTimelineCoordinateBegin = Double.NaN;
                double nextEventTimelineCoordinateEnd = Double.NaN;
                double timelineCoordinateDelta = Double.NaN;

                RefObject<IEvent> tempRef_nextEvent = new RefObject<IEvent>(null);
                RefObject<BigDecimal> tempRef_eventSimulationTime = new RefObject<BigDecimal>(eventSimulationTime);
                RefObject<Double> tempRef_eventTimelineCoordinateBegin = new RefObject<Double>(eventTimelineCoordinateBegin);
                RefObject<Double> tempRef_eventTimelineCoordinateEnd = new RefObject<Double>(eventTimelineCoordinateEnd);
                RefObject<Double> tempRef_nextEventTimelineCoordinateBegin = new RefObject<Double>(nextEventTimelineCoordinateBegin);
                RefObject<Double> tempRef_nextEventTimelineCoordinateEnd = new RefObject<Double>(nextEventTimelineCoordinateEnd);
                RefObject<BigDecimal> tempRef_nextEventSimulationTime = new RefObject<BigDecimal>(nextEventSimulationTime);
                RefObject<BigDecimal> tempRef_simulationTimeDelta = new RefObject<BigDecimal>(simulationTimeDelta);
                RefObject<Double> tempRef_timelineCoordinateDelta = new RefObject<Double>(timelineCoordinateDelta);
                extractSimulationTimesAndTimelineCoordinates(event, tempRef_nextEvent, tempRef_eventSimulationTime, tempRef_eventTimelineCoordinateBegin, tempRef_eventTimelineCoordinateEnd, tempRef_nextEventSimulationTime, tempRef_nextEventTimelineCoordinateBegin, tempRef_nextEventTimelineCoordinateEnd, tempRef_simulationTimeDelta, tempRef_timelineCoordinateDelta);
                eventSimulationTime = tempRef_eventSimulationTime.argValue;
                timelineCoordinateDelta = tempRef_timelineCoordinateDelta.argValue;
                nextEventTimelineCoordinateEnd = tempRef_nextEventTimelineCoordinateEnd.argValue;
                nextEventTimelineCoordinateBegin = tempRef_nextEventTimelineCoordinateBegin.argValue;
                eventTimelineCoordinateEnd = tempRef_eventTimelineCoordinateEnd.argValue;
                nextEventSimulationTime = tempRef_nextEventSimulationTime.argValue;
                simulationTimeDelta = tempRef_simulationTimeDelta.argValue;
                eventTimelineCoordinateBegin = tempRef_eventTimelineCoordinateBegin.argValue;
                nextEvent = tempRef_nextEvent.argValue;

                if (nextEvent != null) {
                    if (timelineCoordinateDelta == 0) {
                        // IMPORTANT NOTE: this is just an approximation
                        if (upperLimit)
                            simulationTime = nextEventSimulationTime;
                        else
                            simulationTime = eventSimulationTime;
                    }
                    else {
                        timelineCoordinate = Math.max(eventTimelineCoordinateEnd, Math.min(nextEventTimelineCoordinateBegin, timelineCoordinate));
                        simulationTime = eventSimulationTime.add(new BigDecimal(simulationTimeDelta.doubleValue() * (timelineCoordinate - eventTimelineCoordinateEnd) / timelineCoordinateDelta));
                        simulationTime = bigDecimalMax(eventSimulationTime, bigDecimalMin(nextEventSimulationTime, simulationTime));
                    }
                }
                else
                    simulationTime = eventSimulationTime;
                break;
            }
            default:
                throw new RuntimeException("Unknown timeline mode");
        }
        Assert.isTrue(!simulationTime.isNaN());
        Assert.isTrue(simulationTime.greaterOrEqual(BigDecimal.getZero()));
        Assert.isTrue(simulationTime.lessOrEqual(eventLog.getLastEvent().getSimulationTime()));
        return simulationTime;
    }

    private BigDecimal bigDecimalMax(BigDecimal a, BigDecimal b) {
        if (a.greater(b))
            return a;
        else
            return b;
    }

    private BigDecimal bigDecimalMin(BigDecimal a, BigDecimal b) {
        if (a.less(b))
            return a;
        else
            return b;
    }

    /**
     * Simulation time must be in the range [0, lastEventSimulationTime].
     * Timeline coordinate will be in the range [0, lastEventTimelineCoordinate] if the
     * timeline origin is at the first event.
     */
    public double getTimelineCoordinateForSimulationTime(BigDecimal simulationTime) {
        return getTimelineCoordinateForSimulationTime(simulationTime, false);
    }

    public double getTimelineCoordinateForSimulationTime(BigDecimal simulationTime, boolean upperLimit) {
        Assert.isTrue(!simulationTime.isNaN());
        if (eventLog.isEmpty())
            return 0;
        Assert.isTrue(simulationTime.greaterOrEqual(BigDecimal.getZero()));
     //   Assert.isTrue(simulationTime.lessOrEqual(eventLog.getLastEvent().getSimulationTime()));
        double timelineCoordinate;
        switch (timelineMode) {
            case SIMULATION_TIME:
                timelineCoordinate = simulationTime.subtract(timelineCoordinateSystemOriginSimulationTime).doubleValue();
                break;
            case EVENT_NUMBER:
            case STEP:
            case NONLINEAR: {
                IEvent nextEvent;
                BigDecimal eventSimulationTime = BigDecimal.getMinusOne();
                BigDecimal nextEventSimulationTime = BigDecimal.getMinusOne();
                BigDecimal simulationTimeDelta = BigDecimal.getZero();
                double eventTimelineCoordinateBegin = Double.NaN;
                double eventTimelineCoordinateEnd = Double.NaN;
                double nextEventTimelineCoordinateBegin = Double.NaN;
                double nextEventTimelineCoordinateEnd = Double.NaN;
                double timelineCoordinateDelta = Double.NaN;

                IEvent event = eventLog.getLastEventNotAfterSimulationTime(simulationTime);
                RefObject<IEvent> tempRef_nextEvent = new RefObject<IEvent>(null);
                RefObject<BigDecimal> tempRef_eventSimulationTime = new RefObject<BigDecimal>(eventSimulationTime);
                RefObject<Double> tempRef_eventTimelineCoordinateBegin = new RefObject<Double>(eventTimelineCoordinateBegin);
                RefObject<Double> tempRef_eventTimelineCoordinateEnd = new RefObject<Double>(eventTimelineCoordinateEnd);
                RefObject<Double> tempRef_nextEventTimelineCoordinateBegin = new RefObject<Double>(nextEventTimelineCoordinateBegin);
                RefObject<Double> tempRef_nextEventTimelineCoordinateEnd = new RefObject<Double>(nextEventTimelineCoordinateEnd);
                RefObject<BigDecimal> tempRef_nextEventSimulationTime = new RefObject<BigDecimal>(nextEventSimulationTime);
                RefObject<BigDecimal> tempRef_simulationTimeDelta = new RefObject<BigDecimal>(simulationTimeDelta);
                RefObject<Double> tempRef_timelineCoordinateDelta = new RefObject<Double>(timelineCoordinateDelta);
                extractSimulationTimesAndTimelineCoordinates(event, tempRef_nextEvent, tempRef_eventSimulationTime, tempRef_eventTimelineCoordinateBegin, tempRef_eventTimelineCoordinateEnd, tempRef_nextEventSimulationTime, tempRef_nextEventTimelineCoordinateBegin, tempRef_nextEventTimelineCoordinateEnd, tempRef_simulationTimeDelta, tempRef_timelineCoordinateDelta);
                eventSimulationTime = tempRef_eventSimulationTime.argValue;
                timelineCoordinateDelta = tempRef_timelineCoordinateDelta.argValue;
                nextEventTimelineCoordinateEnd = tempRef_nextEventTimelineCoordinateEnd.argValue;
                nextEventTimelineCoordinateBegin = tempRef_nextEventTimelineCoordinateBegin.argValue;
                eventTimelineCoordinateEnd = tempRef_eventTimelineCoordinateEnd.argValue;
                nextEventSimulationTime = tempRef_nextEventSimulationTime.argValue;
                simulationTimeDelta = tempRef_simulationTimeDelta.argValue;
                eventTimelineCoordinateBegin = tempRef_eventTimelineCoordinateBegin.argValue;
                nextEvent = tempRef_nextEvent.argValue;

                if (nextEvent != null) {
                    if (simulationTimeDelta.equals(BigDecimal.getZero())) {
                        // IMPORTANT NOTE: this is just an approximation
                        if (upperLimit)
                            timelineCoordinate = nextEventTimelineCoordinateBegin;
                        else
                            timelineCoordinate = eventTimelineCoordinateEnd;
                    }
                    else {
                        simulationTime = bigDecimalMax(eventSimulationTime, bigDecimalMin(nextEventSimulationTime, simulationTime));
                        timelineCoordinate = eventTimelineCoordinateEnd + timelineCoordinateDelta * simulationTime.subtract(eventSimulationTime).doubleValue() / simulationTimeDelta.doubleValue();
                        timelineCoordinate = Math.max(eventTimelineCoordinateEnd, Math.min(nextEventTimelineCoordinateBegin, timelineCoordinate));
                    }
                }
                else
                    timelineCoordinate = eventTimelineCoordinateBegin;
                break;
            }
            default:
                throw new RuntimeException("Unknown timeline mode");
        }
        Assert.isTrue(!Double.isNaN(timelineCoordinate));
        return timelineCoordinate;
    }

    public double getTimelineCoordinateForSimulationTimeAndEventInModule(BigDecimal simulationTime, int moduleId) {
        IEvent event = eventLog.getLastEventNotAfterSimulationTime(simulationTime);
        while (event != null && event.getSimulationTime() == simulationTime) {
            if (event.getModuleId() == moduleId)
                return getTimelineCoordinateBegin(event);
            event = event.getNextEvent();
        }
        return getTimelineCoordinateForSimulationTime(simulationTime);
    }

    public ArrayList<ComponentMethodBeginEntry> getComponentMethodBeginEntries(IEvent startEvent, IEvent endEvent) {
        Assert.isTrue(startEvent != null);
        Assert.isTrue(endEvent != null);
        ArrayList<ComponentMethodBeginEntry> componentMethodBeginEntries = new ArrayList<ComponentMethodBeginEntry>();
        for (IEvent event = startEvent; ; event = event.getNextEvent()) {
            eventLog.progress();
            for (int i = 0; i < event.getNumEventLogEntries(); i++) {
                EventLogEntry eventLogEntry = event.getEventLogEntry(i);
                if (eventLogEntry instanceof ComponentMethodBeginEntry)
                    componentMethodBeginEntries.add((ComponentMethodBeginEntry)eventLogEntry);
            }
            if (event == endEvent)
                break;
        }
        return componentMethodBeginEntries;
    }

    public ArrayList<IMessageDependency> getIntersectingMessageDependencies(IEvent startEvent, IEvent endEvent) {
        Assert.isTrue(startEvent != null);
        Assert.isTrue(endEvent != null);
        HashSet<IMessageDependency> messageDependencies = new HashSet<IMessageDependency>();
        long startEventNumber = startEvent.getEventNumber();
        // LONG RUNNING OPERATION
        // this might take a while if start and end events are far away from each other
        // if not completed then some dependencies will not be included
        for (IEvent event = startEvent; ; event = event.getNextEvent()) {
            eventLog.progress();
            ArrayList<IMessageDependency> causes = event.getCauses();
            for (var messageDependency : causes)
                if (messageDependency.getCauseEventNumber() < startEventNumber)
                    messageDependencies.add(messageDependency);
            ArrayList<IMessageDependency> consequences = event.getConsequences();
            for (var consequence : consequences)
                messageDependencies.add(consequence);
            if (event == endEvent)
                break;
        }
        return new ArrayList<IMessageDependency>(messageDependencies);
    }

    public final java.util.ArrayList<Integer> getMessageDependencyCountAdjacencyMatrix(java.util.TreeMap<Integer, Integer> moduleIdToAxisIndexMap, java.util.TreeMap<Long, IEvent> eventNumberToEventMap, int messageSendWeight) {
        return getMessageDependencyCountAdjacencyMatrix(moduleIdToAxisIndexMap, eventNumberToEventMap, messageSendWeight, 1);
    }

    public final java.util.ArrayList<Integer> getMessageDependencyCountAdjacencyMatrix(java.util.TreeMap<Integer, Integer> moduleIdToAxisIndexMap, java.util.TreeMap<Long, IEvent> eventNumberToEventMap) {
        return getMessageDependencyCountAdjacencyMatrix(moduleIdToAxisIndexMap, eventNumberToEventMap, 1, 1);
    }

    public final ArrayList<Integer> getMessageDependencyCountAdjacencyMatrix(Map<Integer, Integer> moduleIdToAxisIndexMap, TreeMap<Long, IEvent> eventNumberToEventMap, int messageSendWeight, int messageReuseWeight) {
        TreeSet<Integer> axisIndexSet = new TreeSet<Integer>(moduleIdToAxisIndexMap.values());
        int numberOfAxes = axisIndexSet.size();
        Integer[] adjacencyMatrix = new Integer[numberOfAxes * numberOfAxes];
        Arrays.fill(adjacencyMatrix, 0);
        for (var it : eventNumberToEventMap.values()) {
            IEvent event = it;
            ArrayList<IMessageDependency> causes = event.getCauses();
            for (var messageDependency : causes) {
                IEvent causeEvent = messageDependency.getCauseEvent();
                IEvent consequenceEvent = messageDependency.getConsequenceEvent();
                int weight = 0;
                if (messageDependency instanceof MessageSendDependency)
                    weight = messageSendWeight;
                else if (messageDependency instanceof MessageReuseDependency)
                    weight = messageReuseWeight;
                if (causeEvent != null && consequenceEvent != null && weight != 0) {
                    int causeModuleId = causeEvent.getModuleId();
                    int consequenceModuleId = consequenceEvent.getModuleId();
                    var causeModuleIdIt = moduleIdToAxisIndexMap.get(causeModuleId);
                    var consequenceModuleIdIt = moduleIdToAxisIndexMap.get(consequenceModuleId);
                    if (causeModuleIdIt != null && consequenceModuleIdIt != null) {
                        int index = causeModuleIdIt * numberOfAxes + consequenceModuleIdIt;
                        adjacencyMatrix[index] = adjacencyMatrix[index] + weight;
                    }
                }
            }
        }
        return new ArrayList<Integer>(Arrays.asList(adjacencyMatrix));
    }

    public ArrayList<Integer> getMessageDependencyCountAdjacencyMatrix(Map<Integer, Integer> moduleIdToAxisIndexMap, IEvent startEvent, IEvent endEvent, int messageSendWeight, int messageReuseWeight) {
        TreeMap<Long, IEvent> eventNumberToEventMap = new TreeMap<Long, IEvent>();
        for (IEvent event = startEvent; ; event = event.getNextEvent()) {
            eventLog.progress();
            if (event != null)
                eventNumberToEventMap.put(event.getEventNumber(), event);
            if (event == endEvent)
                break;
        }
        return getMessageDependencyCountAdjacencyMatrix(moduleIdToAxisIndexMap, eventNumberToEventMap, messageSendWeight, messageReuseWeight);
    }

    public final java.util.ArrayList<Integer> getApproximateMessageDependencyCountAdjacencyMatrix(java.util.TreeMap<Integer, Integer> moduleIdToAxisIndexMap, int numberOfSamples, int messageSendWeight) {
        return getApproximateMessageDependencyCountAdjacencyMatrix(moduleIdToAxisIndexMap, numberOfSamples, messageSendWeight, 1);
    }

    public final java.util.ArrayList<Integer> getApproximateMessageDependencyCountAdjacencyMatrix(java.util.TreeMap<Integer, Integer> moduleIdToAxisIndexMap, int numberOfSamples) {
        return getApproximateMessageDependencyCountAdjacencyMatrix(moduleIdToAxisIndexMap, numberOfSamples, 1, 1);
    }

    public final ArrayList<Integer> getApproximateMessageDependencyCountAdjacencyMatrix(Map<Integer, Integer> moduleIdToAxisIndexMap, int numberOfSamples, int messageSendWeight, int messageReuseWeight) {
        Random random = new Random();
        TreeMap<Long, IEvent> eventNumberToEventMap = new TreeMap<Long, IEvent>();
        for (int i = 0; i < numberOfSamples; i++) {
            eventLog.progress();
            // draw random
            double percentage = random.nextDouble();
            IEvent event = eventLog.getApproximateEventAt(percentage);
            eventNumberToEventMap.put(event.getEventNumber(), event);
            // look before origin
            if (timelineCoordinateSystemOriginEventNumber != -1) {
                if (timelineCoordinateSystemOriginEventNumber - i >= 0) {
                    event = eventLog.getEventForEventNumber(timelineCoordinateSystemOriginEventNumber - i);
                    if (event != null)
                        eventNumberToEventMap.put(event.getEventNumber(), event);
                }
                // look after origin
                event = eventLog.getEventForEventNumber(timelineCoordinateSystemOriginEventNumber + i);
                if (event != null)
                    eventNumberToEventMap.put(event.getEventNumber(), event);
            }
        }
        return new ArrayList<Integer>(getMessageDependencyCountAdjacencyMatrix(moduleIdToAxisIndexMap, eventNumberToEventMap, messageSendWeight, messageReuseWeight));
    }

    // TODO: extract both begin and end timeline coordinates of events
    public void extractSimulationTimesAndTimelineCoordinates(IEvent event, RefObject<IEvent> nextEvent, RefObject<BigDecimal> eventSimulationTime, RefObject<Double> eventTimelineCoordinateBegin, RefObject<Double> eventTimelineCoordinateEnd, RefObject<BigDecimal> nextEventSimulationTime, RefObject<Double> nextEventTimelineCoordinateBegin, RefObject<Double> nextEventTimelineCoordinateEnd, RefObject<BigDecimal> simulationTimeDelta, RefObject<Double> timelineCoordinateDelta) {
        // if before the first event
        if (event != null) {
            eventSimulationTime.argValue = event.getSimulationTime();
            eventTimelineCoordinateBegin.argValue = getTimelineCoordinateBegin(event);
            eventTimelineCoordinateEnd.argValue = getTimelineCoordinateEnd(event);
        }
        else {
            eventSimulationTime.argValue = BigDecimal.getZero();
            IEvent firstEvent = eventLog.getFirstEvent();
            eventTimelineCoordinateBegin.argValue = getTimelineCoordinateBegin(firstEvent);
            eventTimelineCoordinateEnd.argValue = getTimelineCoordinateEnd(firstEvent);

            if (timelineMode == TimelineMode.EVENT_NUMBER) {
                eventTimelineCoordinateBegin.argValue -= 1;
                eventTimelineCoordinateEnd.argValue -= 1;
            }
            else {
                double delta = getTimelineCoordinateDelta(firstEvent.getSimulationTime().doubleValue());
                eventTimelineCoordinateBegin.argValue -= delta;
                eventTimelineCoordinateEnd.argValue -= delta;
            }
        }
        // linear approximation between two enclosing events
        nextEvent.argValue = event != null ? event.getNextEvent() : eventLog.getFirstEvent();
        if (nextEvent.argValue != null) {
            nextEventSimulationTime.argValue = nextEvent.argValue.getSimulationTime();
            nextEventTimelineCoordinateBegin.argValue = getTimelineCoordinateBegin(nextEvent.argValue);
            nextEventTimelineCoordinateEnd.argValue = getTimelineCoordinateEnd(nextEvent.argValue);

            simulationTimeDelta.argValue = nextEventSimulationTime.argValue.subtract(eventSimulationTime.argValue);
            timelineCoordinateDelta.argValue = nextEventTimelineCoordinateBegin.argValue - eventTimelineCoordinateEnd.argValue;
        }
    }

    protected final void clearInternalState() {
        nonLinearFocus = -1;
        nonLinearMinimumTimelineCoordinateDelta = 0.1;
        undefineTimelineCoordinateSystem();
    }
}

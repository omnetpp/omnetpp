/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.eventlog.engine.IEventLog;

/**
 * Selection that is published by event log editors and viewers.
 *
 * @author andras
 */
/*
 * TODO: change this class so that it can represent selection of
 * - event
 * - eventlogentry
 * - simulation time
 * - animation time
 * - timeline coordinate
 * - module
 * - submodule
 * - gate
 * - connection
 * - message
 * - animation primitive
 *
 * It should support multiple selected elements. For some of those it should support ranges where it is applicable.
 */
public class EventLogSelection implements IEventLogSelection, Cloneable {
    /**
     * The input where this selection is.
     */
    protected EventLogInput eventLogInput;

    /**
     * The selected elements.
     */
    protected ArrayList<Object> elements;

    public EventLogSelection(EventLogInput eventLogInput, ArrayList<Object> elements) {
        Assert.isTrue(eventLogInput != null);
        this.eventLogInput = eventLogInput;
        this.elements = elements;
    }

    public EventLogSelection(EventLogInput eventLogInput, EventNumberRangeSet eventNumbers, ArrayList<BigDecimal> simulationTimes) {
        this(eventLogInput, new ArrayList<Object>());
        if (eventNumbers != null)
            elements.add(eventNumbers);
        if (simulationTimes != null)
            elements.addAll(simulationTimes);
    }

    public Object getInput() {
        return eventLogInput;
    }

    @Override
    public IEventLog getEventLog() {
        return eventLogInput.getEventLog();
    }

    @Override
    public EventLogInput getEventLogInput() {
        return eventLogInput;
    }

    @Override
    public ArrayList<Object> getElements() {
        return elements;
    }

    @Override
    public EventNumberRangeSet getEventNumbers() {
        for (Object element : elements)
            if (element instanceof EventNumberRangeSet)
                return (EventNumberRangeSet)element;
        return new EventNumberRangeSet();
    }

    @Override
    public Long getFirstEventNumber() {
        EventNumberRangeSet eventNumbers = getEventNumbers();
        return eventNumbers.isEmpty() ? null : eventNumbers.iterator().next();
    }

    @Override
    public ArrayList<BigDecimal> getSimulationTimes() {
        ArrayList<BigDecimal> simulationTimes = new ArrayList<BigDecimal>();
        for (Object element : elements)
            if (element instanceof BigDecimal)
                simulationTimes.add((BigDecimal)element);
        return simulationTimes;
    }

    @Override
    public BigDecimal getFirstSimulationTime() {
        ArrayList<BigDecimal> simulationTimes = getSimulationTimes();
        return simulationTimes.isEmpty() ? null : simulationTimes.get(0);
    }

    @Override
    public boolean isEmpty() {
        return elements.isEmpty();
    }

    @Override
    public EventLogSelection clone() {
        return new EventLogSelection(this.eventLogInput, elements);
    }

    @Override
    public boolean equals(Object o) {
        if (o == null || !(o instanceof EventLogSelection))
            return false;
        EventLogSelection other = (EventLogSelection)o;
        if (other.eventLogInput != eventLogInput)
            return false;
        if (other.elements.size() != elements.size())
            return false;
        for (int i = 0; i < elements.size(); i++)
            if (other.elements.get(i) != elements.get(i))
                return false;
        return true;
    }
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.unimod.generator.cpp.util;

import java.util.Collection;

import org.apache.commons.collections.functors.NotPredicate;
import org.apache.commons.collections.functors.TruePredicate;

import com.evelopers.unimod.core.stateworks.Event;
import com.evelopers.unimod.core.stateworks.State;
import com.evelopers.unimod.core.stateworks.StateType;

public class StateTool extends BaseTool {
    private State state;
    private String sConst;
    private Collection events;

    public StateTool(State state) {
        this.state = state;
    }

    public String getConst() {
        if (sConst == null) {
            sConst = createIdentifier("", state.getName(), "", true, true);
        }
        return sConst;
    }

    public boolean isInitial() {
        return StateType.INITIAL.equals(state.getType());
    }

    public Collection getEvents() {
        if (events == null) {
            events = CollectionsTool.filter(
                    state.getEvents(false),
                    TruePredicate.getInstance(),
                    CollectionsTool.eventComparator());
        }
        return events;
    }

    public Event getAnyEvent() {
        return Event.ANY;
    }

    public Collection getTrueTansitions(Event event) {
        return CollectionsTool.filter(
                state.getFilteredOutgoingTransitions(event, false),
                CollectionsTool.trueTransitionPredicate());
    }

    public Collection getCondTansitions(Event event) {
        return CollectionsTool.filter(
                state.getFilteredOutgoingTransitions(event, false),
                NotPredicate.getInstance(
                        CollectionsTool.trueTransitionPredicate()));
    }

    public Collection getElseTansitions(Event event) {
        return state.getFilteredOutgoingTransitions(event, true);
    }
}

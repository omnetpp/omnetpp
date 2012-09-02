package org.omnetpp.simulation.views;

import org.omnetpp.simulation.controller.EventEntry;

/**
 *
 * @author Andras
 */
public interface IEventEntryFilter {
    boolean matches(EventEntry entry);
}

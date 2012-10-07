package org.omnetpp.simulation.ui;

import org.omnetpp.simulation.model.cMessage;

/**
 * 
 * @author Andras
 */
public interface IMessageFilter {
    boolean matches(cMessage msg);
}

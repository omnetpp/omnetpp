package org.omnetpp.scave.writers;

public interface ISimulationTimeProvider {
    Number getSimulationTime();
    long getEventNumber();
}

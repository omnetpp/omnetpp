package org.omnetpp.eventlog;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.engineext.IMatchableObject;

/**
 * Manages one part of the eventlog file that consists of a sequence of chunks.
 */
public interface IChunk extends IMatchableObject
{
    /**
     * Returns the begin offset of the chunk in the eventlog file.
     */
    public long getBeginOffset();

    /**
     * Returns the end offset of the chunk in the eventlog file.
     */
    public long getEndOffset();

    /**
     * Returns the corresponding event number.
     */
    public long getEventNumber();

    /**
     * Returns the corresponding simulation time.
     */
    public BigDecimal getSimulationTime();
}

package org.omnetpp.simulation.ui;

import org.omnetpp.common.engine.BigDecimal;


/**
 * Content provider for the TimelineControl widget.
 * 
 * @author Andras
 */
public interface ITimelineContentProvider {

    /**
     * Return the "current simulation time," to serve as a base to which message times
     * will be compared and displayed.
     */
    BigDecimal getBaseTime();

    /**
     * Return the messages to be displayed. The array must be in increasing timestamp order.
     */
    Object[] getMessages();

    /**
     * Return the arrival time of the message.
     */
    BigDecimal getMessageTime(Object message);
}

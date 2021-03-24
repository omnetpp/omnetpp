package org.omnetpp.eventlog;

/**
 * Variables which store event numbers throughout the API may have these
 * additional values (positive values are real event numbers).
 */
public class EventNumberKind
{
    /**
     * The event number is not yet calculated before.
     */
    public static int EVENT_NOT_YET_CALCULATED = -1;

    /**
     * There is no such event and will never be because it is impossible. (e.g. the previous event of the first event)
     */
    public static int NO_SUCH_EVENT = -2;

    /**
     * The event is not and will never be in the log, although it could be. (e.g. filtered by the user from the file)
     */
    public static int FILTERED_EVENT = -3;

    /**
     * The event is not yet in the log, but it might appear in the future (e.g. simulation not there yet).
     * The last event number is added to the constant and returned.
     * (e.g. reading the event 1000 when the file ends at 100 will give -104)
     */
    public static int EVENT_NOT_YET_REACHED = -4;
}

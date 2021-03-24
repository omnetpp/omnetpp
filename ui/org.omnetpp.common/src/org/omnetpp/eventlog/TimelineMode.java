package org.omnetpp.eventlog;

/**
 * Simulation time and event number based means proportional to distance measured in pixels.
 * Step means subsequent events follow each other with a constant distance.
 * Nonlinear mode means distance measured in pixels is proportional to a nonlinear function of the
 * simulation time difference between subsequent events.
 */
public enum TimelineMode
{
    SIMULATION_TIME,
    EVENT_NUMBER,
    STEP,
    NONLINEAR;
}

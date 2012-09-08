package org.omnetpp.simulation.controller;


/**
 * When the simulation process needs UI interaction, Simulation uses
 * this interface to talk to the UI.
 *
 * @author Andras
 */
public interface ISimulationUICallback {
    /**
     * Display an error message that occurred in the simulation
     */
    //TODO split args? (module,simtime,eventnum,message, etc; see members of cException and subclasses)
    void displayError(String errorMessage);

    /**
     * Prompt the user for a parameter value. Should return null if user
     * cancelled the operation.
     */
    String askParameter(String paramName, String ownerFullPath, String paramType, String prompt, String defaultValue, String unit, String choices[]);
}

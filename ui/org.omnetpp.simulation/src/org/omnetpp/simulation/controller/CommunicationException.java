package org.omnetpp.simulation.controller;

/**
 * Checked exception to signal problem with the (HTTP) communication with the
 * simulation process.
 *
 * DO NOT LOG THE ERROR OR DISPLAY AN ERROR DIALOG WHEN CATCHING THIS EXCEPTION!
 * These tasks are done in the ONE place where the exception is thrown (in the
 * Simulation class). It is not practical to do that in the zillion places this
 * exception is caught.
 *
 * @author Andras
 */
@SuppressWarnings("serial")
public class CommunicationException extends Exception {

    public CommunicationException(String msg) {
        super(msg);
    }

    public CommunicationException(Throwable e) {
        super(e);
    }

    public CommunicationException(String msg, Throwable e) {
        super(msg, e);
    }
}

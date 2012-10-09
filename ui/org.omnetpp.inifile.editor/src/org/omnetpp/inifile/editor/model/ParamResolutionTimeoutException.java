package org.omnetpp.inifile.editor.model;

/**
 * Exception thrown when a computation needs parameter resolution,
 * and it did not finished in the specified amount of time.
 *
 * @author tomi
 */
public class ParamResolutionTimeoutException extends Exception {
    private static final long serialVersionUID = 2734832198980444656L;

    public ParamResolutionTimeoutException() {
        super();
    }

    public ParamResolutionTimeoutException(String message) {
        super(message);
    }
}

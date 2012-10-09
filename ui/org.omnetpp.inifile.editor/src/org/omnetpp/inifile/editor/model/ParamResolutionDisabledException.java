package org.omnetpp.inifile.editor.model;

/**
 * Exception thrown when a computation can not be executed without
 * parameter resolution, and parameter resolution is disabled by the
 * user.
 *
 * @author tomi
 */
public class ParamResolutionDisabledException extends Exception {
    private static final long serialVersionUID = 2400692006296966958L;

    public ParamResolutionDisabledException() {
        super();
    }

    public ParamResolutionDisabledException(String message) {
        super(message);
    }
}

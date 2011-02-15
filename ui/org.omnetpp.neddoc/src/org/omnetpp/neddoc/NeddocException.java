package org.omnetpp.neddoc;

public class NeddocException extends RuntimeException {

    private static final long serialVersionUID = 5395949546163774768L;

    public NeddocException() {
        super();
    }

    public NeddocException(String message, Throwable cause) {
        super(message, cause);
    }

    public NeddocException(String message) {
        super(message);
    }

    public NeddocException(Throwable cause) {
        super(cause);
    }
}

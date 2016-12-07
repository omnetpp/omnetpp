package org.omnetpp.scave.writers;

/**
 * Exception thrown from methods of IOutputScalarManager, IOutputVectorManager,
 * and IOutputVector.
 *
 * @author Andras
 */
public class ResultRecordingException extends RuntimeException {
    private static final long serialVersionUID = 1L;

    public ResultRecordingException() {
        super();
    }

    public ResultRecordingException(String message, Throwable e) {
        super(message, e);
    }

    public ResultRecordingException(String message) {
        super(message);
    }

    public ResultRecordingException(Throwable e) {
        super(e);
    }
}

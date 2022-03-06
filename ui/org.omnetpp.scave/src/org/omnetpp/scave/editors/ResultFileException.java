package org.omnetpp.scave.editors;

/**
 * Exception to signal result file problems, such as a vec file having
 * been changed since it was loaded into ResultFileManager.
 *
 * Its message is directly displayed to the user, so it must be worded
 * carefully (must make sense to end user, etc).
 *
 * @author andras
 */
public class ResultFileException extends RuntimeException {
    private static final long serialVersionUID = 1L;

    public ResultFileException(String message) {
        super(message);
    }

    public ResultFileException(Throwable ex) {
        super(ex);
    }

    public ResultFileException(String message, Throwable ex) {
        super(message, ex);
    }

}
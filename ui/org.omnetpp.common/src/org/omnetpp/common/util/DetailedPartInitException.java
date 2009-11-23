package org.omnetpp.common.util;

import java.io.PrintWriter;

import org.eclipse.ui.PartInitException;

public class DetailedPartInitException extends PartInitException {
    private static final long serialVersionUID = 1L;
    
    String detailMessage;

    public DetailedPartInitException(String message) {
        super(message);
    }

    public DetailedPartInitException(String message, String detailMessage) {
        this(message);
        this.detailMessage = detailMessage;
    }

    public void printStackTrace(PrintWriter output) {
        if (detailMessage != null)
            // intentionally does not print the stacktrace and prints the detail message instead
            output.write(detailMessage);
        else
            super.printStackTrace();
    }
    
    public void realPrintStackTrace(PrintWriter output) {
        super.printStackTrace(output);
    }
}

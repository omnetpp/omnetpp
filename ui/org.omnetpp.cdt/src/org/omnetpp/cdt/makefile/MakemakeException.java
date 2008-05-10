package org.omnetpp.cdt.makefile;

/**
 * Thrown by Makemake when the Makefile cannot be generated 
 * with the given options.
 * 
 * @author Andras
 */
public class MakemakeException extends Exception {
    private static final long serialVersionUID = 1L;

    public MakemakeException(String message) {
        super(message);
    }

    public MakemakeException(String message, Throwable cause) {
        super(message, cause);
    }
}

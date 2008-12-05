package org.omnetpp.cdt.makefile;

/**
 * Thrown by Makemake when the Makefile cannot be generated 
 * with the given options.
 * 
 * @author Andras
 */
public class MakemakeException extends Exception {
    private static final long serialVersionUID = 1L;
    private boolean deleteMakefile = true;
    
    public MakemakeException(String message) {
        super(message);
    }

    public MakemakeException(String message, Throwable cause) {
        super(message, cause);
    }

    public MakemakeException(String message, boolean deleteMakefile) {
        super(message);
        this.deleteMakefile = deleteMakefile;
    }
    
    public boolean getDeleteMakefile() {
		return deleteMakefile;
	}
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.build;

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

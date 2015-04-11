/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.engineext;

/**
 * Exception that signals format errors in a vector or scalar file.
 * Gets instantiated and thrown from JNI code (SWIG wrapper).
 *
 * @author tomi
 */
public class ResultFileFormatException extends RuntimeException {
    private static final long serialVersionUID = -6998263415457737351L;

    private String fileName;
    private int lineNo;

    public ResultFileFormatException(String msg) {
        super(msg);
    }

    public ResultFileFormatException(String msg, String fileName, int lineNo) {
        super(msg);
        this.fileName = fileName;
        this.lineNo = lineNo;
    }

    public String getFileName() {
        return fileName;
    }

    public int getLineNo() {
        return lineNo;
    }
}

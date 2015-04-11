/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.model;

/**
 * Thrown on parse errors.
 *
 * @author Andras
 */
public class ParseException extends RuntimeException {
    private static final long serialVersionUID = 1L;

    private int line;
    private int column;

    /**
     * Constructor.
     */
    public ParseException(String s, int line, int column) {
        super(s);
        this.line = line;
        this.column = column;
    }

    /**
     * Constructor.
     */
    public ParseException(String s, int line) {
        this(s, line, -1);
    }

    /**
     * Returns the line where the error was found.
     */
    public int getLineNumber() {
        return line;
    }

    /**
     * Returns the column where the error was found.
     */
    public int getColumn() {
        return column;
    }
}

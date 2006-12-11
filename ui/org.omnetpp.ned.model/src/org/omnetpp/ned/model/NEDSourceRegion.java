package org.omnetpp.ned.model;

/**
 * Value object, stores a line:col..line:col region in a source file. 
 * Used for relating NEDElements back to the source code.
 */
public class NEDSourceRegion {
    public int startLine;
    public int startColumn;
    public int endLine;
    public int endColumn;

	/**
	 * Returns true if the stored region intersects with the given line.
	 */
    public boolean containsLine(int line) {
		return startLine<=line && endLine>=line;
	}

	/**
	 * Returns true if given line:column position falls inside the stored 
	 * [start,end) range.
	 */
	public boolean contains(int line, int column) {
		if (startLine>line || endLine<line)  // obvious non-match
			return false;  
		return (line!=startLine || column>=startColumn) &&
		       (line!=endLine || column<endColumn); 
	}

	public String toString() {
		return ""+startLine+":"+startColumn+"-"+endLine+":"+endColumn;
	}
}

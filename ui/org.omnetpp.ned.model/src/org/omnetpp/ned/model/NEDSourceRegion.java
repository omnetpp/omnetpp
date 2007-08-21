package org.omnetpp.ned.model;

/**
 * Immutable value object, stores a line:col..line:col region in a source file. 
 * Used for relating NEDElements back to the source code.
 * 
 * NOTE: Needs to be immutable because the tree-diff algorithm copies 
 * NEDSourceRegion *references* from one tree to another, WITHOUT copying\
 * the objects themselves.
 *
 * @author andras
 */
public class NEDSourceRegion {
    private int startLine;
    private int startColumn;
    private int endLine;
    private int endColumn;

    /**
     * Constructor.
     */
	public NEDSourceRegion(int startLine, int startColumn, int endLine, int endColumn) {
		this.startLine = startLine;
		this.startColumn = startColumn;
		this.endLine = endLine;
		this.endColumn = endColumn;
	}

	/**
	 * Returns true if the stored region intersects with the given line.
	 */
    public boolean containsLine(int line) {
		return getStartLine()<=line && getEndLine()>=line;
	}

	/**
	 * Returns true if given line:column position falls inside the stored 
	 * [start,end) range.
	 */
	public boolean contains(int line, int column) {
		if (getStartLine()>line || getEndLine()<line)  // obvious non-match
			return false;  
		return (line!=getStartLine() || column>=getStartColumn()) &&
		       (line!=getEndLine() || column<getEndColumn()); 
	}

	public String toString() {
		return ""+getStartLine()+":"+getStartColumn()+"-"+getEndLine()+":"+getEndColumn();
	}
    
    @Override
    public boolean equals(Object obj) {
        if (!(obj instanceof NEDSourceRegion))
            return false;

        NEDSourceRegion o = (NEDSourceRegion)obj;
        return getStartLine()==o.getStartLine() && getEndLine()==o.getEndLine() && getStartColumn()==o.getStartColumn() && getEndColumn()==o.getEndColumn();  
    }

	public int getStartLine() {
		return startLine;
	}

	public int getStartColumn() {
		return startColumn;
	}

	public int getEndLine() {
		return endLine;
	}

	public int getEndColumn() {
		return endColumn;
	}
}

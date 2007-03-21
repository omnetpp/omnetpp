/**
 * 
 */
package org.omnetpp.inifile.editor.model;


/**
 * Base class for various inifile line types.
 * @author Andras
 */
abstract class InifileLine {
	public static final int COMMENT = 0; // blank or comment line (optional whitespace + optional comment) 
	public static final int HEADING = 1; // section heading (plus optional comment) 
	public static final int KEYVALUE = 2; // key = value (plus optional comment)
	public static final int DIRECTIVE = 3; // like "include foo.ini" (plus optional comment)  
	public static final int INCOMPLETE = 4; // continued from previous line (which ended in backslash)
	

	protected int lineType;
	protected int lineNumber;
	protected String rawText;
	protected String comment; // with "#"

	public InifileLine(int type, int lineNumber, String rawText, String comment) {
		this.lineType = type;
		this.lineNumber = lineNumber;
		this.rawText = rawText;
		this.comment = comment;
	}

	public String getComment() {
		return comment;
	}

	public int getLineNumber() {
		return lineNumber;
	}

	public String getText() {
		return rawText != null ? rawText : assemble();
	}

	public int getLineType() {
		return lineType;
	}
	
	public void setComment(String comment) {
		rawText = null;
		this.comment = comment;
	}

	abstract protected String assemble();
	
	@Override
	public String toString() {
		return getText();
	}
}
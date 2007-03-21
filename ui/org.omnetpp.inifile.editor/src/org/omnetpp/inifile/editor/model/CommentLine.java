/**
 * 
 */
package org.omnetpp.inifile.editor.model;


public class CommentLine extends InifileLine {
	public CommentLine(int lineNumber, String rawText, String comment) {
		super(COMMENT, lineNumber, rawText, comment);
	}

	@Override
	protected String assemble() {
		return comment==null ? "" : comment;
	}
	
}
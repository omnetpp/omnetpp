/**
 * 
 */
package org.omnetpp.inifile.editor.model;


class IncompleteLine extends InifileLine {
	public IncompleteLine(int lineNumber, String rawText) {
		super(INCOMPLETE, lineNumber, rawText, null);
	}
}
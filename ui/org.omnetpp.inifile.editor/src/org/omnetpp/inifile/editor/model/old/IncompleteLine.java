/**
 * 
 */
package org.omnetpp.inifile.editor.model.old;


public class IncompleteLine extends InifileLine {
	public IncompleteLine(int lineNumber, String rawText) {
		super(INCOMPLETE, lineNumber, rawText, null);
	}

	@Override
	protected String assemble() {
		throw new RuntimeException("no data to assemble the line from");
	}
}
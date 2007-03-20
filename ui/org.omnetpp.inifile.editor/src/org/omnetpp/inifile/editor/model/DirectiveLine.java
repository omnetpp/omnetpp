/**
 * 
 */
package org.omnetpp.inifile.editor.model;


class DirectiveLine extends InifileLine {
	private String directive;
	private String args;
	
	public DirectiveLine(int lineNumber, String rawText, String directive, String args, String comment) {
		super(InifileLine.DIRECTIVE, lineNumber, rawText, comment);
		this.directive = directive;
		this.args = args;
	}

	public String getArgs() {
		return args;
	}

	public String getDirective() {
		return directive;
	}
}
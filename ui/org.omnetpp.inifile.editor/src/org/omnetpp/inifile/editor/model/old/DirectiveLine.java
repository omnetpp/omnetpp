/**
 * 
 */
package org.omnetpp.inifile.editor.model.old;


public class DirectiveLine extends InifileLine {
	private String directive;
	private String args;
	
	public DirectiveLine(int lineNumber, String rawText, String directive, String args, String comment) {
		super(DIRECTIVE, lineNumber, rawText, comment);
		this.directive = directive;
		this.args = args;
	}

	public String getArgs() {
		return args;
	}

	public String getDirective() {
		return directive;
	}

	public void setArgs(String args) {
		rawText = null;
		this.args = args;
	}

	public void setDirective(String directive) {
		rawText = null;
		this.directive = directive;
	}

	@Override
	protected String assemble() {
		return directive + " " + args + (comment==null ? "" : comment);
	}
}


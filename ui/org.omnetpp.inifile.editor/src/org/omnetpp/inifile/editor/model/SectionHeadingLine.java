/**
 * 
 */
package org.omnetpp.inifile.editor.model;


class SectionHeadingLine extends InifileLine {
	private String sectionName;

	public SectionHeadingLine(int lineNumber, String rawText, String sectionName, String comment) {
		super(HEADING, lineNumber, rawText, comment);
		this.sectionName = sectionName;
	}

	public String getSectionName() {
		return sectionName;
	}
}
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
	
	public void setSectionName(String sectionName) {
		rawText = null;
		this.sectionName = sectionName;
	}

	@Override
	protected String assemble() {
		return "[" + sectionName + "]" + (comment==null ? "" : " "+comment);
	}
}
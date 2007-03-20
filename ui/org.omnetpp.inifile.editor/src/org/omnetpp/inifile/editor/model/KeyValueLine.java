/**
 * 
 */
package org.omnetpp.inifile.editor.model;


class KeyValueLine extends InifileLine {
	protected String key;
	protected String value;
	
	public KeyValueLine(int lineNumber, String rawText, String key, String value, String comment) {
		super(KEYVALUE, lineNumber, rawText, comment);
		this.key = key;
		this.value = value;
	}

	public String getKey() {
		return key;
	}

	public String getValue() {
		return value;
	}

	public void setKey(String key) {
		rawText = null;
		this.key = key;
	}

	public void setValue(String value) {
		rawText = null;
		this.value = value;
	}

	@Override
	protected String assemble() {
		return key + " = " + value + (comment==null ? "" : " "+comment);
	}
}
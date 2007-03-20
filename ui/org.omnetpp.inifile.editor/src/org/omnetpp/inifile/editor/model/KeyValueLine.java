/**
 * 
 */
package org.omnetpp.inifile.editor.model;


class KeyValueLine extends InifileLine {
	private String key;
	private String value;
	
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
}
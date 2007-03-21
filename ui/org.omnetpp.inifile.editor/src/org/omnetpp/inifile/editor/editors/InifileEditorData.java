package org.omnetpp.inifile.editor.editors;

import org.omnetpp.inifile.editor.model.InifileContents;

/**
 * Data held by InifileEditor
 * @author Andras
 */
public class InifileEditorData {
	// the inifile being edited, in parsed form
	private InifileContents inifileContents = new InifileContents();

	public InifileContents getInifileContents() {
		return inifileContents;
	}
}

package org.omnetpp.inifile.editor.editors;

import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Data held by InifileEditor. Introduced because we don't want to pass around
 * reference to the whole editor part.
 * 
 * @author Andras
 */
public class InifileEditorData {
	// the inifile being edited, in parsed form
	private IInifileDocument inifileDocument;
	
	public void setInifiledocument(IInifileDocument inifileDocument) {
		this.inifileDocument = inifileDocument;
	}

	public IInifileDocument getInifileDocument() {
		return inifileDocument;
	}
}

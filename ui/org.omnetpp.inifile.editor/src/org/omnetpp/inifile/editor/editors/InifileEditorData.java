package org.omnetpp.inifile.editor.editors;

import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;

/**
 * Data held by InifileEditor. Introduced because we don't want to pass around
 * a reference to the whole editor.
 * 
 * @author Andras
 */
public class InifileEditorData {
	private IInifileDocument inifileDocument;
	private InifileAnalyzer inifileAnalyzer;
	
	public void setInifiledocument(IInifileDocument inifileDocument) {
		this.inifileDocument = inifileDocument;
	}

	public void setInifileAnalyzer(InifileAnalyzer inifileAnalyzer) {
		this.inifileAnalyzer = inifileAnalyzer;
	}

	public IInifileDocument getInifileDocument() {
		return inifileDocument;
	}

	public InifileAnalyzer getInifileAnalyzer() {
		return inifileAnalyzer;
	}
}

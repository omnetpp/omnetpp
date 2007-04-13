package org.omnetpp.inifile.editor.editors;

import java.util.HashMap;

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

	/**
	 * Whether the form page with the given category is in the "Detailed" or "Normal" view 
	 */
	public HashMap<String,Boolean> formPageCategoryDetailedFlags = new HashMap<String, Boolean>(); 
	
	public void setInifiledocument(IInifileDocument inifileDocument) {
		this.inifileDocument = inifileDocument;
	}

	public void setInifileAnalyzer(InifileAnalyzer inifileAnalyzer) {
		this.inifileAnalyzer = inifileAnalyzer;
	}

	/**
	 * Access to inifile contents.
	 */
	public IInifileDocument getInifileDocument() {
		return inifileDocument;
	}

	/**
	 * Access to the editor's inifile analyzer. 
	 */
	public InifileAnalyzer getInifileAnalyzer() {
		return inifileAnalyzer;
	}
}

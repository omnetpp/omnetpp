package org.omnetpp.inifile.editor.contentassist;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;

/**
 * Generate proposals for inifile parameter and per-object config keys.
 * 
 * Used for the text editor and field editors.
 * 
 * @author Andras
 */
public class InifileParamKeyContentProposalProvider extends ContentProposalProvider {
	private String section;
	private IInifileDocument doc;
	private InifileAnalyzer analyzer;

	public InifileParamKeyContentProposalProvider(String section, IInifileDocument doc, InifileAnalyzer analyzer) {
		this.section = section;
		this.doc = doc;
		this.analyzer = analyzer;
	}

	public void setInifileSection(String section) {
		this.section = section;
	}
	
	/**
	 * Generate a list of proposal candidates. They will be sorted and filtered by prefix
	 * before presenting them to the user.
	 */
	protected IContentProposal[] getProposalCandidates(String prefix) {
		return null;
	}
}

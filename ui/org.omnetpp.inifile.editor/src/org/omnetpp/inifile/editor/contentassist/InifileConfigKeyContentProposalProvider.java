package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.ArrayList;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;

/**
 * Generate completion proposals for inifile config keys.
 * 
 * Used for the text editor and field editors.
 * 
 * @author Andras
 */
public class InifileConfigKeyContentProposalProvider extends ContentProposalProvider {
	private String section;
	private boolean addEqualSign = false;
	private IInifileDocument doc;
	private InifileAnalyzer analyzer;

	public InifileConfigKeyContentProposalProvider(String section, boolean addEqualSign, IInifileDocument doc, InifileAnalyzer analyzer) {
		super(true);
		this.section = section;
		this.addEqualSign = addEqualSign;
		this.doc = doc;
		this.analyzer = analyzer;
	}

	public void configure(String section, boolean addEqualSign) {
		this.section = section;
		this.addEqualSign = addEqualSign;
	}

	/**
	 * Generate a list of proposal candidates. They will be sorted and filtered by prefix
	 * before presenting them to the user.
	 */
	protected IContentProposal[] getProposalCandidates(String prefix) {
		ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();
		// idea considered and discarded: don't propose those already there (would confuse user)
		if (section != null) {
			for (ConfigurationEntry e : ConfigurationRegistry.getEntries()) {
				if (!section.equals(GENERAL) || e!=CFGID_EXTENDS) { // don't propose "extends" in [General]
					String content = e.getKey()+(addEqualSign ? " = " : "");
					result.add(new ContentProposal(content, content, InifileHoverUtils.getConfigHoverText(section, e.getKey(), doc)));
				}
			}
		}
		return result.toArray(new IContentProposal[]{});
	}
}

package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DESCRIPTION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;

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
	//private InifileAnalyzer analyzer;

	public InifileConfigKeyContentProposalProvider(String section, boolean addEqualSign, IInifileDocument doc, InifileAnalyzer analyzer) {
		super(true);
		this.section = section;
		this.addEqualSign = addEqualSign;
		this.doc = doc;
		//this.analyzer = analyzer;
	}

	public void configure(String section, boolean addEqualSign) {
		this.section = section;
		this.addEqualSign = addEqualSign;
	}

	/**
	 * Generate a list of proposal candidates. They will be sorted and filtered by prefix
	 * before getting presented to the user.
	 */
	@Override
	protected List<IContentProposal> getProposalCandidates(String prefix) {
		ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();
		// idea considered and discarded: don't propose those already there (would confuse user)
		if (section != null) {
			for (ConfigKey e : ConfigRegistry.getEntries()) {
				if (!section.equals(GENERAL) || e!=CFGID_EXTENDS) { // don't propose "extends" in [General]
					String content = e.getKey()+(addEqualSign ? " = " : "");
					result.add(new ContentProposal(content, content, getConfigHelpText(e, section, doc)));
				}
			}
		}
		return sort(result);
	}
	
	/**
	 * Generate help text for the given config entry, to be displayed by the content assistant.
	 */
	protected static String getConfigHelpText(ConfigKey entry, String section, IInifileDocument doc) {
		String key = entry.getKey();
		String text = "";

		// see where else it is already set
		if (doc.containsKey(section, key)) {
			text += "<!> Already set in this section.\n\n";
			return text;
		}
		if (doc != null && entry!=CFGID_DESCRIPTION && entry!=CFGID_EXTENDS) {
			String[] sectionChain = InifileUtils.resolveSectionChain(doc, section);
			for (String sec : sectionChain)
				if (doc.containsKey(sec, key))
					text += "<!> Set in ["+sec+"] to "+doc.getValue(sec, key)+"; set here to override it.\n\n";
		}
		
		// generate "standard" documentation for it
		text += key + " = <" + entry.getDataType().name().replaceFirst("CFG_", "");
		if (entry.getDefaultValue()!=null && !entry.getDefaultValue().equals(""))
			text += ", default: " + entry.getDefaultValue();
		if (entry.getUnit()!=null)
			text += ", unit: "+entry.getUnit();
		text += ">\n\n";
		text += StringUtils.breakLines(entry.getDescription(), 50) + "\n";  // default tooltip is ~55 chars wide (text is not wrapped: longer lines simply not fully visible!)

		return text;
	}
	
}

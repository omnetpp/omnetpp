package org.omnetpp.inifile.editor.contentassist;

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

/**
 * Generate proposals for inifile per-object config keys.
 * 
 * Used for the text editor and field editors.
 * 
 * @author Andras
 */
public class PerObjectConfigKeyContentProposalProvider extends ContentProposalProvider {
	private String section;
	private boolean addEqualSign = false;
	private IInifileDocument doc;
	//private InifileAnalyzer analyzer;

	public PerObjectConfigKeyContentProposalProvider(String section, boolean addEqualSign, IInifileDocument doc, InifileAnalyzer analyzer) {
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

	@Override
	protected List<IContentProposal> getProposalCandidates(String prefix) {
		ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();
		if (section != null && prefix.contains(".")) {
			String prefixBase = prefix.substring(0, prefix.lastIndexOf('.')+1);
			for (ConfigKey e : ConfigRegistry.getPerObjectEntries()) {
				if (e != ConfigRegistry.CFGID_APPLY_DEFAULT) { // that's handled by InifileParamKeyContentProposalProvider
					String content = prefixBase + e.getKey()+(addEqualSign ? " = " : "");
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

		// generate "standard" documentation for it
		text += "<object-fullpath-pattern>." + key + " = <" + entry.getDataType().name().replaceFirst("CFG_", "");
		if (entry.getDefaultValue()!=null && !entry.getDefaultValue().equals(""))
			text += ", default: " + entry.getDefaultValue();
		if (entry.getUnit()!=null)
			text += ", unit: "+entry.getUnit();
		text += ">\n\n";
		text += StringUtils.breakLines(entry.getDescription(), 50) + "\n";  // default tooltip is ~55 chars wide (text is not wrapped: longer lines simply not fully visible!)

		return text;
	}
}
package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PRELOAD_NED_FILES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.ArrayList;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * Generate proposals for config values, parameter values, per-object config values etc.
 *
 * Used for the text editor and field editors.
 * 
 * @author Andras
 */
public class InifileValueContentProposalProvider extends ContentProposalProvider {
	private String section;
	private String key;
	private IInifileDocument doc;
	private InifileAnalyzer analyzer;

	public InifileValueContentProposalProvider(String section, String key, IInifileDocument doc, InifileAnalyzer analyzer) {
		this.section = section;
		this.key = key;
		this.doc = doc;
		this.analyzer = analyzer;
	}

	public void setInifileEntry(String section, String key) {
		this.section = section;
		this.key = key;
	}
	
	/**
	 * Generate a list of proposal candidates. They will be sorted and filtered by prefix
	 * before presenting them to the user.
	 */
	protected IContentProposal[] getProposalCandidates(String prefix) {
		KeyType keyType = (key == null) ? KeyType.CONFIG : InifileAnalyzer.getKeyType(key);
		if (keyType==KeyType.CONFIG) {
			return getCandidatesForConfig();
		}
		else if (keyType == KeyType.PARAM) {
			return getCandidatesForParam();
		}
		else if (keyType == KeyType.PER_OBJECT_CONFIG) {
			return getCandidatesForPerObjectConfig();
		}
		return null; // should not happen (invalid key type)
	}

	/**
	 * Generate proposals for a config entry
	 */
	protected IContentProposal[] getCandidatesForConfig() {
		ConfigurationEntry entry = ConfigurationRegistry.getEntry(key);
		if (entry == null)
			return null;

		if (entry==CFGID_EXTENDS) {
			ArrayList<String> names = new ArrayList<String>();
			for (String section : doc.getSectionNames())
				if (!section.equals(GENERAL) && !InifileUtils.sectionChainContains(doc, section, this.section)) // prevent circles
					names.add(section.replaceFirst("^Config +", ""));
			return toProposals(names.toArray(new String[]{}));
		}
		if (entry==CFGID_NETWORK) {
			ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();
			for (INEDTypeInfo ned : NEDResourcesPlugin.getNEDResources().getNetworks())
				result.add(new ContentProposal(ned.getName(), ned.getName(), ned.getNEDElement().getComment()));
			return result.toArray(new IContentProposal[] {});
		}
		if (entry==CFGID_PRELOAD_NED_FILES) {
			return toProposals(new String[] {"*.ned"});
		}
		if (entry==CFGID_USER_INTERFACE) {
			return toProposals(new String[] {"Cmdenv", "Tkenv"});
		}
		if (entry.getDataType()==ConfigurationEntry.DataType.CFG_BOOL) {
			return toProposals(new String[] {"true", "false"});
		}
		return null;
	}

	/** 
	 * Generate proposals for a module parameter key
	 */
	protected IContentProposal[] getCandidatesForParam() {
		if (analyzer==null)
			return null; // sorry

		return toProposals(new String[] {"TODO", "FIXME", "XXX", section, key}); //XXX

//		ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key);
//		for (ParamResolution res : resList)
//			; //TODO
//		return null; //XXX
	}

	/**
	 * Generate proposals for per-object configuration keys
	 */
	protected IContentProposal[] getCandidatesForPerObjectConfig() {
		//XXX TODO...		
		//		if (key.endsWith(".apply-default") || key.endsWith(".ev-output")) {
		//			proposals.add("true\n");
		//			proposals.add("false\n");
		//		}
		//		if (key.endsWith(".record-interval")) { 
		//			proposals.add("$1..");
		//			proposals.add("$1..$2");
		//			proposals.add("..$2"); //XXX use templated proposals here!
		//		}
		return null;
	}
	
}

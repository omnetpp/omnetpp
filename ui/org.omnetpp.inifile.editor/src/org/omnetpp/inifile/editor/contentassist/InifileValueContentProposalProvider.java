package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PRELOAD_NED_FILES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.text.templates.Template;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.NEDElementUtil;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.pojo.ParamNode;

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

	public InifileValueContentProposalProvider(String section, String key, IInifileDocument doc, InifileAnalyzer analyzer, boolean replace) {
		super(false, replace);
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
	 * Returns whether content proposals are available for a given key. This can be
	 * used to decide whether to install content assist support on a given edit field.
	 */
	public boolean isContentAssistAvailable() {
		KeyType keyType = (key == null) ? KeyType.CONFIG : InifileAnalyzer.getKeyType(key);
		if (keyType == KeyType.CONFIG) {
			// we call this for each edit field during form editor creation, so it should be reasonably fast
			ConfigKey entry = ConfigRegistry.getEntry(key);
			if (entry==CFGID_EXTENDS || entry==CFGID_NETWORK || entry==CFGID_PRELOAD_NED_FILES || entry==CFGID_USER_INTERFACE)
				return true;
			if (entry == null && entry.getDataType()==ConfigKey.DataType.CFG_BOOL)
				return true;
			return false;
		}
		else {
			// for parameters etc, we have time to just check if there are actually any proposals   
			IContentProposal[] proposals = getProposalCandidates("");
			return proposals != null || proposals.length > 0;
		}
	}

	/**
	 * Generate a list of proposal candidates. They will be sorted and filtered by prefix
s	 * before getting presented to the user.
	 */
	protected IContentProposal[] getProposalCandidates(String prefix) {
		KeyType keyType = (key == null) ? KeyType.CONFIG : InifileAnalyzer.getKeyType(key);
		switch (keyType) {
			case CONFIG: return getCandidatesForConfig();
			case PARAM:  return getCandidatesForParam();
			case PER_OBJECT_CONFIG: return getCandidatesForPerObjectConfig();
			default: return null; // should not happen (invalid key type)
		}
	}

	/**
	 * Generate proposals for a config entry
	 */
	protected IContentProposal[] getCandidatesForConfig() {
		ConfigKey entry = ConfigRegistry.getEntry(key);
		if (entry == null)
			return null;

		// IMPORTANT: Remember to update isContentAssistAvailable() when this method gets extended!
		if (entry==CFGID_EXTENDS) {
			ArrayList<String> names = new ArrayList<String>();
			for (String section : doc.getSectionNames())
				if (!section.equals(GENERAL) && !InifileUtils.sectionChainContains(doc, section, this.section)) // prevent circles
					names.add(section.replaceFirst("^Config +", ""));
			return sort(toProposals(names.toArray(new String[]{})));
		}
		if (entry==CFGID_NETWORK) {
			ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();
			for (INEDTypeInfo ned : NEDResourcesPlugin.getNEDResources().getNetworks())
				result.add(new ContentProposal(ned.getName(), ned.getName(), StringUtils.makeTextDocu(ned.getNEDElement().getComment())));
			return sort(result.toArray(new IContentProposal[]{}));
		}
		if (entry==CFGID_PRELOAD_NED_FILES) {
			return toProposals(new String[] {"*.ned"});
		}
		if (entry==CFGID_USER_INTERFACE) {
			return toProposals(new String[] {"Cmdenv", "Tkenv"});
		}
		if (entry.getDataType()==ConfigKey.DataType.CFG_BOOL) {
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

		ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key);

		// collect unique param nodes
		Set<ParamNode> paramSet = new HashSet<ParamNode>();
		for (ParamResolution res : resList)
			paramSet.add(res.paramDeclNode);

		// determine param type (all params matched must have the same type)
		int dataType = -1;
		for (ParamNode par : paramSet) {
			if (dataType == -1)
				dataType = par.getType();
			else if (dataType != par.getType())
				return null; // just refuse to suggest anything if types are inconsistent
		}

		// generate proposals
		//TODO make use of parameter properties (like @choice)
		//TODO detect parameters which are used as "like" params, and offer suitable module types for them
		List<IContentProposal> p = new ArrayList<IContentProposal>();
		switch (dataType) {
		case NEDElementUtil.NED_PARTYPE_BOOL: 
			addProposals(p, new String[] {"true", "false"}, null, true); 
			break;
		case NEDElementUtil.NED_PARTYPE_INT: 
			addProposals(p, new String[] {"0"}, "or any integer value", true);
			addProposals(p, templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTempl), "discrete distr.", true); 
			addProposals(p, templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTemplExt), "using a given RNG", true); 
			addProposals(p, templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTempl), "continuous distr.", true); 
			addProposals(p, templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTemplExt), "using a given RNG", true); 
			break;
		case NEDElementUtil.NED_PARTYPE_DOUBLE:
			addProposals(p, new String[] {"0.0"}, "or any double value", true); 
			addProposals(p, templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTempl), "continuous distr.", true); 
			addProposals(p, templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTemplExt), "using a given RNG", true); 
			addProposals(p, templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTempl), "discrete distr.", true); 
			addProposals(p, templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTemplExt), "using a given RNG", true); 
			break;
		case NEDElementUtil.NED_PARTYPE_STRING: 
			addProposals(p, new String[] {"\"\""}, "or any string value", true); 
			break;
		case NEDElementUtil.NED_PARTYPE_XML: 
			addProposals(p, new String[] {"xmldoc(\"filename\")", "xmldoc(\"filename\", \"xpath\")"}, null, true); 
			break;
		}
		return p.toArray(new IContentProposal[]{});
	}

	protected static String[] templatesToProposals(Template[] templates) {
		//XXX find a way to return these things as TemplateProposals to the text editor!
		String[] s = new String[templates.length];
		for (int i=0; i<templates.length; i++) {
			Template template = templates[i];
			s[i] = template.getName(); //XXX not very clean, or good even...
		}
		return s;
	}

	/**
	 * Generate proposals for per-object configuration keys
	 */
	protected IContentProposal[] getCandidatesForPerObjectConfig() {
		List<String> proposals = new ArrayList<String>();
		//FIXME use the ConfigRegistry!!!
		if (key.endsWith(".apply-default") || key.endsWith(".ev-output")) {
			proposals.add("true");
			proposals.add("false");
		}
		if (key.endsWith(".record-interval")) { 
			proposals.add("$1..");
			proposals.add("$1..$2");
			proposals.add("..$2"); //XXX use templated proposals here!
		}
		return toProposals(proposals.toArray(new String[]{}));
	}

}

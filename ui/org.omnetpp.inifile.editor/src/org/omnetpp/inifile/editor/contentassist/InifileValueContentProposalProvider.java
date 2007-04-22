package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PRELOAD_NED_FILES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.text.templates.Template;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;
import org.omnetpp.ned.model.NEDElementUtil;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.pojo.ParamNode;
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
		super(false);
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
		//XXX offer default value?
		//XXX make use of parameter's @choice etc properties?
		String[] p = null;
		switch (dataType) {
		case NEDElementUtil.NED_PARTYPE_BOOL: 
			p = new String[] {"true", "false"}; 
			break;
		case NEDElementUtil.NED_PARTYPE_INT: 
		case NEDElementUtil.NED_PARTYPE_DOUBLE:
			//XXX shoulds use it as template...
			p = templatesToProposals(NedCompletionHelper.proposedNedDistributionsTempl); 
			break;
		case NEDElementUtil.NED_PARTYPE_STRING: 
			p = new String[] {"\"\""}; 
			break;
		case NEDElementUtil.NED_PARTYPE_XML: 
			p = new String[] {"xmldoc(\"filename\")", "xmldoc(\"filename\", \"xpath\")"}; 
			break; //XXX should be template?
		}
		return p==null ? null : toProposals(p);
	}

	protected static String[] templatesToProposals(Template[] templates) {
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
		if (key.endsWith(".apply-default") || key.endsWith(".ev-output")) {
			proposals.add("true\n");
			proposals.add("false\n");
		}
		if (key.endsWith(".record-interval")) { 
			proposals.add("$1..");
			proposals.add("$1..$2");
			proposals.add("..$2"); //XXX use templated proposals here!
		}
		return toProposals(proposals.toArray(new String[]{}));
	}

}

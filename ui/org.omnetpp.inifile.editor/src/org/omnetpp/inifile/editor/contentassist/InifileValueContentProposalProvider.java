package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CONSTRAINT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PRELOAD_NED_FILES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RECORDING_INTERVAL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.PREDEFINED_CONFIGVARS;

import java.util.ArrayList;
import java.util.Arrays;
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
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.NEDElementConstants;
import org.omnetpp.ned.model.pojo.ParamElement;

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
			List<IContentProposal> proposals = getProposalCandidates("");
			return proposals.size()>0;
		}
	}

	/**
	 * Generate a list of proposal candidates. They will be sorted and filtered by prefix
s	 * before getting presented to the user.
	 */
	@Override
	protected List<IContentProposal> getProposalCandidates(String prefix) {
		KeyType keyType = (key == null) ? KeyType.CONFIG : InifileAnalyzer.getKeyType(key);
		switch (keyType) {
			case CONFIG: return getCandidatesForConfig(prefix);
			case PARAM:  return getCandidatesForParam(prefix);
			case PER_OBJECT_CONFIG: return getCandidatesForPerObjectConfig(prefix);
			default: return null; // should not happen (invalid key type)
		}
	}

	/**
	 * Generate proposals for a config entry
	 */
	protected List<IContentProposal> getCandidatesForConfig(String prefix) {
		ConfigKey entry = ConfigRegistry.getEntry(key);
		if (entry == null)
			return new ArrayList<IContentProposal>();  // nothing

		// IMPORTANT: Remember to update isContentAssistAvailable() when this method gets extended!
		if (entry==CFGID_EXTENDS) {
			ArrayList<String> names = new ArrayList<String>();
			// propose only sections that won't introduce section cycles
			for (String section : doc.getSectionNames())
				if (!section.equals(GENERAL) && !InifileUtils.sectionChainContains(doc, section, this.section))
					names.add(InifileUtils.removeSectionNamePrefix(section));
			return sort(toProposals(names.toArray(new String[]{})));
		}

		List<IContentProposal> p = new ArrayList<IContentProposal>();

		// after "${", offer variable names
		if (prefix.matches(".*\\$\\{[A-Za-z0-9_]*")) {
			p.addAll(toProposals(analyzer.getIterationVariableNames(section)));
			p.addAll(toProposals(PREDEFINED_CONFIGVARS));
		}

		if (entry==CFGID_NETWORK) {
			NEDResources nedResources = NEDResourcesPlugin.getNEDResources();
			for (String networkName : nedResources.getNetworkQNames())
				p.add(new ContentProposal(networkName, networkName, StringUtils.makeTextDocu(nedResources.getNedType(networkName).getNEDElement().getComment())));
			sort(p);
		}
		else if (entry==CFGID_PRELOAD_NED_FILES) {
			p.addAll(toProposals(new String[] {"*.ned"}));
		}
		else if (entry==CFGID_USER_INTERFACE) {
			p.addAll(toProposals(new String[] {"Cmdenv", "Tkenv"}));
		}
		else if (entry==CFGID_CONSTRAINT) {
			// offer variable names after "$"
			if (prefix.matches(".*\\$[A-Za-z0-9_]*")) {
				p.addAll(toProposals(analyzer.getIterationVariableNames(section)));
				p.addAll(toProposals(PREDEFINED_CONFIGVARS));
			}
		}
		else if (entry.getDataType()==ConfigKey.DataType.CFG_BOOL) {
			p.addAll(toProposals(new String[] {"true", "false"}));
		}
		return p;
	}

	/** 
	 * Generate proposals for a module parameter key
	 */
	protected List<IContentProposal> getCandidatesForParam(String prefix) {
		if (analyzer==null)
			return new ArrayList<IContentProposal>();  // nothing

		ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key);

		// collect unique param nodes
		Set<ParamElement> paramSet = new HashSet<ParamElement>();
		for (ParamResolution res : resList)
			paramSet.add(res.paramDeclNode);

		// determine param type (all params matched must have the same type)
		int dataType = -1;
		for (ParamElement par : paramSet) {
			if (dataType == -1)
				dataType = par.getType();
			else if (dataType != par.getType())
				return null; // just refuse to suggest anything if types are inconsistent
		}

		// generate proposals
		//TODO make use of parameter properties (like @choice)
		//TODO detect parameters which are used as "like" params, and offer suitable module types for them
		List<IContentProposal> p = new ArrayList<IContentProposal>();

		// after "${", offer variable names
		if (prefix.matches(".*\\$\\{[A-Za-z0-9_]*")) {
			p.addAll(toProposals(analyzer.getIterationVariableNames(section)));
			p.addAll(toProposals(PREDEFINED_CONFIGVARS));
		}
		
		switch (dataType) {
		case NEDElementConstants.NED_PARTYPE_BOOL: 
			p.addAll(toProposals(new String[] {"true", "false"})); 
			break;
		case NEDElementConstants.NED_PARTYPE_INT: 
			p.addAll(toProposals(new String[] {"0"}, "or any integer value"));
			p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTempl), "discrete distr.")); 
			p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTemplExt), "using a given RNG")); 
			p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTempl), "continuous distr.")); 
			p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTemplExt), "using a given RNG")); 
			break;
		case NEDElementConstants.NED_PARTYPE_DOUBLE:
			p.addAll(toProposals(new String[] {"0.0"}, "or any double value")); 
			p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTempl), "continuous distr.")); 
			p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTemplExt), "using a given RNG")); 
			p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTempl), "discrete distr.")); 
			p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTemplExt), "using a given RNG")); 
			break;
		case NEDElementConstants.NED_PARTYPE_STRING: 
			p.addAll(toProposals(new String[] {"\"\""}, "or any string value")); 
			break;
		case NEDElementConstants.NED_PARTYPE_XML: 
			p.addAll(toProposals(new String[] {"xmldoc(\"filename\")", "xmldoc(\"filename\", \"xpath\")"})); 
			break;
		}
		return p;
	}

	protected static String[] templatesToProposals(Template[] templates) {
		//XXX find a way to return these things as TemplateProposals to the text editor!
		String[] s = new String[templates.length];
		for (int i=0; i<templates.length; i++) {
			Template template = templates[i];
			s[i] = template.getName(); //XXX not very clean, or good even...
		}
		Arrays.sort(s);
		return s;
	}

	/**
	 * Generate proposals for per-object configuration keys
	 */
	protected List<IContentProposal> getCandidatesForPerObjectConfig(String prefix) {
		String keySuffix = key.replaceFirst(".*\\.", ""); // only keep substring after last dot
		ConfigKey entry = ConfigRegistry.getPerObjectEntry(keySuffix);
		if (entry == null)
			return new ArrayList<IContentProposal>();  // nothing

		List<IContentProposal> p = new ArrayList<IContentProposal>();

		// after "${", offer variable names
		if (prefix.matches(".*\\$\\{[A-Za-z0-9_]*")) {
			p.addAll(toProposals(analyzer.getIterationVariableNames(section)));
			p.addAll(toProposals(PREDEFINED_CONFIGVARS));
		}

		if (entry==CFGID_RECORDING_INTERVAL) { 
			p.addAll(toProposals(new String[]{"$1..", "$1..$2, $3.."})); //XXX use templated proposals here!
		}
		if (entry.getDataType()==ConfigKey.DataType.CFG_BOOL) {
			p.addAll(toProposals(new String[] {"true", "false"}));
		}
		return p;
	}

}

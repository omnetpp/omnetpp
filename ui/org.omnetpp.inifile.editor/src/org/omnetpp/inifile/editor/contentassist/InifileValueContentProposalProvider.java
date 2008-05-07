package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CONSTRAINT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RECORDING_INTERVAL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.PREDEFINED_CONFIGVARS;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
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
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
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
			if (entry==CFGID_EXTENDS || entry==CFGID_NETWORK || entry==CFGID_USER_INTERFACE)
				return true;
			if (entry != null && entry.getDataType()==ConfigKey.DataType.CFG_BOOL)
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
			IProject contextProject = doc.getDocumentFile().getProject();
			NEDResources nedResources = NEDResourcesPlugin.getNEDResources();
			// first 
			List<IContentProposal> p1 = new ArrayList<IContentProposal>();
			String iniFilePackage = NEDResourcesPlugin.getNEDResources().getExpectedPackageFor(doc.getDocumentFile());
			if (StringUtils.isNotEmpty(iniFilePackage)) {
				for (String networkName : nedResources.getNetworkQNames(contextProject)) {
					INedTypeElement network = nedResources.getToplevelNedType(networkName, contextProject).getNEDElement();
					if ((iniFilePackage+".").equals(network.getNEDTypeInfo().getNamePrefix())) {
						String docu = StringUtils.makeTextDocu(network.getComment());
						p1.add(new ContentProposal(network.getName(), network.getName()+" - "+iniFilePackage, docu));
					}
				}
				sort(p1);
			}
			List<IContentProposal> p2 = new ArrayList<IContentProposal>();
			for (String networkName : nedResources.getNetworkQNames(contextProject)) {
				INedTypeElement network = nedResources.getToplevelNedType(networkName, contextProject).getNEDElement();
				String docu = StringUtils.makeTextDocu(network.getComment());
				// TODO make a better presentation ( name - package ) = prefix filtering should be correctly implemented (prefix should match the name ONLY excluding the package)
//				String namePrefix = StringUtils.removeEnd(network.getNEDTypeInfo().getNamePrefix(),".");
//				String label = StringUtils.join(network.getName(), " - ", namePrefix);
				p2.add(new ContentProposal(networkName, networkName, docu));
			}
			sort(p2);
			p.addAll(p1);
			p.addAll(p2);
			
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
		    //FIXME after "$", offer: ${start..end}  ${start..end step x} ${start..end step x}s ${var=start..end step x} ${var=val1,val2,val3} ${var=val1,val2,val3 ! something}
            //p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTemplExt), "using a given RNG")); 
	        //return new Template(name, description, DEFAULT_NED_CONTEXT_TYPE, pattern, false);

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
		    // if the param is used in a "<param> like IFoo", propose all modules that implement IFoo
		    Collection<INEDTypeInfo> types = getProposedNedTypesFor(resList);
		    if (types != null)
	            for (INEDTypeInfo type : types)
	                p.add(new ContentProposal("\""+type.getName()+"\"", "\""+type.getName()+"\"", "TODO")); //FIXME display package, docu!
			p.addAll(toProposals(new String[] {"\"\""}, "or any string value")); 
			break;
		case NEDElementConstants.NED_PARTYPE_XML: 
			p.addAll(toProposals(new String[] {"xmldoc(\"filename\")", "xmldoc(\"filename\", \"xpath\")"})); 
			break;
		}
		return p;
	}

	/**
	 * If some of the matching parameters are used in a "&lt;param&gt; like IFoo" submodule, 
	 * propose all modules that implement IFoo. If there's IFoo, IBar etc, propose modules
	 * that implement both (all).
	 */
	protected Collection<INEDTypeInfo> getProposedNedTypesFor(ParamResolution[] paramResList) {
        IProject context = doc.getDocumentFile().getProject();

	    // collect all interfaces that parameters require
	    Set<INEDTypeInfo> likeInterfaces = new HashSet<INEDTypeInfo>();
        for (ParamResolution param : paramResList)
            likeInterfaces.addAll(extractParamLikeInterfaces(param, context));

        // if different params require different interfaces, find the common subset 
        // of modules that implement both/all, and return that.
	    INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
        Set<INEDTypeInfo> result = new HashSet<INEDTypeInfo>();
        boolean firstIter = true;
	    for (INEDTypeInfo likeInterface : likeInterfaces) {
	        Collection<INEDTypeInfo> types = res.getNedTypesThatImplement(likeInterface, context);
	        if (firstIter) {
	            result.addAll(types);
	            firstIter = false;
	        }
	        else {
	            result.retainAll(types); // that is, result = intersect(result,types)   
	        }
	        if (result.isEmpty())
	            break; // common subset is empty, makes no sense to continue
	    }
	    return result;
    }
	
	/**
	 * If the param is used in a "<param> like IFoo", return IFoo. If it's used
	 * in several submodules (IFoo, IBar, etc), return all.
	 */
	protected Set<INEDTypeInfo> extractParamLikeInterfaces(ParamResolution param, IProject context) {
        // find compound module in which parameter is declared. We'll be looking for
        // matching "like" submodules in there.
        //FIXME there's a chance that the module instantiated in the network is actually
        // a subclass of this compound module ("StandardHostExt" not "StandardHost"),
	    // so we miss submodules added in the subclass. However, I doubt that we can
	    // find out the subclass from ParamResolution -- TODO add it
        INedTypeLookupContext paramContext = param.paramDeclNode.getEnclosingLookupContext();
        Assert.isTrue(paramContext instanceof CompoundModuleElementEx);
        CompoundModuleElementEx module = (CompoundModuleElementEx)paramContext;
            
        // collect its "like" submodules that refer to our parameter.
        // note: we only look to local submodules: inherited submodules wouldn't see 
        // this parameter (since it's declared in a subclass)
        Set<INEDTypeInfo> result = new HashSet<INEDTypeInfo>();
        INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
        String paramName = param.paramDeclNode.getName();
        for (SubmoduleElementEx submodule : module.getSubmodules()) {
            if (submodule.getLikeParam().equals(paramName)) {
                // resolve interface, then add to the result
                //System.out.println("found 'like' submodule: " + submodule + " --> " + submodule.getLikeType());
                String likeType = submodule.getLikeType();
                INEDTypeInfo likeInterface = res.lookupNedType(likeType, module);
                if (likeInterface != null && likeInterface.getNEDElement() instanceof ModuleInterfaceElement)
                    result.add(likeInterface);
            }
        }
        //TODO: look for "like" channels as well
	    return result;
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

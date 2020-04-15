/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_CONFIG_NAME;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CONSTRAINT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_QTENV_DEFAULT_CONFIG;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SIMTIME_RESOLUTION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SIMTIME_SCALE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_RECORDING_INTERVALS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.SIMTIME_RESOLUTION_CHOICES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.SIMTIME_SCALE_CHOICES;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IProject;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.text.templates.Template;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.editor.text.NedCommentFormatter;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.KeyType;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.ParamResolutionDisabledException;
import org.omnetpp.inifile.editor.model.ParamResolutionTimeoutException;
import org.omnetpp.inifile.editor.model.Timeout;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.NedElementConstants;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
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
    private IReadonlyInifileDocument doc;
    private InifileAnalyzer analyzer;

    private ConfigOption[] assistedOptions = {
            CFGID_EXTENDS, CFGID_NETWORK, CFGID_USER_INTERFACE, CFGID_CMDENV_CONFIG_NAME,
            CFGID_QTENV_DEFAULT_CONFIG, CFGID_CONSTRAINT,
            CFGID_SIMTIME_RESOLUTION,CFGID_SIMTIME_SCALE
    };

    public InifileValueContentProposalProvider(String section, String key, IReadonlyInifileDocument doc, InifileAnalyzer analyzer, boolean replace) {
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
        KeyType keyType = (key == null) ? KeyType.CONFIG : KeyType.getKeyType(key);
        if (keyType == KeyType.CONFIG) {
            // we call this for each edit field during form editor creation, so it should be reasonably fast
            ConfigOption option = ConfigRegistry.getOption(key);
            if (option == null)
                return false;
            if (ArrayUtils.contains(assistedOptions, option))
                return true;
            if (option.getDataType() == ConfigOption.DataType.CFG_BOOL)
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
s    * before getting presented to the user.
     */
    @Override
    protected List<IContentProposal> getProposalCandidates(String prefix) {
        KeyType keyType = (key == null) ? KeyType.CONFIG : KeyType.getKeyType(key);
        switch (keyType) {
            case CONFIG: return getCandidatesForConfig(prefix);
            case PARAM:  return getCandidatesForParam(prefix);
            case PER_OBJECT_CONFIG: return getCandidatesForPerObjectConfig(prefix);
            default: return null; // should not happen (invalid key type)
        }
    }

    /**
     * Generate proposals for a config entry.
     * IMPORTANT: update isContentAssistAvailable() when this method gets extended!
     */
    protected List<IContentProposal> getCandidatesForConfig(String prefix) {
        ConfigOption entry = ConfigRegistry.getOption(key);
        if (entry == null)
            return new ArrayList<IContentProposal>();  // nothing

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
            addConfigVariableProposals(p);
        }

        if (entry==CFGID_NETWORK) {
            IProject contextProject = doc.getDocumentFile().getProject();
            INedTypeResolver nedResources = NedResourcesPlugin.getNedResources().getImmutableCopy();
            // first
            List<IContentProposal> p1 = new ArrayList<IContentProposal>();
            String iniFilePackage = nedResources.getPackageFor(doc.getDocumentFile().getParent());
            if (StringUtils.isNotEmpty(iniFilePackage)) {
                for (String networkName : nedResources.getNetworkQNames(contextProject)) {
                    INedTypeElement network = nedResources.getToplevelNedType(networkName, contextProject).getNedElement();
                    if ((iniFilePackage+".").equals(network.getNedTypeInfo().getNamePrefix())) {
                        String docu = NedCommentFormatter.makeTextDocu(network.getComment());
                        p1.add(new ContentProposalEx(network.getName(), network.getName()+" - "+iniFilePackage, docu));
                    }
                }
                sort(p1);
            }
            List<IContentProposal> p2 = new ArrayList<IContentProposal>();
            for (String networkName : nedResources.getNetworkQNames(contextProject)) {
                INedTypeElement network = nedResources.getToplevelNedType(networkName, contextProject).getNedElement();
                String docu = NedCommentFormatter.makeTextDocu(network.getComment());
                // TODO make a better presentation ( name - package ) = prefix filtering should be correctly implemented (prefix should match the name ONLY excluding the package)
//              String namePrefix = StringUtils.removeEnd(network.getNEDTypeInfo().getNamePrefix(),".");
//              String label = StringUtils.join(network.getName(), " - ", namePrefix);
                p2.add(new ContentProposalEx(networkName, networkName, docu));
            }
            sort(p2);
            p.addAll(p1);
            p.addAll(p2);

        }
        else if (entry==CFGID_USER_INTERFACE) {
            p.addAll(toProposals(new String[] {"Cmdenv", "Qtenv"}));
        }
        else if (entry==CFGID_CMDENV_CONFIG_NAME || entry==CFGID_QTENV_DEFAULT_CONFIG) {
            ArrayList<String> names = new ArrayList<String>();
            for (String section : doc.getSectionNames())
                names.add(InifileUtils.removeSectionNamePrefix(section));
            return sort(toProposals(names.toArray(new String[]{})));
        }
        else if (entry==CFGID_CONSTRAINT) {
            // offer variable names after "$"
            if (prefix.matches(".*\\$[A-Za-z0-9_]*"))
                addConfigVariableProposals(p);
        }
        else if (entry==CFGID_SIMTIME_RESOLUTION) {
            p.addAll(toProposals(SIMTIME_RESOLUTION_CHOICES));
        }
        else if (entry==CFGID_SIMTIME_SCALE) {
            p.addAll(toProposals(SIMTIME_SCALE_CHOICES));
        }
        else if (entry.getDataType()==ConfigOption.DataType.CFG_BOOL) {
            p.addAll(toProposals(new String[] {"true", "false"}));
        }
        return p;
    }

    /**
     * Overrides the base method to add '.' to word characters,
     * because config values (e.g. qualified network names) may
     * contain '.'.
     */
    protected String getCompletionPrefix(String text) {
        // calculate the last word that the user started to type. This is the basis of
        // proposal filtering: they have to start with this prefix.
        return text.replaceFirst("^.*?([A-Za-z0-9_.]*)$", "$1");
    }

    /**
     * Generate proposals for per-object configuration keys
     */
    protected List<IContentProposal> getCandidatesForPerObjectConfig(String prefix) {
        String keySuffix = key.replaceFirst(".*\\.", ""); // only keep substring after last dot
        ConfigOption entry = ConfigRegistry.getPerObjectOption(keySuffix);
        if (entry == null)
            return new ArrayList<IContentProposal>();  // nothing

        List<IContentProposal> p = new ArrayList<IContentProposal>();

        // after "${", offer variable names
        if (prefix.matches(".*\\$\\{[A-Za-z0-9_]*"))
            addConfigVariableProposals(p);

        if (entry==CFGID_VECTOR_RECORDING_INTERVALS) {
            p.addAll(toProposals(new String[]{"$1..", "$1..$2, $3.."})); //XXX use templated proposals here!
        }
        if (entry.getDataType()==ConfigOption.DataType.CFG_BOOL) {
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

        List<IContentProposal> p = new ArrayList<IContentProposal>();

        // collect unique param nodes
        Set<ParamElement> paramSet = new HashSet<ParamElement>();
        ParamResolution[] resList = null;
        try {
            resList = analyzer.getParamResolutionsForKey(section, key, new Timeout(InifileEditor.CONTENTASSIST_TIMEOUT));
            for (ParamResolution res : resList)
                paramSet.add(res.paramDeclaration);
        } catch (ParamResolutionDisabledException e) {
            // analysis disabled, no param nodes
        } catch (ParamResolutionTimeoutException e) {
            // analysis timeout, no param nodes
        }

        // determine param type (all params matched must have the same type)
        int dataType = -1;
        for (ParamElement par : paramSet) {
            if (dataType == -1)
                dataType = par.getType();
            else if (dataType != par.getType()) {
                p.addAll(toProposals(new String[] {"default", "ask"}));
                return p; // just refuse to suggest anything more if types are inconsistent
            }
        }

        // generate proposals
        //TODO make use of parameter properties (like @choice)

        // after "${", offer variable names
        if (prefix.matches(".*\\$\\{[A-Za-z0-9_]*"))
            addConfigVariableProposals(p);

        p.addAll(toProposals(new String[] {"default", "ask"}));

        switch (dataType) {
        case NedElementConstants.NED_PARTYPE_BOOL:
            p.addAll(toProposals(new String[] {"true", "false"}));
            break;
        case NedElementConstants.NED_PARTYPE_INT:
            p.addAll(toProposals(new String[] {"0"}, "or any integer value"));
            p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTempl), "discrete distr."));
            p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTemplExt), "using a given RNG"));
            p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTempl), "continuous distr."));
            p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTemplExt), "using a given RNG"));
            break;
        case NedElementConstants.NED_PARTYPE_DOUBLE:
            p.addAll(toProposals(new String[] {"0.0"}, "or any double value"));
            p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTempl), "continuous distr."));
            p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTemplExt), "using a given RNG"));
            p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTempl), "discrete distr."));
            p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedDiscreteDistributionsTemplExt), "using a given RNG"));
            break;
        case NedElementConstants.NED_PARTYPE_STRING:
            // if the param is used in a "<param> like IFoo", propose all modules that implement IFoo
            if (resList != null) {
                Collection<INedTypeInfo> types = getProposedNedTypesFor(resList);
                if (types != null)
                    for (INedTypeInfo type : types) {
                        String docu = type.getFullyQualifiedName() + "\n\n";
                        docu += StringUtils.nullToEmpty(NedCommentFormatter.makeTextDocu(type.getNedElement().getComment()));
                        p.add(new ContentProposalEx("\""+type.getName()+"\"", "\""+type.getName()+"\"", docu));
                    }
                p.addAll(toProposals(new String[] {"\"\""}, "or any string value"));
            }
            break;
        case NedElementConstants.NED_PARTYPE_XML:
            p.addAll(toProposals(new String[] {"xml(\"string\")", "xml(\"string\", \"xpath\")"}));
            p.addAll(toProposals(new String[] {"xmldoc(\"filename\")", "xmldoc(\"filename\", \"xpath\")"}));
            break;
        }
        return p;
    }

    protected void addConfigVariableProposals(List<IContentProposal> p) {
        //FIXME after "$", offer: ${start..end}  ${start..end step x} ${start..end step x}s ${var=start..end step x} ${var=val1,val2,val3} ${var=val1,val2,val3 ! something
        //p.addAll(toProposals(templatesToProposals(NedCompletionHelper.proposedNedContinuousDistributionsTemplExt), "using a given RNG"));
        //return new Template(name, description, DEFAULT_NED_CONTEXT_TYPE, pattern, false);
        //XXX long descriptions do not get wrapped somehow -- do it manually?
        //XXX also: offer vars inside string constants too

        p.addAll(toProposals(analyzer.getIterationVariableNames(section), "Iteration variable"));
        for (String variable : ConfigRegistry.getConfigVariableNames())
            p.add(new ContentProposalEx(variable, variable, ConfigRegistry.getConfigVariableDescription(variable)));
    }

    /**
     * If some of the matching parameters are used in a "&lt;param&gt; like IFoo" submodule,
     * propose all modules that implement IFoo. If there's IFoo, IBar etc, propose modules
     * that implement both (all).
     */
    protected Collection<INedTypeInfo> getProposedNedTypesFor(ParamResolution[] paramResList) {
        IProject context = doc.getDocumentFile().getProject();

        // collect all interfaces that parameters require
        Set<INedTypeInfo> likeInterfaces = new HashSet<INedTypeInfo>();
        for (ParamResolution param : paramResList)
            likeInterfaces.addAll(extractParamLikeInterfaces(param, context));

        // if different params require different interfaces, find the common subset
        // of modules that implement both/all, and return that.
        Set<INedTypeInfo> result = new HashSet<INedTypeInfo>();
        boolean firstIter = true;
        for (INedTypeInfo likeInterface : likeInterfaces) {
            Collection<INedTypeInfo> types = likeInterface.getResolver().getToplevelNedTypesThatImplement(likeInterface, context);
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
    protected Set<INedTypeInfo> extractParamLikeInterfaces(ParamResolution param, IProject context) {
        // find compound module in which the parameter is declared. We'll be looking for
        // matching "like" submodules in there.

        //FIXME there's a chance that the module instantiated in the network is actually
        // a subclass of this compound module ("StandardHostExt" not "StandardHost"),
        // so we miss submodules added in the subclass. However, I doubt that we can
        // find out the subclass from ParamResolution -- TODO add it
        INedTypeLookupContext paramContext = param.paramDeclaration.getEnclosingLookupContext();
        if (!(paramContext instanceof CompoundModuleElementEx))
            return new HashSet<INedTypeInfo>(); // not a compound module parameter

        CompoundModuleElementEx module = (CompoundModuleElementEx)paramContext;

        // collect its "like" submodules that refer to our parameter.
        // note: we only look to local submodules: inherited submodules wouldn't see
        // this parameter (since it's declared in a subclass)
        Set<INedTypeInfo> result = new HashSet<INedTypeInfo>();
        INedTypeResolver res = module.getResolver();
        String paramName = param.paramDeclaration.getName();
        for (SubmoduleElementEx submodule : module.getSubmodules()) {
            if (submodule.getLikeExpr().equals(paramName)) {
                // resolve interface, then add to the result
                //Debug.println("found 'like' submodule: " + submodule + " --> " + submodule.getLikeType());
                String likeType = submodule.getLikeType();
                INedTypeInfo likeInterface = res.lookupNedType(likeType, module);
                if (likeInterface != null && likeInterface.getNedElement() instanceof ModuleInterfaceElement)
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

}

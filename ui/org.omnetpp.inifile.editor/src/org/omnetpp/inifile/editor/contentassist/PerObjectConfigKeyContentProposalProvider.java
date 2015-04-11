/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.contentassist;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.ConfigOption.ObjectKind;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamCollector;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.ParamResolutionDisabledException;
import org.omnetpp.inifile.editor.model.ParamResolutionTimeoutException;
import org.omnetpp.inifile.editor.model.PropertyResolution;
import org.omnetpp.inifile.editor.model.Timeout;
import org.omnetpp.ned.core.NedResources;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;

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
    private IReadonlyInifileDocument doc;
    private InifileAnalyzer analyzer;

    public PerObjectConfigKeyContentProposalProvider(String section, boolean addEqualSign, IReadonlyInifileDocument doc, InifileAnalyzer analyzer) {
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

    @Override
    protected List<IContentProposal> getProposalCandidates(String prefix) {
        ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();
        if (section != null && prefix.contains(".")) {
            String prefixBase = prefix.substring(0, prefix.lastIndexOf('.'));
            Set<ObjectKind> possibleObjectKinds = getPossibleObjectKinds(prefixBase);
            for (ConfigOption e : ConfigRegistry.getPerObjectOptions()) {
                if (possibleObjectKinds.contains(e.getObjectKind())) {
                    String content = prefixBase + "." + e.getName() + (addEqualSign ? " = " : "");
                    result.add(new ContentProposal(content, content, getConfigHelpText(e, section, doc), getImage(e.getObjectKind())));
                }
            }
        }
        return sort(result);
    }

    /**
     * Generate help text for the given config entry, to be displayed by the content assistant.
     */
    protected static String getConfigHelpText(ConfigOption entry, String section, IReadonlyInifileDocument doc) {
        String key = entry.getName();
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

    protected static Image getImage(ObjectKind kind) {
        switch (kind) {
        case KIND_PARAMETER: return InifileUtils.ICON_PROPOSAL_PARAMETERCONFIG;
        case KIND_STATISTIC: return InifileUtils.ICON_PROPOSAL_STATISTICCONFIG;
        case KIND_MODULE:
        case KIND_SIMPLE_MODULE:
        case KIND_UNSPECIFIED_TYPE: return InifileUtils.ICON_PROPOSAL_MODULECONFIG;
        case KIND_SCALAR: return InifileUtils.ICON_PROPOSAL_SCALARCONFIG;
        case KIND_VECTOR: return InifileUtils.ICON_PROPOSAL_VECTORCONFIG;
        case KIND_OTHER: //TODO
        }
        return null;
    }

    protected Set<ObjectKind> getPossibleObjectKinds(String objectNamePattern) {
        Set<ObjectKind> objectKinds = EnumSet.noneOf(ObjectKind.class);

        PatternMatcher nameMatcher = new PatternMatcher(objectNamePattern, true, true, true);
        ParamResolution[] paramResolutions = null;
        PropertyResolution[] propertyResolutions = null;
        INedTypeResolver resolver = NedResources.getInstance().getImmutableCopy();
        Map<String,ISubmoduleOrConnection> modules = null;
        try {
            Timeout timeout = new Timeout(InifileEditor.CONTENTASSIST_TIMEOUT);
            paramResolutions = analyzer.getParamResolutions(section, timeout);
            propertyResolutions = analyzer.getPropertyResolutions(section, timeout);
            modules = ParamCollector.collectModules(doc, section, nameMatcher, resolver, null); // XXX this traverses the module tree again, should be collected during the analysis
        } catch (ParamResolutionDisabledException e) {
            return objectKinds;
        } catch (ParamResolutionTimeoutException e) {
            return objectKinds;
        }

        // match any parameter?
        for (ParamResolution paramResolution : paramResolutions) {
            if (paramResolution.type != ParamResolution.ParamResolutionType.NED) {
                String paramName = paramResolution.fullPath + "." +paramResolution.paramDeclaration.getName();
                if (nameMatcher.matches(paramName)) {
                    objectKinds.add(ObjectKind.KIND_PARAMETER);
                    break;
                }
            }
        }

        // match any module name?
        Set<ObjectKind> moduleKinds = EnumSet.of(ObjectKind.KIND_MODULE, ObjectKind.KIND_SIMPLE_MODULE, ObjectKind.KIND_UNSPECIFIED_TYPE);
        for (Map.Entry<String, ISubmoduleOrConnection> entry : modules.entrySet()) {
            objectKinds.add(ObjectKind.KIND_MODULE);
            ISubmoduleOrConnection module = entry.getValue();
            if (module != null && module.getEffectiveTypeRef() instanceof SimpleModuleElement) //FIXME getEffectiveTypeRef(): not good here: does not obey inifile content (it always returns an IModuleInterfaceElement for "like" submodules)
                objectKinds.add(ObjectKind.KIND_SIMPLE_MODULE);
            if (module != null && !StringUtils.isEmpty(module.getLikeType()))
                objectKinds.add(ObjectKind.KIND_UNSPECIFIED_TYPE);
            if (objectKinds.containsAll(moduleKinds))
                break;
        }

        // match any statistic?
        for (PropertyResolution propertyResolution : propertyResolutions) {
            if ("statistic".equals(propertyResolution.propertyDeclaration.getName())) {
                if (nameMatcher.matches(propertyResolution.fullPath)) {
                    objectKinds.add(ObjectKind.KIND_STATISTIC);
                    break;
                }
            }
        }

        // If pattern is <module_pattern>.<something>, and <something> is not a parameter or statistic or module,
        // then assume that it is the name of a scalar or vector.
        // Also add SCALAR and VECTOR to the possibilities if pattern ends with '**'.
        if ((objectKinds.isEmpty() && objectNamePattern.contains(".")) || objectNamePattern.endsWith("**")) {
            int index = objectNamePattern.lastIndexOf('.');
            String modulePrefix = index >= 0 ? objectNamePattern.substring(0, index) : objectNamePattern;
            PatternMatcher modulePattern = new PatternMatcher(modulePrefix, true, true, true);
            Map<String,ISubmoduleOrConnection> parentModules = ParamCollector.collectModules(doc, section, modulePattern, resolver, null);
            if (!parentModules.isEmpty()) {
                objectKinds.add(ObjectKind.KIND_SCALAR);
                objectKinds.add(ObjectKind.KIND_VECTOR);
            }
        }

        //TODO add KIND_OTHER

        return objectKinds;
    }
}
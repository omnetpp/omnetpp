package org.omnetpp.inifile.editor.model;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ParamResolution.ParamResolutionType;
import org.omnetpp.ned.core.NedTreeTraversal;
import org.omnetpp.ned.core.ParamUtil;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;

public class ParamCollector {

    /**
     * Collect parameters, signals and statistics from the network configured in the given
     * section of the ini file document.
     */
    public static ParamResolutionStatus.Entry collectParametersAndProperties(IReadonlyInifileDocument doc,
            INedTypeResolver nedResolver, String activeSection, IProgressMonitor monitor) {

        Assert.isNotNull(doc);
        Assert.isNotNull(activeSection);

        ModuleTreeVisitor visitor = new ModuleTreeVisitor(doc, activeSection, true, new String[] {"signal", "statistic"}, null, monitor);
        String networkName = InifileUtils.lookupNetwork(doc, activeSection);
        INedTypeInfo network = networkName != null ? resolveNetwork(doc, nedResolver, networkName) : null;
        if (network != null) {
            IProject contextProject = doc.getDocumentFile().getProject();
            NedTreeTraversal treeTraversal = new NedTreeTraversal(nedResolver, visitor, contextProject);
            treeTraversal.traverse(network.getFullyQualifiedName());
        }

        ParamResolutionStatus.Entry result = new ParamResolutionStatus.Entry();
        result.section = activeSection;
        result.paramResolutions = visitor.getParamResolutions();
        result.propertyResolutions = visitor.getPropertyResolutions();
        return result;
    }


    /**
     * Collect module names from the network configured in the given
     * section of the ini file document and whose name matches with {@namePattern}.
     */
    public static Map<String,ISubmoduleOrConnection> collectModules(IReadonlyInifileDocument doc, String section,
            PatternMatcher namePattern, INedTypeResolver nedResolver, IProgressMonitor monitor) {

        Assert.isNotNull(doc);
        Assert.isNotNull(section);

        ModuleTreeVisitor visitor = new ModuleTreeVisitor(doc, section, true, null, namePattern, monitor);
        String networkName = InifileUtils.lookupNetwork(doc, section);
        INedTypeInfo network = networkName != null ? resolveNetwork(doc, nedResolver, networkName) : null;
        if (network != null) {
            IProject contextProject = doc.getDocumentFile().getProject();
            NedTreeTraversal treeTraversal = new NedTreeTraversal(nedResolver, visitor, contextProject);
            treeTraversal.traverse(network.getFullyQualifiedName());
        }

        return visitor.getModules();
    }

    /**
     * Collects parameters of a module type (recursively), *without* an inifile present.
     */
    public static List<ParamResolution> collectParameters(INedTypeInfo moduleType, INedTypeResolver nedResolver) {
        ModuleTreeVisitor visitor = new ModuleTreeVisitor(true, null, null);
        // the contextProject parameter affects the resolution of parametric submodule types ("like").
        NedTreeTraversal treeTraversal = new NedTreeTraversal(nedResolver, visitor, moduleType.getProject());
        treeTraversal.traverse(moduleType);
        return visitor.getParamResolutions();
    }

    /**
     * Collects parameters of a submodule subtree, *without* an inifile present.
     */
    public static List<ParamResolution> collectParameters(SubmoduleElementEx submodule, INedTypeResolver nedResolver) {
        // the contextProject parameter affects the resolution of parametric submodule types ("like").
        IProject contextProject = submodule.getEnclosingTypeElement().getNedTypeInfo().getProject();
        // TODO: this ignores deep parameter settings from the compound module above the submodule
        ModuleTreeVisitor visitor = new ModuleTreeVisitor(true, null, null);
        NedTreeTraversal treeTraversal = new NedTreeTraversal(nedResolver,  visitor, contextProject);
        treeTraversal.traverse(submodule);
        return visitor.getParamResolutions();
    }

    /**
     * Resolve parameters of a module type or submodule.
     */
    public static void resolveModuleParameters(List<ParamResolution> resultList, String fullPath, Vector<INedTypeInfo> typeInfoPath,
            Vector<ISubmoduleOrConnection> elementPath, String[] sectionChain, IReadonlyInifileDocument doc) {
        for (ParamElementEx paramDeclaration : typeInfoPath.lastElement().getParamDeclarations().values())
            resolveParameter(resultList, fullPath, typeInfoPath, elementPath, sectionChain, doc, paramDeclaration);
    }

    /**
     * Resolve properties of a module type or submodule.
     */
    public static void resolveModuleProperties(String propertyName, List<PropertyResolution> list,
            String fullPath, Vector<INedTypeInfo> typeInfoPath, Vector<ISubmoduleOrConnection> elementPath, String activeSection) {
        INedTypeInfo typeInfo = typeInfoPath.lastElement();
        Map<String, PropertyElementEx> propertyMap = typeInfo.getProperties().get(propertyName);
        if (propertyMap != null)
            for (PropertyElementEx property : propertyMap.values())
                list.add(new PropertyResolution(fullPath + "." + property.getIndex(), elementPath, property, activeSection));
    }

    /**
     * Determines how a NED parameter gets assigned (inifile, NED file, etc).
     * The sectionChain and doc parameters may be null, which means that only parameter
     * assignments given in NED will be taken into account.
     *
     * This method adds one or more ParamResolution objects to resultList. For example,
     * if the inifile contains lines like:
     *     Network.node[0].address = value1
     *     Network.node[1].address = value2
     *     Network.node[*].address = valueX
     * then this method will add three ParamResolutions.
     */
    // TODO: normalize param resolutions in terms of vector indices, that is the resulting param resolutions should be disjunct
    //       this makes the order of resolutions unimportant, it also helps the user to find the actual value of a particluar parameter
    //       since indices are always individual constants, or constant ranges, or wildcards,
    //       and vector lower bound is always 0, while vector upper bound is either constant or unknown
    //       it is quite doable even if not so simple
    public static void resolveParameter(List<ParamResolution> resultList, String fullPath, Vector<INedTypeInfo> typeInfoPath, Vector<ISubmoduleOrConnection> elementPath, String[] sectionChain, IReadonlyInifileDocument doc, ParamElementEx paramDeclaration)
    {
        // look up parameter assignments in NED
        ArrayList<ParamElementEx> paramAssignments = ParamUtil.findParamAssignmentsForParamDeclaration(typeInfoPath, elementPath, paramDeclaration);
        boolean hasNedUnassigned = false;
        boolean hasNedTotalAssignment = false;
        boolean hasNedDefaultAssignment = false;
        for (ParamElementEx paramAssignment : paramAssignments) {
            hasNedUnassigned |= StringUtils.isEmpty(paramAssignment.getValue());
            hasNedTotalAssignment |= ParamUtil.isTotalParamAssignment(paramAssignment);
            hasNedDefaultAssignment |= paramAssignment.getIsDefault();
        }

        // look up parameter assignments in INI
        String activeSection = null;
        List<SectionKey> sectionKeys = null;
        boolean hasIniTotalAssignment = false;

        if (doc != null) {
            activeSection = sectionChain[0];

            // TODO: avoid calling lookupParameter twice
            String paramPath = fullPath + "." + paramDeclaration.getName();
            sectionKeys = InifileUtils.lookupParameter(paramPath, false, sectionChain, doc);

            for (SectionKey sectionKey : sectionKeys)
                hasIniTotalAssignment |= ParamUtil.isTotalParamAssignment(sectionKey.key);

            sectionKeys = InifileUtils.lookupParameter(paramPath, hasNedDefaultAssignment, sectionChain, doc);
        }

        // process non default parameter assignments from NED
        for (ParamElementEx paramAssignment : paramAssignments) {
            if (!paramAssignment.getIsDefault() && !StringUtils.isEmpty(paramAssignment.getValue())) {
                resultList.add(new ParamResolution(fullPath, elementPath, paramDeclaration, paramAssignment, ParamResolutionType.NED, activeSection, null, null));

                if (ParamUtil.isTotalParamAssignment(paramAssignment))
                    return;
            }
        }

        // process parameter assignments from INI
        if (doc != null) {
            for (SectionKey sectionKey : sectionKeys) {
                String iniSection = sectionKey.section;
                String iniKey = sectionKey.key;
                String iniValue = doc.getValue(iniSection, iniKey);
                Assert.isTrue(iniValue != null); // must exist

                // so, find out how the parameter's going to be assigned...
                ParamResolutionType type;
                if (iniValue.equals(ConfigRegistry.DEFAULT)) {
                    if (!hasNedDefaultAssignment)
                        continue;
                    else
                        type = ParamResolutionType.INI_DEFAULT;
                }
                else if (iniValue.equals(ConfigRegistry.ASK))
                    type = ParamResolutionType.INI_ASK;
                else if (paramAssignments.size() == 1 && hasNedUnassigned)
                    type = ParamResolutionType.INI;
                else if (paramAssignments.size() == 1 && paramAssignments.get(0).getValue().equals(iniValue))
                    type = ParamResolutionType.INI_NEDDEFAULT;
                else
                    type = ParamResolutionType.INI_OVERRIDE;

                ParamElementEx paramAssignment = paramAssignments.size() > 0 ? paramAssignments.get(0) : null;
                resultList.add(new ParamResolution(fullPath, elementPath, paramDeclaration, paramAssignment, type, activeSection, iniSection, iniKey));
            }
        }

        // process default parameter assignments from NED (this is already in reverse order)
        for (ParamElementEx paramAssignment : paramAssignments) {
            if (StringUtils.isEmpty(paramAssignment.getValue())) {
                if (hasIniTotalAssignment)
                    continue;
                else
                    resultList.add(new ParamResolution(fullPath, elementPath, paramDeclaration, null, ParamResolutionType.UNASSIGNED, activeSection, null, null));
            }
            else if (paramAssignment.getIsDefault() && !hasIniTotalAssignment)
                resultList.add(new ParamResolution(fullPath, elementPath, paramDeclaration, paramAssignment, ParamResolutionType.IMPLICITDEFAULT, activeSection, null, null));
        }
    }

    public static INedTypeInfo resolveNetwork(IReadonlyInifileDocument doc, INedTypeResolver ned, String value) {
        INedTypeInfo network = null;
        IFile iniFile = doc.getDocumentFile();
        String inifilePackage = ned.getPackageFor(iniFile.getParent());
        IProject contextProject = iniFile.getProject();
        if (inifilePackage != null && value != null) {
            String networkName = inifilePackage + (inifilePackage.length()!=0 && value.length()!=0 ? "." : "")+value;
            network = ned.getToplevelNedType(networkName, contextProject);
        }
        if (network == null)
            network = ned.getToplevelNedType(value, contextProject);

        return network;
    }
}
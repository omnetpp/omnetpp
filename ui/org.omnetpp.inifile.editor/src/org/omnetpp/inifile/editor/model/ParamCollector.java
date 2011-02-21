package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Stack;
import java.util.Vector;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.OperationCanceledException;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ParamResolution.ParamResolutionType;
import org.omnetpp.ned.core.IModuleTreeVisitor;
import org.omnetpp.ned.core.NedTreeTraversal;
import org.omnetpp.ned.core.ParamUtil;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;

public class ParamCollector {
    
    private static class ParamCollectingVisitor extends ParamUtil.RecursiveParamDeclarationVisitor {
        private final IReadonlyInifileDocument doc;
        private final String[] sectionChain;
        private final List<ParamResolution> resultList;
        private final IProgressMonitor monitor;

        public ParamCollectingVisitor() {
            this.doc = null;
            this.sectionChain = null;
            this.resultList = new ArrayList<ParamResolution>();
            this.monitor = null;
        }
        
        public ParamCollectingVisitor(IReadonlyInifileDocument doc, String[] sectionChain, IProgressMonitor monitor) {
            this.doc = doc;
            this.resultList = new ArrayList<ParamResolution>();
            this.sectionChain = sectionChain;
            this.monitor = monitor;
        }
        
        public List<ParamResolution> getResult() {
            return resultList;
        }
        
        

        @Override
        public boolean enter(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
            if (monitor != null && monitor.isCanceled())
                throw new OperationCanceledException();
            return super.enter(element, typeInfo);
        }

        @Override
        protected boolean visitParamDeclaration(String fullPath, Stack<INedTypeInfo> typeInfoPath, Stack<ISubmoduleOrConnection> elementPath, ParamElementEx paramDeclaration) {
            ParamCollector.resolveParameter(resultList, fullPath, typeInfoPath, elementPath, sectionChain, doc, paramDeclaration);
            return true;
        }

        @Override
        public String resolveLikeType(ISubmoduleOrConnection element) {
            // Note: we cannot use InifileUtils.resolveLikeParam(), as that calls
            // resolveLikeParam() which relies on the data structure we are currently building

            // get like parameter name
            String likeParamName = element.getLikeParam();
            if (likeParamName != null && !likeParamName.matches("[A-Za-z_][A-Za-z0-9_]*"))
                return null;  // sorry, we are only prepared to resolve parent module parameters (but not expressions)

            // look up parameter value (note: we cannot use resolveLikeParam() here yet)
            String fullPath = StringUtils.join(fullPathStack, ".");
            ParamResolution res = null;
            for (ParamResolution r : resultList)
                if (r.paramDeclaration.getName().equals(likeParamName) && r.fullPath.equals(fullPath))
                    {res = r; break;}
            if (res == null)
                return null; // likely no such parameter
            String value = getParamValue(res, doc);
            if (value == null)
                return null; // likely unassigned
            try {
                value = Common.parseQuotedString(value);
            } catch (RuntimeException e) {
                return null; // something is wrong: value is not a string constant?
            }
            // note: value is likely a simple (unqualified) name, it'll be resolved
            // to fully qualified name in the caller (NedTreeTraversal)
            return value;
        }
    }
    
    private static final class SignalCollectingVisitor implements IModuleTreeVisitor {
        private final ArrayList<PropertyResolution> list;
        private final String activeSection;
        protected Stack<ISubmoduleOrConnection> elementPath = new Stack<ISubmoduleOrConnection>();
        protected Stack<String> fullPathStack = new Stack<String>();  //XXX performance: use cumulative names, so that StringUtils.join() can be eliminated (like: "Net", "Net.node[*]", "Net.node[*].ip" etc)
        protected IProgressMonitor monitor;
        
        public SignalCollectingVisitor(String activeSection, IProgressMonitor monitor) {
            this.list = new ArrayList<PropertyResolution>();
            this.activeSection = activeSection;
            this.monitor = monitor;
        }
        
        public List<PropertyResolution> getResult() {
            return list;
        }

        public boolean enter(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
            if (monitor != null && monitor.isCanceled())
                throw new OperationCanceledException();
            
            elementPath.push(element);
            fullPathStack.push(element == null ? typeInfo.getName() : ParamUtil.getParamPathElementName(element));
            for (String propertyName : new String[] {"signal", "statistic"}) {
                Map<String, PropertyElementEx> propertyMap = typeInfo.getProperties().get(propertyName);
                String fullPath = StringUtils.join(fullPathStack, ".");
                if (propertyMap != null)
                    for (PropertyElementEx property : propertyMap.values())
                        list.add(new PropertyResolution(fullPath + "." + property.getIndex(), elementPath, property, activeSection));
            }
            return true;
        }

        public void leave() {
            elementPath.pop();
            fullPathStack.pop();
        }

        public void recursiveType(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
        }

        public String resolveLikeType(ISubmoduleOrConnection element) {
            return null;
        }

        public void unresolvedType(ISubmoduleOrConnection element, String typeName) {
        }
    }
    
    public static List<ParamResolution> collectParameters(IReadonlyInifileDocument doc, INedTypeResolver nedResolver, String activeSection,
                                                            IProgressMonitor monitor) {
        // resolve section chain and network
        INedTypeInfo network = findConfiguredNetwork(doc, nedResolver, activeSection);
        if (network == null)
            return new ArrayList<ParamResolution>();
    
        // traverse the network and collect resolutions meanwhile
        final String[] sectionChain = InifileUtils.resolveSectionChain(doc, activeSection);
        IProject contextProject = doc.getDocumentFile().getProject();
        ParamCollectingVisitor visitor = new ParamCollectingVisitor(doc, sectionChain, monitor);
        NedTreeTraversal treeTraversal = new NedTreeTraversal(nedResolver, visitor, contextProject);
        treeTraversal.traverse(network.getFullyQualifiedName());
    
        return visitor.getResult();
    }

    /**
     * Collects parameters of a module type (recursively), *without* an inifile present.
     */
    public static List<ParamResolution> collectParameters(INedTypeInfo moduleType, INedTypeResolver nedResolver) {
        return collectParameters(moduleType, nedResolver, moduleType.getProject());
    }
    
    /**
     * Collects parameters of a submodule subtree, *without* an inifile present.
     */
    public static List<ParamResolution> collectParameters(SubmoduleElementEx submodule, INedTypeResolver nedResolver) {
        IProject contextProject = submodule.getEnclosingTypeElement().getNedTypeInfo().getProject();
        return collectParameters(submodule, nedResolver, contextProject);
    }
    
    public static List<PropertyResolution> collectSignalResolutions(IReadonlyInifileDocument doc, INedTypeResolver nedResolver, String activeSection,
                                                                     IProgressMonitor monitor) {
        INedTypeInfo network = findConfiguredNetwork(doc, nedResolver, activeSection);
        if (network == null )
            return new ArrayList<PropertyResolution>();
    
        // traverse the network and collect resolutions meanwhile
        SignalCollectingVisitor visitor = new SignalCollectingVisitor(activeSection, monitor);
        NedTreeTraversal treeTraversal = new NedTreeTraversal(nedResolver, visitor, doc.getDocumentFile().getProject());
        treeTraversal.traverse(network.getFullyQualifiedName());
        return visitor.getResult();
    }


    /**
     * Collects parameters of a module type (recursively), *without* an inifile present.
     * The contextProject parameter affects the resolution of parametric submodule types ("like").
     */
    private static List<ParamResolution> collectParameters(INedTypeInfo moduleType, INedTypeResolver nedResolver, IProject contextProject) {
        ParamCollectingVisitor visitor = new ParamCollectingVisitor();
        NedTreeTraversal treeTraversal = new NedTreeTraversal(nedResolver, visitor, contextProject);
        treeTraversal.traverse(moduleType);
        return visitor.getResult();
    }

    /**
     * Collects parameters of a submodule subtree, *without* an inifile present.
     * The contextProject parameter affects the resolution of parametric submodule types ("like").
     */
    private static List<ParamResolution> collectParameters(SubmoduleElementEx submodule, INedTypeResolver nedResolver, IProject contextProject) {
        // TODO: this ignores deep parameter settings from the compound module above the submodule
        ParamCollectingVisitor visitor = new ParamCollectingVisitor();
        NedTreeTraversal treeTraversal = new NedTreeTraversal(nedResolver,  visitor, contextProject);
        treeTraversal.traverse(submodule);
        return visitor.getResult();
    }

    private static String getParamValue(ParamResolution res, IReadonlyInifileDocument doc) {
        return getParamValue(res, doc, true);
    }

    private static String getParamValue(ParamResolution res, IReadonlyInifileDocument doc, boolean allowNull) {
        switch (res.type) {
            case UNASSIGNED:
                if (allowNull)
                    return null;
                else
                    return "(unassigned)";
            case INI_ASK:
                if (allowNull)
                    return null;
                else
                    return "(ask)";
            case NED: case INI_DEFAULT: case IMPLICITDEFAULT:
                return res.paramAssignment.getValue();
            case INI: case INI_OVERRIDE: case INI_NEDDEFAULT:
                return doc.getValue(res.section, res.key);
            default: throw new IllegalArgumentException("invalid param resolution type: "+res.type);
        }
    }
    
    /**
     * Resolve parameters of a module type or submodule, based solely on NED information,
     * without inifile. This is useful for views when a NED editor is active.
     */
    public static void resolveModuleParameters(List<ParamResolution> resultList, String fullPath, Vector<INedTypeInfo> typeInfoPath, Vector<ISubmoduleOrConnection> elementPath) {
        for (ParamElementEx paramDeclaration : typeInfoPath.lastElement().getParamDeclarations().values())
            resolveParameter(resultList, fullPath, typeInfoPath, elementPath, null, null, paramDeclaration);
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
            sectionKeys = InifileUtils.lookupParameter(fullPath + "." + paramDeclaration.getName(), false, sectionChain, doc);
    
            for (SectionKey sectionKey : sectionKeys)
                hasIniTotalAssignment |= ParamUtil.isTotalParamAssignment(sectionKey.key);
    
            sectionKeys = InifileUtils.lookupParameter(fullPath + "." + paramDeclaration.getName(), hasNedDefaultAssignment, sectionChain, doc);
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
        String inifilePackage = ned.getExpectedPackageFor(iniFile);
        IProject contextProject = iniFile.getProject();
        if (inifilePackage != null && value != null) {
            String networkName = inifilePackage + (inifilePackage.length()!=0 && value.length()!=0 ? "." : "")+value;
            network = ned.getToplevelNedType(networkName, contextProject);
        }
        if (network == null)
            network = ned.getToplevelNedType(value, contextProject);
    
        return network;
    }
    
    private static INedTypeInfo findConfiguredNetwork(IReadonlyInifileDocument doc, INedTypeResolver nedResolver, String section) {
        String[] sectionChain = InifileUtils.resolveSectionChain(doc, section);
        String networkName = InifileUtils.lookupConfig(sectionChain, CFGID_NETWORK.getName(), doc);
        if (networkName == null)
            networkName = CFGID_NETWORK.getDefaultValue();
        INedTypeInfo network = networkName != null ? resolveNetwork(doc, nedResolver, networkName) : null;
        return network;
    }

    public static void resolveModuleProperties(String propertyName, List<PropertyResolution> list, String fullPath, Vector<INedTypeInfo> typeInfoPath, Vector<ISubmoduleOrConnection> elementPath) {
        INedTypeInfo typeInfo = typeInfoPath.lastElement();
        Map<String, PropertyElementEx> propertyMap = typeInfo.getProperties().get(propertyName);
        if (propertyMap != null)
            for (PropertyElementEx property : propertyMap.values())
                list.add(new PropertyResolution(fullPath + "." + property.getIndex(), elementPath, property, null));
    }
}
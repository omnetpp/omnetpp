package org.omnetpp.inifile.editor.model;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.OperationCanceledException;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.IModuleTreeVisitor;
import org.omnetpp.ned.core.ParamUtil;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;

/**
 * Module tree visitor for collecting module parameters and properties.
 */
class ModuleTreeVisitor  implements IModuleTreeVisitor {
    
    private final IReadonlyInifileDocument doc;
    private final String[] sectionChain;
    private final IProgressMonitor monitor;
    private final boolean collectParameters;
    private final String[] propertiesToCollect;     // names of properties to be collected, null if none
    private final PatternMatcher moduleNamePattern;
    
    private final List<ParamResolution> paramResolutions;
    private final List<PropertyResolution> propertyResolutions;
    private final Map<String,ISubmoduleOrConnection> modules;

    protected Stack<ISubmoduleOrConnection> elementPath = new Stack<ISubmoduleOrConnection>();
    protected Stack<INedTypeInfo> typeInfoPath = new Stack<INedTypeInfo>();
    protected Stack<String> fullPathStack = new Stack<String>();  //XXX performance: use cumulative names, so that StringUtils.join() can be eliminated (like: "Net", "Net.node[*]", "Net.node[*].ip" etc)
    
    /**
     * For traversing a given ned element or ned type.
     */
    public ModuleTreeVisitor(boolean collectParameters, String[] propertiesToCollect, PatternMatcher moduleNamePattern) {
        this.doc = null;
        this.sectionChain = null;
        this.monitor = null;
        this.collectParameters = collectParameters;
        this.propertiesToCollect = propertiesToCollect;
        this.moduleNamePattern = moduleNamePattern;
        
        this.paramResolutions = collectParameters ? new ArrayList<ParamResolution>() : null;
        this.propertyResolutions = propertiesToCollect != null ? new ArrayList<PropertyResolution>() : null;
        this.modules = moduleNamePattern != null ? new HashMap<String,ISubmoduleOrConnection>() : null;
    }
    
    /**
     * For traversing the configured network.
     */
    public ModuleTreeVisitor(IReadonlyInifileDocument doc, String activeSection,
            boolean collectParameters, String[] propertiesToCollect, PatternMatcher moduleNamePattern, IProgressMonitor monitor) {
        Assert.isTrue(collectParameters || propertiesToCollect != null || moduleNamePattern != null);
        this.doc = doc;
        this.sectionChain = InifileUtils.resolveSectionChain(doc, activeSection);
        this.collectParameters = collectParameters;
        this.propertiesToCollect = propertiesToCollect;
        this.moduleNamePattern = moduleNamePattern;
        this.monitor = monitor;

        this.paramResolutions = collectParameters ? new ArrayList<ParamResolution>() : null;
        this.propertyResolutions = propertiesToCollect != null ? new ArrayList<PropertyResolution>() : null;
        this.modules = moduleNamePattern != null ? new HashMap<String,ISubmoduleOrConnection>() : null;
    }
    
    public List<ParamResolution> getParamResolutions() {
        return paramResolutions;
    }
    
    public List<PropertyResolution> getPropertyResolutions() {
        return propertyResolutions;
    }
    
    public Map<String,ISubmoduleOrConnection> getModules() {
        return modules;
    }

    public boolean enter(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
        if (monitor != null && monitor.isCanceled())
            throw new OperationCanceledException();
        
        elementPath.push(element);
        typeInfoPath.push(typeInfo);

        fullPathStack.push(element == null ? typeInfo.getName() : ParamUtil.getParamPathElementName(element));
        String fullPath = StringUtils.join(fullPathStack, ".");
        
        if (moduleNamePattern == null || moduleNamePattern.matches(fullPath)) {
            if (moduleNamePattern != null)
                modules.put(fullPath,element);
            
            // collect parameters
            if (collectParameters) {
                ParamCollector.resolveModuleParameters(paramResolutions, fullPath, typeInfoPath, elementPath, sectionChain, doc);
            }
            
            // collect properties
            if (propertiesToCollect != null) {
                String activeSection = sectionChain != null ? sectionChain[0] : null;
                for (String propertyName : propertiesToCollect)
                    ParamCollector.resolveModuleProperties(propertyName, propertyResolutions, fullPath, typeInfoPath, elementPath, activeSection);
            }
        }
        
        return true;
    }
    
    public void leave() {
        elementPath.pop();
        typeInfoPath.pop();
        fullPathStack.pop();
    }

    public void unresolvedType(ISubmoduleOrConnection element, String typeName) {
    }

    public void recursiveType(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
    }

    public String resolveLikeType(ISubmoduleOrConnection element) {
        // Note: we cannot use InifileUtils.resolveLikeParam(), as that calls
        // resolveLikeParam() which relies on the data structure we are currently building
        
        if (!collectParameters)
            return null;

        // get like parameter name
        String likeParamName = element.getLikeParam();
        if (likeParamName != null && !likeParamName.matches("[A-Za-z_][A-Za-z0-9_]*"))
            return null;  // sorry, we are only prepared to resolve parent module parameters (but not expressions)

        // look up parameter value (note: we cannot use resolveLikeParam() here yet)
        String fullPath = StringUtils.join(fullPathStack, ".");
        ParamResolution res = null;
        for (ParamResolution r : paramResolutions)
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
}
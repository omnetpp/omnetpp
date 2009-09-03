package org.omnetpp.ned.core;

import java.util.Map;
import java.util.Stack;
import java.util.Vector;

import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;

public class ParamUtil {
    /**
     * Calls visitor for each parameter declaration accessible under the provided type info.
     * Recurses down on submodules.
     */
    public static void mapParamDeclarationsRecursively(INEDTypeInfo typeInfo, RecursiveParamDeclarationVisitor visitor) {
        NEDTreeTraversal treeTraversal = new NEDTreeTraversal(NEDResourcesPlugin.getNEDResources(), visitor);
        treeTraversal.traverse(typeInfo);
    }

    /**
     * Calls visitor for each parameter declaration accessible under the provided submodule.
     * Recurses down on submodules.
     */
    public static void mapParamDeclarationsRecursively(SubmoduleElementEx submodule, RecursiveParamDeclarationVisitor visitor) {
        NEDTreeTraversal treeTraversal = new NEDTreeTraversal(NEDResourcesPlugin.getNEDResources(), visitor);
        treeTraversal.traverse(submodule);
    }

    /**
     * Returns true if there is at least one parameter declaration under type info that matches the provided name pattern.
     * Recurses down on submodules.
     */
    public static boolean hasMatchingParamDeclarationRecursively(INEDTypeInfo typeInfo, String paramNamePattern) {
        final String fullPattern = typeInfo.getName() + "." + paramNamePattern;
        final boolean[] result = new boolean[] {false};
        mapParamDeclarationsRecursively(typeInfo, new RecursiveParamDeclarationVisitor() {
            @Override
            protected boolean visitParamDeclaration(String fullPath, Stack<INEDTypeInfo> typeInfoPath, Stack<ISubmoduleOrConnection> elementPath, ParamElementEx paramDeclaration) {
                String paramName = paramDeclaration.getName();

                if (matchesPattern(fullPattern, fullPath == null ? paramName : fullPath + "." + paramName))
                    result[0] = true;
                
                return !result[0];
            }
        });

        return result[0];
    }

    /**
     * Finds the parameter assignment for the provided parameter declaration by walking up on the submodule path.
     * Returns the first non default assignment, or the last default assignment, or null if there is no such assignment.
     * The returned assignment is guaranteed to have a non empty value.
     */
    public static ParamElementEx findParamAssignmentForParamDeclaration(Vector<INEDTypeInfo> typeInfoPath, Vector<ISubmoduleOrConnection> elementPath, ParamElementEx paramDeclaration) {
        ParamElementEx paramAssignment = null;
        String paramName = paramDeclaration.getName();
        String paramRelativePath = paramName;

        // walk up the submodule path starting from the end
        for (int i = elementPath.size() - 1; i >= 0; i--) {
            INEDTypeInfo moduleType = typeInfoPath.get(i);
            ISubmoduleOrConnection element = elementPath.get(i);

            Map<String, ParamElementEx> paramAssignments = null;
            
            if (element == null)
                paramAssignments = moduleType.getParamAssignments();
            else if (element instanceof SubmoduleElementEx)
                paramAssignments = ((SubmoduleElementEx)element).getParamAssignments(moduleType);
            else
                paramAssignments = element.getParamAssignments();

            // handle name patterns
            for (String name : paramAssignments.keySet()) {
                if (matchesPattern(name, paramRelativePath)) {
                    ParamElementEx value = paramAssignments.get(name);
                    
                    if (!StringUtils.isEmpty(value.getValue())) {
                        paramAssignment = value;
                        break;
                    }
                }
            }

            if (paramAssignment != null && !paramAssignment.getIsDefault())
                return paramAssignment;

            // extend paramRelativePath
            String fullName = element == null ? moduleType.getName() : getElementName(element);
            paramRelativePath = fullName + "." + paramRelativePath;
        }
        
        return paramAssignment;
    }

    public static String getElementName(ISubmoduleOrConnection element) {
        if (element instanceof ConnectionElementEx) {
            ConnectionElementEx connection = (ConnectionElementEx)element; 
            String gateName = connection.getGateNameWithIndex(connection.getSrcGate(), connection.getSrcGateSubg(), connection.getSrcGateIndex(), false);
            return gateName + ".channel";
        }
        else {
            SubmoduleElementEx submodule = (SubmoduleElementEx)element;
            String submoduleName = submodule.getName();
    
            if (!StringUtils.isEmpty(submodule.getVectorSize()))
                submoduleName += "[*]";
            
            return submoduleName;
        }
    }

    // TODO: move and factor with InifileUtils
    public static boolean matchesPattern(String pattern, String value) {
        if (PatternMatcher.containsWildcards(pattern)) {
            try {
                // TODO: cache matcher (see InifileUtils and factor out)
                return new PatternMatcher(pattern, true, true, true).matches(value);
            }
            catch (RuntimeException e) {
                return false;
            }
        }
        else
            return pattern.equals(value);
    }

    public static abstract class RecursiveParamDeclarationVisitor implements IModuleTreeVisitor {
        protected Stack<ISubmoduleOrConnection> elementPath = new Stack<ISubmoduleOrConnection>();
        protected Stack<INEDTypeInfo> typeInfoPath = new Stack<INEDTypeInfo>();
        protected Stack<String> fullPathStack = new Stack<String>();  //XXX performance: use cumulative names, so that StringUtils.join() can be eliminated (like: "Net", "Net.node[*]", "Net.node[*].ip" etc) 

        public boolean enter(SubmoduleElementEx submodule, INEDTypeInfo submoduleType) {
            elementPath.push(submodule);
            typeInfoPath.push(submoduleType);
            fullPathStack.push(submodule == null ? submoduleType.getName() : getElementName(submodule));

            // skip if this is a zero-size submodule vector
            boolean isZeroSizedVector = false;

            if (submodule != null) {
                String vectorSize = submodule.getVectorSize();

                if (!StringUtils.isEmpty(vectorSize)) {
                    if (vectorSize.equals("0"))
                        isZeroSizedVector = true;
                    else if (vectorSize.matches("[a-zA-Z_][a-zA-Z0-9_]*")) {  //XXX performance (precompile regex!)
                        //TODO look up param value, and check if it's zero
                    }
                }
            }

            String fullPath = StringUtils.join(fullPathStack, ".");
            return !isZeroSizedVector && visitParamDeclarations(fullPath, typeInfoPath, elementPath);
        }

        public void leave() {
            elementPath.pop();
            typeInfoPath.pop();
            fullPathStack.pop();
        }

        public void unresolvedType(SubmoduleElementEx submodule, String submoduleTypeName) {
        }

        public void recursiveType(SubmoduleElementEx submodule, INEDTypeInfo submoduleType) {
        }

        public String resolveLikeType(SubmoduleElementEx submodule) {
            // TODO: at least look at the NED param assignments
            return null;
        }
        
        protected boolean visitParamDeclarations(String fullPath, Stack<INEDTypeInfo> typeInfoPath, Stack<ISubmoduleOrConnection> elementPath) {
            INEDTypeInfo typeInfo = typeInfoPath.lastElement();

            visitParamDeclarations(fullPath, typeInfoPath, elementPath, typeInfo.getParamDeclarations());

            if (typeInfo.getNEDElement() instanceof CompoundModuleElementEx) {
                CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)typeInfo.getNEDElement();

                for (ConnectionElementEx connection : compoundModule.getSrcConnections())
                    visitParamDeclarations(fullPath + "." + getElementName(connection) , typeInfoPath, elementPath, connection.getParamDeclarations());
                
                for (SubmoduleElementEx submodule : compoundModule.getSubmodules()) {
                    String submoduleName = submodule.getName();
                    String fullPathPrefix = fullPath + "." + submoduleName + ".";

                    for (ConnectionElementEx connection : compoundModule.getSrcConnectionsFor(submoduleName))
                        visitParamDeclarations(fullPathPrefix + getElementName(connection), typeInfoPath, elementPath, connection.getParamDeclarations());
                }
            }

            return true;
        }

        protected boolean visitParamDeclarations(String fullPath, Stack<INEDTypeInfo> typeInfoPath, Stack<ISubmoduleOrConnection> elementPath, Map<String, ParamElementEx> paramDeclarations) {
            for (ParamElementEx paramDeclaration : paramDeclarations.values())
                if (!visitParamDeclaration(fullPath, typeInfoPath, elementPath, paramDeclaration))
                    return false;

            return true;
        }

        protected abstract boolean visitParamDeclaration(String fullPath, Stack<INEDTypeInfo> typeInfoPath, Stack<ISubmoduleOrConnection> elementPath, ParamElementEx paramDeclaration);
    };
}

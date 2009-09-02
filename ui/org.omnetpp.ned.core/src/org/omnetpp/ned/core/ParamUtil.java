package org.omnetpp.ned.core;

import java.util.Map;
import java.util.Stack;
import java.util.Vector;

import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

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
            protected boolean visitParamDeclaration(String fullPath, Stack<INEDTypeInfo> moduleTypePath, Stack<SubmoduleElementEx> submodulePath, ParamElementEx paramDeclaration) {
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
    public static ParamElementEx findParamAssignmentForParamDeclaration(Vector<INEDTypeInfo> moduleTypePath, Vector<SubmoduleElementEx> submodulePath, ParamElementEx paramDeclaration) {
        ParamElementEx paramAssignment = null;
        String paramName = paramDeclaration.getName();
        String paramRelativePath = paramName;

        // walk up the submodule path starting from the end
        for (int i = submodulePath.size() - 1; i >= 0 && paramAssignment == null; i--) {
            INEDTypeInfo moduleType = moduleTypePath.get(i);
            SubmoduleElementEx submodule = submodulePath.get(i);

            Map<String, ParamElementEx> paramAssignments = submodule == null ?
                moduleType.getParamAssignments() : submodule.getParamAssignments(moduleType);

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
            String fullName = submodule == null ? moduleType.getName() : getSubmoduleFullName(submodule);
            paramRelativePath = fullName + "." + paramRelativePath;
        }
        
        return paramAssignment;
    }

    // TODO: move and factor with InifileUtils
    public static String getSubmoduleFullName(SubmoduleElement submodule) {
        String submoduleName = submodule.getName();

        if (!StringUtils.isEmpty(submodule.getVectorSize()))
            submoduleName += "[*]";
        
        return submoduleName;
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
        protected Stack<SubmoduleElementEx> submodulePath = new Stack<SubmoduleElementEx>();
        protected Stack<INEDTypeInfo> moduleTypePath = new Stack<INEDTypeInfo>();
        protected Stack<String> fullPathStack = new Stack<String>();  //XXX performance: use cumulative names, so that StringUtils.join() can be eliminated (like: "Net", "Net.node[*]", "Net.node[*].ip" etc) 

        public boolean enter(SubmoduleElementEx submodule, INEDTypeInfo submoduleType) {
            submodulePath.push(submodule);
            moduleTypePath.push(submoduleType);
            fullPathStack.push(submodule == null ? submoduleType.getName() : getSubmoduleFullName(submodule));

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
            return !isZeroSizedVector && visitParamDeclarations(fullPath, moduleTypePath, submodulePath);
        }

        public void leave() {
            submodulePath.pop();
            moduleTypePath.pop();
            fullPathStack.pop();
        }

        public void unresolvedType(SubmoduleElementEx submodule, String submoduleTypeName) {
        }

        public void recursiveType(SubmoduleElementEx submodule, INEDTypeInfo submoduleType) {
        }

        public String resolveLikeType(SubmoduleElementEx submodule) {
            return null;
        }
        
        protected boolean visitParamDeclarations(String fullPath, Stack<INEDTypeInfo> moduleTypePath, Stack<SubmoduleElementEx> submodulePath) {
            for (ParamElementEx paramDeclaration : moduleTypePath.lastElement().getParamDeclarations().values())
                if (!visitParamDeclaration(fullPath, moduleTypePath, submodulePath, paramDeclaration))
                    return false;
            
            return true;
        }

        protected abstract boolean visitParamDeclaration(String fullPath, Stack<INEDTypeInfo> moduleTypePath, Stack<SubmoduleElementEx> submodulePath, ParamElementEx paramDeclaration);
    };
}

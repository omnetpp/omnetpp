package org.omnetpp.ned.core;

import java.util.Stack;

import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;

public class ParamUtil {
    public static void mapParamDeclarationsRecursively(INEDTypeInfo typeInfo, RecursiveParamDeclarationVisitor visitor) {
        NEDTreeTraversal treeTraversal = new NEDTreeTraversal(NEDResourcesPlugin.getNEDResources(), visitor);
        treeTraversal.traverse(typeInfo);
    }

    public static void mapParamDeclarationsRecursively(SubmoduleElementEx submodule, RecursiveParamDeclarationVisitor visitor) {
        NEDTreeTraversal treeTraversal = new NEDTreeTraversal(NEDResourcesPlugin.getNEDResources(), visitor);
        treeTraversal.traverse(submodule);
    }
    
    public static boolean hasMatchingParamDeclarationRecursively(INEDTypeInfo typeInfo, final String paramNamePattern) {
        final boolean[] result = new boolean[] {false};
        mapParamDeclarationsRecursively(typeInfo, new RecursiveParamDeclarationVisitor() {
            @Override
            protected boolean visitParamDeclaration(String fullPath, Stack<INEDTypeInfo> moduleTypePath, Stack<SubmoduleElementEx> submodulePath, ParamElementEx paramDeclaration) {
                String name = paramDeclaration.getName();

                if (matchesPattern(paramNamePattern, fullPath == null ? name : fullPath + "." + name))
                    result[0] = true;
                
                return !result[0];
            }
        });
        
        return result[0];
    }

    // TODO: move to a utility class
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

            if (submodule != null) {
                String submoduleName = submodule.getName();

                if (!StringUtils.isEmpty(submodule.getVectorSize()))
                    submoduleName += "[*]";
                
                fullPathStack.push(submoduleName);
            }

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

            String fullPath = fullPathStack.size() == 0 ? null : StringUtils.join(fullPathStack, ".");
            return !isZeroSizedVector && visitParamDeclarations(fullPath, moduleTypePath, submodulePath);
        }

        public void leave() {
            submodulePath.pop();
            moduleTypePath.pop();
            
            if (fullPathStack.size() != 0)
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

package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Stack;
import java.util.Vector;

import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;

public class ParamUtil {
    /**
     * Stores a cached pattern matcher created from an inifile key; used during inifile analysis
     */
    public static class KeyMatcher {
        public String key;
        public String generalizedKey; // key, with indices ("[0]", "[5..]" etc) replaced with "[*]" 
        public boolean keyEqualsGeneralizedKey;  // if key.equals(generalizedKey) 
        public PatternMatcher matcher;  // pattern is generalizedKey
    }
    private static Map<String,KeyMatcher> keyMatcherCache = new HashMap<String, KeyMatcher>();

    public static KeyMatcher getOrCreateKeyMatcher(String key) {
        KeyMatcher keyMatcher = keyMatcherCache.get(key);
        if (keyMatcher == null) {
            keyMatcher = new KeyMatcher();
            keyMatcher.key = key;
            keyMatcher.generalizedKey = key.replaceAll("\\[[^]]+\\]", "[*]");  // replace "[anything]" with "[*]"
            keyMatcher.keyEqualsGeneralizedKey = key.equals(keyMatcher.generalizedKey);
            try {
                keyMatcher.matcher = new PatternMatcher(keyMatcher.generalizedKey, true, true, true); 
            } catch (RuntimeException e) {
                // bogus key (contains unmatched "{" or something); create matcher that does not match anything
                keyMatcher.matcher = new PatternMatcher("", true, true, true); 
            }
            keyMatcherCache.put(key, keyMatcher);
        }
        return keyMatcher;
    }
    
    public static boolean matchesPattern(String pattern, String value) {
        if (PatternMatcher.containsWildcards(pattern) || pattern.indexOf('[') != -1)
            return getOrCreateKeyMatcher(pattern).matcher.matches(value);
        else
            return pattern.equals(value);
    }

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
     * Finds the parameter assignments for the provided parameter declaration by walking up on the element path.
     * 
     * For single parameters this function returns a list with either the first non default assignment, or the last default assignment,
     * or the declaration if there is no such assignment.
     * 
     * For parameters under a submodule vector it returns the list of all matching assignments without considering
     * the index arguments. The only exception is when a total parameter assignment is used that overrides all previous assignments.
     * 
     * The list of returned assignments is guaranteed to have a non empty value.
     */
    public static ArrayList<ParamElementEx> findParamAssignmentsForParamDeclaration(Vector<INEDTypeInfo> typeInfoPath, Vector<ISubmoduleOrConnection> elementPath, ParamElementEx paramDeclaration) {
        ArrayList<ParamElementEx> foundParamAssignments = new ArrayList<ParamElementEx>();
        String paramName = paramDeclaration.getName();
        String paramRelativePath = paramName;

	    // always add the parameter declaration, it will either be overwritten with a total assignment or extended with partial assignments 
        if (collectParamAssignments(foundParamAssignments, paramDeclaration, true))
        	return foundParamAssignments;

        // walk up the submodule path starting from the end (i.e. from the deepest submodule)
        for (int i = elementPath.size() - 1; i >= 0; i--) {
            INEDTypeInfo typeInfo = typeInfoPath.get(i);
            ISubmoduleOrConnection element = elementPath.get(i);

            Map<String, ParamElementEx> paramAssignments = null;
            
            if (element == null)
                paramAssignments = typeInfo.getParamAssignments();
            else if (element instanceof SubmoduleElementEx)
                paramAssignments = ((SubmoduleElementEx)element).getParamAssignments(typeInfo);
            else if (element instanceof ConnectionElementEx)
                paramAssignments = ((ConnectionElementEx)element).getParamAssignments(typeInfo);
            else
                paramAssignments = element.getParamAssignments();

            // handle name patterns
            for (String name : paramAssignments.keySet()) {
                if (matchesPattern(name, paramRelativePath)) {
                    ParamElementEx paramAssignment = paramAssignments.get(name);

                    if (collectParamAssignments(foundParamAssignments, paramAssignment, false))
                    	return foundParamAssignments;
                }
            }

            // extend paramRelativePath
            String fullName = element == null ? typeInfo.getName() : getParamPathPart(element);
            paramRelativePath = fullName + "." + paramRelativePath;
        }

        return foundParamAssignments;
    }

	private static boolean collectParamAssignments(ArrayList<ParamElementEx> foundParamAssignments, ParamElementEx paramAssignment, boolean addEmptyValue) {
	    boolean isEmpty = StringUtils.isEmpty(paramAssignment.getValue());

	    if (addEmptyValue || !isEmpty) {
    		if (!isTotalParamAssignment(paramAssignment))
    			foundParamAssignments.add(0, paramAssignment);
    		else {
    	    	if (foundParamAssignments.size() != 0)
    	    		foundParamAssignments.clear();
    	    	
    	    	foundParamAssignments.add(paramAssignment);
    
    	        if (!paramAssignment.getIsDefault() && !isEmpty)
    	            return true;
    		}
	    }

		return false;
	}

    public static String getParamPathPart(ISubmoduleOrConnection element) {
        if (element instanceof ConnectionElementEx)
            return element.getName();
        else {
            SubmoduleElementEx submodule = (SubmoduleElementEx)element;
            String submoduleName = submodule.getName();
    
            if (!StringUtils.isEmpty(submodule.getVectorSize()))
                submoduleName += "[*]";
            
            return submoduleName;
        }
    }

    public static boolean isTotalParamAssignment(ParamElementEx paramAssignment) {
        return !paramAssignment.getIsPattern() || isTotalParamAssignment(paramAssignment.getName());
    }

    public static boolean isTotalParamAssignment(String pattern) {
		for (int i = 0; i < pattern.length(); i++) {
			char ch = pattern.charAt(i);

			if (ch == '[' && i + 2 < pattern.length() && (pattern.charAt(i + 1) != '*' || pattern.charAt(i + 2) != ']'))
				return false;
		}

		return true;
    }

    public static abstract class RecursiveParamDeclarationVisitor implements IModuleTreeVisitor {
        protected Stack<ISubmoduleOrConnection> elementPath = new Stack<ISubmoduleOrConnection>();
        protected Stack<INEDTypeInfo> typeInfoPath = new Stack<INEDTypeInfo>();
        protected Stack<String> fullPathStack = new Stack<String>();  //XXX performance: use cumulative names, so that StringUtils.join() can be eliminated (like: "Net", "Net.node[*]", "Net.node[*].ip" etc) 

        public boolean enter(ISubmoduleOrConnection element, INEDTypeInfo typeInfo) {
            elementPath.push(element);
            typeInfoPath.push(typeInfo);
            fullPathStack.push(element == null ? typeInfo.getName() : getParamPathPart(element));

            // skip if this is a zero-size submodule vector
            boolean isZeroSizedVector = false;

            if (element != null && element instanceof SubmoduleElementEx) {
                String vectorSize = ((SubmoduleElementEx)element).getVectorSize();

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

        public void unresolvedType(ISubmoduleOrConnection element, String typeName) {
        }

        public void recursiveType(ISubmoduleOrConnection element, INEDTypeInfo typeInfo) {
        }

        public String resolveLikeType(ISubmoduleOrConnection element) {
            // TODO: at least look at the NED param assignments
            return null;
        }
        
        protected boolean visitParamDeclarations(String fullPath, Stack<INEDTypeInfo> typeInfoPath, Stack<ISubmoduleOrConnection> elementPath) {
            INEDTypeInfo typeInfo = typeInfoPath.lastElement();
            return visitParamDeclarations(fullPath, typeInfoPath, elementPath, typeInfo.getParamDeclarations());
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

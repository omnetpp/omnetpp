package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.Stack;
import java.util.Vector;

import org.eclipse.core.resources.IProject;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.CollectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
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
    public static void mapParamDeclarationsRecursively(INedTypeInfo typeInfo, RecursiveParamDeclarationVisitor visitor, IProject contextProject) {
        NedTreeTraversal treeTraversal = new NedTreeTraversal(typeInfo.getResolver(), visitor, contextProject);
        treeTraversal.traverse(typeInfo);
    }

    /**
     * Calls visitor for each parameter declaration accessible under the provided submodule.
     * Recurses down on submodules.
     */
    public static void mapParamDeclarationsRecursively(SubmoduleElementEx submodule, RecursiveParamDeclarationVisitor visitor, IProject contextProject) {
        NedTreeTraversal treeTraversal = new NedTreeTraversal(submodule.getResolver(), visitor, contextProject);
        treeTraversal.traverse(submodule);
    }

    /**
     * Returns the parameter declaration under type info with the path that matches the provided name pattern.
     * Recurses down on submodules.
     * The contextProject parameter affects the resolution of parametric submodule types ("like").
     */
    public static Object[] findMatchingParamDeclarationRecursively(INedTypeInfo typeInfo, String paramNamePattern, IProject contextProject) {
        String fullPattern = typeInfo.getName() + "." + paramNamePattern;
        Object[] result = new Object[] {null, null, null};
        mapParamDeclarationsRecursively(typeInfo, createFindMatchingParamDeclarationVisitor(fullPattern, result), contextProject);

        return result;
    }

    /**
     * Returns the parameter declaration under type info with the path that matches the provided name pattern.
     * Recurses down on submodules.
     * The contextProject parameter affects the resolution of parametric submodule types ("like").
     */
    public static Object[] findMatchingParamDeclarationRecursively(SubmoduleElementEx submodule, String paramNamePattern, IProject contextProject) {
        String fullPattern = getParamPathElementName(submodule) + "." + paramNamePattern;
        Object[] result = new Object[] {null, null, null};
        mapParamDeclarationsRecursively(submodule, createFindMatchingParamDeclarationVisitor(fullPattern, result), contextProject);

        return result;
    }

    private static RecursiveParamDeclarationVisitor createFindMatchingParamDeclarationVisitor(final String fullPattern, final Object[] result) {
        return new RecursiveParamDeclarationVisitor() {
            @Override
            protected boolean visitParamDeclaration(String fullPath, Stack<INedTypeInfo> typeInfoPath, Stack<ISubmoduleOrConnection> elementPath, ParamElementEx paramDeclaration) {
                String paramName = paramDeclaration.getName();

                if (matchesPattern(fullPattern, fullPath == null ? paramName : fullPath + "." + paramName)) {
                    result[0] = paramDeclaration;
                    result[1] = new Vector<INedTypeInfo>(typeInfoPath);
                    result[2] = new Vector<ISubmoduleOrConnection>(elementPath);

                    return false;
                }
                else
                    return true;
            }
        };
    }

    /**
     * Finds the parameter assignments for the provided parameter declaration by walking up on the element path.
     *
     * For single parameters this function returns a list with either the first non default assignment, or the last default assignment,
     * or the declaration if there is no such assignment.
     *
     * For parameters under a submodule vector path it returns the list of all matching assignments without considering
     * the index arguments. The only exception is when a total parameter assignment is used that overrides all previous assignments.
     *
     * The list of returned assignments is guaranteed to be non empty. The order of assignments reflects the way they are applied when
     * the runtime will determine the parameter value. That is, the first matching assignment (in terms of indices) will determine the
     * value. This is achieved by sorting out the non default assignments to the beginning, while leaving the defaults at the end in
     * reverse order (to account for being default values).
     */
    public static ArrayList<ParamElementEx> findParamAssignmentsForParamDeclaration(Vector<INedTypeInfo> typeInfoPath, Vector<ISubmoduleOrConnection> elementPath, final ParamElementEx paramDeclaration) {
        ArrayList<ParamElementEx> foundParamAssignments = new ArrayList<ParamElementEx>();
        String paramName = paramDeclaration.getName();
        String paramRelativePath = paramName;

        // always add the parameter declaration, it will either be overwritten with a total assignment or extended with partial assignments
        if (!collectParamAssignments(foundParamAssignments, paramDeclaration, true)) {
            // walk up the submodule path starting from the end (i.e. from the deepest submodule)
            outer: for (int i = elementPath.size() - 1; i >= 0; i--) {
                INedTypeInfo typeInfo = typeInfoPath.get(i);
                ISubmoduleOrConnection element = elementPath.get(i);

                Map<String, ParamElementEx> paramAssignments = null;

                if (typeInfo == null)
                    paramAssignments = new HashMap<>();
                else if (element == null)
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
                            break outer;
                    }
                }

                // extend paramRelativePath
                String fullName = element == null ? typeInfo.getName() : getParamPathElementName(element);
                paramRelativePath = fullName + "." + paramRelativePath;
            }
        }

        return (ArrayList<ParamElementEx>)CollectionUtils.toSorted(CollectionUtils.toReversed(foundParamAssignments), new Comparator<ParamElementEx>() {
            public int compare(ParamElementEx p1, ParamElementEx p2) {
                if (p1 == p2)
                    return 0;
                else if (p1 == paramDeclaration)
                    return 1;
                else if (p2 == paramDeclaration)
                    return -1;
                else if (p1.getIsDefault() == p2.getIsDefault())
                    return 0;
                else if (p1.getIsDefault())
                    return 1;
                else
                    return -1;
            }
        });
    }

    private static boolean collectParamAssignments(ArrayList<ParamElementEx> foundParamAssignments, ParamElementEx paramAssignment, boolean addEmptyValue) {
        boolean isEmpty = StringUtils.isEmpty(paramAssignment.getValue());

        if (addEmptyValue || !isEmpty) {
            if (!isTotalParamAssignment(paramAssignment))
                foundParamAssignments.add(paramAssignment);
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

    public static String getParamPathElementName(ISubmoduleOrConnection element) {
        return getParamPathElementName(element, true);
    }

    public static String getParamPathElementName(ISubmoduleOrConnection element, boolean useWildcard) {
        if (element instanceof ConnectionElementEx) {
            ConnectionElementEx connection = (ConnectionElementEx)element;
            INedElement connectedElement = connection.getSrcModuleRef();
            String gateIndex = "";

            if (StringUtils.isNotEmpty(connection.getSrcGateIndex()))
                gateIndex = useWildcard ? "*" : connection.getSrcGateIndex();
            else if (connection.getSrcGatePlusplus())
                gateIndex = "*";

            String gateName = ConnectionElementEx.getGateNameWithIndex(connection.getSrcGate(), connection.getSrcGateSubg(), gateIndex, false);
            String moduleName = connection.getSrcModule();
            String moduleIndex = "";

            if (connectedElement instanceof SubmoduleElementEx) {
                SubmoduleElementEx submoduleElement = (SubmoduleElementEx)connectedElement;
                moduleIndex = StringUtils.isNotEmpty(submoduleElement.getVectorSize()) ? useWildcard ? "[*]" : "[" + submoduleElement.getVectorSize() + "]" : "";
            }

            return (StringUtils.isEmpty(moduleName) ? "" : moduleName + moduleIndex + ".") + gateName + ".channel";
        }
        else {
            SubmoduleElementEx submodule = (SubmoduleElementEx)element;
            String submoduleName = submodule.getName();

            if (StringUtils.isNotEmpty(submodule.getVectorSize()))
                submoduleName += useWildcard ? "[*]" : "[" + submodule.getVectorSize() + "]";

            return submoduleName;
        }
    }

    public static boolean isTotalParamAssignment(ParamElementEx paramAssignment) {
        return !paramAssignment.getIsPattern() || isTotalParamAssignment(paramAssignment.getName());
    }

    /**
     * Expects a module path pattern, and returns true if it matches all module indices
     * at every level, i.e. all bracketed expressions in it are "[*]".
     */
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
        protected Stack<INedTypeInfo> typeInfoPath = new Stack<INedTypeInfo>();
        protected Stack<String> fullPathStack = new Stack<String>();  //XXX performance: use cumulative names, so that StringUtils.join() can be eliminated (like: "Net", "Net.node[*]", "Net.node[*].ip" etc)

        public boolean enter(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
            elementPath.push(element);
            typeInfoPath.push(typeInfo);
            fullPathStack.push(element == null ? typeInfo.getName() : getParamPathElementName(element));
            String fullPath = StringUtils.join(fullPathStack, ".");
            return visitParamDeclarations(fullPath, typeInfoPath, elementPath);
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
            // note: we can only make a local decision here; we also ignore getIsDefault()
            String expr = element.getLikeExpr();
            if (expr.length() > 0 && expr.charAt(0)=='"') {
                try {
                    return Common.parseQuotedString(expr);
                }
                catch (RuntimeException e) {
                    return null;
                }
            }
            return null;
        }

        protected boolean visitParamDeclarations(String fullPath, Stack<INedTypeInfo> typeInfoPath, Stack<ISubmoduleOrConnection> elementPath) {
            INedTypeInfo typeInfo = typeInfoPath.lastElement();
            return visitParamDeclarations(fullPath, typeInfoPath, elementPath, typeInfo.getParamDeclarations());
        }

        protected boolean visitParamDeclarations(String fullPath, Stack<INedTypeInfo> typeInfoPath, Stack<ISubmoduleOrConnection> elementPath, Map<String, ParamElementEx> paramDeclarations) {
            for (ParamElementEx paramDeclaration : paramDeclarations.values())
                if (!visitParamDeclaration(fullPath, typeInfoPath, elementPath, paramDeclaration))
                    return false;

            return true;
        }

        protected abstract boolean visitParamDeclaration(String fullPath, Stack<INedTypeInfo> typeInfoPath, Stack<ISubmoduleOrConnection> elementPath, ParamElementEx paramDeclaration);
    };
}

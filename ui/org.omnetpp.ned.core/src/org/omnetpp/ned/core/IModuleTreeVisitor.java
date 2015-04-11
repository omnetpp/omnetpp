/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import java.util.Stack;

import org.omnetpp.common.Debug;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;

/**
 * Visitor interface for use with NedTreeTraversal.
 *
 * @author Andras
 */
public interface IModuleTreeVisitor {
    /**
     * Enter a submodule/connection or start recursion.
     * Should return true to recurse down.
     *
     * @param submodule or connection     null at the root only
     * @param typeInfo                    type of the element or root type, never null
     * @return true                       recurse
     */
    boolean enter(ISubmoduleOrConnection element, INedTypeInfo typeInfo);

    /**
     * Leave the module last entered.
     */
    void leave();

    /**
     * Encountered a submodule/connection whose type is unresolved. No leave() will follow.
     */
    void unresolvedType(ISubmoduleOrConnection element, String typeName);

    /**
     * Encountered a submodule/connection which introduces type recursion, and traversal is
     * pruned here. No leave() will follow.
     */
    void recursiveType(ISubmoduleOrConnection element, INedTypeInfo typeInfo);

    /**
     * Resolve the "like" parameter of a submodule/connection. The method should return the
     * actual (resolved) module type, or null if it cannot (or does not wish to)
     * resolve it.
     */
    // TODO: this should probably return a string array, because if the submodule is a vector, different indices may have different NED types
    String resolveLikeType(ISubmoduleOrConnection element);

    /**
     * Example visitor 1: builds a tree
     */
    public static class TreeBuilder implements IModuleTreeVisitor {
        private GenericTreeNode root = new GenericTreeNode("root");
        private GenericTreeNode current = root;

        public boolean enter(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
            GenericTreeNode child = new GenericTreeNode("("+typeInfo.getName()+")"+(element==null ? "" : ParamUtil.getParamPathElementName(element)));
            current.addChild(child);
            current = child;
            return true;
        }

        public void leave() {
            current = current.getParent();
        }

        public void unresolvedType(ISubmoduleOrConnection element, String typeName) {
        }

        public void recursiveType(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
        }

        public String resolveLikeType(ISubmoduleOrConnection element) {
            return null;
        }

        public GenericTreeNode getResult() {
            return root;
        }
    }

    /**
     * Example visitor 2: prints module full paths
     */
    public static class FullPathBuilder implements IModuleTreeVisitor {
        Stack<String> fullPath = new Stack<String>();

        public boolean enter(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
            //fullPath.push("("+typeInfo.getName()+")"+(element==null ? "" : element.getName()));
            fullPath.push(element==null ? typeInfo.getName() : ParamUtil.getParamPathElementName(element));
            Debug.println(StringUtils.join(fullPath.toArray(), "."));
            return true;
        }

        public void leave() {
            fullPath.pop();
        }

        public void unresolvedType(ISubmoduleOrConnection element, String typeName) {
        }

        public void recursiveType(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
        }

        public String resolveLikeType(ISubmoduleOrConnection element) {
            return null;
        }
    }
}
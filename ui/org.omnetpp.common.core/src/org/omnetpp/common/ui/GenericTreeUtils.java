/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.omnetpp.common.util.IPredicate;


/**
 * GenericTreeNode utility functions.
 * @author andras
 */
public class GenericTreeUtils {

    /**
     * Tree comparison. Payload objects are compared with equals().
     *
     * Comparison is useful if a tree is displayed in a view, and we want
     * to avoid replacing tree input with an identical one (which would
     * cause the tree widget to lose its state, like node open/close states).
     */
    public static boolean treeEquals(GenericTreeNode root1, GenericTreeNode root2) {
        return treeEquals(root1, root2, false);
    }

    /**
     * Tree comparison. Payload objects are compared with equals().
     *
     * When {@code lazy} is true, then the method does not expands lazy nodes,
     * but assumes it contains the same children as the corresponding node in
     * the other tree.
     */
    public static boolean treeEquals(GenericTreeNode root1, GenericTreeNode root2, boolean lazy)
    {
        if (root1 == root2)
            return true;
        if (root1 == null || root2 == null)
            return false;
        return internalTreeEquals(root1, root2, lazy);
    }

    private static boolean internalTreeEquals(GenericTreeNode node1, GenericTreeNode node2, boolean lazy) {
        if (!node1.getPayload().equals(node2.getPayload()))
            return false;
        GenericTreeNode[] children1 = lazy ? node1.internalGetChildren() : node1.getChildren();
        GenericTreeNode[] children2 = lazy ? node2.internalGetChildren() : node2.getChildren();
        if (children1 == null || children2 == null)
            return true;
        if (children1.length != children2.length)
            return false;
        for (int i=0; i<children1.length; i++)
            if (!internalTreeEquals(children1[i], children2[i], lazy))
                return false;
        return true;
    }

    public static Object findFirstMatchingNode(GenericTreeNode root, IPredicate payloadMatcher) {
        return root==null ? null : internalFindFirstMatchingNode(root, payloadMatcher);
    }

    private static Object internalFindFirstMatchingNode(GenericTreeNode node, IPredicate payloadMatcher) {
        if (payloadMatcher.matches(node.getPayload()))
            return node.getPayload();
        for (GenericTreeNode child : node.getChildren()) {
            Object foundObject = internalFindFirstMatchingNode(child, payloadMatcher);
            if (foundObject!=null)
                return foundObject;
        }
        return null;
    }

}

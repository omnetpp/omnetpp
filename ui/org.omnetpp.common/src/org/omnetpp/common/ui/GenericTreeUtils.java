package org.omnetpp.common.ui;


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
		if (root1 == root2)
			return true;
		if (root1 == null || root2 == null)
			return false;
		return internalTreeEquals(root1, root2);
	}
	
	private static boolean internalTreeEquals(GenericTreeNode node1, GenericTreeNode node2) {
		if (!node1.getPayload().equals(node2.getPayload()))
			return false;
		GenericTreeNode[] children1 = node1.getChildren();
		GenericTreeNode[] children2 = node2.getChildren();
		if (children1.length != children2.length)
			return false;
		for (int i=0; i<children1.length; i++) 
			if (!internalTreeEquals(children1[i], children2[i]))
				return false;
		return true;		
	}
}

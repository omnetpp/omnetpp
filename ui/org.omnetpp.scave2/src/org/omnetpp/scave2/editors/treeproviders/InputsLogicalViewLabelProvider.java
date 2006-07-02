package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.jface.viewers.LabelProvider;

/**
 * Label provider for the "Logical view" tree of the Inputs page.
 */
public class InputsLogicalViewLabelProvider extends LabelProvider {
	
	public String getText(Object element) {
		if (element instanceof GenericTreeNode) {
			GenericTreeNode node = (GenericTreeNode)element;
			if (node.getPayload()!=null && !node.getPayload().equals(""))
				return node.getPayload().toString();
		}
		return null;
	}
};

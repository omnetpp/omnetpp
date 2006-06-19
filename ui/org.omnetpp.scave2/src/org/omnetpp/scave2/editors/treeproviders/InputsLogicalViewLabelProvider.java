package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.jface.viewers.LabelProvider;

public class InputsLogicalViewLabelProvider extends LabelProvider {
	
	public String getText(Object element) {
		if (element instanceof GenericTreeNode) {
			GenericTreeNode node = (GenericTreeNode)element;
			if (node.getPayload() != null && !node.getPayload().equals(""))
				return node.getPayload().toString();
			// test code
			if (node.getParent() == null)
				return null;
			else if (node.getParent().getParent() == null)
				return "experiment-" + node.indexInParent();
			else if (node.getParent().getParent().getParent() == null)
				return "measurement-" + node.indexInParent();
			else if (node.getParent().getParent().getParent().getParent() == null)
				return "replication-" + node.indexInParent();
		}
		return null;
	}
};

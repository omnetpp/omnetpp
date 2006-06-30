package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.viewers.LabelProvider;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Label provider for the "Physical view" tree of the Inputs page.
 */
public class InputsPhysicalViewLabelProvider extends LabelProvider {

	public String getText(Object element) {
		if (element instanceof GenericTreeNode) {
			GenericTreeNode node = (GenericTreeNode)element;
			if (node.getPayload() instanceof Inputs)
				return "";
			else if (node.getPayload() instanceof ResultFile) {
				ResultFile file = (ResultFile)node.getPayload();
				IFile ifile = ScaveEditor.findFileInWorkspace(file.getFilePath());
				return ifile != null ? ifile.getFullPath().toString() : file.getFilePath();
			}
			else if (node.getPayload() instanceof Run) {
				Run run = (Run)node.getPayload();
				return run.getRunName() + ", " + run.getDate();
			}
		}
		return null;
	}
}

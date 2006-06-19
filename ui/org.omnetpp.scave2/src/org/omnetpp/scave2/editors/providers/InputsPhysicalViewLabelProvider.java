package org.omnetpp.scave2.editors.providers;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.viewers.LabelProvider;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

public class InputsPhysicalViewLabelProvider extends LabelProvider {

	public String getText(Object element) {
		if (element instanceof GenericTreeNode) {
			GenericTreeNode node = (GenericTreeNode)element;
			if (node.getPayload() instanceof Inputs)
				return "";
			else if (node.getPayload() instanceof File) {
				File file = (File)node.getPayload();
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

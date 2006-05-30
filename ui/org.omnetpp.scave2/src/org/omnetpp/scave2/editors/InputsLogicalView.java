package org.omnetpp.scave2.editors;

import java.util.List;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;

public class InputsLogicalView extends InputsView {
	
	public InputsLogicalView(ScaveEditor editor) {
		super(editor);
	}
	
	public ITreeContentProvider getContentProvider() {
		return new ContentProvider() {
			// Inputs/Experiment/Measurement/Replication
			protected void buildTree(Inputs inputs) {
				root = new TreeNode(null, inputs);
				
				for (Object element : inputs.getInputs()) {
					InputFile inputFile = (InputFile)element;
					List<File> files = loadFiles(inputFile.getName());
					for (File file : files) {
						RunList runlist = manager.getRunsInFile(file);
						for (int i = 0; i < runlist.size(); ++i) {
							Run run = runlist.get(i);
							TreeNode experimentNode = getOrCreateNode(root, run.getExperimentName());
							TreeNode measurementNode = getOrCreateNode(experimentNode, run.getMeasurementName());
							getOrCreateNode(measurementNode, run.getReplicationName());
						}
					}
				}
			}
		};
	}
	
	public ILabelProvider getLabelProvider() {
		return new LabelProvider() {
			
			public String getText(Object element) {
				if (element instanceof TreeNode) {
					TreeNode node = (TreeNode)element;
					if (node.payload != null && !node.payload.equals(""))
						return node.payload.toString();
					// test code
					if (node.parent == null)
						return null;
					else if (node.parent.parent == null)
						return "experiment-" + node.index();
					else if (node.parent.parent.parent == null)
						return "measurement-" + node.index();
					else if (node.parent.parent.parent.parent == null)
						return "replication-" + node.index();
				}
				return null;
			}
		};
	}
}

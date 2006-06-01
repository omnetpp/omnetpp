package org.omnetpp.scave2.editors.providers;

import java.util.List;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

public class InputsLogicalViewProvider extends InputsTreeViewProvider {
	
	public InputsLogicalViewProvider(ScaveEditor editor) {
		super(editor);
	}
	
	public ITreeContentProvider getContentProvider() {
		return new ContentProvider() {
			// Inputs/Experiment/Measurement/Replication
			protected void buildTree(Inputs inputs) {
				ScaveEditor editor = getEditor();
				ResultFileManager manager = editor.getResultFileManager();
				root = new TreeNode(null, inputs);
				for (File file : editor.getInputFiles()) {
					RunList runlist = manager.getRunsInFile(file);
					for (int j = 0; j < runlist.size(); ++j) {
						Run run = runlist.get(j);
						TreeNode experimentNode = getOrCreateNode(root, run.getExperimentName());
						TreeNode measurementNode = getOrCreateNode(experimentNode, run.getMeasurementName());
						getOrCreateNode(measurementNode, run.getReplicationName());
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

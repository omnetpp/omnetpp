package org.omnetpp.scave2.editors.providers;

import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * This class configures the viewer of the logical input tree.
 *  
 * @author Tomi
 */
public class InputsLogicalViewProvider extends InputsTreeViewProvider {
	
	public InputsLogicalViewProvider(ScaveEditor editor) {
		super(editor);
	}
	
	public ITreeContentProvider getContentProvider() {
		return new ContentProvider() {
			// Inputs/Experiment/Measurement/Replication
			protected TreeNode buildTree(Inputs inputs) {
				ResultFileManager manager = editor.getResultFileManager();
				TreeNode root = new TreeNode(null, inputs);
				for (File file : editor.getInputFiles()) {
					RunList runlist = manager.getRunsInFile(file);
					for (int j = 0; j < runlist.size(); ++j) {
						Run run = runlist.get(j);
						TreeNode experimentNode = getOrCreateNode(root, run.getExperimentName());
						TreeNode measurementNode = getOrCreateNode(experimentNode, run.getMeasurementName());
						getOrCreateNode(measurementNode, run.getReplicationName());
					}
				}
				return root;
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

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
			protected GenericTreeNode buildTree(Inputs inputs) {
				ResultFileManager manager = editor.getResultFileManager();
				GenericTreeNode root = new GenericTreeNode(null, inputs);
				for (File file : editor.getInputFiles()) {
					RunList runlist = manager.getRunsInFile(file);
					for (int j = 0; j < runlist.size(); ++j) {
						Run run = runlist.get(j);
						GenericTreeNode experimentNode = root.getOrCreateChild(run.getExperimentName());
						GenericTreeNode measurementNode = experimentNode.getOrCreateChild(run.getMeasurementName());
						measurementNode.getOrCreateChild(run.getReplicationName());
					}
				}
				return root;
			}
		};
	}
	
	public ILabelProvider getLabelProvider() {
		return new LabelProvider() {
			
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
	}
}

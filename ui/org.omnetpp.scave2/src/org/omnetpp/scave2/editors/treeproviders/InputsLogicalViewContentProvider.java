package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Content provider for the "Logical view" tree of the Inputs page.
 */
//FIXME tree has to be rebuilt upon resource changes as well! ResultFileTracker should fire some notification
public class InputsLogicalViewContentProvider extends CachedTreeContentProvider {
	private ScaveEditor editor;
	
	public InputsLogicalViewContentProvider(ScaveEditor editor) {
		super((IChangeNotifier)editor.getAdapterFactory());
		this.editor = editor;
	}

	// Inputs/Experiment/Measurement/Replication
	protected GenericTreeNode buildTree(Object element) {
		Inputs inputs = (Inputs)element;
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
}

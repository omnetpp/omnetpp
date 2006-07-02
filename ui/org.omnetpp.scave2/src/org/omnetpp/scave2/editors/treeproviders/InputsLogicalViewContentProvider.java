package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
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
		ResultFileManagerEx manager = editor.getResultFileManager();
		GenericTreeNode root = new GenericTreeNode(null, inputs);
		for (ResultFile file : manager.getFiles().toArray()) {
			RunList runlist = manager.getRunsInFile(file);
			for (int j = 0; j < runlist.size(); ++j) {
				Run run = runlist.get(j);
				GenericTreeNode experimentNode = root.getOrCreateChild("experiment \""+fallback(run.getAttribute("experiment"), "n/a")+"\""); //XXX make constant
				GenericTreeNode measurementNode = experimentNode.getOrCreateChild("measurement \""+fallback(run.getAttribute("measurement"), "n/a")+"\""); //XXX make constant
				measurementNode.getOrCreateChild("replication \""+fallback(run.getAttribute("replication"), "n/a")+"\""); //XXX make constant
			}
		}
		return root;
	}
	
	static private String fallback(String string, String fallbackValue) {
		return string!=null ? string : fallbackValue;
	}
}

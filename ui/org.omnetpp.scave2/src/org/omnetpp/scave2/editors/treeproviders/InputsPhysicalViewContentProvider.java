package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Content provider for the "Physical view" tree of the Inputs page.
 */
//FIXME tree has to be rebuilt upon resource changes as well! ResultFileTracker should fire some notification
public class InputsPhysicalViewContentProvider extends CachedTreeContentProvider  {
	private ScaveEditor editor;
	
	public InputsPhysicalViewContentProvider(ScaveEditor editor) {
		super((IChangeNotifier)editor.getAdapterFactory());
		this.editor = editor;
	}

	protected GenericTreeNode buildTree(Object element) {
		// Inputs/File/Run
		Inputs inputs = (Inputs)element;
		ResultFileManagerEx manager = editor.getResultFileManager();
		GenericTreeNode root = new GenericTreeNode(null, inputs);
		for (ResultFile file : editor.getInputFiles()) {
			GenericTreeNode fileNode = new GenericTreeNode(root, file);
			root.addChild(fileNode);
			RunList runlist = manager.getRunsInFile(file);
			for (int i = 0; i < runlist.size(); ++i) {
				Run run = runlist.get(i);
				GenericTreeNode runNode = new GenericTreeNode(fileNode, run);
				fileNode.addChild(runNode);
			}
		}
		return root;
	}
}

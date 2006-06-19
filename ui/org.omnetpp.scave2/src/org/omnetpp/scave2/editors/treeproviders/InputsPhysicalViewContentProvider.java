package org.omnetpp.scave2.editors.treeproviders;

import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

public class InputsPhysicalViewContentProvider extends CachedTreeContentProvider  {
	private ScaveEditor editor;
	
	public InputsPhysicalViewContentProvider(ScaveEditor editor) {
		super((IChangeNotifier)editor.getAdapterFactory());
		this.editor = editor;
	}

	protected GenericTreeNode buildTree(Object element) {
		// Inputs/File/Run
		Inputs inputs = (Inputs)element;
		ResultFileManager manager = editor.getResultFileManager();
		GenericTreeNode root = new GenericTreeNode(null, inputs);
		for (File file : editor.getInputFiles()) {
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

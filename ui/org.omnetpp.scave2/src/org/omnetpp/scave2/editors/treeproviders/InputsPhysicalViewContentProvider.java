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
		GenericTreeNode root = new GenericTreeNode(inputs);
		for (ResultFile file : manager.getFiles().toArray()) {
			GenericTreeNode fileNode = new GenericTreeNode(file);
			root.addChild(fileNode);
			RunList runlist = manager.getRunsInFile(file);
			for (int i = 0; i < runlist.size(); ++i) {
				Run run = runlist.get(i);
				GenericTreeNode runNode = new GenericTreeNode(run);
				fileNode.addChild(runNode);
			}
		}
		return root;
	}
}

package org.omnetpp.scave.editors.treeproviders;

import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * Content provider for the "File/Run" tree of the Inputs page.
 */
public class InputsFileRunViewContentProvider extends CachedTreeContentProvider  {

	public GenericTreeNode buildTree(Object element) {
		// Manager/ResultFile/Run
		ResultFileManagerEx manager = (ResultFileManagerEx)element;
		GenericTreeNode root = new GenericTreeNode(manager);
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

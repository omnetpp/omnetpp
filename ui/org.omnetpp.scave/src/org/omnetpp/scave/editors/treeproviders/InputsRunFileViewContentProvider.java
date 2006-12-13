package org.omnetpp.scave.editors.treeproviders;

import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * Content provider for the "Run/File" tree of the Inputs page.
 */
public class InputsRunFileViewContentProvider extends CachedTreeContentProvider  {

	public GenericTreeNode buildTree(Object element) {
		// Manager/Run/ResultFile
		ResultFileManagerEx manager = (ResultFileManagerEx)element;
		GenericTreeNode root = new GenericTreeNode(manager);
		for (Run run : manager.getRuns().toArray()) {
			GenericTreeNode runNode = new GenericTreeNode(run);
			root.addChild(runNode);
			ResultFileList filelist = manager.getFilesForRun(run);
			for (int i = 0; i < filelist.size(); ++i) {
				ResultFile file = filelist.get(i);
				GenericTreeNode fileNode = new GenericTreeNode(file);
				runNode.addChild(fileNode);
			}
		}
		return root;
	}
}

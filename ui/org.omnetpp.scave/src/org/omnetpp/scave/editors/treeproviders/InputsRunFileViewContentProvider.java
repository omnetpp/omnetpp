package org.omnetpp.scave.editors.treeproviders;

import org.omnetpp.common.ui.CachedTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
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
		for (Run run : Sorter.sort(manager.getRuns())) {
			GenericTreeNode runNode = new GenericTreeNode(run);
			root.addChild(runNode);
			ResultFileList filelist = manager.getFilesForRun(run);
			for (ResultFile file : Sorter.sort(filelist)) {
				GenericTreeNode fileNode = new GenericTreeNode(file);
				runNode.addChild(fileNode);
			}
		}
		return root;
	}
}

package org.omnetpp.scave2.editors.treeproviders;

import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * Content provider for the "Logical view" tree of the Inputs page.
 */
public class InputsLogicalViewContentProvider extends CachedTreeContentProvider {

	public GenericTreeNode buildTree(Object element) {
		// ResultFileManager/Experiment/Measurement/Replication
		ResultFileManagerEx manager = (ResultFileManagerEx)element;
		GenericTreeNode root = new GenericTreeNode(manager);
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

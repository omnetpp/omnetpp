package org.omnetpp.scave.editors.treeproviders;

import static org.omnetpp.scave.model2.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.model2.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.model2.RunAttribute.REPLICATION;
import static org.omnetpp.scave.model2.RunAttribute.getRunAttribute;

import org.omnetpp.common.ui.CachedTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileList;
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
		GenericTreeNode root = new GenericTreeNode(manager, Sorter.runAttributeComparator);
		for (ResultFile file : manager.getFiles().toArray()) {
			RunList runlist = manager.getRunsInFile(file);
			for (int j = 0; j < runlist.size(); ++j) {
				Run run = runlist.get(j);
				GenericTreeNode experimentNode = root.getOrCreateChild(getRunAttribute(run, EXPERIMENT), Sorter.runAttributeComparator);
				GenericTreeNode measurementNode = experimentNode.getOrCreateChild(getRunAttribute(run, MEASUREMENT), Sorter.runAttributeComparator);
				GenericTreeNode replicationNode = measurementNode.getOrCreateChild(getRunAttribute(run, REPLICATION), Sorter.runComparator);
				GenericTreeNode runNode = replicationNode.getOrCreateChild(run, Sorter.resultFileComparator);
				runNode.getOrCreateChild(file);
			}
		}
		return root;
	}
}

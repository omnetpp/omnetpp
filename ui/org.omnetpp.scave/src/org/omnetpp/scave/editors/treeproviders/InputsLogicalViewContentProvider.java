/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.treeproviders;

import static org.omnetpp.scave.engine.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.engine.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.engine.RunAttribute.REPLICATION;
import static org.omnetpp.scave.model2.RunAttributePayload.getRunAttribute;

import org.omnetpp.common.ui.CachedTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunAttribute;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model2.ResultFilePayload;
import org.omnetpp.scave.model2.RunPayload;

/**
 * Content provider for the "Logical view" tree of the Inputs page.
 */
public class InputsLogicalViewContentProvider extends CachedTreeContentProvider {

	public GenericTreeNode buildTree(Object element) {
		// ResultFileManager/Experiment/Measurement/Replication
		ResultFileManagerEx manager = (ResultFileManagerEx)element;
		manager.getReadLock().lock();
		
		GenericTreeNode root;
		try {
			root = new GenericTreeNode(manager, Sorter.runAttributeComparator);
			for (ResultFile file : manager.getFiles().toArray()) {
				RunList runlist = manager.getRunsInFile(file);
				for (int j = 0; j < runlist.size(); ++j) {
					Run run = runlist.get(j);
					GenericTreeNode experimentNode = root.getOrCreateChild(getRunAttribute(run, EXPERIMENT, null), Sorter.runAttributeComparator);
					GenericTreeNode measurementNode = experimentNode.getOrCreateChild(getRunAttribute(run, MEASUREMENT, null), Sorter.runAttributeComparator);
					String remarkOnReplication = "(seedset=#" + StringUtils.defaultIfEmpty(run.getAttribute(RunAttribute.SEEDSET),"n.a.")+")";
					GenericTreeNode replicationNode = measurementNode.getOrCreateChild(getRunAttribute(run, REPLICATION, remarkOnReplication), Sorter.runPayloadComparator);
					GenericTreeNode runNode = replicationNode.getOrCreateChild(new RunPayload(run), Sorter.resultFilePayloadComparator);
					runNode.getOrCreateChild(new ResultFilePayload(file));
				}
			}
		}
		finally {
			manager.getReadLock().unlock();
		}
		return root;
	}
}

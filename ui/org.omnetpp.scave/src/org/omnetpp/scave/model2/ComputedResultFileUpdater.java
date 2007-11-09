package org.omnetpp.scave.model2;

import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;

public class ComputedResultFileUpdater {
	
	public static void generateComputedFiles(Dataset dataset, DatasetItem target, ResultFileManager manager) {
		try {
			String fileName = ComputedResultFileLocator.instance().getFileNameFor(dataset, target);
			if (fileName != null) {
				DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
				DataflowManager network = builder.build(dataset, target, fileName);
				network.dump();
				network.execute();
			}
		} catch (Exception e) {
			ScavePlugin.logError(e); // XXX 
		}
	}
	

}

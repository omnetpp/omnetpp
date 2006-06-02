package org.omnetpp.scave2.model;

import org.eclipse.emf.ecore.EObject;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.util.ScaveModelSwitch;

public class DatasetManager {

	static class DatasetContent {
		public boolean isScalar;
		public FileList files;
		public IDList idlist;
		
		public DatasetContent() {
			files = new FileList();
			idlist = new IDList();
		}
	}
	
	public static IDList getIDListFromDataset(ResultFileManager manager, Dataset dataset) {
		DatasetContent content = (DatasetContent)new ProcessDatasetSwitch(manager).doSwitch(dataset);
		return content != null ? content.idlist : new IDList();
	}
	
	static class ProcessDatasetSwitch extends ScaveModelSwitch {
		
		ResultFileManager manager;
		DatasetContent content;
		
		public ProcessDatasetSwitch(ResultFileManager manager) {
			this.manager = manager;
			this.content = new DatasetContent();
		}

		@Override
		public Object caseDataset(Dataset dataset) {
			if ("scalar".equals(dataset.getType())) {
				content.isScalar = true;
			} else if ("vector".equals(dataset.getType())) {
				content.isScalar = false;
			}
			
			for (Object item : dataset.getItems())
				doSwitch((EObject)item);
			return content;
		}

		@Override
		public Object caseAdd(Add add) {
			content.idlist = manager.getFilteredList(
					content.isScalar ? manager.getAllScalars() : manager.getAllVectors(),
					add.getFilenamePattern(),
					add.getModuleNamePattern(),
					add.getNamePattern());
			
			// TODO: excepts, experiments, ...
			
			return content;
		}


		// TODO: Discard, Apply, Group, ...
		
	}
}

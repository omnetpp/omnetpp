package org.omnetpp.scave2.model;

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.FileRunList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Discard;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.SetOperation;
import org.omnetpp.scave.model.util.ScaveModelSwitch;

/**
 * This class calculates the content of a dataset
 * applying the operations described by the dataset. 
 */
public class DatasetManager {
	
	static class DatasetContent {
		public boolean isScalar;
		public ResultFileList files;
		public IDList idlist;
		public DataflowManager dataflowManager;
		
		public DatasetContent() {
			files = new ResultFileList();
			idlist = new IDList();
		}
	}
	
	public static IDList getIDListFromDataset(ResultFileManagerEx manager, Dataset dataset, DatasetItem lastProcessedItem) {
		ProcessDatasetSwitch processor = new ProcessDatasetSwitch(manager, lastProcessedItem);
		DatasetContent content = (DatasetContent)processor.doSwitch(dataset);
		return content != null ? content.idlist : new IDList();
	}
	
	
	static class ProcessDatasetSwitch extends ScaveModelSwitch {
		
		ResultFileManagerEx manager;
		DatasetContent content;
		EObject stopAfter;
		boolean stopped;
		
		public ProcessDatasetSwitch(ResultFileManagerEx manager, EObject stopAfter) {
			this.manager = manager;
			this.content = new DatasetContent();
			this.stopAfter = stopAfter;
			this.stopped = false;
		}

		@Override
		public Object caseDataset(Dataset dataset) {
			if (stopped)
				return content;
			
			if ("scalar".equals(dataset.getType())) {
				content.isScalar = true;
			} else if ("vector".equals(dataset.getType())) {
				content.isScalar = false;
			}
			
			if (dataset.getBasedOn() != null)
				content.idlist = getIDListFromDataset(manager, (Dataset)dataset.getBasedOn(), null);
			
			for (Object item : dataset.getItems())
				doSwitch((EObject)item);
			
			if (dataset == stopAfter)
				stopped = true;
			return content;
		}

		@Override
		public Object caseAdd(Add add) {
			if (stopped)
				return content;

			add(content.idlist, add, content.isScalar);

			if (add == stopAfter)
				stopped = true;

			return content;
		}
		
		@Override
		public Object caseDiscard(Discard discard) {
			if (stopped)
				return content;

			remove(content.idlist, discard);
			
			if (discard == stopAfter)
				stopped = true;

			return content;
		}

		@Override
		public Object caseGroup(Group group) {
			if (stopped)
				return content;

			for (Object item : group.getItems())
				doSwitch((EObject)item);
			
			if (group == stopAfter)
				stopped = true;
			
			return content;
		}

		@Override
		public Object caseApply(Apply apply) {
			if (stopped)
				return content;

			/*
			String operation = apply.getOperation();
			EList params = apply.getParams();
			
			// build input idlist
			IDList idlist = content.idlist.dup();
			for (Object filter : apply.getFilters()) {
				SelectDeselectOp filterOp = (SelectDeselectOp)filter;
				if (filterOp instanceof Select) {
					add(idlist, filterOp);
				} else if (filterOp instanceof Deselect) {
					remove(idlist, filterOp);
				}
			}
			
			// execute operation with input/params
			content.idlist = executeFilter(idlist, operation, params);
			*/
			
			if (apply == stopAfter)
				stopped = true;
			
			return content;
		}

		private void add(IDList idlist, SetOperation add, boolean scalar) {
			String fileAndRunFilter = add.getFilenamePattern() != null ? add.getFilenamePattern() : "";
			String moduleFilter = add.getModuleNamePattern() != null ? add.getModuleNamePattern() : "";
			String nameFilter = add.getNamePattern() != null ? add.getNamePattern() : "";

			FileRunList fileRunFilter = null; //FIXME
			idlist.merge(manager.filterIDList(scalar ? manager.getAllScalars() : manager.getAllVectors(),
					fileRunFilter, moduleFilter, nameFilter));

			// TODO: excepts, experiments, ...
		}
		
		private void remove(IDList idlist, SetOperation remove) {
			String fileAndRunFilter = remove.getFilenamePattern() != null ? remove.getFilenamePattern() : "";
			String moduleFilter = remove.getModuleNamePattern() != null ? remove.getModuleNamePattern() : "";
			String nameFilter = remove.getNamePattern() != null ? remove.getNamePattern() : "";
			
			FileRunList fileRunFilter = null; //FIXME
			idlist.substract(manager.filterIDList(idlist,
					fileRunFilter, moduleFilter, nameFilter));

			// TODO: excepts, experiments, ...
		}
		
		private IDList executeFilter(IDList idlist, String operation, EList params) {
			// TODO
			return idlist;
		}
	}
}

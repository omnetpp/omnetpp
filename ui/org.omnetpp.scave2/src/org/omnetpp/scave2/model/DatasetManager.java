package org.omnetpp.scave2.model;

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Deselect;
import org.omnetpp.scave.model.Discard;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.Select;
import org.omnetpp.scave.model.SelectDeselectOp;
import org.omnetpp.scave.model.SetOperation;
import org.omnetpp.scave.model.util.ScaveModelSwitch;

public class DatasetManager {

	static class DatasetContent {
		public boolean isScalar;
		public FileList files;
		public IDList idlist;
		public DataflowManager dataflowManager;
		
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
			add(content.idlist, add, content.isScalar);
			return content;
		}
		
		@Override
		public Object caseDiscard(Discard discard) {
			remove(content.idlist, discard);
			return content;
		}

		@Override
		public Object caseGroup(Group group) {
			for (Object item : group.getItems())
				doSwitch((EObject)item);
			return content;
		}

		@Override
		public Object caseApply(Apply apply) {
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
			return content;
		}

		private void add(IDList idlist, SetOperation add, boolean scalar) {
			String fileAndRunFilter = add.getFilenamePattern() != null ? add.getFilenamePattern() : "";
			String moduleFilter = add.getModuleNamePattern() != null ? add.getModuleNamePattern() : "";
			String nameFilter = add.getNamePattern() != null ? add.getNamePattern() : "";
			idlist.merge(manager.getFilteredList(scalar ? manager.getAllScalars() : manager.getAllVectors(),
					fileAndRunFilter, moduleFilter, nameFilter));

			// TODO: excepts, experiments, ...
		}
		
		private void remove(IDList idlist, SetOperation remove) {
			String fileAndRunFilter = remove.getFilenamePattern() != null ? remove.getFilenamePattern() : "";
			String moduleFilter = remove.getModuleNamePattern() != null ? remove.getModuleNamePattern() : "";
			String nameFilter = remove.getNamePattern() != null ? remove.getNamePattern() : "";
			
			idlist.substract(manager.getFilteredList(idlist,
					fileAndRunFilter, moduleFilter, nameFilter));

			// TODO: excepts, experiments, ...
		}
		
		private IDList executeFilter(IDList idlist, String operation, EList params) {
			// TODO
			return idlist;
		}
	}
}

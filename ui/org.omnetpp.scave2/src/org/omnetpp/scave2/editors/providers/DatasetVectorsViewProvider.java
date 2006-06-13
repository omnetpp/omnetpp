package org.omnetpp.scave2.editors.providers;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.model.DatasetManager;

public class DatasetVectorsViewProvider extends InputsVectorsViewProvider {

	public DatasetVectorsViewProvider(ScaveEditor editor) {
		super(editor);
	}

	@Override
	public ContentProvider getContentProvider() {
		return new ContentProvider() {
			public IDList buildIDList(Object inputElement) {
				if (inputElement instanceof Dataset) {
					ResultFileManager manager = editor.getResultFileManager();
					Dataset dataset = (Dataset)inputElement;
					IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, null);
					return idlist;
				}
				return new IDList();
			}
		};
	}
}

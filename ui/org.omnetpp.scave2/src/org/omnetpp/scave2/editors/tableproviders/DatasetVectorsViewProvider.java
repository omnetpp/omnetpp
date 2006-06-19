package org.omnetpp.scave2.editors.tableproviders;

import org.eclipse.emf.ecore.EObject;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.model.DatasetManager;
import org.omnetpp.scave2.model.ScaveModelUtil;

public class DatasetVectorsViewProvider extends InputsVectorsViewProvider {

	public DatasetVectorsViewProvider(ScaveEditor editor) {
		super(editor);
	}

	@Override
	public ContentProvider getContentProvider() {
		return new ContentProvider() {
			public IDList buildIDList(Object inputElement) {
				if (inputElement instanceof EObject) {
					Dataset dataset = ScaveModelUtil.findEnclosingObject((EObject)inputElement, Dataset.class);
					DatasetItem item = ScaveModelUtil.findEnclosingObject((EObject)inputElement, DatasetItem.class);
					if (dataset != null) {
						ResultFileManager manager = editor.getResultFileManager();
						IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, item);
						return idlist;
					}
				}
				return new IDList();
			}
		};
	}
}

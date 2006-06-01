package org.omnetpp.scave2.editors.providers;

import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave2.editors.ScaveEditor;

import static org.omnetpp.scave2.editors.ui.VectorsPanel.*;

public class InputsVectorsViewProvider extends InputsTableViewProvider {

	public InputsVectorsViewProvider(ScaveEditor editor) {
		super(editor);
	}

	public IStructuredContentProvider getContentProvider() {
		return new ContentProvider() {
			public void buildIDList() {
				ResultFileManager manager = editor.getResultFileManager();
				idlist = new IDList();
				for (File file : editor.getInputFiles()) {
					if (file.getFileType() == File.VECTOR_FILE)
						idlist.merge(manager.getDataInFile(file));
				}
			}
		};
	}

	public ITableLabelProvider getLabelProvider() {
		return new LabelProvider() {
			public String getColumnText(Object element, int columnIndex) {
				long id = ((Long)element).longValue();
				VectorResult vec = editor.getResultFileManager().getVector(id);
				switch (columnIndex)
				{
				case COL_DIRECTORY: return vec.getRun().getFile().getDirectory();
				case COL_FILENAME: return vec.getRun().getFile().getFileName();
				case COL_MODULE: return vec.getModuleName();
				case COL_NAME: return vec.getName();
				case COL_LENGTH: return "not counted";
				case COL_MEAN: return "not yet";
				case COL_STDDEV: return "not yet";
				default: return "n/a";
				}
			}
		};
	}

}

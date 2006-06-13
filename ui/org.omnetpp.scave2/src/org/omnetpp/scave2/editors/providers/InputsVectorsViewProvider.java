package org.omnetpp.scave2.editors.providers;

import static org.omnetpp.scave2.editors.ui.VectorsPanel.COL_DIRECTORY;
import static org.omnetpp.scave2.editors.ui.VectorsPanel.COL_FILENAME;
import static org.omnetpp.scave2.editors.ui.VectorsPanel.COL_LENGTH;
import static org.omnetpp.scave2.editors.ui.VectorsPanel.COL_MEAN;
import static org.omnetpp.scave2.editors.ui.VectorsPanel.COL_MODULE;
import static org.omnetpp.scave2.editors.ui.VectorsPanel.COL_NAME;
import static org.omnetpp.scave2.editors.ui.VectorsPanel.COL_STDDEV;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * This class configures the viewer of the vectors table.
 * 
 * @author Tomi
 */
public class InputsVectorsViewProvider extends InputsTableViewProvider {

	public InputsVectorsViewProvider(ScaveEditor editor) {
		super(editor);
	}

	public ContentProvider getContentProvider() {
		return new ContentProvider() {
			public IDList buildIDList(Object inputElement) {
				ResultFileManager manager = editor.getResultFileManager();
				IDList idlist = new IDList();
				for (File file : editor.getInputFiles()) {
					if (file.getFileType() == File.VECTOR_FILE)
						idlist.merge(manager.getDataInFile(file));
				}
				return idlist;
			}
		};
	}

	public ITableLabelProvider getLabelProvider() {
		return new TableLabelProvider() {
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

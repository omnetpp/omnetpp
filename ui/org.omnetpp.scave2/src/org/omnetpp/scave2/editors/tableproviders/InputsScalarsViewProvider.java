package org.omnetpp.scave2.editors.tableproviders;

import static org.omnetpp.scave2.editors.ui.ScalarsPanel.COL_DIRECTORY;
import static org.omnetpp.scave2.editors.ui.ScalarsPanel.COL_FILENAME;
import static org.omnetpp.scave2.editors.ui.ScalarsPanel.COL_MODULE;
import static org.omnetpp.scave2.editors.ui.ScalarsPanel.COL_NAME;
import static org.omnetpp.scave2.editors.ui.ScalarsPanel.COL_RUN;
import static org.omnetpp.scave2.editors.ui.ScalarsPanel.COL_VALUE;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * This class configures the viewer of the scalars table.
 * 
 * @author Tomi
 */
public class InputsScalarsViewProvider extends InputsTableViewProvider {

	public InputsScalarsViewProvider(ScaveEditor editor) {
		super(editor);
	}
	
	public ContentProvider getContentProvider() {
		return new ContentProvider() {
			public IDList buildIDList(Object inputElement) {
				ResultFileManager manager = editor.getResultFileManager();
				IDList idlist = new IDList();
				for (File file : editor.getInputFiles()) {
					if (file.getFileType() == File.SCALAR_FILE)
						idlist.merge(manager.getDataInFile(file));
				}
				return idlist;
			}
		};
	}

	public ITableLabelProvider getLabelProvider() {
		return new TableLabelProvider() {
			public String getColumnText(Object element, int columnIndex) {
				long id = (Long)element;
				ScalarResult scalar = editor.getResultFileManager().getScalar(id);
				switch (columnIndex)
				{
					case COL_DIRECTORY: return scalar.getRun().getFile().getDirectory();
					case COL_FILENAME: return scalar.getRun().getFile().getFileName();
					case COL_RUN: return scalar.getRun().getRunName();
					case COL_MODULE: return scalar.getModuleName();
					case COL_NAME: return scalar.getName();
					case COL_VALUE: return String.valueOf(scalar.getValue());
					default: return "n/a";
				}
			}
		};
	}
}

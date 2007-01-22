package org.omnetpp.scave.editors;

import org.omnetpp.scave.model.IDatasetStrategy;
import org.omnetpp.scave.model.ScalarDatasetStrategy;

/**
 * Editor for scalar datasets.
 */
public class ScalarDatasetEditor extends DatasetEditor {

	private static IDatasetStrategy strategy = new ScalarDatasetStrategy();

	public ScalarDatasetEditor() {
		super(strategy,
			new IDatasetEditorPage[] {
				new FilterPanelPage(strategy)
			});
	}
}

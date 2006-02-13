package org.omnetpp.scave.editors;

import org.omnetpp.scave.model.ScalarDatasetStrategy;

/**
 * Editor for scalar datasets.
 */
public class ScalarDatasetEditor extends DatasetEditor {

	public ScalarDatasetEditor() {
		super(new ScalarDatasetStrategy());
	}

	/**
	 * Creates the pages of the multi-page editor.
	 */
	protected void createPages() {
		createDatasetPage();

		setupDragSource(filterPanel.getPanel().getTable());
		setupDropTarget(filterPanel.getPanel());
	}
}

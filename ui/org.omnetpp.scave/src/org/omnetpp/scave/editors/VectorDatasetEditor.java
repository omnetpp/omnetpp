package org.omnetpp.scave.editors;

import org.omnetpp.scave.model.VectorDatasetStrategy;

/**
 * Editor for scalar datasets.
 */
public class VectorDatasetEditor extends DatasetEditor {

	public VectorDatasetEditor() {
		super(new VectorDatasetStrategy());
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

package org.omnetpp.scave.editors;


import org.omnetpp.scave.model.IDatasetStrategy;
import org.omnetpp.scave.model.VectorDatasetStrategy;


/**
 * Editor for scalar datasets.
 */
public class VectorChartEditor extends DatasetEditor {
	
	private static IDatasetStrategy strategy = new VectorDatasetStrategy();

	public VectorChartEditor() {
		super(strategy,
			new IDatasetEditorPage[] {
				new FilterPanelPage(strategy),
				new ChartPage(strategy)
			});
	}
}

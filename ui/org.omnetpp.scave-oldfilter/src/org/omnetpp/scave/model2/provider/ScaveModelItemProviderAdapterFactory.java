package org.omnetpp.scave.model2.provider;

import org.eclipse.emf.common.notify.Adapter;

/**
 * 
 *
 * @author tomi
 */
public class ScaveModelItemProviderAdapterFactory extends
		org.omnetpp.scave.model.provider.ScaveModelItemProviderAdapterFactory {

	@Override
	public Adapter createAnalysisAdapter() {
		if (analysisItemProvider == null) {
			analysisItemProvider = new AnalysisItemProvider(this);
		}

		return analysisItemProvider;
	}

	@Override
	public Adapter createChartSheetAdapter() {
		if (chartSheetItemProvider == null) {
			chartSheetItemProvider = new ChartSheetItemProvider(this);
		}

		return chartSheetItemProvider;
	}

	@Override
	public Adapter createDatasetAdapter() {
		if (datasetItemProvider == null) {
			datasetItemProvider = new DatasetItemProvider(this);
		}

		return datasetItemProvider;
	}

	@Override
	public Adapter createDatasetsAdapter() {
		if (datasetsItemProvider == null) {
			datasetsItemProvider = new DatasetsItemProvider(this);
		}

		return datasetsItemProvider;
	}

	@Override
	public Adapter createGroupAdapter() {
		if (groupItemProvider == null) {
			groupItemProvider = new GroupItemProvider(this);
		}
		return groupItemProvider;
	}
}

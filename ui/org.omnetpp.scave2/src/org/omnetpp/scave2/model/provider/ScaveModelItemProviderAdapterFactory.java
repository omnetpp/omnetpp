package org.omnetpp.scave2.model.provider;

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
	
	

}

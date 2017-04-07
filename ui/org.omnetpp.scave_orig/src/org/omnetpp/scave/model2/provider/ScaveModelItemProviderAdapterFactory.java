/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
    public Adapter createBarChartAdapter() {
        if (barChartItemProvider == null) {
            barChartItemProvider = new BarChartItemProvider(this);
        }

        return barChartItemProvider;
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

    @Override
    public Adapter createHistogramChartAdapter() {
        if (histogramChartItemProvider == null) {
            histogramChartItemProvider = new HistogramChartItemProvider(this);
        }

        return histogramChartItemProvider;
    }

    @Override
    public Adapter createLineChartAdapter() {
        if (lineChartItemProvider == null) {
            lineChartItemProvider = new LineChartItemProvider(this);
        }

        return lineChartItemProvider;
    }
}

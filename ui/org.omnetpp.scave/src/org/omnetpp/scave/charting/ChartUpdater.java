/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import java.util.List;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * This class listens on changes in the model, and refreshes the chart accordingly.
 *
 * @author tomi
 */
public class ChartUpdater {
    private static final int CHART_UPDATE_DELAY_MS = 200;

    private final Chart chart;
    private final IChartView view;
    private final ResultFileManager manager;
    private final DelayedJob startUpdateJob = new DelayedJob(CHART_UPDATE_DELAY_MS) {
        @Override
        public void run() {
            updateDataset();
        }
    };

    public ChartUpdater(Chart chart, IChartView view, ResultFileManager manager) {
        this.chart = chart;
        this.view = view;
        this.manager = manager;
    }

    public ResultFileManager getResultFileManager() {
        return manager;
    }

    public Chart getChart() {
        return chart;
    }

    public IChartView getChartView() {
        return view;
    }

    /**
     * Propagate changes on the "Chart" model object to the chart view.
     */
    @SuppressWarnings("unchecked")
    public void updateChart(Notification notification) {
        if (notification.isTouch() || !(notification.getNotifier() instanceof EObject))
            return;

        EObject notifier = (EObject)notification.getNotifier();

        // add/remove or change input file
        if (notifier instanceof Inputs || notifier instanceof InputFile) {
            // TODO should be checked that visible items are affected
            scheduleDatasetUpdate();
            return;
        }

        if (notifier.eResource() != chart.eResource())
            return;

        // add/remove chart property
        if (notifier instanceof Chart && notifier == chart) {
            switch (notification.getFeatureID(Chart.class)) {
            case ScaveModelPackage.CHART__INPUT:
                scheduleDatasetUpdate();
                break;
            case ScaveModelPackage.CHART__PROPERTIES:
                Property property;
                switch (notification.getEventType()) {
                case Notification.ADD:
                    property = (Property)notification.getNewValue();
                    setChartProperty(property.getName(), property.getValue());
                    break;
                case Notification.REMOVE:
                    property = (Property)notification.getOldValue();
                    setChartProperty(property.getName(), null);
                    break;
                case Notification.ADD_MANY:
                    for (Property prop : (List<Property>)notification.getNewValue()) {
                        setChartProperty(prop.getName(), prop.getValue());
                    }
                    break;
                case Notification.REMOVE_MANY:
                    for (Property prop : (List<Property>)notification.getOldValue()) {
                        setChartProperty(prop.getName(), null);
                    }
                    break;
                }
                break;
            }
            if (notifier instanceof BarChart) {
                switch (notification.getFeatureID(BarChart.class)) {
                case ScaveModelPackage.BAR_CHART__GROUP_BY_FIELDS:
                case ScaveModelPackage.BAR_CHART__BAR_FIELDS:
                case ScaveModelPackage.BAR_CHART__AVERAGED_FIELDS:
                case ScaveModelPackage.BAR_CHART__GROUP_NAME_FORMAT:
                case ScaveModelPackage.BAR_CHART__BAR_NAME_FORMAT:
                    scheduleDatasetUpdate();
                    break;
                }
            }
            else if (notifier instanceof LineChart) {
                switch (notification.getFeatureID(LineChart.class)) {
                case ScaveModelPackage.LINE_CHART__LINE_NAME_FORMAT:
                    scheduleDatasetUpdate();
                    break;
                }
            }
            else if (notifier instanceof ScatterChart) {
                switch (notification.getFeatureID(ScatterChart.class)) {
                case ScaveModelPackage.SCATTER_CHART__XDATA_PATTERN:
                case ScaveModelPackage.SCATTER_CHART__ISO_DATA_PATTERN:
                case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
                    scheduleDatasetUpdate();
                    break;
                }
            }
        }
        // change chart property
        else if (notifier instanceof Property && notifier.eContainer() == chart) {
            switch (notification.getFeatureID(Property.class)) {
            case ScaveModelPackage.PROPERTY__VALUE:
                Property property = (Property)notification.getNotifier();
                setChartProperty(property.getName(), (String)notification.getNewValue());
                break;
            }
        }
        else if (notification.getFeature() != null &&
                notification.getFeature() instanceof EStructuralFeature &&
                "name".equals(((EStructuralFeature)notification.getFeature()).getName())) {
            // ignore name changes
        }
        // TODO add/remove change in the dataset of the chart or in its base dataset
    }

    private void setChartProperty(String name, String value) {
        if (!view.getCanvas().isDisposed())
            view.setProperty(name, value);
    }

    private void scheduleDatasetUpdate() {
        startUpdateJob.restartTimer();
    }

    /**
     * Starts a job that recalculates chart contents.
     */
    public void updateDataset() {
        ChartFactory.populateChart(view, chart, ChartUpdater.this.manager, null);
    }
}

package org.omnetpp.scave.charting;

import java.util.List;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.jfree.data.category.CategoryDataset;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * This class listens on changes in the model, and refreshes the chart accordingly.
 * Currently it only listens on chart property changes. 
 * 
 * TODO Should also listen on changes that might affect chart contents, and recalculate the chart. 
 *
 * @author tomi
 */
public class ChartUpdater {
	private Chart chart;
	private ChartCanvas view;
	private ResultFileManager manager;

	public ChartUpdater(Chart chart, ChartCanvas view, ResultFileManager manager) {
		this.chart = chart;
		this.view = view;
		this.manager = manager;
	}

	@SuppressWarnings("unchecked")
	public void updateChart(Notification notification) {
		// we are only interested in changes within our Chart object (otherwise return) 
		if (!(notification.getNotifier() instanceof EObject) ||
				(notification.getNotifier() != chart &&
						((EObject)notification.getNotifier()).eContainer() != chart))
				return; 

		ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
		if (pkg.getChart_Properties().equals(notification.getFeature())) {
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
		}
		else if (pkg.getProperty_Value().equals(notification.getFeature())) {
			Property property = (Property)notification.getNotifier();
			setChartProperty(property.getName(), (String)notification.getNewValue());
		}
	}

	public void updateDataset() {
		Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);
		if (chart instanceof BarChart)
			setDataset(DatasetManager.createScalarDataset(chart, dataset, manager));
		if (chart instanceof LineChart)
			setDataset(DatasetManager.createVectorDataset(chart, dataset, manager));
		if (chart instanceof HistogramChart)
			; //TODO
		throw new RuntimeException("unknown chart type");
	}

	private void setChartProperty(String name, String value) {
		view.setProperty(name, value);
	}

	private void setDataset(CategoryDataset dataset) {
		if (view instanceof ScalarChart)
			((ScalarChart)view).setDataset(dataset);

	}

	private void setDataset(OutputVectorDataset dataset) {
		if (view instanceof VectorChart)
			((VectorChart)view).setDataset(dataset);
	}
}

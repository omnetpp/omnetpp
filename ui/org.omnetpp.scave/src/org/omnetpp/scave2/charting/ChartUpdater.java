package org.omnetpp.scave2.charting;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.swt.widgets.Control;
import org.jfree.data.category.CategoryDataset;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.model.DatasetManager;
import org.omnetpp.scave2.model.ScaveModelUtil;

public class ChartUpdater {
	private Chart chart;
	private Control view;
	private ResultFileManager manager;
	
	public ChartUpdater(Chart chart, Control view, ResultFileManager manager) {
		this.chart = chart;
		this.view = view;
		this.manager = manager;
	}

	public void updateChart(Notification notification) {
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
		switch (dataset.getType().getValue()) {
		case DatasetType.SCALAR:
			setDataset(DatasetManager.createScalarDataset(chart, dataset, manager));
			break;
		case DatasetType.VECTOR:
			setDataset(DatasetManager.createVectorDataset(chart, dataset, manager));
			break;
		case DatasetType.HISTOGRAM:
			// TODO
			break;
		default:
			Assert.isTrue(false, "Unknown DatasetType: " + dataset.getType());
			break;
		}
	}
	
	private void setChartProperty(String name, String value) {
		if (view instanceof ScalarChart)
			((ScalarChart)view).setProperty(name, value);
		else if (view instanceof ChartCanvas)
			((ChartCanvas)view).setProperty(name, value);
	}
	
	private void setDataset(CategoryDataset dataset) {
		if (view instanceof ScalarChart)
			((ScalarChart)view).setDataset(dataset);
		else if (view instanceof ScalarChart2)
			((ScalarChart2)view).setDataset(dataset);
		
	}
	
	private void setDataset(OutputVectorDataset dataset) {
		if (view instanceof VectorChart)
			((VectorChart)view).setDataset(dataset);
	}
}

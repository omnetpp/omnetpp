package org.omnetpp.scave2.charting;

import java.util.List;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelPackage;

public class ChartUpdater {
	private Chart chart;
	private Control view;
	
	public ChartUpdater(Chart chart, Control view) {
		this.chart = chart;
		this.view = view;
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
	
	private void setChartProperty(String name, String value) {
		if (view instanceof ScalarChart)
			((ScalarChart)view).setProperty(name, value);
		else if (view instanceof ChartCanvas)
			((ChartCanvas)view).setProperty(name, value);
	}
}

package org.omnetpp.scave2.model;

import org.eclipse.emf.ecore.EObject;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave.model.ScaveModelFactory;

/**
 * A collection of static methods to manipulate model objects
 * @author andras
 */
public class ScaveModelUtil {
	
	public static Dataset createDataset(String name, DatasetType type, FilterParams params) {
		Dataset dataset = ScaveModelFactory.eINSTANCE.createDataset();
		dataset.setName(name);
		dataset.setType(type);
		dataset.getItems().add(createAdd(params));
		return dataset;
	}
	
	public static Chart createChart(String name) {
		Chart chart = ScaveModelFactory.eINSTANCE.createChart();
		chart.setName(name);
		return chart;
	}
	
	public static Add createAdd(FilterParams params) {
		Add add = ScaveModelFactory.eINSTANCE.createAdd();
		add.setFilenamePattern(params.getRunNamePattern());
		add.setModuleNamePattern(params.getModuleNamePattern());
		add.setNamePattern(params.getDataNamePattern());
		return add;
	}
	
	public static Dataset findEnclosingDataset(Chart chart) {
		EObject parent = chart.eContainer();
		while (parent != null && !(parent instanceof Dataset))
			parent = parent.eContainer();
		return (Dataset)parent;
	}
	
	public static <T extends EObject> T findEnclosingObject(EObject object, Class<T> type) {
		while (object != null && !(type.isAssignableFrom(object.getClass())))
			object = object.eContainer();
		return (T)object;
	}
}

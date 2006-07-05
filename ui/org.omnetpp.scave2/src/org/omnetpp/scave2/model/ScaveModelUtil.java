package org.omnetpp.scave2.model;

import org.eclipse.emf.ecore.EObject;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
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
	
	public enum RunIdKind {
		FILE_RUN,
		EXPERIMENT_MEASUREMENT_REPLICATION
	}
	
	public static Dataset createDataset(String name, DatasetType type, FilterParams params) {
		Dataset dataset = ScaveModelFactory.eINSTANCE.createDataset();
		dataset.setName(name);
		dataset.setType(type);
		dataset.getItems().add(createAdd(params));
		return dataset;
	}
	
	public static Dataset createDataset(String name, DatasetType type, ResultItem[] items, RunIdKind id) {
		Dataset dataset = ScaveModelFactory.eINSTANCE.createDataset();
		dataset.setName(name);
		dataset.setType(type);
		for (ResultItem item : items)
			dataset.getItems().add(createAdd(item, id));
		return dataset;
	}
	
	public static Chart createChart(String name) {
		Chart chart = ScaveModelFactory.eINSTANCE.createChart();
		chart.setName(name);
		return chart;
	}
	
	public static Add createAdd(FilterParams params) {
		Add add = ScaveModelFactory.eINSTANCE.createAdd();
		add.setFilenamePattern(params.getFileNamePattern());
		add.setRunNamePattern(params.getRunNamePattern());
		add.setExperimentNamePattern(params.getExperimentNamePattern());
		add.setMeasurementNamePattern(params.getMeasurementNamePattern());
		add.setReplicationNamePattern(params.getReplicationNamePattern());
		add.setModuleNamePattern(params.getModuleNamePattern());
		add.setNamePattern(params.getDataNamePattern());
		return add;
	}
	
	public static Add createAdd(ResultItem item, RunIdKind id) {
		Add add = ScaveModelFactory.eINSTANCE.createAdd();
		ResultFile file = item.getFileRun().getFile();
		Run run = item.getFileRun().getRun();
		if (id == RunIdKind.FILE_RUN) {
			add.setFilenamePattern(file.getFilePath());
			add.setRunNamePattern(run.getRunName());
		}
		else if (id == RunIdKind.EXPERIMENT_MEASUREMENT_REPLICATION) {
			add.setExperimentNamePattern(run.getAttribute(RunAttribute.EXPERIMENT));
			add.setMeasurementNamePattern(run.getAttribute(RunAttribute.MEASUREMENT));
			add.setReplicationNamePattern(run.getAttribute(RunAttribute.REPLICATION));
		}
		add.setModuleNamePattern(item.getModuleName());
		add.setNamePattern(item.getName());
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

package org.omnetpp.scave.editors.ui;

import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model.SetOperation;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Base class for edit forms of set operations.
 *
 * @author tomi
 */
public class SetOperationEditForm implements IScaveObjectEditForm {

	protected static final EStructuralFeature[] features = new EStructuralFeature[] {
		ScaveModelPackage.eINSTANCE.getSetOperation_ModuleNamePattern(),
		ScaveModelPackage.eINSTANCE.getSetOperation_NamePattern(),
		ScaveModelPackage.eINSTANCE.getSetOperation_ExperimentNamePattern(),
		ScaveModelPackage.eINSTANCE.getSetOperation_MeasurementNamePattern(),
		ScaveModelPackage.eINSTANCE.getSetOperation_ReplicationNamePattern(),
		ScaveModelPackage.eINSTANCE.getSetOperation_FilenamePattern(),
		ScaveModelPackage.eINSTANCE.getSetOperation_RunNamePattern(),
		ScaveModelPackage.eINSTANCE.getSetOperation_FromRunsWhere(),
		ScaveModelPackage.eINSTANCE.getSetOperation_SourceDataset(),
	};
	
	/**
	 * The edited SetOperation.
	 */
	protected SetOperation setOperation;
	
	/**
	 * List of datasets that can be the source of this operation.
	 * First elements is <code>null</code> corresponding to "All".
	 */
	protected java.util.List<Dataset> sourceDatasets;

	protected FilterHints filterHints; 
	
	// controls
	private CCombo sourceDatasetCombo;
	private CCombo moduleNameCombo;
	private CCombo nameCombo;
	private CCombo experimentNameCombo;
	private CCombo measurementNameCombo;
	private CCombo replicationNameCombo;
	private CCombo fileNameCombo;
	private CCombo runNameCombo;
	private Text fromRunsWhereText;
	
	/**
	 * Number of visible items in combos.
	 */
	private static final int VISIBLE_ITEM_COUNT = 15;
	
	public SetOperationEditForm(SetOperation setOperation, ResultFileManager manager) {
		this.setOperation = setOperation;
		
		sourceDatasets = new java.util.ArrayList<Dataset>();
		sourceDatasets.add(null);
		Dataset dataset = ScaveModelUtil.findEnclosingObject(setOperation, Dataset.class);
		java.util.List<Dataset> datasets = ScaveModelUtil.findDatasets(setOperation.eResource(), dataset.getType());
		for (Dataset ds : datasets)
			if (!ds.equals(dataset))
				sourceDatasets.add(ds);
		
		filterHints = new FilterHints(manager, dataset.getType());
	}
	
	public String getTitle() {
		return String.format("%s operation", setOperation.eClass().getName());
	}

	public String getDescription() {
		return "Modify the properties of the operation.";
	}

	public int getFeatureCount() {
		return features.length;
	}

	public EStructuralFeature[] getFeatures() {
		return features;
	}

	public void populatePanel(Composite panel) {
		Label label;
		Group group;

		panel.setLayout(new GridLayout(2, false));
		
		// source dataset
		label = new Label(panel, SWT.NONE);
		label.setText("Source dataset:");
		label.setLayoutData(new GridData());
		sourceDatasetCombo = new CCombo(panel, SWT.BORDER | SWT.READ_ONLY);
		sourceDatasetCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		sourceDatasetCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		String[] datasetNames = new String[sourceDatasets.size()];
		datasetNames[0] = "All";
		for (int i = 1; i < sourceDatasets.size(); ++i)
			datasetNames[i] = sourceDatasets.get(i).getName();
		sourceDatasetCombo.setItems(datasetNames);
		
		// module name, result name
		group = new Group(panel, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		group.setLayout(new GridLayout(2, false));
		label = new Label(group, SWT.NONE);
		label.setText("Module:");
		label.setLayoutData(new GridData());
		moduleNameCombo = new CCombo(group, SWT.BORDER);
		moduleNameCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		moduleNameCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		label = new Label(group, SWT.NONE);
		label.setText("Name:");
		label.setLayoutData(new GridData());
		nameCombo = new CCombo(group, SWT.BORDER);
		nameCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		nameCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		
		// file/run
		group = new Group(panel, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		group.setLayout(new GridLayout(2, false));
		label = new Label(group, SWT.NONE);
		label.setText("File:");
		label.setLayoutData(new GridData());
		fileNameCombo = new CCombo(group, SWT.BORDER);
		fileNameCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		fileNameCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		label = new Label(group, SWT.NONE);
		label.setText("Run name:");
		label.setLayoutData(new GridData());
		runNameCombo = new CCombo(group, SWT.BORDER);
		runNameCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		runNameCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		label = new Label(group, SWT.NONE);
		label.setText("Only runs where:");
		label.setLayoutData(new GridData());
		fromRunsWhereText = new Text(group, SWT.BORDER);
		fromRunsWhereText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		
		// experiment/measurement/replication
		group = new Group(panel, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		group.setLayout(new GridLayout(2, false));
		label = new Label(group, SWT.NONE);
		label.setText("Experiment:");
		label.setLayoutData(new GridData());
		experimentNameCombo = new CCombo(group, SWT.BORDER);
		experimentNameCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		experimentNameCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		label = new Label(group, SWT.NONE);
		label.setText("Measurement:");
		label.setLayoutData(new GridData());
		measurementNameCombo = new CCombo(group, SWT.BORDER);
		measurementNameCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		measurementNameCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		label = new Label(group, SWT.NONE);
		label.setText("Replication:");
		label.setLayoutData(new GridData());
		replicationNameCombo = new CCombo(group, SWT.BORDER);
		replicationNameCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		replicationNameCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
	
		// set filter hints
		nameCombo.setItems(filterHints.getHints(Filter.FIELD_DATANAME));
		moduleNameCombo.setItems(filterHints.getHints(Filter.FIELD_MODULENAME));
		fileNameCombo.setItems(filterHints.getHints(Filter.FIELD_FILENAME));
		runNameCombo.setItems(filterHints.getHints(Filter.FIELD_RUNNAME));
		experimentNameCombo.setItems(filterHints.getHints(Filter.FIELD_EXPERIMENT));
		measurementNameCombo.setItems(filterHints.getHints(Filter.FIELD_MEASUREMENT));
		replicationNameCombo.setItems(filterHints.getHints(Filter.FIELD_REPLICATION));
	}
	
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SET_OPERATION__MODULE_NAME_PATTERN:
			return moduleNameCombo.getText();
		case ScaveModelPackage.SET_OPERATION__NAME_PATTERN:
			return nameCombo.getText();
		case ScaveModelPackage.SET_OPERATION__EXPERIMENT_NAME_PATTERN:
			return experimentNameCombo.getText();
		case ScaveModelPackage.SET_OPERATION__MEASUREMENT_NAME_PATTERN:
			return measurementNameCombo.getText();
		case ScaveModelPackage.SET_OPERATION__REPLICATION_NAME_PATTERN:
			return replicationNameCombo.getText();
		case ScaveModelPackage.SET_OPERATION__FILENAME_PATTERN:
			return fileNameCombo.getText();
		case ScaveModelPackage.SET_OPERATION__RUN_NAME_PATTERN:
			return runNameCombo.getText();
		case ScaveModelPackage.SET_OPERATION__FROM_RUNS_WHERE:
			return fromRunsWhereText.getText();
		case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
			int index = sourceDatasetCombo.getSelectionIndex();
			return index >= 0 ? sourceDatasets.get(index) : null;
		}
		
		throw new IllegalArgumentException("Unexpected feature: " + feature.getName());
	}

	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SET_OPERATION__MODULE_NAME_PATTERN:
			if (value != null) moduleNameCombo.setText((String)value);
			break;
		case ScaveModelPackage.SET_OPERATION__NAME_PATTERN:
			if (value != null) nameCombo.setText((String)value);
			break;
		case ScaveModelPackage.SET_OPERATION__EXPERIMENT_NAME_PATTERN:
			if (value != null) experimentNameCombo.setText((String)value);
			break;
		case ScaveModelPackage.SET_OPERATION__MEASUREMENT_NAME_PATTERN:
			if (value != null) measurementNameCombo.setText((String)value);
			break;
		case ScaveModelPackage.SET_OPERATION__REPLICATION_NAME_PATTERN:
			if (value != null) replicationNameCombo.setText((String)value);
			break;
		case ScaveModelPackage.SET_OPERATION__FILENAME_PATTERN:
			if (value != null) fileNameCombo.setText((String)value);
			break;
		case ScaveModelPackage.SET_OPERATION__RUN_NAME_PATTERN:
			if (value != null) runNameCombo.setText((String)value);
			break;
		case ScaveModelPackage.SET_OPERATION__FROM_RUNS_WHERE:
			if (value != null) fromRunsWhereText.setText((String)value);
			break;
		case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
			sourceDatasetCombo.setText(value != null ? ((Dataset)value).getName() : "All");
			break;
		default:
			throw new IllegalArgumentException("Unexpected feature: " + feature.getName());
		}
	}
}


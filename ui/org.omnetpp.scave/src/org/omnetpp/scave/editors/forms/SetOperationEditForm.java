package org.omnetpp.scave.editors.forms;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.editors.datatable.FilterField;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model.SelectDeselectOp;
import org.omnetpp.scave.model.SetOperation;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Base class for edit forms of set operations.
 *
 * @author tomi
 */
public class SetOperationEditForm implements IScaveObjectEditForm {

	protected static final EStructuralFeature[] features = new EStructuralFeature[] {
		ScaveModelPackage.eINSTANCE.getSetOperation_FilterPattern(),
		ScaveModelPackage.eINSTANCE.getSetOperation_SourceDataset(),
		ScaveModelPackage.eINSTANCE.getSetOperation_Type(),
	};
	
	/**
	 * The edited SetOperation.
	 */
	protected SetOperation setOperation;
	
	/**
	 * The parent of the setOperation.
	 * It is always included in the model.
	 */
	protected EObject parent;
	
	/**
	 * List of datasets that can be the source of this operation.
	 * First elements is <code>null</code> corresponding to "All".
	 */
	protected java.util.List<Dataset> sourceDatasets;

	/**
	 * ResultFileManager used to get the filter hints.
	 */
	protected ResultFileManager manager;
	
	// controls
	private Combo sourceDatasetCombo;
	private Combo datatypeCombo;
	private FilterField filterField;
	private Text filterText;
	
	public SetOperationEditForm(SetOperation setOperation, EObject parent, ResultFileManager manager) {
		this.setOperation = setOperation;
		this.parent = parent;
		this.manager = manager;
		
		sourceDatasets = new java.util.ArrayList<Dataset>();
		sourceDatasets.add(null);
		Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
		java.util.List<Dataset> datasets = ScaveModelUtil.findDatasets(parent);
		for (Dataset ds : datasets)
			if (!ds.equals(dataset))
				sourceDatasets.add(ds);
	}
	
	public String getTitle() {
		return String.format("%s operation", setOperation.eClass().getName());
	}

	public String getDescription() {
		return "Modify the properties of the operation.";
	}

	public EStructuralFeature[] getFeatures() {
		return features;
	}

	public void populatePanel(Composite panel) {
		Label label;

		panel.setLayout(new GridLayout(2, false));
		
		// source dataset
		label = new Label(panel, SWT.NONE);
		label.setText("Source dataset:");
		label.setLayoutData(new GridData());
		sourceDatasetCombo = new Combo(panel, SWT.BORDER | SWT.READ_ONLY);
		sourceDatasetCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		sourceDatasetCombo.setVisibleItemCount(15);
		String[] datasetNames = new String[sourceDatasets.size()];
		datasetNames[0] = "All";
		for (int i = 1; i < sourceDatasets.size(); ++i)
			datasetNames[i] = sourceDatasets.get(i).getName();
		sourceDatasetCombo.setItems(datasetNames);
		
		label = new Label(panel, SWT.NONE);
		label.setText("Data type:");
		label.setLayoutData(new GridData());
		datatypeCombo = new Combo(panel, SWT.BORDER | SWT.READ_ONLY);
		datatypeCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		datatypeCombo.setItems(ScaveModelUtil.getResultTypeNames());
		
		label = new Label(panel, SWT.NONE);
		label.setText("Filter pattern:");
		label.setLayoutData(new GridData());
		filterField = new FilterField(panel, SWT.SINGLE | SWT.BORDER);
		filterField.getLayoutControl().setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		filterText = filterField.getText();

		filterText.setFocus();
		
		// update the filter hints when the type selection changes
		datatypeCombo.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				updateFilterHints();
			}
		});

		// set default selection in the type combo
		ResultType datatype = (setOperation instanceof SelectDeselectOp && parent instanceof Chart ?
				ScaveModelUtil.getDataTypesOfChart((Chart)parent)[0] : ResultType.SCALAR_LITERAL);
		selectDatatype(datatype);
	}
	
	private void selectDatatype(ResultType datatype) {
		int index = datatypeCombo.indexOf(datatype.getName());
		if (index >= 0) {
			datatypeCombo.select(index);
			// the listener is not called by CCombo.select() so update the filter hints manually
			updateFilterHints();
		}
	}
	
	private void updateFilterHints() {
		String datatypeName = datatypeCombo.getText();
		ResultType datatype = datatypeName != null ? ResultType.getByName(datatypeName) : ResultType.SCALAR_LITERAL;
		filterField.setFilterHints(new FilterHints(manager, datatype));
	}
	
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SET_OPERATION__FILTER_PATTERN:
			return filterText.getText();
		case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
			int index = sourceDatasetCombo.getSelectionIndex();
			return index >= 0 ? sourceDatasets.get(index) : null;
		case ScaveModelPackage.SET_OPERATION__TYPE:
			String text = datatypeCombo.getText();
			return text != null ? ResultType.getByName(text) : null;
		default:
			throw new IllegalArgumentException("Unexpected feature: " + feature.getName());
		}
		
	}

	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SET_OPERATION__FILTER_PATTERN:
			if (value != null) filterText.setText((String)value);
			break;
		case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
			sourceDatasetCombo.setText(value != null ? ((Dataset)value).getName() : "All");
			break;
		case ScaveModelPackage.SET_OPERATION__TYPE:
			ResultType datatype = value != null ? (ResultType)value : ResultType.SCALAR_LITERAL;
			selectDatatype(datatype);
			break;
		default:
			throw new IllegalArgumentException("Unexpected feature: " + feature.getName());
		}
	}
}


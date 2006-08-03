package org.omnetpp.scave2.editors.ui;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.model.ScaveModelUtil;

/**
 * Edit form of datasets.
 *
 * @author tomi
 */
public class DatasetEditForm implements IScaveObjectEditForm {
	
	/**
	 * Features edited on this panel.
	 */
	private static final EStructuralFeature[] features = new EStructuralFeature[] {
		ScaveModelPackage.eINSTANCE.getDataset_Name(),
		ScaveModelPackage.eINSTANCE.getDataset_BasedOn()
	};
	
	/**
	 * The edited dataset.
	 */
	private Dataset dataset;
	
	/**
	 * List of datasets that the edited dataset can be the based on.
	 * First element is null.
	 */
	private List<Dataset> baseDatasets;

	// edit controls of the features
	private Text nameText;
	private CCombo basedOnCombo;
	

	
	public DatasetEditForm(Dataset dataset) {
		this.dataset = dataset;
		// collect datasets that can be the base of this dataset
		// XXX: circularity check is missing
		baseDatasets = new ArrayList<Dataset>();
		baseDatasets.add(null);
		for (Dataset ds : ScaveModelUtil.findObjects(dataset.eResource(), Dataset.class))
			if (ds != dataset && ds.getType().equals(dataset.getType()))
				baseDatasets.add(ds);
	}
	
	/**
	 * Returns the title displayed on the top of the dialog.
	 */
	public String getTitle() {
		return StringUtils.capitalize(dataset.getType().getName()) + " dataset";
	}

	/**
	 * Returns the description displayed below the title.
	 */
	public String getDescription() {
		return "Modify dataset properties.";
	}

	/**
	 * Returns the number of features on this form.
	 */
	public int getFeatureCount() {
		return features.length;
	}

	/**
	 * Returns the features edited on this form.
	 */
	public EStructuralFeature[] getFeatures() {
		return features;
	}

	/**
	 * Add edit controls the panel.
	 */
	public void populatePanel(Composite panel) {
		panel.setLayout(new GridLayout(2, false));
		
		Label nameLabel = new Label(panel, SWT.NONE);
		nameLabel.setText("Name:");
		nameLabel.setLayoutData(new GridData());
		nameText = new Text(panel, SWT.BORDER);
		nameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		
		Label basedOnLabel = new Label(panel, SWT.NONE);
		basedOnLabel.setText("Based on:");
		basedOnLabel.setLayoutData(new GridData());
		basedOnCombo = new CCombo(panel, SWT.BORDER | SWT.READ_ONLY);
		basedOnCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		String[] datasetNames = new String[baseDatasets.size()];
		for (int i = 0; i < baseDatasets.size(); ++i) {
			Dataset dataset = baseDatasets.get(i);
			datasetNames[i] = dataset == null ? "" : dataset.getName();
		}
		basedOnCombo.setItems(datasetNames);
	}
	
	/**
	 * Reads the value of the specified feature from the corresponding control.
	 */
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.DATASET__NAME:
			return nameText.getText();
		case ScaveModelPackage.DATASET__BASED_ON:
			int index = basedOnCombo.getSelectionIndex();
			return index >= 0 ? baseDatasets.get(index) : null;
		}
		return null;
	}

	/**
	 * Writes the value of a feature into the corresponding control.
	 */
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.DATASET__NAME:
			nameText.setText(value != null ? (String)value : "");
			break;
		case ScaveModelPackage.DATASET__BASED_ON:
			Dataset dataset = (Dataset)value;
			basedOnCombo.setText(dataset != null ? dataset.getName() : "");
			break;
		}
	}
}

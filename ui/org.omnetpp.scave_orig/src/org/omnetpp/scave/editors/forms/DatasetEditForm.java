/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.forms;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Edit form of datasets.
 *
 * @author tomi
 */
public class DatasetEditForm extends BaseScaveObjectEditForm {

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
    private Combo basedOnCombo;

    /**
     * Number of visible items in combos.
     */
    private static final int VISIBLE_ITEM_COUNT = 15;


    public DatasetEditForm(Dataset dataset, EObject parent) {
        super(dataset, parent);
        this.dataset = dataset;
        // collect datasets that can be the base of this dataset
        // XXX: circularity check is missing
        baseDatasets = new ArrayList<Dataset>();
        baseDatasets.add(null);
        for (Dataset ds : ScaveModelUtil.findObjects(parent.eResource(), Dataset.class))
            if (ds != dataset)
                baseDatasets.add(ds);
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
        basedOnCombo = new Combo(panel, SWT.BORDER | SWT.READ_ONLY);
        basedOnCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        basedOnCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
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

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.forms;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * Edit form of input files.
 *
 * @author tomi
 */
public class InputFileEditForm extends BaseScaveObjectEditForm {

    /**
     * Features edited on this panel.
     */
    private static final EStructuralFeature[] features = new EStructuralFeature[] {
        ScaveModelPackage.eINSTANCE.getInputFile_Name(),
    };

    /**
     * The edited input file.
     */
    //private InputFile inputFile;

    // edit controls of the features
    private Text nameText;

    public InputFileEditForm(InputFile inputFile, EObject parent) {
        super(inputFile, parent);
        //this.inputFile = inputFile;
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
        Label label = new Label(panel, SWT.NONE);
        label.setLayoutData(new GridData());
        label.setText("File name:");
        nameText = new Text(panel, SWT.BORDER);
        nameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
    }

    /**
     * Reads the value of the specified feature from the corresponding control.
     */
    public Object getValue(EStructuralFeature feature) {
        switch (feature.getFeatureID()) {
        case ScaveModelPackage.INPUT_FILE__NAME:
            return nameText.getText();
        }
        return null;
    }


    /**
     * Writes the value of a feature into the corresponding control.
     */
    public void setValue(EStructuralFeature feature, Object value) {
        switch (feature.getFeatureID()) {
        case ScaveModelPackage.INPUT_FILE__NAME:
            nameText.setText((String)value);
            break;
        }
    }
}

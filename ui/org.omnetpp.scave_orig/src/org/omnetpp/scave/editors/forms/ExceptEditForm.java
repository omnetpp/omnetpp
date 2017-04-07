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
import org.omnetpp.scave.editors.datatable.FilterField;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Except;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model.SetOperation;
import org.omnetpp.scave.model2.FilterHints;

/**
 * Edit form for Except nodes.
 *
 * @author tomi
 */
public class ExceptEditForm extends BaseScaveObjectEditForm {

    protected static final EStructuralFeature[] features = new EStructuralFeature[] {
        ScaveModelPackage.eINSTANCE.getExcept_FilterPattern(),
    };

    /**
     * The edited Except node.
     */
    protected Except except;

    /**
     * The parent of the Except node.
     * It is always included in the model.
     */
    protected EObject parent;

    /**
     * ResultFileManager used to get the filter hints.
     */
    protected ResultFileManager manager;

    // controls
    private FilterField filterField;
    private Text filterText;

    public ExceptEditForm(Except except, EObject parent, ResultFileManager manager) {
        super(except, parent);
        this.except = except;
        this.parent = parent;
        this.manager = manager;
    }

    public EStructuralFeature[] getFeatures() {
        return features;
    }

    public void populatePanel(Composite panel) {
        Label label;

        panel.setLayout(new GridLayout(2, false));

        label = new Label(panel, SWT.NONE);
        label.setText("Filter pattern:");
        label.setLayoutData(new GridData());
        filterField = new FilterField(panel, SWT.SINGLE | SWT.BORDER);
        filterField.getLayoutControl().setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        filterText = filterField.getText();

        filterText.setFocus();
        updateFilterHints();
    }

    private void updateFilterHints() {
        if (parent instanceof SetOperation) {
            ResultType datatype = ((SetOperation)parent).getType();
            if (datatype != null)
                filterField.setFilterHints(new FilterHints(manager, datatype));
        }
    }

    public Object getValue(EStructuralFeature feature) {
        switch (feature.getFeatureID()) {
        case ScaveModelPackage.EXCEPT__FILTER_PATTERN:
            return filterText.getText();
        default:
            throw new IllegalArgumentException("Unexpected feature: " + feature.getName());
        }

    }

    public void setValue(EStructuralFeature feature, Object value) {
        switch (feature.getFeatureID()) {
        case ScaveModelPackage.EXCEPT__FILTER_PATTERN:
            if (value != null) filterText.setText((String)value);
            break;
        default:
            throw new IllegalArgumentException("Unexpected feature: " + feature.getName());
        }
    }
}


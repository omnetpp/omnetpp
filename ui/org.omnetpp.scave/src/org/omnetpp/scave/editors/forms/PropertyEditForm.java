/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.forms;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.Property;

/**
 * Edit form of Property objects.
 *
 * @author andras
 */
public class PropertyEditForm extends BaseScaveObjectEditForm {

    /**
     * The edited Property.
     */
    //private Property property;

    // edit controls of the features
    private Text nameText;
    private Text valueText;

    public PropertyEditForm(Property property, ModelObject parent) {
        super(property);
        //this.property = property;
    }

    /**
     * Add edit controls the panel.
     */
    public void populatePanel(Composite panel) {
        panel.setLayout(new GridLayout(2, false));

        Label label = new Label(panel, SWT.NONE);
        label.setText("Name:");
        nameText = new Text(panel, SWT.BORDER);
        nameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        label = new Label(panel, SWT.NONE);
        label.setText("Value:");
        valueText = new Text(panel, SWT.BORDER);
        valueText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

    }
}

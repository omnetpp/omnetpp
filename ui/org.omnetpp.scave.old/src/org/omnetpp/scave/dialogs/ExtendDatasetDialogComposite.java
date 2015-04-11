/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.dialogs;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;

public class ExtendDatasetDialogComposite extends Composite {

    Button selectAllButton = null;
    Button deselectAllButton = null;
    Button invertSelectionButton = null;
    Group group = null;
    Button showAllRadio = null;
    Button showMissingRadio = null;
    public ExtendDatasetDialogComposite(Composite parent, int style) {
        super(parent, style);
        initialize();
    }

    private void initialize() {
        createGroup();
        GridData gridData2 = new GridData();
        gridData2.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridData gridData1 = new GridData();
        gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridData gridData = new GridData(SWT.FILL,SWT.TOP,false,false);
        gridData.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        selectAllButton = new Button(this, SWT.NONE);
        selectAllButton.setText("Select All");
        selectAllButton.setLayoutData(gridData);
        deselectAllButton = new Button(this, SWT.NONE);
        deselectAllButton.setText("Deselect All");
        deselectAllButton.setLayoutData(gridData1);
        invertSelectionButton = new Button(this, SWT.NONE);
        invertSelectionButton.setText("Invert Selection");
        invertSelectionButton.setLayoutData(gridData2);
        this.setLayout(new GridLayout());
    }

    /**
     * This method initializes group
     *
     */
    private void createGroup() {
        GridData gridData3 = new GridData();
        gridData3.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        group = new Group(this, SWT.NONE);
        group.setText("Show");
        group.setLayoutData(gridData3);
        group.setLayout(new GridLayout());
        showAllRadio = new Button(group, SWT.RADIO);
        showAllRadio.setText("all");
        showMissingRadio = new Button(group, SWT.RADIO);
        showMissingRadio.setText("missing");
    }

}  //  @jve:decl-index=0:visual-constraint="10,10"

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.dialogs;

import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.layout.GridData;

public class BarsPreferencePanel extends Composite {

    private Label label = null;
    private Label label1 = null;
    private Text text = null;
    private Combo combo = null;
    public BarsPreferencePanel(Composite parent, int style) {
        super(parent, style);
        initialize();
    }

    private void initialize() {
        GridData gridData = new GridData();
        gridData.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 2;
        gridLayout.horizontalSpacing = 10;
        gridLayout.marginWidth = 10;
        gridLayout.marginHeight = 20;
        label = new Label(this, SWT.NONE);
        label.setText("Baseline:");
        text = new Text(this, SWT.BORDER);
        text.setLayoutData(gridData);
        label1 = new Label(this, SWT.NONE);
        label1.setText("Bar placement");
        this.setLayout(gridLayout);
        createCombo();
        setSize(new Point(526, 358));
    }

    /**
     * This method initializes combo
     *
     */
    private void createCombo() {
        GridData gridData1 = new GridData();
        gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        combo = new Combo(this, SWT.NONE);
        combo.setLayoutData(gridData1);
    }

}  //  @jve:decl-index=0:visual-constraint="10,10"

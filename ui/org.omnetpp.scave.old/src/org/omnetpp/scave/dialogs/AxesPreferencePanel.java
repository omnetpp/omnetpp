/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.dialogs;

import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Button;

public class AxesPreferencePanel extends Composite {

    private Group boundsGroup = null;
    private Group optionsGroup = null;
    private Group gridGroup = null;
    private Label xAxisBoundsLabel = null;
    private Label xMinLabel = null;
    private Text xMinText = null;
    private Label xMaxLabel = null;
    private Text xMaxText = null;
    private Label yBoundsLabel = null;
    private Label yMinLabel = null;
    private Text text = null;
    private Text text1 = null;
    private Label label = null;
    private Label label1 = null;
    private Button checkBox = null;
    private Button checkBox1 = null;
    private Label label2 = null;
    private Button checkBox2 = null;
    private Label label3 = null;
    private Button radioButton = null;
    private Label label4 = null;
    private Button radioButton1 = null;
    private Button radioButton2 = null;
    private Label label5 = null;
    private Label label6 = null;

    public AxesPreferencePanel(Composite parent, int style) {
        super(parent, style);
        initialize();
    }

    private void initialize() {
        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 2;
        gridLayout.verticalSpacing = 40;
        gridLayout.marginHeight = 20;
        createBoundsGroup();
        this.setLayout(gridLayout);
        createOptionsGroup();
        createGridGroup();
        setSize(new Point(529, 362));
    }

    /**
     * This method initializes boundsGroup
     *
     */
    private void createBoundsGroup() {
        GridData gridData4 = new GridData();
        gridData4.grabExcessHorizontalSpace = true;
        GridData gridData3 = new GridData();
        gridData3.grabExcessHorizontalSpace = true;
        GridLayout gridLayout1 = new GridLayout();
        gridLayout1.numColumns = 5;
        gridLayout1.horizontalSpacing = 10;
        GridData gridData = new GridData();
        gridData.horizontalSpan = 2;
        gridData.grabExcessHorizontalSpace = true;
        gridData.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        boundsGroup = new Group(this, SWT.NONE);
        boundsGroup.setText("Bounds");
        boundsGroup.setLayout(gridLayout1);
        boundsGroup.setLayoutData(gridData);
        xAxisBoundsLabel = new Label(boundsGroup, SWT.NONE);
        xAxisBoundsLabel.setText("X axis");
        xMinLabel = new Label(boundsGroup, SWT.NONE);
        xMinLabel.setText("Min:");
        xMinText = new Text(boundsGroup, SWT.BORDER);
        xMinText.setLayoutData(gridData3);
        xMaxLabel = new Label(boundsGroup, SWT.NONE);
        xMaxLabel.setText("Max:");
        xMaxText = new Text(boundsGroup, SWT.BORDER);
        xMaxText.setLayoutData(gridData4);
        yBoundsLabel = new Label(boundsGroup, SWT.NONE);
        yBoundsLabel.setText("Label");
        yMinLabel = new Label(boundsGroup, SWT.NONE);
        yMinLabel.setText("Label");
        text = new Text(boundsGroup, SWT.BORDER);
        label = new Label(boundsGroup, SWT.NONE);
        label.setText("Label");
        text1 = new Text(boundsGroup, SWT.BORDER);
    }

    /**
     * This method initializes optionsGroup
     *
     */
    private void createOptionsGroup() {
        GridLayout gridLayout2 = new GridLayout();
        gridLayout2.numColumns = 2;
        GridData gridData1 = new GridData();
        gridData1.grabExcessHorizontalSpace = true;
        gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        optionsGroup = new Group(this, SWT.NONE);
        optionsGroup.setText("Options");
        optionsGroup.setLayout(gridLayout2);
        optionsGroup.setLayoutData(gridData1);
        checkBox = new Button(optionsGroup, SWT.CHECK);
        label1 = new Label(optionsGroup, SWT.NONE);
        label1.setText("logarithmic X axis");
        checkBox1 = new Button(optionsGroup, SWT.CHECK);
        label2 = new Label(optionsGroup, SWT.NONE);
        label2.setText("logarithmic Y axis");
        checkBox2 = new Button(optionsGroup, SWT.CHECK);
        label3 = new Label(optionsGroup, SWT.NONE);
        label3.setText("invert X,Y");
    }

    /**
     * This method initializes gridGroup
     *
     */
    private void createGridGroup() {
        GridLayout gridLayout3 = new GridLayout();
        gridLayout3.numColumns = 2;
        GridData gridData2 = new GridData();
        gridData2.grabExcessHorizontalSpace = true;
        gridData2.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        gridGroup = new Group(this, SWT.NONE);
        gridGroup.setText("Grid");
        gridGroup.setLayout(gridLayout3);
        gridGroup.setLayoutData(gridData2);
        radioButton = new Button(gridGroup, SWT.RADIO);
        label4 = new Label(gridGroup, SWT.NONE);
        label4.setText("none");
        radioButton1 = new Button(gridGroup, SWT.RADIO);
        label5 = new Label(gridGroup, SWT.NONE);
        label5.setText("at major ticks");
        radioButton2 = new Button(gridGroup, SWT.RADIO);
        label6 = new Label(gridGroup, SWT.NONE);
        label6.setText("at all ticks");
    }

}  //  @jve:decl-index=0:visual-constraint="10,10"

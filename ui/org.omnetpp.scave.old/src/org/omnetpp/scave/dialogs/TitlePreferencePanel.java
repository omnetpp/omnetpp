/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.dialogs;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Combo;

public class TitlePreferencePanel extends Composite {

    private Group titleGroup = null;
    private Label label = null;
    private Group axisGroup = null;
    private Text titleText = null;
    private Label titleFontLabel = null;
    private Text titleFontText = null;
    private Label xAxisTextLabel = null;
    private Text xAxisText = null;
    private Label yAxisTextLabel = null;
    private Text yAxisText = null;
    private Label axisFontLabel = null;
    private Text axisFontText = null;
    private Label labelFontLabel = null;
    private Text labelFontText = null;
    private Label rotateLabelsLabel = null;
    private Combo rotateLabelCombo = null;

    public TitlePreferencePanel(Composite parent, int style) {
        super(parent, style);
        initialize();
    }

    private void initialize() {
        GridLayout gridLayout2 = new GridLayout();
        gridLayout2.verticalSpacing = 40;
        gridLayout2.marginHeight = 20;
        createTitleGroup();
        this.setLayout(gridLayout2);
        createAxisGroup();
        setSize(new Point(545, 348));
    }

    /**
     * This method initializes titleGroup
     *
     */
    private void createTitleGroup() {
        GridData gridData11 = new GridData();
        gridData11.grabExcessHorizontalSpace = true;
        gridData11.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridData gridData2 = new GridData();
        gridData2.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridData gridData1 = new GridData();
        gridData1.grabExcessHorizontalSpace = true;
        gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridData gridData = new GridData();
        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 2;
        gridLayout.horizontalSpacing = 10;
        gridLayout.marginHeight = 5;
        titleGroup = new Group(this, SWT.NONE);
        titleGroup.setText("Title");
        titleGroup.setLayoutData(gridData11);
        titleGroup.setLayout(gridLayout);
        label = new Label(titleGroup, SWT.NONE);
        label.setText("Text:");
        label.setLayoutData(gridData);
        titleText = new Text(titleGroup, SWT.BORDER);
        titleText.setLayoutData(gridData1);
        titleFontLabel = new Label(titleGroup, SWT.NONE);
        titleFontLabel.setText("Font:");
        titleFontText = new Text(titleGroup, SWT.BORDER);
        titleFontText.setLayoutData(gridData2);
    }

    /**
     * This method initializes axisGroup
     *
     */
    private void createAxisGroup() {
        GridData gridData7 = new GridData();
        gridData7.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridData gridData6 = new GridData();
        gridData6.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridData gridData5 = new GridData();
        gridData5.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridData gridData4 = new GridData();
        gridData4.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridData gridData3 = new GridData();
        gridData3.grabExcessHorizontalSpace = true;
        gridData3.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        GridLayout gridLayout1 = new GridLayout();
        gridLayout1.numColumns = 2;
        gridLayout1.horizontalSpacing = 10;
        axisGroup = new Group(this, SWT.NONE);
        axisGroup.setText("Axis titles");
        axisGroup.setLayoutData(gridData6);
        axisGroup.setLayout(gridLayout1);
        xAxisTextLabel = new Label(axisGroup, SWT.NONE);
        xAxisTextLabel.setText("X axis text:");
        xAxisText = new Text(axisGroup, SWT.BORDER);
        xAxisText.setLayoutData(gridData3);
        yAxisTextLabel = new Label(axisGroup, SWT.NONE);
        yAxisTextLabel.setText("Y axis text:");
        yAxisText = new Text(axisGroup, SWT.BORDER);
        yAxisText.setLayoutData(gridData4);
        axisFontLabel = new Label(axisGroup, SWT.NONE);
        axisFontLabel.setText("Axis font:");
        axisFontText = new Text(axisGroup, SWT.BORDER);
        axisFontText.setLayoutData(gridData5);
        labelFontLabel = new Label(axisGroup, SWT.NONE);
        labelFontLabel.setText("Label font:");
        labelFontText = new Text(axisGroup, SWT.BORDER);
        labelFontText.setLayoutData(gridData7);
        rotateLabelsLabel = new Label(axisGroup, SWT.NONE);
        rotateLabelsLabel.setText("Rotate labels");
        createRotateLabelCombo();
    }

    /**
     * This method initializes rotateLabelCombo
     *
     */
    private void createRotateLabelCombo() {
        rotateLabelCombo = new Combo(axisGroup, SWT.READ_ONLY);
        rotateLabelCombo.setText("0");
    }

    public String getTitleText() {
        return titleText.getText();
    }

    public void setTitleText(String text) {
        titleText.setText(text);
    }

    public String getXAxisText() {
        return xAxisText.getText();
    }

    public void setXAxisText(String text) {
        xAxisText.setText(text);
    }

    public String getYAxisText() {
        return yAxisText.getText();
    }

    public void setYAxisText(String text) {
        yAxisText.setText(text);
    }


}  //  @jve:decl-index=0:visual-constraint="10,2"

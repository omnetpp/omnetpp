/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.panel;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;


public class ScalarPanelComposite extends FilterPanelComposite {

    private Label runNameLabel = null;
    private Label moduleNameLabel = null;
    private Label scalarNameLabel = null;

    public CCombo runNameCombo = null;
    public CCombo moduleNameCombo = null;
    public CCombo scalarNameCombo = null;

    private Label dummy = null;
    public Button filterButton = null;

    public Table table = null;

    public ScalarPanelComposite(Composite parent, int style) {
        super(parent, style);
        initialize();
    }

    public Button getFilterButton() {
        return filterButton;
    }

    public CCombo getModuleNameCombo() {
        return moduleNameCombo;
    }

    public CCombo getRunNameCombo() {
        return runNameCombo;
    }

    public CCombo getNameCombo() {
        return scalarNameCombo;
    }

    public Table getTable() {
        return table;
    }

    private void initialize() {
        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 4;
        this.setLayout(gridLayout);
        GridData gridData3 = new GridData();
        gridData3.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        gridData3.grabExcessHorizontalSpace = true;
        GridData gridData2 = new GridData();
        gridData2.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        gridData2.grabExcessHorizontalSpace = true;
        GridData gridData1 = new GridData();
        gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        gridData1.grabExcessHorizontalSpace = true;
        GridData gridData = new GridData();
        runNameLabel = new Label(this, SWT.NONE);
        runNameLabel.setText("File name and run number:");
        runNameLabel.setLayoutData(gridData);
        moduleNameLabel = new Label(this, SWT.NONE);
        moduleNameLabel.setText("Module name:");
        scalarNameLabel = new Label(this, SWT.NONE);
        scalarNameLabel.setText("Scalar name:");
        dummy = new Label(this, SWT.NONE);
        runNameCombo = new CCombo(this, SWT.BORDER);
        runNameCombo.setVisibleItemCount(10);
        runNameCombo.setToolTipText("Filter displayed items by file and run. Wildcards *,? are OK.");
        runNameCombo.setLayoutData(gridData1);
        moduleNameCombo = new CCombo(this, SWT.BORDER);
        moduleNameCombo.setVisibleItemCount(10);
        moduleNameCombo.setToolTipText("Filter displayed items by module name. Wildcards *,? are OK.");
        moduleNameCombo.setLayoutData(gridData2);
        scalarNameCombo = new CCombo(this, SWT.BORDER);
        scalarNameCombo.setVisibleItemCount(10);
        scalarNameCombo.setToolTipText("Filter lines shown by the scalar's name. Wildcards *,? are OK.");
        scalarNameCombo.setLayoutData(gridData3);
        filterButton = new Button(this, SWT.NONE);
        filterButton.setText("Filter");
        createTable();
    }

    /**
     * This method initializes table
     *
     */
    private void createTable() {
        GridData gridData4 = new GridData();
        gridData4.grabExcessHorizontalSpace = true;
        gridData4.grabExcessVerticalSpace = true;
        gridData4.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        gridData4.verticalAlignment = org.eclipse.swt.layout.GridData.FILL;
        gridData4.horizontalSpan = 4;
        gridData4.heightHint = 200;
        table = new Table(this, SWT.MULTI | SWT.FULL_SELECTION | SWT.VIRTUAL);
        table.setHeaderVisible(true);
        table.setLayoutData(gridData4);
        table.setLinesVisible(true);
        TableColumn tableColumn = new TableColumn(table, SWT.NONE);
        tableColumn.setWidth(60);
        tableColumn.setText("Directory");
        TableColumn tableColumn1 = new TableColumn(table, SWT.NONE);
        tableColumn1.setWidth(80);
        tableColumn1.setText("File");
        TableColumn tableColumn2 = new TableColumn(table, SWT.NONE);
        tableColumn2.setWidth(80);
        tableColumn2.setText("Run#");
        TableColumn tableColumn3 = new TableColumn(table, SWT.NONE);
        tableColumn3.setWidth(160);
        tableColumn3.setText("Module");
        TableColumn tableColumn4 = new TableColumn(table, SWT.NONE);
        tableColumn4.setWidth(100);
        tableColumn4.setText("Name");
        TableColumn tableColumn5 = new TableColumn(table, SWT.NONE);
        tableColumn5.setWidth(80);
        tableColumn5.setText("Value");
    }
}  //  @jve:decl-index=0:visual-constraint="15,14"

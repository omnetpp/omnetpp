/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.dialogs;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.model.IDListModel;
import org.omnetpp.scave.model.IDatasetStrategy;
import org.omnetpp.scave.panel.FilterPanel;

/**
 * Dialog to add items to a given dataset.
 */
public class ExtendDatasetDialog extends Dialog {

    // Allows us common handling of scalars and vectors
    private IDatasetStrategy strategy;

    // data and components of the dialog
    private IDListModel dataset = new IDListModel();
    private FilterPanel filterPanel;
    private ExtendDatasetDialogComposite rightPanel;

    // the dataset we're adding to
    private IDListModel subjectDataset;

    /**
     * This method initializes the dialog
     */
    public ExtendDatasetDialog(Shell parentShell, IDatasetStrategy strategy) {
        super(parentShell);
        this.strategy = strategy;
        setShellStyle(getShellStyle()|SWT.RESIZE);
    }

    public void dispose() {
        //super.dispose(); -- there isn't such thing?
        filterPanel.dispose();
    }

    /* (non-Javadoc)
     * Method declared on Window.
     */
    protected void configureShell(Shell newShell) {
        super.configureShell(newShell);
        newShell.setText("Extend Dataset");
        //PlatformUI.getWorkbench().getHelpSystem().setHelp(newShell, IReadmeConstants.SECTIONS_DIALOG_CONTEXT);
    }

    /* (non-Javadoc)
     * Method declared on Dialog
     */
    protected Control createDialogArea(Composite parent) {
        Composite composite = (Composite) super.createDialogArea(parent);
        ((GridLayout)composite.getLayout()).numColumns = 2;

        // add filter panel
        filterPanel = strategy.createFilterPanel(composite, SWT.NONE);
        filterPanel.getPanel().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        filterPanel.setEditingAllowed(false);
        filterPanel.setDataset(dataset);

        // on double-click, add selected item to dataset
        filterPanel.getPanel().getTable().addSelectionListener(new SelectionAdapter() {
            public void widgetDefaultSelected(SelectionEvent e) {
                addSelection();
            }});

        // add buttons panel
        rightPanel = new ExtendDatasetDialogComposite(composite, SWT.None);
        rightPanel.setLayoutData(new GridData(SWT.FILL,SWT.TOP, false, true));

        // configure buttons in panel
        rightPanel.selectAllButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                filterPanel.selectAll();
                filterPanel.getPanel().getTable().setFocus();
            }});
        rightPanel.deselectAllButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                filterPanel.deselectAll();
                filterPanel.getPanel().getTable().setFocus();
            }});
        rightPanel.invertSelectionButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                filterPanel.invertSelection();
                filterPanel.getPanel().getTable().setFocus();
            }});
        rightPanel.showAllRadio.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                dataset.set(strategy.getAll());
                filterPanel.getPanel().getTable().setFocus();
            }});
        rightPanel.showMissingRadio.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                IDList idList = strategy.getAll();
                idList.substract(subjectDataset.get());
                dataset.set(idList);
                filterPanel.getPanel().getTable().setFocus();
            }});

        // by default, all are shown
        dataset.set(strategy.getAll());
        rightPanel.showAllRadio.setSelection(true);

        // focus on the table
        filterPanel.getPanel().getTable().setFocus();

        return composite;
    }

    /**
     * This is the dataset that should be extended from this dialog.
     */
    public void setDatasetToExtend(IDListModel dataset) {
        subjectDataset = dataset;
    }

    /**
     * Mapped to the Add button.
     */
    private void addSelection() {
        IDList sel = filterPanel.getSelectedOrAllItems();
        subjectDataset.merge(sel);
    }

    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, 1, "Add", true);
        createButton(parent, 2, "Close", false);
    }

    protected void buttonPressed(int buttonId) {
        if (buttonId==1) {
            addSelection();
            filterPanel.getPanel().getTable().setFocus();
        }
        else if (buttonId==2) {
            close();
        }
    }

}

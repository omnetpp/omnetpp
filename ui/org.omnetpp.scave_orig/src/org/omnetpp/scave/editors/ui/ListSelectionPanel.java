/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.List;

/**
 * Panel for choosing multiple elements from a list.
 *
 * When created the left list is filled with the selectable elements.
 * The selected elements are shown in the right list.
 *
 * Elements can be moved between the two list with the "Add", "Add all",
 * "Remove", "Remove all" buttons.
 *
 * @author tomi
 */
public class ListSelectionPanel extends Composite {

    // List controls
    private List leftList = null;
    private List rightList = null;

    /**
     * List of all items, i.e. union of the contents of the left and right list.
     */
    private java.util.List<String> items;

    public ListSelectionPanel(
            Composite parent,
            int style,
            java.util.List<String> items) {
        super(parent, style);
        this.items = items;
        initialize();
    }

    /**
     * Returns the selected items, i.e. the content of the right list.
     */
    public String[] getSelectedItems() {
        return rightList.getItems();
    }

    /**
     * Sets the selected items.
     */
    public void setSelectedItems(java.util.List<String> items) {
        java.util.List<String> leftItems = new ArrayList<String>(this.items);
        leftItems.removeAll(items);
        leftList.setItems(leftItems.toArray(new String[leftItems.size()]));
        rightList.setItems(items.toArray(new String[items.size()]));
    }

    /**
     * Create child controls.
     */
    // XXX layout
    private void initialize() {
        GridLayout gridLayout = new GridLayout();
        gridLayout.numColumns = 3;
        this.setLayout(gridLayout);

        leftList = new List(this, SWT.MULTI | SWT.BORDER);
        GridData gridData1 = new GridData();
        gridData1.grabExcessHorizontalSpace = true;
        gridData1.grabExcessVerticalSpace = true;
        gridData1.verticalAlignment = org.eclipse.swt.layout.GridData.FILL;
        gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        leftList.setLayoutData(gridData1);
        leftList.setItems(items.toArray(new String[items.size()]));

        Composite buttonPanel = new Composite(this, SWT.NONE);
        GridData gridData2 = new GridData();
        gridData2.grabExcessVerticalSpace = true;
        gridData2.verticalAlignment = org.eclipse.swt.layout.GridData.FILL;
        buttonPanel.setLayoutData(gridData2);
        buttonPanel.setLayout(new GridLayout());

        Button addAllButton = new Button(buttonPanel, SWT.NONE);
        addAllButton.setText(">>");
        addAllButton.setToolTipText("Add All");
        GridData gridData3 = new GridData();
        gridData3.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        addAllButton.setLayoutData(gridData3);
        addAllButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                move(leftList, rightList, true);
            }
        });

        Button addButton = new Button(buttonPanel, SWT.NONE);
        addButton.setText(">");
        addButton.setToolTipText("Add");
        GridData gridData4 = new GridData();
        gridData4.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        addButton.setLayoutData(gridData4);
        addButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                move(leftList, rightList, false);
            }
        });

        Button removeButton = new Button(buttonPanel, SWT.NONE);
        removeButton.setText("<");
        removeButton.setToolTipText("Remove");
        GridData gridData5 = new GridData();
        gridData5.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        removeButton.setLayoutData(gridData5);
        removeButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                move(rightList, leftList, false);
            }
        });

        Button removeAllButton = new Button(buttonPanel, SWT.NONE);
        removeAllButton.setText("<<");
        removeAllButton.setToolTipText("Remove All");
        GridData gridData6 = new GridData();
        gridData6.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        removeAllButton.setLayoutData(gridData6);
        removeAllButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                move(rightList, leftList, true);
            }
        });

        rightList = new List(this, SWT.MULTI | SWT.BORDER);
        GridData gridData = new GridData();
        gridData.grabExcessHorizontalSpace = true;
        gridData.grabExcessVerticalSpace = true;
        gridData.verticalAlignment = org.eclipse.swt.layout.GridData.FILL;
        gridData.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
        rightList.setLayoutData(gridData);
    }

    /**
     * Moves selected or all items from <code>fromList</code> to <code>toList</code>.
     * The moved elements get selected in the <code>toList</code>.
     */
    private void move(List fromList, List toList, boolean all) {
        String[] items = all ? fromList.getItems() : fromList.getSelection();
        // remove items from fromList
        if (all)
            fromList.removeAll();
        else
            fromList.remove(fromList.getSelectionIndices());

        // move items to toList and select them
        toList.setRedraw(false);
        try {
            toList.deselectAll();
            for (String item : items) {
                toList.add(item);
                toList.select(toList.getItemCount() - 1);
            }
        } finally {
            toList.setRedraw(true);
        }
    }
}

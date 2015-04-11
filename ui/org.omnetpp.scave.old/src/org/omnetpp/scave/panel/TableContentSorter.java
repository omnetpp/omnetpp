/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.panel;

import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.ViewerSorter;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.TableColumn;

/**
 * This class implements sorting for a Table. Also good for virtual Tables
 * (SWT.VIRTUAL).
 */
public abstract class TableContentSorter extends ViewerSorter {

    private TableViewer tableViewer;
    private int column = -1; // not yet sorted
    private boolean ascending;

    /**
     * Makes the given table sortable, by adding selection listeners
     * to every column.
     */
    public static void makeTableSortable(final TableViewer viewer, final TableContentSorter sorter) {
        viewer.setSorter(sorter);
        sorter.tableViewer = viewer;
        TableColumn[] columns = viewer.getTable().getColumns();
        for (int i=0; i<columns.length; i++) {
            final int index = i;
            columns[i].addSelectionListener(new SelectionAdapter() {
                public void widgetSelected(SelectionEvent event) {
                    long t0 = System.currentTimeMillis();
                    sorter.sortByColumn(index);
                    System.out.println("sorting: " + (System.currentTimeMillis()-t0));
                    viewer.refresh();
                }
            });
        }
    }

    /**
     * Does the sort. If it's a different column from the previous sort, do an
     * ascending sort. If it's the same column as the last sort, toggle the sort
     * direction.
     */
    protected void sortByColumn(int column) {
        if (column == this.column) {
            // Same column as last sort; toggle the direction
            ascending = !ascending;
            reverse(tableViewer, column);
        }
        else {
            // New column; do an ascending sort
            this.column = column;
            ascending = true;
            sort(tableViewer, column, ascending);
        }
    }

    /**
     * Implement the sorting here.
     */
    public abstract void sort(TableViewer tableViewer, int column, boolean ascending);

    /**
     * Reverse order of dataset, sorting not necessary.
     */
    public abstract void reverse(TableViewer tableViewer, int column);
}

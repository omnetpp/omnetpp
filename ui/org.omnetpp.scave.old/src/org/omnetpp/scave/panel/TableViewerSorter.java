/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.panel;

import org.eclipse.jface.viewers.ContentViewer;
import org.eclipse.jface.viewers.IBaseLabelProvider;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerSorter;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.TableColumn;

import org.omnetpp.scave.engine.ScaveEngine;

/**
 * This class implements the sorting for a Table.
 *
 * Note: currently unused because it's not compatible with VIRTUAL tables.
 */
public class TableViewerSorter extends ViewerSorter {

    private static final int ASCENDING = 0;
    private static final int DESCENDING = 1;

    private int column;
    private int direction;

    /**
     * Makes the given table sortable, by adding selection listeners
     * to every column.
     */
    public static void makeTableSortable(final TableViewer viewer, final TableViewerSorter sorter) {
        viewer.setSorter(sorter);
        TableColumn[] columns = viewer.getTable().getColumns();
        for (int i=0; i<columns.length; i++) {
            final int index = i;
            columns[i].addSelectionListener(new SelectionAdapter() {
                public void widgetSelected(SelectionEvent event) {
                    System.out.println("Sort by:"+index);
                    sorter.setSortColumn(index);
                    viewer.refresh();
                }
            });
        }
    }

    /**
     * Does the sort. If it's a different column from the previous sort, do an
     * ascending sort. If it's the same column as the last sort, toggle the sort
     * direction.
     *
     * @param column
     */
    public void setSortColumn(int column) {
        if (column == this.column) {
            // Same column as last sort; toggle the direction
            direction = 1 - direction;
        }
        else {
            // New column; do an ascending sort
            this.column = column;
            direction = ASCENDING;
        }
    }

    /**
     * Compares the object for sorting. Note: this method's code is a copy of
     * ViewerSorter's, with the addition of the ITableLabelProvider branch.
     *
     * NOTE: Collator is not used! We use ScaveEngine.strdictcmp() instead.
     *
     * FIXME numeric columns are also sorted alphabetically :(
     */
    public int compare(Viewer viewer, Object e1, Object e2) {

        int cat1 = category(e1);
        int cat2 = category(e2);

        if (cat1 != cat2)
            return cat1 - cat2;

        // cat1 == cat2

        String name1;
        String name2;

        if (viewer == null || !(viewer instanceof ContentViewer)) {
            name1 = e1.toString();
            name2 = e2.toString();
        }
        else {
            IBaseLabelProvider prov = ((ContentViewer) viewer).getLabelProvider();
            if (prov instanceof ITableLabelProvider) {
                // this is the addition compared to ViewerSorter
                ITableLabelProvider lprov = (ITableLabelProvider) prov;
                name1 = lprov.getColumnText(e1, column);
                name2 = lprov.getColumnText(e2, column);
            }
            else if (prov instanceof ILabelProvider) {
                ILabelProvider lprov = (ILabelProvider) prov;
                name1 = lprov.getText(e1);
                name2 = lprov.getText(e2);
            }
            else {
                name1 = e1.toString();
                name2 = e2.toString();
            }
        }
        if (name1 == null)
            name1 = "";//$NON-NLS-1$
        if (name2 == null)
            name2 = "";//$NON-NLS-1$

        // If descending order, flip the direction
        // use our strdictcmp instead of collator.compare(name1, name2);
        if (direction == DESCENDING)
            return ScaveEngine.strdictcmp(name2, name1);
        else
            return ScaveEngine.strdictcmp(name1, name2);
    }
}

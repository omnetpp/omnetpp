/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.VectorResult;

/**
 * Represents a selection of result item IDs in the editor.
 *
 * Note: This class is intentionally NOT an IStructuredSelection. Calls IStructuredSelection.toArray()
 * can cause serious performance issues when IDList contains many thousands of IDs.
 */
public class IDListSelection implements ISelection {
    private IDList idlist;
    private ResultFileManager manager;
    private FilteredDataPanel source; // may be null

    public IDListSelection(IDList idlist, ResultFileManager manager, FilteredDataPanel source) {
        Assert.isNotNull(idlist);
        Assert.isNotNull(manager);

        this.idlist = idlist;
        this.manager = manager;
        this.source = source;
    }

    public IDListSelection(long id, ResultFileManager manager) {
        Assert.isNotNull(manager);

        this.idlist = new IDList(id);
        this.manager = manager;
    }

    public IDList getIDList() {
        return idlist;
    }

    public ResultFileManager getResultFileManager() {
        return manager;
    }

    public FilteredDataPanel getSource() {
        return source;
    }

    public int size() {
        return idlist.size();
    }

    public boolean isEmpty() {
        return idlist.isEmpty();
    }

    public VectorResult getFirstAsVector() {
        if (!idlist.isEmpty()) {
            ResultItem item = manager.getItem(idlist.get(0));
            if (item instanceof VectorResult)
                return (VectorResult)item;
        }
        return null;
    }

    public int getScalarsCount() {
        return idlist.countByTypes(ResultFileManager.SCALAR);
    }

    public int getParametersCount() {
        return idlist.countByTypes(ResultFileManager.PARAMETER);
    }

    public int getVectorsCount() {
        return idlist.countByTypes(ResultFileManager.VECTOR);
    }

    public int getStatisticsCount() {
        return idlist.countByTypes(ResultFileManager.STATISTICS);
    }

    public int getHistogramsCount() {
        return idlist.countByTypes(ResultFileManager.HISTOGRAM);
    }

    public boolean equals(Object other) {
        if (this == other)
            return true;
        if (other == null || getClass() != other.getClass())
            return false;
        IDListSelection otherSelection = (IDListSelection)other;
        return manager == otherSelection.manager && idlist.equals(otherSelection.idlist);
    }

    @Override
    public int hashCode() {
        return 31 * manager.hashCode() + idlist.hashCode();
    }

    @Override
    public String toString() {
        return getClass().getSimpleName() + "(len=" + idlist.size() + ")";
    }

}

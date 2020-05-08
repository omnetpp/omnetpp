/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model.ResultType;

/**
 * Represents a selection of result item IDs in the editor.
 *
 * Note: This class is intentionally NOT an IStructuredSelection. Calls IStructuredSelection.toArray()
 * can cause serious performance issues when IDList contains many thousands of IDs.
 */
public class IDListSelection implements ISelection {
    IDList idlist;
    ResultFileManager manager;
    ResultType type;

    public IDListSelection(IDList idlist, ResultFileManager manager) {
        Assert.isNotNull(idlist);
        Assert.isNotNull(manager);

        this.idlist = idlist;
        this.manager = manager;
        if (idlist.areAllScalars())
            type = ResultType.SCALAR;
        if (idlist.areAllParameters())
            type = ResultType.PARAMETER;
        else if (idlist.areAllVectors())
            type = ResultType.VECTOR;
        else if (idlist.areAllHistograms())
            type = ResultType.HISTOGRAM;
        else
            type = null;
    }

    public IDListSelection(long id, ResultFileManager manager) {
        Assert.isNotNull(manager);

        this.idlist = new IDList(id);
        this.manager = manager;

        int internalType = ResultFileManager.getTypeOf(id);
        if (internalType == ResultFileManager.SCALAR)
            type = ResultType.SCALAR;
        if (internalType == ResultFileManager.PARAMETER)
            type = ResultType.PARAMETER;
        else if (internalType == ResultFileManager.VECTOR)
            type = ResultType.VECTOR;
        else if (internalType == ResultFileManager.STATISTICS)
            type = ResultType.STATISTICS;
        else if (internalType == ResultFileManager.HISTOGRAM)
            type = ResultType.HISTOGRAM;
    }

    public ResultFileManager getResultFileManager() {
        return manager;
    }

    public ResultType getResultType() {
        return type;
    }

    public VectorResult getFirstAsVector() {
        if (!idlist.isEmpty()) {
            ResultItem item = manager.getItem(idlist.get(0));
            if (item instanceof VectorResult)
                return (VectorResult)item;
        }
        return null;
    }

    public IDList getScalarIDs() {
        return idlist.filterByTypes(ResultFileManager.SCALAR); //TODO or self, if it's all scalars!
    }

    public IDList getParameterIDs() {
        return idlist.filterByTypes(ResultFileManager.PARAMETER); //TODO or self, if it's all scalars!
    }

    public IDList getVectorIDs() {
        return idlist.filterByTypes(ResultFileManager.VECTOR);
    }

    public IDList getStatisticIDs() {
        return idlist.filterByTypes(ResultFileManager.STATISTICS);
    }

    public IDList getHistogramIDs() {
        return idlist.filterByTypes(ResultFileManager.HISTOGRAM);
    }

    public int getScalarsCount() {
        return idlist.countByTypes(ResultFileManager.SCALAR);  //TODO or size() if it's all scalars, or 0 if it contains no scalar!
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

    public int size() {
        return idlist.size();
    }

    public IDList getIDList() {
        return idlist;
    }

    public boolean isEmpty() {
        return idlist.isEmpty();
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

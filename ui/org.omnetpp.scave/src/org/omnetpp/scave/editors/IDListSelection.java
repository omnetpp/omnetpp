/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.Arrays;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.common.Debug;
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
            type = ResultType.SCALAR_LITERAL;
        else if (idlist.areAllVectors())
            type = ResultType.VECTOR_LITERAL;
        else if (idlist.areAllHistograms())
            type = ResultType.HISTOGRAM_LITERAL;
        else
            type = null;
    }

    public IDListSelection(long id, ResultFileManager manager) {
        Assert.isNotNull(manager);

        this.idlist = new IDList();
        this.manager = manager;
        idlist.add(id);

        int internalType = ResultFileManager.getTypeOf(id);
        if (internalType == ResultFileManager.SCALAR)
            type = ResultType.SCALAR_LITERAL;
        else if (internalType == ResultFileManager.VECTOR)
            type = ResultType.VECTOR_LITERAL;
        else if (internalType == ResultFileManager.STATISTICS)
            type = ResultType.STATISTICS_LITERAL;
        else if (internalType == ResultFileManager.HISTOGRAM)
            type = ResultType.HISTOGRAM_LITERAL;
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
        Debug.println("UNIMPLEMENTED METHOD CALLED: IDListSelection.equals()");
        if (this == other)
            return true;
        if (other == null || getClass() != other.getClass())
            return false;
        IDListSelection otherSelection = (IDListSelection)other;
        boolean result = manager == otherSelection.manager && idlist.size() == otherSelection.size();
        Debug.println("IDListSelection.equals() done");
        return result; //TODO
    }

    public int hashCode() {
        Debug.println("INEFFICIENT METHOD CALLED: IDListSelection.hashCode()");
        return 31 * manager.hashCode() + Arrays.hashCode(idlist.toArray());  //TODO optimize if needed, or implement in C++
    }

}

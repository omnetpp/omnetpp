/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.NoSuchElementException;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model.ResultType;

/**
 * Represents a selection of result item IDs in the editor.
 */
//FIXME this should NOT be an IStructuredSelection!!! Then we could spare most methods,including toArray() and toList!!!
public class IDListSelection implements IStructuredSelection {
    IDList idlist;
    ResultFileManager manager;
    ResultType type;

    public IDListSelection(IDList idlist, ResultFileManager manager) { //TODO we do NOT copy the idlist!!! check call sites!
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

    public Object getFirstElement() {
        return idlist.isEmpty() ? null : idlist.get(0);
    }

    public int size() {
        return idlist.size();
    }

    public Object[] toArray() {
        Debug.println("INEFFICIENT METHOD CALLED: IDListSelection.toArray()");
        return idlist.toArray();
    }

    public List<Long> toList() {
        Debug.println("INEFFICIENT METHOD CALLED: IDListSelection.toList()");
        return Arrays.asList(idlist.toArray());
    }

    public IDList getIDList() {
        return idlist;
    }

    public boolean isEmpty() {
        return idlist.isEmpty();
    }

    @Override
    public Iterator<Long> iterator() {
        return new Iterator<Long>() {
            private int pos = 0;

            @Override
            public boolean hasNext() {
                return pos < idlist.size();
            }

            @Override
            public Long next() {
                if (pos >= idlist.size())
                    throw new NoSuchElementException();
                return idlist.get(pos++);
            }
        };
    }

    public boolean equals(Object other) {
        Debug.println("UNIMPLEMENTED METHOD CALLED: IDListSelection.equals()");
        if (this == other)
            return true;
        if (other == null || getClass() != other.getClass())
            return false;
        IDListSelection otherSelection = (IDListSelection)other;
        return manager == otherSelection.manager && idlist.size() == otherSelection.size(); //TODO
    }

    public int hashCode() {
        Debug.println("INEFFICIENT METHOD CALLED: IDListSelection.hashCode()");
        return 31 * manager.hashCode() + Arrays.hashCode(idlist.toArray());  //TODO optimize if needed, or implement in C++
    }

}

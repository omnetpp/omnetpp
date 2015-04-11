/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Represents a selection of result item IDs in the editor.
 * The <code>ResultFileManager</code> is available from the
 * selection for the purpose of performing operations on the selected ids.
 *
 * @author tomi
 */
public class IDListSelection implements IStructuredSelection {

    Long[] elements;
    ResultFileManager manager;
    ResultType type;

    public IDListSelection(IDList idlist, ResultFileManager manager) {
        Assert.isNotNull(idlist);
        Assert.isNotNull(manager);

        this.elements = idlist.toArray();
        this.manager = manager;
        if (idlist.areAllScalars())
            type = ResultType.SCALAR_LITERAL;
        else if (idlist.areAllVectors())
            type = ResultType.VECTOR_LITERAL;
        else if (idlist.areAllHistograms())
            type = ResultType.HISTOGRAM_LITERAL;
    }

    public IDListSelection(long id, ResultFileManager manager) {
        Assert.isNotNull(manager);

        this.elements = new Long[] { id };
        this.manager = manager;
        int internalType = ResultFileManager.getTypeOf(id);
        if (internalType == ResultFileManager.SCALAR)
            type = ResultType.SCALAR_LITERAL;
        else if (internalType == ResultFileManager.VECTOR)
            type = ResultType.VECTOR_LITERAL;
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
        if (elements.length > 0) {
            ResultItem item = manager.getItem(elements[0]);
            if (item instanceof VectorResult)
                return (VectorResult)item;
        }
        return null;
    }

    public Long[] getScalarIDs() {
        return getIDs(ResultType.SCALAR_LITERAL);
    }

    public Long[] getVectorIDs() {
        return getIDs(ResultType.VECTOR_LITERAL);
    }

    public Long[] getHistogramIDs() {
        return getIDs(ResultType.HISTOGRAM_LITERAL);
    }

    private Long[] getIDs(ResultType type) {
        if (this.type == type)
            return elements;
        else {
            int type2 = ScaveModelUtil.asInternalResultType(type);
            List<Long> ids = new ArrayList<Long>();
            for (Long id : elements)
                if (ResultFileManager.getTypeOf(id) == type2)
                    ids.add(id);
            return ids.toArray(new Long[ids.size()]);
        }
    }

    public int getScalarsCount() {
        return getCount(ResultType.SCALAR_LITERAL);
    }

    public int getVectorsCount() {
        return getCount(ResultType.VECTOR_LITERAL);
    }

    public int getHistogramsCount() {
        return getCount(ResultType.HISTOGRAM_LITERAL);
    }

    private int getCount(ResultType type) {
        if (this.type == type)
            return elements.length;
        else {
            int type2 = ScaveModelUtil.asInternalResultType(type);
            int c = 0;
            for (Long id : elements)
                if (ResultFileManager.getTypeOf(id) == type2)
                    c++;
            return c;
        }
    }

    public Object getFirstElement() {
        return elements.length > 0 ? elements[0] : null;
    }

    public Iterator<Long> iterator() {
        return toList().iterator();
    }

    public int size() {
        return elements.length;
    }

    public Object[] toArray() {
        return elements;
    }

    public List<Long> toList() {
        return Arrays.asList(elements);
    }

    public IDList toIDList() {
        return IDList.fromArray(elements);
    }

    public boolean isEmpty() {
        return elements.length == 0;
    }

    public boolean equals(Object other) {
        if (this == other)
            return true;
        if (other == null || getClass() != other.getClass())
            return false;
        IDListSelection otherSelection = (IDListSelection)other;
        return manager == otherSelection.manager && Arrays.equals(elements, otherSelection.elements);
    }

    public int hashCode() {
        return 31 * manager.hashCode() + Arrays.hashCode(elements);
    }
}

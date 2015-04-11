/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.virtualtable;

import java.util.Iterator;
import java.util.List;

import org.omnetpp.common.collections.IEnumerator;
import org.omnetpp.common.collections.IRangeSet;
import org.omnetpp.common.collections.Range;
import org.omnetpp.common.collections.RangeSet;

/**
 * Represents a selection in the virtual table. Default implementation.
 */
public class VirtualTableSelection<T> implements IVirtualTableSelection<T> {
    protected Object input;

    protected IRangeSet<T> elements;

    public VirtualTableSelection(Object input, IRangeSet<T> elements) {
        this.input = input;
        this.elements = elements; // TODO copy
    }

    public VirtualTableSelection(Object input, List<T> elements) {
        this.input = input;
        this.elements = new RangeSet<T>();
        for (T e : elements)
            this.elements.add(e);
    }

    public VirtualTableSelection(Object input, List<T> elements, IEnumerator<T> enumerator) {
        this.input = input;
        this.elements = new RangeSet<T>(enumerator);
        for (T e : elements)
            this.elements.add(e);
    }

    public Object getInput() {
        return input;
    }

    public IRangeSet<T> getElements() {
        return elements;
    }

    public boolean isEmpty() {
        return elements == null || elements.size() == 0;
    }

    @Override
    public int getNumberOfRanges() {
        return elements.getNumberOfRanges();
    }

    @Override
    public Iterator<Range<T>> rangeIterator() {
        return elements.rangeIterator();
    }

    @Override
    public List<Range<T>> toRangeList() {
        return elements.toRangeList();
    }

    @Override
    public int getApproximateSize() {
        return elements.approximateSize();
    }
}

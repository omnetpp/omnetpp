package org.omnetpp.common.virtualtable;

import java.util.Iterator;
import java.util.List;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.collections.Range;

/**
 * A selection containing ranges of elements.
 *
 * @param E the type of elements
 *
 * @author tomi
 */
public interface IRangeSelection<E> extends ISelection {

    /**
     * Returns the number of selected ranges.
     */
    int getNumberOfRanges();

    /**
     * Returns an iterator over the selected ranges.
     */
    Iterator<Range<E>> rangeIterator();

    /**
     * Returns the list of selected ranges.
     */
    List<Range<E>> toRangeList();

    /**
     * Use this instead of {@link IStructuredSelection#size size()},
     * because {@code size()} will iterate on the elements in the worst case.
     */
    public int getApproximateSize();
}

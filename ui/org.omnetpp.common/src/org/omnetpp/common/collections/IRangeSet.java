package org.omnetpp.common.collections;

import java.util.Iterator;
import java.util.List;
import java.util.Set;

/**
 * A set that stores its elements as a set of non-overlapping ranges.
 *
 * @author tomi
 *
 * @param <E> the type of the elements of the set.
 */
public interface IRangeSet<E> extends Set<E> {

    /**
     * Returns the number of ranges in this set.
     */
    int getNumberOfRanges();

    /**
     * Returns the approximate size (number of elements) of this set.
     */
    int approximateSize();

    /**
     * Returns an iterator of the ranges in this set.
     *
     * Important: the iterator returns the instances stored in the IRangeSet,
     * so do not store or modify them!
     */
    Iterator<Range<E>> rangeIterator();

    /**
     * Returns the ranges of this set as a list.
     *
     * The ranges are a copy of the ranges stored in this set,
     * so they can be safely modified.
     */
    List<Range<E>> toRangeList();

    /**
     * Adds a new range to this set.
     *
     * @param first the first element of the new range
     * @param last the last element of the new range
     * @return true if the set has been changed
     */
    boolean addRange(E first, E last);

    /**
     * Removes a range from this set.
     *
     * @param first the first element of the removed range
     * @param last the last element of the removed range
     * @return true if the set has been changed
     */
    boolean removeRange(E first, E last);
}

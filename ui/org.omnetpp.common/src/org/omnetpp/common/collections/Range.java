package org.omnetpp.common.collections;

import java.util.Iterator;

import org.eclipse.core.runtime.Assert;

/**
 * Class that represents a range, i.e. all elements between two end-points.
 *
 * The range stores only the two extremal elements of the range, any element between
 * the two extremum is considered to be part of the range.
 *
 * Note: empty ranges can not be represented by instances of this class, use {@code null}
 * instead.
 *
 * @param <E> type of the elements
 *
 * @author tomi
 */
public class Range<E> implements Iterable<E> {

    /**
     * The enumerator used for iterating on the elements of the range.
     * It may be null if E implements IEnumerable
     */
    private final IEnumerator<E> enumerator;

    /** The first element of the range */
    private E first;

    /** The last element of the range */
    private E last;

    /**
     * Creates a new Range. The element type must implement IEnumerable.
     *
     * @param first the first element of the range
     * @param last the last element of the range
     */
    public Range(E first, E last) {
        Assert.isLegal(first instanceof IEnumerable, "Range: element type must implement IEnumerable");
        this.first = first;
        this.last = last;
        this.enumerator = null;
        Assert.isTrue(compare(first, last) <= 0, "Range: first > last");
    }

    /**
     * Creates a new Range. The elements are enumerated with the specified
     * enumerator.
     *
     * @param first the first element of the range
     * @param last the last element of the range
     * @param enumerator the enumerator of the elements, can be null if E implements IEnumerable
     */
    public Range(E first, E last, IEnumerator<E> enumerator) {
        this.first = first;
        this.last = last;
        this.enumerator = enumerator;
        Assert.isTrue(compare(first, last) <= 0, "Range: first > last");
    }

    /**
     * Creates a copy of this range.
     */
    public Range(Range<E> other) {
        this.first = other.first;
        this.last = other.last;
        this.enumerator = other.enumerator;
    }

    /**
     * Returns the first element of the range.
     */
    public E getFirst() {
        return first;
    }

    /**
     * Returns the last element of the range.
     */
    public E getLast() {
        return last;
    }

    /**
     * Returns true if the range contains the specified element.
     */
    public boolean contains(E element) {
        return compare(first, element) <= 0 && compare(element, last) <= 0;
    }

    /**
     * Returns true if this range contains all elements of the specified range.
     */
    public boolean contains(Range<E> range) {
        return compare(first, range.getFirst()) <= 0 && compare(range.getLast(), last) <= 0;
    }

    /**
     * Returns true if this range contains all elements in the [first,last] range.
     */
    public boolean containedBy(E first, E last) {
        Assert.isTrue(compare(first, last) <= 0);
        return compare(first, this.first) <= 0 && compare(this.last, last) <= 0;
    }

    /**
     * Returns true if this range has common elements with the specified range.
     */
    public boolean overlaps(Range<E> range) {
        return overlaps(range.getFirst(), range.getLast());
    }

    /**
     * Returns true if this range has common elements with the [first,last] range.
     */
    public boolean overlaps(E first, E last) {
        Assert.isTrue(compare(first, last) <= 0);
        return compare(this.last, first) >= 0 && compare(last, this.first) >= 0;
    }

    /**
     * Returns true if the specified range can be added to this range, so that
     * the result is one range, i.e. if they overlap or they are adjacent.
     */
    public boolean canAdd(Range<E> range) {
        return this.overlaps(range) || this.getFirst().equals(next(range.getLast())) || this.getLast().equals(prev(range.getFirst()));
    }

    /**
     * Adds the elements of range to this range.
     * It fails, if the result can not be represented by a range.
     *
     * @throws IllegalArgumentException if {@code range} can not be added to this range.
     */
    public void add(Range<E> range) {
        Assert.isLegal(canAdd(range));
        merge(range);
    }

    /**
     * Merges the elements of {@code range} into this range.
     *
     * <p> The result is the smallest range containing the elements of
     * both {@code this} and {@code range}.
     */
    public void merge(Range<E> range) {
        first = min(first, range.getFirst());
        last = max(last, range.getLast());
    }

    /**
     * Removes the specified element from this range.
     * Equivalent to {@code remove(element,element}.
     *
     * @see Range#remove(Object, Object)
     */
    public Range<E> remove(E element) {
        return remove(element, element);
    }

    /**
     * Removes the elements of the specified range from this range.
     * Equavalent to {@code remove(range.getFirst(), range.getLast()}.
     *
     * @see Range#remove(Object, Object)
     */
    public Range<E> remove(Range<E> range) {
        return remove(range.getFirst(), range.getLast());
    }

    /**
     * Removes an overlapping range from this range.
     *
     * <p> The operation will fail if the [first,last] range covers this range
     * (the result would be null).
     *
     * <p> The result can be one or two range. This range is modified to be the first part
     * of the difference, the optional subsequent part is returned by the method.
     *
     * @param first the first removed element
     * @param last the last removed element
     * @return the other part of the difference, if there is any
     * @throws IllegalArgumentException if [first,last] not a valid range, or
     *                                  [first,last] covers this range
     */
    public Range<E> remove(E first, E last) {
        Assert.isTrue(compare(first, last) <= 0);
        Assert.isTrue(this.overlaps(first, last) && !this.containedBy(first, last));

        //
        // this:      |-------|         |--------|       |--------|      |-------|      |--------|
        // other:  |-----|              |----|             |----|            |---|           |-------|
        //
        //               (1)                (2)              (3)            (4)              (5)

        if (compare(last, this.last) < 0 && compare(first, this.first) <= 0) { // (1) and (2)
            this.first = next(last);
            return null;
        }
        if (compare(this.first, first) < 0 && compare(this.last, last) <= 0) { // (4) and (5)
            this.last = prev(first);
            return null;
        }

        // (3)
        Range<E> rest = new Range<E>(next(last), this.last, enumerator);
        this.last = prev(first);
        return rest;
    }

    /**
     * Returns true if this range equals to {@code object},
     * i.e. {@code object} is a range and has the same
     * end-points and enumerators.
     */
    public boolean equals(Object object) {
        if (this == object)
            return true;
        if (!(object instanceof Range))
            return false;

        @SuppressWarnings("unchecked")
        Range<E> other = (Range<E>)object;
        return first.equals(other.getFirst()) && last.equals(other.getLast());
    }

    /**
     * Returns the hash code of this range.
     */
    public int hashCode() {
        return 37 * first.hashCode() + last.hashCode();
    }

    /**
     * Returns the string representation of this range.
     */
    public String toString() {
        return "Range[" + first + "," + last + "]";
    }

    //
    // Helpers
    //

    @SuppressWarnings("unchecked")
    final int compare(E o1, E o2) {
        return enumerator==null ? ((Comparable<? super E>)o1).compareTo(o2)
            : enumerator.compare(o1, o2);
    }

    @SuppressWarnings("unchecked")
    final E next(E e) {
        return enumerator==null ? ((IEnumerable<E>)e).getNext()
            : enumerator.getNext(e);
    }

    @SuppressWarnings("unchecked")
    final E prev(E e) {
        return enumerator==null ? ((IEnumerable<E>)e).getPrevious()
            : enumerator.getPrevious(e);
    }

    final E min(E e1, E e2) {
        return compare(e1, e2) <= 0 ? e1 : e2;
    }

    final E max(E e1, E e2) {
        return compare(e1, e2) >= 0 ? e1 : e2;
    }

    /**
     * For iterating the elements of a range.
     * remove() is only supported at the edge of the range.
     */
    @Override
    public Iterator<E> iterator() {
        return new Iterator<E>() {
            E current = null;
            E next = getFirst();

            @Override
            public boolean hasNext() {
                return next != null;
            }

            @Override
            public E next() {
                current = next;
                next = (getLast().equals(next)) ? null : next(next);
                return current;
            }

            @Override
            public void remove() {
                if (current == null)
                    throw new IllegalStateException();

                boolean atFirst = current.equals(getFirst());
                boolean atLast = current.equals(getLast());
                if (atFirst && !atLast) {
                    Range.this.remove(current);
                    current = null;
                    next = Range.this.getFirst();
                }
                else if (atLast && !atFirst) {
                    Range.this.remove(current);
                    current = null;
                    next = null;
                }
                else
                    throw new UnsupportedOperationException("Iterator can only remove elements from the ends of a Range.");
            }

            @SuppressWarnings("unchecked")
            private E next(E e) {
                return enumerator==null ? ((IEnumerable<E>)e).getNext() : enumerator.getNext(e);
            }
        };
    }
}

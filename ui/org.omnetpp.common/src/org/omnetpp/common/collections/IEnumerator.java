package org.omnetpp.common.collections;

import java.util.Comparator;

/**
 * Interface to enumerate a (possibly infinite) set of elements.
 *
 * <p> Chosing an arbitrary element, all other elements can be accessed
 * by repeatedly calling getNext() or getPrevious().
 *
 * @param <E> the type of the elements that this enumerator can enumerate
 * @see IEnumerable
 *
 * @author tomi
 */
public interface IEnumerator<E> extends Comparator<E> {

    /**
     * Returns the element immediately preceding {@code e},
     * or {@code null} if there is no previous element.
     */
    E getPrevious(E e);

    /**
     * Returns the element immediately following {@code e},
     * or {@code null} if there is no next element.
     */
    E getNext(E e);

    /**
     * Returns an approximate value of the delta between {@code e1} and {@code e2}.
     *
     * <p> The method can compute an approximate value of the delta, however it must be
     * computed more efficiently they iterating on the instances by getPrevious() or getNext().
     * The only requirements is that {@code -sgn(getApproximateDelta(e1,e2))} must define
     * an ordering that is compatible with the {@link Comparator#compare compare()} method of
     * the {@link Comparator} interface.
     *
     * @see Comparator
     */
    int getApproximateDelta(E e1, E e2);

    /**
     * Returns true, if the {@link getApproximateDelta()} method always returns the
     * exact value of delta between elements.
     */
    boolean isExact();
}

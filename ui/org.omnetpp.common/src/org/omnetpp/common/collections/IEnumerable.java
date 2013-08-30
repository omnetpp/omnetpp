package org.omnetpp.common.collections;

/**
 * Interface for types to enumerate its elements.
 *
 * <p> A type is enumerable, if there is a bijection between its instances
 * and a (possibly infinite) interval of the integer numbers.
 *
 * <p> Given an instance of T, all other instances of T can be obtained by
 * repeated calls of getNext() or getPrevious().
 * Furthermore the following list can not contain duplicate elements:
 * <pre>
 *   ..., getPrevious().getPrevious(), getPrevious(), this, getNext(), getNext().getNext(), ...
 * </pre>
 *
 * <p> Let there be {@code o1} and {@code o2} instances of {@code T}. Their {@code delta} is
 * defined as the signed number of steps to reach {@code o2} from {@code o1}. The following
 * code computes the exact value of delta:
 *
 * <pre>
 * delta = 0;
 * if (compare(o1, o2) <= 0) {
 *      for (T o = o1; !o.equals(o2); o = o.getNext())
 *          delta++;
 * }
 * else {
 *      for (T o = o1; !o.equals(o2); o = o.getPrevious())
 *          delta--;
 * }
 * return delta;
 * </pre>
 *
 * @param <T> the enumerable type
 *
 * @author tomi
 */
public interface IEnumerable<T> {

    /**
     * Returns the previous instance of T, or {@code null} if there is no previous instance.
     */
    T getPrevious();

    /**
     * Returns the next instance of T, or {@code null} if there is not next instance.
     */
    T getNext();

    /**
     * Returns an approximate value of the delta between {@code this} and {@code other}.
     *
     * <p> The method can compute an approximate value of the delta, however it must be
     * computed more efficiently they iterating on the instances by getPrevious() or getNext().
     * The only requirements is that {@code -sgn(o1.getApproximateDelta(o2))} must define
     * an ordering on {@code T}, and if {@code T} implements {@link Comparable}, than
     * this method and {@link Comparable#compareTo compareTo()} must define the same ordering.
     *
     * @see Comparable
     */
    int getApproximateDelta(T other);

    /**
     * Returns true, if the {@link getApproximateDelta()} method always returns the
     * exact value of delta between instances of {@code T}.
     */
    boolean isExact();
}

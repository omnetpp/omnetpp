/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.apache.commons.lang3.ObjectUtils;

/**
 * Utility class to group two values.
 * Mainly used to return two values from a method.
 *
 * @author tomi
 */
public class ComparablePair<T1 extends Comparable<T1>, T2 extends Comparable<T2>> implements Comparable<ComparablePair<T1, T2>> {

    public T1 first;
    public T2 second;

    public ComparablePair(T1 first, T2 second) {
        this.first = first;
        this.second = second;
    }

    public static <T1 extends Comparable<T1>, T2 extends Comparable<T2>> ComparablePair<T1,T2> pair(T1 first, T2 second) {
        return new ComparablePair<T1,T2>(first, second);
    }

    @Override
    public int hashCode() {
        return ObjectUtils.hashCode(first) ^ ObjectUtils.hashCode(second);
    }

    @Override
    public boolean equals(Object other) {
        if (this == other)
            return true;
        if (other == null || getClass() != other.getClass())
            return false;
        ComparablePair<?,?> otherPair = (ComparablePair<?,?>)other;
        return ObjectUtils.equals(first, otherPair.first) &&
                ObjectUtils.equals(second, otherPair.second);
    }

    @Override
    public String toString() {
        return "(" + first.toString() + ", " + second.toString() + ")";
    }

    @Override
    public int compareTo(ComparablePair<T1, T2> otherPair) {
        int result = first.compareTo(otherPair.first);
        if (result == 0)
            return second.compareTo(otherPair.second);
        else
            return result;
    }
}

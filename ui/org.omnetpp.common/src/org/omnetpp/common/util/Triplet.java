/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.apache.commons.lang3.ObjectUtils;

/**
 * Utility class to group three values.
 * Can be used to return three values from a method,
 * or to form a compound key for a map.
 *
 * @author tomi
 */
public class Triplet<T1,T2,T3> {

    public T1 first;
    public T2 second;
    public T3 third;

    public Triplet(T1 first, T2 second, T3 third) {
        this.first = first;
        this.second = second;
        this.third = third;
    }

    public static <T1,T2,T3> Triplet<T1,T2,T3> triplet(T1 first, T2 second, T3 third) {
        return new Triplet<T1,T2,T3>(first, second, third);
    }

    @Override
    public int hashCode() {
        return ObjectUtils.hashCode(first) + 37 * (ObjectUtils.hashCode(second) + 37 * ObjectUtils.hashCode(third));
    }

    @Override
    public boolean equals(Object other) {
        if (this == other)
            return true;
        if (other == null || getClass() != other.getClass())
            return false;
        Triplet<?,?,?> otherTriple = (Triplet<?,?,?>)other;
        return ObjectUtils.equals(first, otherTriple.first) &&
               ObjectUtils.equals(second, otherTriple.second) &&
               ObjectUtils.equals(third,  otherTriple.third);
    }

    @Override
    public String toString() {
        return "(" + first + ", " + second + ", " + third + ")";
    }
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.apache.commons.lang3.ObjectUtils;

/**
 * Utility class to group four values.
 * Can be used to return four values from a method,
 * or to form a compound key for a map.
 *
 * @author tomi
 */
public class Tuple4<T1,T2,T3,T4> {

    public T1 first;
    public T2 second;
    public T3 third;
    public T4 fourth;

    public Tuple4(T1 first, T2 second, T3 third, T4 fourth) {
        this.first = first;
        this.second = second;
        this.third = third;
        this.fourth = fourth;
    }

    public static <T1,T2,T3,T4> Tuple4<T1,T2,T3,T4> tuple4(T1 first, T2 second, T3 third, T4 fourth) {
        return new Tuple4<T1,T2,T3,T4>(first, second, third, fourth);
    }

    @Override
    public int hashCode() {
        return ObjectUtils.hashCode(first) +
                37 * (ObjectUtils.hashCode(second) +
                        37 * (ObjectUtils.hashCode(third) +
                                37 * ObjectUtils.hashCode(fourth)));
    }

    @Override
    public boolean equals(Object other) {
        if (this == other)
            return true;
        if (other == null || getClass() != other.getClass())
            return false;
        Tuple4<?,?,?,?> otherTuple = (Tuple4<?,?,?,?>)other;
        return ObjectUtils.equals(first, otherTuple.first) &&
               ObjectUtils.equals(second, otherTuple.second) &&
               ObjectUtils.equals(third,  otherTuple.third) &&
               ObjectUtils.equals(fourth, otherTuple.fourth);
    }

    @Override
    public String toString() {
        return "(" + first + ", " + second + ", " + third + ", " + fourth +")";
    }
}

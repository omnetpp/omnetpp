/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import java.util.Comparator;
import java.util.List;

import org.eclipse.core.runtime.Assert;

/**
 * Various interfaces to a binary search algorithm.
 *
 * @author levy
 */
public class BinarySearchUtils {
    public enum BoundKind {
        LOWER_BOUND,
        UPPER_BOUND
    }

    /**
     * Provides a way to compare the search key against an element of the list.
     */
    public interface KeyComparator {
        /**
         * Compares the key to the element of the implicit list at the provided index.
         *
         * @param index the index of the element in the implicit list
         * @return a negative integer, zero, or a positive integer as the key
         *         is less than, equal to, or greater than the element of the list
         */
        public int compareTo(long index);
    }

    /**
     * See below.
     */
    public static <T extends Comparable<T>> long binarySearch(List<T> list, T key, BoundKind boundKind) {
        return binarySearch(list, key, new Comparator<T>() {
            public int compare(T o1, T o2) {
                return o1.compareTo(o2);
            }
        }, boundKind);
    }

    /**
     * See below.
     */
    public static <T> long binarySearch(final List<T> list, final T key, final Comparator<T> comparator, BoundKind boundKind) {
        return binarySearch(new KeyComparator() {
            public int compareTo(long index) {
                return comparator.compare(key, list.get((int)index));
            }
        }, list.size(), boundKind);
    }

    /**
     * Binary search for a key among the elements of a list. Note that neither
     * the list nor the key are passed directly to this function. Rather they
     * are present implicitly in the key comparator function.
     *
     * The implicit list must be sorted into ascending order according to the
     * comparator before calling this method, otherwise the result is
     * unspecified.
     *
     * @param keyComparator a function that compares the key with an element
     *                      of the list at the specified index
     * @param size the size of the implicit list
     * @param boundKind specifies which index will be returned if there are
     *                  multiple equal elements in the list. Providing upper
     *                  bound means inserted equal elements are stored in
     *                  insertion order, providing lower bound means inserted
     *                  equal elements are stored in reverse insertion order.
     * @return the index may be used to insert into the list and the list will
     *         remain properly sorted. Valid return values are in the closed
     *         range of [0, list.size()]
     */
    public static long binarySearch(KeyComparator keyComparator, long size, BoundKind boundKind) {
        long index = -1;
        long lowerIndex = 0;
        long upperIndex = size - 1;
        boolean equalElementFound = false;
        while (lowerIndex <= upperIndex) {
            long middleIndex = (long)Math.floor((upperIndex + lowerIndex) / 2);
            int comparisonResult = keyComparator.compareTo(middleIndex);
            if (comparisonResult == 0) {
                equalElementFound = true;
                do {
                    index = middleIndex;
                    if (boundKind == BoundKind.LOWER_BOUND)
                        middleIndex--;
                    else // if (boundKind == BoundKind.UPPER_BOUND)
                        middleIndex++;
                }
                while (0 <= middleIndex && middleIndex <= size - 1 && keyComparator.compareTo(middleIndex) == 0);
                // index points to the lower bound or the upper bound element that is still equal to key
                if (boundKind == BoundKind.UPPER_BOUND)
                    index++; // insertion at index shifts elements up
                break;
            }
            else if (comparisonResult < 0)
                upperIndex = middleIndex - 1;
            else // if (comparisonResult > 0)
                lowerIndex = middleIndex + 1;
        }
        if (lowerIndex > upperIndex)
            index = lowerIndex;
        // the index is a valid insertion index
        Assert.isTrue(index >= 0 && index <= size);
        if (equalElementFound) {
            if (boundKind == BoundKind.LOWER_BOUND) {
                // the element right before the insertion index (if any) is less than key
                Assert.isTrue(index == 0 || keyComparator.compareTo(index - 1) > 0);
                // the element at the insertion index is equal to key
                Assert.isTrue(index == size || keyComparator.compareTo(index) == 0);
            }
            else {
                // the element right before the insertion index (if any) is equal to key
                Assert.isTrue(index == 0 || keyComparator.compareTo(index - 1) == 0);
                // the element at the insertion index is greater than key
                Assert.isTrue(index == size || keyComparator.compareTo(index) < 0);
            }
        }
        else {
            // the element right before the insertion index (if any) is less than key
            Assert.isTrue(index == 0 || keyComparator.compareTo(index - 1) > 0);
            // the element at the insertion index is greater than key
            Assert.isTrue(index == size || keyComparator.compareTo(index) < 0);
        }
        return index;
    }
}

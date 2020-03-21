package org.omnetpp.common.collections;

/**
 *  This class represents an immutable non-empty range of consecutive int values.
 */
public class IntRange implements Comparable<IntRange> {
    private final int first;
    private final int last;

    public IntRange(int first, int last) {
        if (last < first)
            throw new IllegalArgumentException("invalid range bounds");
        this.first = first;
        this.last = last;
    }

    public int first() {
        return first;
    }

    public int last() {
        return last;
    }

    public int length() {
        return last - first + 1;
    }

    public boolean intersects(IntRange other) {
        return (first >= other.first && first <= other.last)
            || (other.first >= first && other.first <= last);
    }

    public boolean adjacentTo(IntRange range) {
        return last+1 == range.first || range.last+1 == first;
    }

    public boolean canMergeWith(IntRange range) {
        return intersects(range) || adjacentTo(range);
    }

    public IntRange mergeWith(IntRange range) {
        if (!canMergeWith(range))
            throw new RuntimeException("cannot merge ranges");
        return quickMergeWith(range);
    }

    private IntRange quickMergeWith(IntRange range) {
        int nfirst = first < range.first ? first : range.first;
        int nlast = last > range.last ? last : range.last;
        return new IntRange(nfirst, nlast);
    }

    public IntRange tryMergeWith(IntRange range) {
        if (!canMergeWith(range))
            return null;
        return quickMergeWith(range);
    }

    public boolean contains(int v) {
        return v >= first && v <= last;
    }

    public int intersectionLength(IntRange other) {
        if (other.contains(first)) {
            int end = Math.min(last, other.last);
            return end - first + 1;
        } else if (this.contains(other.first)) {
            int end = Math.min(last, other.last);
            return end - other.first + 1;
        } else {
            return 0;
        }
    }

    public boolean equals(Object obj) {
        if (!(obj instanceof IntRange))
            return false;
        IntRange range = (IntRange)obj;
        return first == range.first && last == range.last;
    }

    public int compareTo(IntRange range) {
        if (first < range.first)
            return -1;
        if (first > range.first)
            return 1;
        if (last < range.last)
            return -1;
        if (last > range.last)
            return 1;
        return 0;
    }

    public int hashCode() {
        return first ^ last;
    }

    public String toString() {
        return "" + first + "-" + last;
    }
}
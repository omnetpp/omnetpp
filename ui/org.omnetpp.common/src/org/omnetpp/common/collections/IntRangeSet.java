package org.omnetpp.common.collections;

import java.util.ArrayList;

/**
 * This class represents a range-based set of int values.
 */
public class IntRangeSet {
    private ArrayList<IntRange> ranges = new ArrayList<>();
    private int size = 0;

    public IntRangeSet() {
    }

    public IntRangeSet(int v) {
        add(v);
    }

    public IntRangeSet(int first, int last) {
        addRange(first, last);
    }

    public IntRangeSet(int[] vs) { // note: numbers MUST be sorted
        if (vs.length == 0)
            return;
        int rangeStart, previous;
        rangeStart = previous = vs[0];
        for (int i = 1; i < vs.length; i++) {
            int current = vs[i];
            if (current < previous)
                throw new IllegalArgumentException("Sorted array expected");
            else if (current == previous || current == previous+1)
                /*keep going*/;
            else {
                ranges.add(new IntRange(rangeStart, previous));
                size += previous - rangeStart + 1;
                rangeStart = current;
            }
            previous = current;
        }
        ranges.add(new IntRange(rangeStart, previous));
        size += previous - rangeStart + 1;
    }

    public IntRangeSet(IntRangeSet other) {
        ranges = new ArrayList<>(other.ranges);
        size = other.size;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public int size() {
        return size;
    }

    public boolean contains(int v) {
        return findRangeOf(v) >= 0;
    }

    /**
     *  Returns the normalized (ordered, non-overlapping, non-adjacent) ranges in this set.
     */
    public IntRange[] ranges() {
        return ranges.toArray(new IntRange[0]);
    }

    /**
     *  Returns the index of the range that contains the specified value.
     */
    private int findRangeOf(int v) {
        if (size == 0)
            return -1;
        //  Binary search
        IntRange r;
        int lo = 0;
        int hi = ranges.size()-1;
        int mid;
        while (lo <= hi) {
            mid = (lo+hi)/2;
            r = ranges.get(mid);
            if (r.contains(v))
                return mid;
            if (v < r.first()) {
                hi = mid-1;
            } else {    // v > r.last()
                lo = mid+1;
            }
        }
        return -(lo+1);
    }

    /**
     *  Inserts a range at the sorted position in the ranges.
     */
    private int insertRange(IntRange range) {
        //  Binary search
        IntRange r;
        int lo = 0;
        int hi = ranges.size()-1;
        int mid;
        while (lo <= hi) {
            mid = (lo+hi)/2;
            r = ranges.get(mid);
            int compare = range.compareTo(r);
            if (compare == 0)
                return -1;
            if (compare < 0) {
                hi = mid-1;
            } else {    // compare > 0
                lo = mid+1;
            }
        }
        ranges.add(lo, range);
        return lo;
    }

    /**
     *  Normalizes the ranges after the insertion of a new range and
     *  recalculates the size of this set. The range list must be
     *  sorted when this method is invoked.
     */
    private void normalize(int index) {
        while (index < ranges.size()-1) {
            IntRange r1 = ranges.get(index);
            IntRange r2 = ranges.get(index+1);
            IntRange r3 = r1.tryMergeWith(r2);
            if (r3 == null)
                break;
            ranges.set(index, r3);
            ranges.remove(index+1);
            size -= r1.intersectionLength(r2);
        }
    }

    public boolean add(int v) {
        int index = findRangeOf(v);
        if (index >= 0)
            return false;
        int insertionIndex = -index-1;
        ranges.add(insertionIndex, new IntRange(v, v));
        if (insertionIndex > 0)
            insertionIndex--;
        size++;
        normalize(insertionIndex);
        return true;
    }

    public void clear() {
        ranges.clear();
        size = 0;
    }

    public boolean remove(int v) {
        int index = findRangeOf(v);
        if (index < 0)
            return false;
        //  Treat end points special since we can avoid splitting a range
        IntRange r = ranges.get(index);
        if (v == r.first()) {
            if (r.length() == 1)
                ranges.remove(index);
            else
                ranges.set(index, new IntRange((int)(r.first()+1), r.last()));
        } else if (v == r.last()) {
            //  r.length() > 1
            ranges.set(index, new IntRange(r.first(), (int)(r.last()-1)));
        } else {
            //  Split the range
            IntRange r1 = new IntRange(r.first(), (int)(v-1));
            IntRange r2 = new IntRange((int)(v+1), r.last());
            ranges.set(index, r1);
            ranges.add(index+1, r2);
        }
        size--;
        return true;
    }

    public int[] toArray() {
        int[] array = new int[size];
        int index = 0;
        for (IntRange r : ranges)
            for (int c = r.first(), last = r.last(); c <= last; c++)
                array[index++] = c;
        return array;
    }

    /**
     * Return true if all elements of the given range are contained in this set.
     */
    public boolean containsAll(IntRange range) {
        int index = findRangeOf(range.first());
        return index < 0 ? false : ranges.get(index).contains(range.last());
    }

    /**
     * Adds the contents of the given range set to this set.
     */
    public boolean addAll(IntRangeSet other) {
        int oldSize = size;
        for (IntRange r : other.ranges)
            addRange(r);
        return size != oldSize;
    }

    /**
     * Adds the given integers to this set.
     */
    public void addAll(int[] vs) {
        for (int v : vs)
            add(v);
    }

    /**
     * Adds the contents of the given range to this set.
     */
    public boolean addRange(int first, int last) {
        return addRange(new IntRange(first, last));
    }

    /**
     * Adds the contents of the given range to this set.
     */
    public boolean addRange(IntRange range) {
        int oldSize = size;
        int index = insertRange(range);
        if (index != -1) {
            int nindex = index;
            if (nindex > 0)
                nindex--;
            size += range.length();
            normalize(nindex);
        }
        return size != oldSize;
    }

    public int hashCode() {
        int h = 0;
        for (IntRange r : ranges)
            h = h * 31 + r.hashCode();
        return h;
    }

    public String toString() {
        StringBuffer s = new StringBuffer();
        s.append('[');
        int i = 0;
        for (IntRange r : ranges) {
            if (i++ > 0)
                s.append(',');
            s.append(r);
        }
        s.append(']');
        return s.toString();
    }


}

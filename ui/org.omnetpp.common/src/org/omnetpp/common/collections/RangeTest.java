package org.omnetpp.common.collections;

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.AssertionFailedException;
import org.junit.Test;

public class RangeTest {

    @Test
    public void testBounds() {
        Range<Integer> range = range(42, 137);
        assertEquals(Integer.valueOf(42), range.getFirst());
        assertEquals(Integer.valueOf(137), range.getLast());
    }

    @Test(expected = AssertionFailedException.class)
    public void testBoundsCheck() {
        range(0, -1);
    }

    @Test
    public void testContainsElement() {
        Range<Integer> range = range(-10, 10);
        assertFalse(range.contains(-11));
        assertTrue(range.contains(-10));
        assertTrue(range.contains(0));
        assertTrue(range.contains(10));
        assertFalse(range.contains(11));
    }

    @Test
    public void testContainsRange() {
        Range<Integer> range = range(-10, 10);
        assertFalse(range.contains(range(-20, -11)));
        assertFalse(range.contains(range(-11, 0)));
        assertTrue(range.contains(range(-10, 0)));
        assertTrue(range.contains(range(-10, 10)));
        assertTrue(range.contains(range(0, 10)));
        assertFalse(range.contains(range(0, 11)));
        assertFalse(range.contains(range(11, 20)));
    }

    @Test
    public void testOverlaps() {
        Range<Integer> range = range(-10, 10);
        assertFalse(range.overlaps(range(-20, -11)));
        assertTrue(range.overlaps(range(-11, 0)));
        assertTrue(range.overlaps(range(-10, 0)));
        assertTrue(range.overlaps(range(-10, 10)));
        assertTrue(range.overlaps(range(0, 10)));
        assertTrue(range.overlaps(range(0, 11)));
        assertFalse(range.overlaps(range(11, 20)));
    }

    @Test
    public void testAdd() {
        Range<Integer> range;

        (range = range(-10, 10)).add(range(-20, -11));
        assertEquals(range(-20, 10), range);

        (range = range(-10, 10)).add(range(-20, -10));
        assertEquals(range(-20, 10), range);

        (range = range(-10, 10)).add(range(-10, 10));
        assertEquals(range(-10, 10), range);

        (range = range(-10, 10)).add(range(-1, 1));
        assertEquals(range(-10, 10), range);

        (range = range(-10, 10)).add(range(10, 20));
        assertEquals(range(-10, 20), range);

        (range = range(-10, 10)).add(range(11, 20));
        assertEquals(range(-10, 20), range);

        (range = range(-10, 10)).add(range(-20, 20));
        assertEquals(range(-20, 20), range);
    }

    @Test
    public void testMerge() {
        Range<Integer> range;

        (range = range(-10, 10)).merge(range(-20, -11));
        assertEquals(range(-20, 10), range);

        (range = range(-10, 10)).merge(range(-20, -10));
        assertEquals(range(-20, 10), range);

        (range = range(-10, 10)).merge(range(-10, 10));
        assertEquals(range(-10, 10), range);

        (range = range(-10, 10)).merge(range(-1, 1));
        assertEquals(range(-10, 10), range);

        (range = range(-10, 10)).merge(range(10, 20));
        assertEquals(range(-10, 20), range);

        (range = range(-10, 10)).merge(range(11, 20));
        assertEquals(range(-10, 20), range);

        (range = range(-10, 10)).merge(range(-20, 20));
        assertEquals(range(-20, 20), range);
    }

    @Test
    public void testRemove() {
        Range<Integer> range, rest;

        rest = (range = range(-10, 10)).remove(range(-20, -10));
        assertEquals(range(-9, 10), range);

        rest = (range = range(-10, 10)).remove(range(-20, 0));
        assertEquals(range(1, 10), range);

        rest = (range = range(-10, 10)).remove(range(-10, 0));
        assertEquals(range(1, 10), range);

        rest = (range = range(-10, 10)).remove(range(-1, 1));
        assertEquals(range(-10, -2), range);
        assertEquals(range(2, 10), rest);

        rest = (range = range(-10, 10)).remove(range(0, 10));
        assertEquals(range(-10, -1), range);

        rest = (range = range(-10, 10)).remove(range(0, 20));
        assertEquals(range(-10, -1), range);

        rest = (range = range(-10, 10)).remove(range(10, 20));
        assertEquals(range(-10, 9), range);
    }

    @Test
    public void testIterator() {
        Range<Integer> range = range(-3,3);
        List<Integer> values = new ArrayList<Integer>();
        for (Integer i : range)
            values.add(i);
        assertEquals(Arrays.asList(new Integer[] {-3, -2, -1, 0, 1, 2, 3}), values);
    }

    @Test
    public void testIteratorRemove() {
        Range<Integer> range = range(-3,3);
        Iterator<Integer> iterator = range.iterator();
        while (iterator.hasNext()) {
            Integer i = iterator.next();
            if (i == -3 || i == 3)
                iterator.remove();
        }

        List<Integer> values = new ArrayList<Integer>();
        for (Integer i : range)
            values.add(i);
        assertEquals(Arrays.asList(new Integer[] {-2, -1, 0, 1, 2}), values);
    }

    // End of tests

    static class IntEnumerator implements IEnumerator<Integer> {
        public int compare(Integer e1, Integer e2) { return e1.compareTo(e2); }
        public Integer getPrevious(Integer e) { return e - 1; }
        public Integer getNext(Integer e) { return e + 1; }
        public int getApproximateDelta(Integer e1, Integer e2) { return e1 - e2; }
        public boolean isExact() { return true; }
    }

    private final static IEnumerator<Integer> enumerator = new IntEnumerator();

    private final static Range<Integer> range(int first, int last) {
        return new Range<Integer>(first, last, enumerator);
    }
}

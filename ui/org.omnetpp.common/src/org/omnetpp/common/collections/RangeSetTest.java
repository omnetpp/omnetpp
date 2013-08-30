package org.omnetpp.common.collections;

import static org.junit.Assert.*;

import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.Random;
import java.util.Set;
import java.util.TreeSet;

import org.junit.Test;

public class RangeSetTest {

    @Test
    public void testEmpty() {
        IRangeSet<Integer> empty = rangeSet();
        assertTrue(empty.isEmpty());
        assertEquals(0, empty.getNumberOfRanges());
        assertFalse(empty.rangeIterator().hasNext());
        assertEquals(0, empty.size());
        assertFalse(empty.iterator().hasNext());
    }

    @Test
    public void testNormalization() {
        for (int i = 0; i < 20; i++) {
            IRangeSet<Integer> set = randomRangeSet(-100, 100, 10);
            IRangeSet<Integer> other = rangeSet();
            for (Integer value : set)
                other.add(value);
            assertEquals(set.getNumberOfRanges(), other.getNumberOfRanges());
            assertEquals(set.toRangeList(), other.toRangeList());
        }
    }

    @Test
    public void testContains() {
        CheckedRangeSet<Integer> set = randomCheckedRangeSet(-100, 100, 10);
        for (int i = 0; i < 20; i++)
            set.contains(random(-150, 150));
    }

    @Test
    public void testSize() {
        for (int i = 0; i < 10; i++)
            randomCheckedRangeSet(-100, 100, 10).size();
    }

    @Test
    public void testIterator() {
        for (int i = 0; i < 10; i++) {
            Iterator<Integer> iterator = randomCheckedRangeSet(-100, 100, 10).iterator();
            while (iterator.hasNext())
                iterator.next();
            // TODO test iterator.remove()
        }
    }

    @Test
    public void testToArray1() {
        for (int i = 0; i < 10; i++) {
            randomCheckedRangeSet(-100, 100, 10).toArray();
        }
    }

    @Test
    public void testToArray2() {
        for (int i = 0; i < 10; i++) {
            CheckedRangeSet<Integer> set = randomCheckedRangeSet(-100, 100, 10);
            set.toArray(new Integer[set.size()]);
            // TODO test with smaller and greater size too
        }
    }

    @Test
    public void testAdd() {
        for (int i = 0; i < 10; i++) {
            randomCheckedRangeSet(-100, 100, 10).add(random(-120, 120));
        }
    }

    @Test
    public void testRemove() {
        for (int i = 0; i < 10; i++) {
            randomCheckedRangeSet(-100, 100, 10).remove(random(-120, 120));
        }
    }

    @Test
    public void testContainsAll() {
        for (int i = 0; i < 10; i++) {
            @SuppressWarnings("unchecked")
            IRangeSet<Integer> range = rangeSet(range(0,3));
            randomCheckedRangeSet(-100, 100, 10).containsAll(range);
        }
    }

    @Test
    public void testAddAll() {
        for (int i = 0; i < 10; i++) {
            IRangeSet<Integer> range = randomRangeSet(-100, 100, 1);
            randomCheckedRangeSet(-100, 100, 10).addAll(range);
        }
    }

    @Test
    public void testRetainAll() {
        for (int i = 0; i < 10; i++) {
            IRangeSet<Integer> range = randomRangeSet(-100, 100, 10);
            randomCheckedRangeSet(-100, 100, 10).retainAll(range);
        }
    }

    @Test
    public void testRemoveAll() {
        for (int i = 0; i < 10; i++) {
            IRangeSet<Integer> range = randomRangeSet(-100, 100, 10);
            randomCheckedRangeSet(-100, 100, 10).removeAll(range);
        }
    }

    @Test
    public void testCountCalls() {
        enumerator.resetCounter();

        IRangeSet<Integer> range = randomRangeSet(-100, 100, 10);
        range.isEmpty();
        range.contains(0);
        range.add(0);
        range.addRange(10, 30);
        range.remove(20);
        range.removeRange(15, 25);
        range.equals(randomRangeSet(-100, 100, 10));
        Iterator<Integer> iterator = range.iterator();
        while(iterator.hasNext())
            iterator.next();

        assertEquals(0, enumerator.getCountOfDeltaCalls());
    }

    // End of tests

    static class IntEnumerator implements IEnumerator<Integer> {
        int count; // count of getDelta() calls

        public int compare(Integer e1, Integer e2) { return e1.compareTo(e2); }
        public Integer getPrevious(Integer e) { return e - 1; }
        public Integer getNext(Integer e) { return e + 1; }
        public int getApproximateDelta(Integer e1, Integer e2) { count++; return e2 - e1; }
        public boolean isExact() { return true; }
        void resetCounter() { count = 0; }
        int getCountOfDeltaCalls() { return count; }
    }

    private final static IntEnumerator enumerator = new IntEnumerator();

    private final static Random rg = new Random(1984);

    private final static Range<Integer> range(int first, int last) {
        return new Range<Integer>(first, last, enumerator);
    }

    private final static IRangeSet<Integer> rangeSet() {
        return new RangeSet<Integer>(enumerator);
    }

    private static int random(int min, int max) {
        return min + (rg.nextInt(max - min + 1));
    }

    private final static IRangeSet<Integer> rangeSet(Range<Integer>... ranges) {
        RangeSet<Integer> rangeSet = new RangeSet<Integer>(enumerator);
        for (Range<Integer> range : ranges)
            rangeSet.addRange(range.getFirst(), range.getLast());
        return rangeSet;
    }

    private final static IRangeSet<Integer> randomRangeSet(int min, int max, int numOfRanges) {
        int[] bounds = new int[numOfRanges * 2];
        for (int i = 0; i < bounds.length; ++i)
            bounds[i] = random(min, max);
        Arrays.sort(bounds);
        RangeSet<Integer> rangeSet = new RangeSet<Integer>(enumerator);
        for (int i = 0; i < bounds.length; i += 2) {
            if (i == 0 || bounds[i] > bounds[i-1] + 1)
                rangeSet.addRange(bounds[i], bounds[i+1]);
        }
        return rangeSet;
    }

    private final static CheckedRangeSet<Integer> randomCheckedRangeSet(int min, int max, int numOfRanges) {
        return new CheckedRangeSet<Integer>(randomRangeSet(min, max, numOfRanges));
    }

    /**
     * Helper class to check that IRangeSet<E> behaves like a TreeSet<E>
     * regarding the methods of the Set<E> interface.
     *
     * The class stores both an IRangeSet and a TreeSet and every operation
     * is performed on both of them. Then it is checked that they give the same result.
     */
    static class CheckedRangeSet<E> implements Set<E> {
        private IRangeSet<E> actual;
        private Set<E> expected;

        public CheckedRangeSet(IRangeSet<E> set) {
            this.actual = set;
            this.expected = new TreeSet<E>(set);
        }

        @Override
        public int size() {
            assertEquals(expected.size(), actual.size());
            return actual.size();
        }

        @Override
        public boolean isEmpty() {
            assertEquals(expected.isEmpty(), actual.isEmpty());
            return actual.isEmpty();
        }

        @Override
        public boolean contains(Object o) {
            boolean expectedResult = expected.contains(o);
            boolean actualResult = actual.contains(o);
            assertEquals(expectedResult, actualResult);
            return actualResult;
        }

        @Override
        public Iterator<E> iterator() {
            return new Iterator<E>() {
                Iterator<E> actualIterator = actual.iterator();
                Iterator<E> expectedIterator = expected.iterator();

                @Override
                public boolean hasNext() {
                    boolean expectedResult = expectedIterator.hasNext();
                    boolean actualResult = actualIterator.hasNext();
                    assertEquals(expectedResult, actualResult);
                    return actualResult;
                }

                @Override
                public E next() {
                    E expectedResult = expectedIterator.next();
                    E actualResult = actualIterator.next();
                    assertEquals(expectedResult, actualResult);
                    return actualResult;
                }

                @Override
                public void remove() {
                    expectedIterator.remove();
                    actualIterator.remove();
                    assertEquals(expected, actual);
                }
            };
        }

        @Override
        public Object[] toArray() {
            Object[] expectedResult = expected.toArray();
            Object[] actualResult = actual.toArray();
            assertArrayEquals(expectedResult, actualResult);
            return actualResult;
        }

        @Override
        public <T> T[] toArray(T[] a) {
            T[] b = Arrays.copyOf(a, a.length);
            T[] expectedResult = expected.toArray(a);
            T[] actualResult = actual.toArray(b);
            assertArrayEquals(expectedResult, actualResult);
            return actualResult;
        }

        @Override
        public boolean add(E e) {
            boolean expectedResult = expected.add(e);
            boolean actualResult = actual.add(e);
            assertEquals(expectedResult, actualResult);
            assertEquals(expected, actual);
            return actualResult;
        }

        @Override
        public boolean remove(Object o) {
            boolean expectedResult = expected.remove(o);
            boolean actualResult = actual.remove(o);
            assertEquals(expectedResult, actualResult);
            assertEquals(expected, actual);
            return actualResult;
        }

        @Override
        public boolean containsAll(Collection<?> c) {
            boolean expectedResult = expected.containsAll(c);
            boolean actualResult = actual.containsAll(c);
            assertEquals(expectedResult, actualResult);
            return actualResult;
        }

        @Override
        public boolean addAll(Collection<? extends E> c) {
            boolean expectedResult = expected.addAll(c);
            boolean actualResult = actual.addAll(c);
            assertEquals(expectedResult, actualResult);
            assertEquals(expected, actual);
            return actualResult;
        }

        @Override
        public boolean retainAll(Collection<?> c) {
            boolean expectedResult = expected.retainAll(c);
            boolean actualResult = actual.retainAll(c);
            assertEquals(expectedResult, actualResult);
            assertEquals(expected, actual);
            return actualResult;
        }

        @Override
        public boolean removeAll(Collection<?> c) {
            boolean expectedResult = expected.removeAll(c);
            boolean actualResult = actual.removeAll(c);
            assertEquals(expectedResult, actualResult);
            assertEquals(expected, actual);
            return actualResult;
        }

        @Override
        public void clear() {
            expected.clear();
            actual.clear();
            assertEquals(expected, actual);
        }
    }
}

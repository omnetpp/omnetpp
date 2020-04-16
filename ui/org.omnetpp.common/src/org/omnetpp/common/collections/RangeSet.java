package org.omnetpp.common.collections;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.TreeSet;

import org.eclipse.core.runtime.Assert;

/**
 * An {@link IRangeSet} implementation, that stores an ordered set of
 * ranges.
 *
 * @param <E> the type of elements of the set
 *
 * @author tomi
 */
public class RangeSet<E> implements IRangeSet<E> {
    /**
     * The enumerator for iterating on the elements of ranges,
     * can be null if all element implements {@link IEnumerable}.
     */
    protected final IEnumerator<E> enumerator;

    /**
     * The sorted set of ranges of this set.
     */
    protected TreeSet<Range<E>> ranges; // ordered set of non-overlapping ranges

    /**
     * The cached size of this set (number of elements).
     * -1 if not yet computed
     */
    protected int cachedSize;

    /**
     * Creates a new empty set, for elements that implement
     * the {@link IEnumerable} interface itself.
     */
    public RangeSet() {
        this.enumerator = null;
        this.cachedSize = 0;
        this.ranges = new TreeSet<Range<E>>(createRangeComparator());
    }

    /**
     * Creates a new empty set, the elements are enumerated using the
     * specified {@code enumerator}.
     */
    public RangeSet(IEnumerator<E> enumerator) {
        this.enumerator = enumerator;
        this.cachedSize = 0;
        this.ranges = new TreeSet<Range<E>>(createRangeComparator());
    }

    protected Range<E> createRange(E first, E last) {
        return new Range<E>(first, last, enumerator);
    }

    //
    //   IRangeSet<E> interface
    //

    @Override
    public int getNumberOfRanges() {
        return ranges.size();
    }

    @Override
    public Iterator<Range<E>> rangeIterator() {
        return ranges.iterator();
    }

    @Override
    public List<Range<E>> toRangeList() {
        List<Range<E>> result = new ArrayList<Range<E>>();
        for (Range<E> range : ranges)
            result.add(new Range<E>(range));
        return result;
    }

    @Override
    public int approximateSize() {
        if (cachedSize >= 0)
            return cachedSize;

        int size = 0;
        for (Range<E> range : ranges)
            size += delta(range.getFirst(), range.getLast()) + 1;
        return size;
    }

    // TODO update size, return the number of added elements
    @Override
    public boolean addRange(E first, E last) {
        Range<E> newRange = createRange(first, last);

        Range<E> current = ranges.floor(newRange);
        if (current != null && current.contains(newRange))
            return false;

        // remove all ranges whose elements can be added to newRange
        Iterator<Range<E>> iterator = current != null ? ranges.tailSet(current, newRange.canAdd(current)).iterator() : ranges.iterator();
        while (iterator.hasNext()) {
            current = iterator.next();
            if (newRange.canAdd(current)) {
                newRange.add(current);
                iterator.remove();
            }
            else
                break;
        }

        ranges.add(newRange);
        cachedSize = -1;
        return true;
    }

    // TODO update size, return the number of removed elements
    @Override
    public boolean removeRange(E first, E last) {
        boolean changed = false;
        Range<E> range = createRange(first, last);

        Range<E> lastBefore = ranges.floor(range);
        if (lastBefore != null && lastBefore.overlaps(range)) {
            changed = true;
            if (range.contains(lastBefore))
                ranges.remove(lastBefore);
            else {
                Range<E> rest = lastBefore.remove(range);
                if (rest != null) {
                    ranges.add(rest);
                    cachedSize = -1;
                    return changed;
                }
            }
        }

        Iterator<Range<E>> rangesAfter = ranges.tailSet(range, false).iterator();
        while (rangesAfter.hasNext()) {
            Range<E> nextRange = rangesAfter.next();
            if (nextRange.overlaps(range)) {
                changed = true;
                if (range.contains(nextRange))
                    rangesAfter.remove();
                else
                    nextRange.remove(range);
            }
            else
                break;
        }

        cachedSize = -1;
        return changed;
    }

    //
    // Set<E> interface
    //

    @Override
    public int size() {
        if (cachedSize < 0) {
            if (enumerator.isExact()) {
                cachedSize = 0;
                for (Range<E> range : ranges)
                    cachedSize += delta(range.getFirst(), range.getLast()) + 1;
            }
            else {
                cachedSize = 0;
                for (Iterator<E> i = iterator(); i.hasNext(); i.next())
                    cachedSize++;
            }
        }
        return cachedSize;
    }

    @Override
    public boolean isEmpty() {
        return cachedSize == 0 || ranges.isEmpty();
    }

    @Override
    public boolean contains(Object o) {
        @SuppressWarnings("unchecked")
        E e = (E)o;
        Range<E> f = ranges.floor(createRange(e,e));
        return f != null && f.contains(e);
    }

    @Override
    public Iterator<E> iterator() {
        return new ElementIterator();
    }

    @Override
    public Object[] toArray() {
        Object[] result = new Object[size()];
        int i = 0;
        Iterator<E> iterator = iterator();
        while (iterator.hasNext())
            result[i++] = iterator.next();
        return result;
    }

    @SuppressWarnings("unchecked")
    @Override
    public <T> T[] toArray(T[] a) {
        int size = size();
        T[] result = a.length >= size ? a : (T[])Array.newInstance(a.getClass().getComponentType(), size);
        int i = 0;
        Iterator<E> iterator = iterator();
        while (iterator.hasNext())
            result[i++] = (T)iterator.next();
        // TODO null extra elements, see Collection.toArray(T[]). Also AbstractCollection.toArray(T[]).
        return result;
    }

    @Override
    public boolean add(E e) {
        return addRange(e, e);
    }

    @Override
    public boolean remove(Object o) {
        @SuppressWarnings("unchecked")
        E e = (E)o;
        return removeRange(e, e);
    }

    @Override
    public boolean containsAll(Collection<?> c) {
        // TODO optimize if c is RangeSet too
        Iterator<?> e = c.iterator();
        while (e.hasNext())
            if (!contains(e.next()))
                return false;
        return true;
    }

    @Override
    public boolean addAll(Collection<? extends E> c) {
        if (c.isEmpty())
            return false;

        if (c instanceof IRangeSet) {
            @SuppressWarnings("unchecked")
            IRangeSet<E> rangeSet = (IRangeSet<E>)c;
            if (this.ranges.isEmpty()) {
                Iterator<Range<E>> ranges = rangeSet.rangeIterator();
                while (ranges.hasNext()) {
                    Range<E> range = ranges.next();
                    this.ranges.add(createRange(range.getFirst(), range.getLast()));
                }
                cachedSize = -1;
                return true;
            }
            else {
                boolean modified = false;
                Iterator<Range<E>> ranges = rangeSet.rangeIterator();
                while (ranges.hasNext()) {
                    Range<E> range = ranges.next();
                    modified |= addRange(range.getFirst(), range.getLast());
                }
                return modified;
            }
        }
        else {
            boolean modified = false;
            Iterator<? extends E> e = c.iterator();
            while (e.hasNext()) {
                if (add(e.next()))
                    modified = true;
            }
            return modified;
        }
    }

    @Override
    public boolean retainAll(Collection<?> c) {
        // TODO optimize if c is RangeSet too
        boolean modified = false;
        Iterator<E> e = iterator();
        while (e.hasNext()) {
            if (!c.contains(e.next())) {
                e.remove();
                modified = true;
            }
        }
        return modified;
    }

    @Override
    public boolean removeAll(Collection<?> c) {
        if (c.isEmpty())
            return false;
        if (c instanceof IRangeSet) {
            @SuppressWarnings("unchecked")
            IRangeSet<E> rangeSet = (IRangeSet<E>)c;
            boolean modified = false;
            Iterator<Range<E>> ranges = rangeSet.rangeIterator();
            while (ranges.hasNext()) {
                Range<E> range = ranges.next();
                modified |= removeRange(range.getFirst(), range.getLast());
            }
            return modified;
        }
        else {
            boolean modified = false;

            if (size() > c.size()) {
                for (Iterator<?> i = c.iterator(); i.hasNext(); )
                    modified |= remove(i.next());
            } else {
                for (Iterator<?> i = iterator(); i.hasNext(); ) {
                    if (c.contains(i.next())) {
                        i.remove();
                        modified = true;
                    }
                }
            }
            return modified;
        }
    }

    @Override
    public void clear() {
        ranges.clear();
        cachedSize = 0;
    }

    // Overridden Object methods

    public boolean equals(Object o) {
        if (o == this)
            return true;

        if (!(o instanceof Set))
            return false;

        // The constract of Set requires, that this set must be equal to
        // any set containing the same elements.
        if (o instanceof IRangeSet) {
            // avoid calling size()
            // avoid iterating on the elements of o
            IRangeSet<?> otherRangeSet = (IRangeSet<?>)o;
            if (getNumberOfRanges() != otherRangeSet.getNumberOfRanges())
                return false;
            Iterator<Range<E>> thisRanges = rangeIterator();
            Iterator<Range<?>> otherRanges = (Iterator)otherRangeSet.rangeIterator();
            while (thisRanges.hasNext()) {
                Range<E> thisRange = thisRanges.next();
                Range<?> otherRange = otherRanges.next();
                if (!thisRange.equals(otherRange))
                    return false;
            }
            return true;
        }
        else {
            Collection<?> c = (Collection<?>) o;
            if (c.size() != size())
                return false;
            try {
                return containsAll(c);
            } catch (ClassCastException unused)   {
                return false;
            } catch (NullPointerException unused) {
                return false;
            }
        }
    }

    public int hashCode() {
        // The hash code must be the sum of the hash codes of the elements (see Set.hashCode()).
        int h = 0;
        Iterator<E> i = iterator();
        while (i.hasNext()) {
            E obj = i.next();
            if (obj != null)
                h += obj.hashCode();
        }
        return h;
    }

    public String toString() {
        Iterator<Range<E>> i = rangeIterator();
        if (! i.hasNext())
            return "RangeSet()";

        StringBuilder sb = new StringBuilder();
        sb.append("RangeSet(");
        for (;;) {
            Range<E> e = i.next();
            sb.append(e);
            if (! i.hasNext())
                return sb.append(')').toString();
            sb.append(", ");
        }
    }

    //
    // Helpers
    //

    @SuppressWarnings("unchecked")
    final int compare(E o1, E o2) {
        return enumerator != null ? enumerator.compare(o1, o2) : ((Comparable<E>)o1).compareTo(o2);
    }

    @SuppressWarnings("unchecked")
    final int delta(E o1, E o2) {
        return enumerator != null ? enumerator.getApproximateDelta(o1, o2) : ((IEnumerable<E>)o1).getApproximateDelta(o2);
    }

    @SuppressWarnings("unchecked")
    final E next(E e) {
        return enumerator != null ? enumerator.getNext(e) : ((IEnumerable<E>)e).getNext();
    }

    @SuppressWarnings("unchecked")
    final E prev(E e) {
        return enumerator != null ? enumerator.getPrevious(e) : ((IEnumerable<E>)e).getPrevious();
    }

    final Comparator<Range<E>> createRangeComparator() {
        if (enumerator == null) {
            return new Comparator<Range<E>>() {
                @SuppressWarnings("unchecked")
                @Override public int compare(Range<E> o1, Range<E> o2) {
                    return ((Comparable<E>)o1.getFirst()).compareTo(o2.getFirst());
                }
            };
        }
        else {
            return new Comparator<Range<E>>() {
                @Override public int compare(Range<E> o1, Range<E> o2) {
                    return RangeSet.this.enumerator.compare(o1.getFirst(), o2.getFirst());
                }
            };
        }
    }

    //
    // Iterator
    //

    class ElementIterator implements Iterator<E> {

        private Iterator<Range<E>> rangeIterator;
        private Range<E> currentRange;
        private Iterator<E> elementIterator;
        private E currentElement;

        public ElementIterator() {
            this.rangeIterator = rangeIterator();
            this.currentRange = rangeIterator.hasNext() ? rangeIterator.next() : null;
            this.elementIterator = currentRange != null ? currentRange.iterator() : null;
            this.currentElement = null;
        }

        @Override
        public boolean hasNext() {
            return (elementIterator != null && elementIterator.hasNext()) || rangeIterator.hasNext();
        }

        @Override
        public E next() {
            if (elementIterator == null)
                throw new NoSuchElementException();

            if (!elementIterator.hasNext() && rangeIterator.hasNext()) {
                currentRange = rangeIterator.next();
                elementIterator = currentRange.iterator();
            }

            currentElement = elementIterator.hasNext() ? elementIterator.next() : null;
            return currentElement;
        }

        @Override
        public void remove() {
            if (elementIterator == null || currentRange == null || currentElement == null)
                throw new IllegalStateException();

            // update size
            if (cachedSize >= 0)
                cachedSize -= 1;

            if (currentRange.containedBy(currentElement, currentElement)) {
                rangeIterator.remove();
                //Assert.isTrue(!rangeIterator.hasNext());
            }
            else if (currentElement.equals(currentRange.getFirst()) || currentElement.equals(currentRange.getLast())) {
                elementIterator.remove();
            }
            else {
                Range<E> rest = currentRange.remove(currentElement);
                if (rest != null) {
                    ranges.add(rest);
                    rangeIterator = ranges.tailSet(currentRange, false).iterator();
                    Assert.isTrue(rangeIterator.hasNext());
                    currentRange = rangeIterator.next();
                    Assert.isTrue(currentRange == rest);
                    elementIterator = currentRange.iterator();
                }
            }
        }
    }
}

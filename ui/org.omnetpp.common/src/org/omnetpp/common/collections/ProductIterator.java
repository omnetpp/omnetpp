package org.omnetpp.common.collections;

import org.apache.commons.collections.ResettableIterator;

/**
 * Iterator for iterating the cartesian product of n iterators.
 *
 * @author tomi
 */
public class ProductIterator implements ResettableIterator {
	
	ResettableIterator[] iterators;
	Object[] current;
	Object[] next;
	boolean isEmpty;
	boolean hasNext;
	
	public ProductIterator(ResettableIterator... iterators) {
		this.iterators = iterators;
		this.current = new Object[iterators.length];
		this.next = new Object[iterators.length];
		this.hasNext = init();
		this.isEmpty = !hasNext;
	}
	
	public boolean init() {
		for(int i = 0; i < iterators.length; ++i) {
			ResettableIterator iterator = iterators[i];
			iterator.reset();
			if (iterator.hasNext())
				next[i] = iterator.next();
			else
				return false;
		}
		return next.length > 0;
	}

	public void reset() {
		if (isEmpty)
			hasNext = false;
		else {
			for(int i = 0; i < iterators.length; ++i) {
				ResettableIterator iterator = iterators[i];
				iterator.reset();
				next[i] = iterator.next();
			}
			hasNext = true;
		}
	}

	public boolean hasNext() {
		return hasNext;
	}

	public Object next() {
		if (!hasNext)
			return null;
		
		System.arraycopy(next, 0, current, 0, next.length);
		for (int i = 0; i < iterators.length; ++i) {
			ResettableIterator iterator = iterators[i];
			if (iterator.hasNext()) {
				next[i] = iterator.next();
				break;
			}
			else if (i < iterators.length - 1) {
				iterator.reset();
				next[i] = iterator.next();
			}
			else {
				hasNext = false;
			}
		}
		return current;
	}

	public void remove() {
		throw new UnsupportedOperationException("Cannot remove from a product iteration");
	}
}

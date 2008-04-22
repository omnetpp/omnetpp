package org.omnetpp.common.util;

import org.apache.commons.lang.ObjectUtils;

/**
 * Utility class to group two values.
 * Mainly used to return two values from a method.
 *
 * @author tomi
 */
public class Pair<T1,T2> {
	
	public T1 first;
	public T2 second;

	public Pair(T1 first, T2 second) {
		this.first = first;
		this.second = second;
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
		Pair<?,?> otherPair = (Pair<?,?>)other;
		return ObjectUtils.equals(first, otherPair.first) &&
				ObjectUtils.equals(second, otherPair.second);
	}
}

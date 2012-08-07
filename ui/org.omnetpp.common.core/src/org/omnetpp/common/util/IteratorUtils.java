package org.omnetpp.common.util;

import java.util.Iterator;

/**
 * Common functions for iterators.
 *
 * @author tomi
 */
public class IteratorUtils {

    public interface Mapping<From, To> {
        To map(From object);
    }

    /**
     * Maps the elements of {@code input} stream by applying the specified {@code mapping}.
     */
    public static <From,To> Iterator<To> map(final Iterator<From> input, final Mapping<From,To> mapping) {
        return new Iterator<To>() {
            public boolean hasNext() { return input.hasNext(); }
            public To next() { From obj = input.next(); return mapping.map(obj); }
            public void remove() { input.remove(); }
        };
    }

    /**
     * Maps the elements of {@code input} stream by applying the specified {@code mapping} to
     * them and concatenating the results.
     */
    public static <From,To> Iterator<To> concatMap(final Iterator<From> input, final Mapping<From,Iterator<To>> mapping) {
        return new Iterator<To>() {
            Iterator<To> current;
            public boolean hasNext() {
                if (current == null || !current.hasNext()) {
                    while (input.hasNext()) {
                        From nextInput = input.next();
                        current = mapping.map(nextInput);
                        if (current.hasNext())
                            break;
                    }
                }
                return current != null && current.hasNext();
            }
            public To next() { return current.next(); }
            public void remove() { throw new UnsupportedOperationException(); }
        };
    }
}

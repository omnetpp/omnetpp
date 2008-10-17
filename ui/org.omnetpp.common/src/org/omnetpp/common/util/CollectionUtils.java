package org.omnetpp.common.util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class CollectionUtils {
    public static <T> List<T> toSorted(Collection<T> collection) {
        return toSorted(collection, new Comparator<T>() {
            public int compare(T o1, T o2) {
                return o1.toString().compareToIgnoreCase(o2.toString());
            }
        });
    }

    public static <T> List<T> toSorted(Collection<T> collection, Comparator<T> comparator) {
        List<T> sortable = new ArrayList<T>(collection);
        Collections.sort(sortable, comparator);
        return sortable;
    }

    public static <T> List<T> toReversed(Collection<T> collection) {
        List<T> sortable = new ArrayList<T>(collection);
        Collections.reverse(sortable);
        return sortable;
    }
}

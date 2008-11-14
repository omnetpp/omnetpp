package org.omnetpp.common.util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * Yet another CollectionUtils class.
 * 
 * @author Levy, Andras
 */
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
    
    @SuppressWarnings("unchecked")
    public static Map getDeepCopyOf(Map map) {
        Map result = new HashMap();
        for (Object key : map.keySet())
            result.put(deepCopyOf(key), deepCopyOf(map.get(key)));
        return result;
    }

    @SuppressWarnings("unchecked")
    public static Object getDeepCopyOf(Set set) {
        Set result = new HashSet();
        for (Object value : set)
            result.add(deepCopyOf(value));
        return result;
    }

    @SuppressWarnings("unchecked")
    public static Object getDeepCopyOf(List list) {
        List result = new ArrayList();
        for (Object value : list)
            result.add(deepCopyOf(value));
        return result;
    }

    @SuppressWarnings("unchecked")
    public static Object deepCopyOf(Object object) {
        if (object instanceof String || object instanceof Boolean || object instanceof Number)
            return object; // immutable (probably; except for exotic stuff like apache.MutableDouble)
        if (object instanceof Map)
            return getDeepCopyOf((Map)object);
        if (object instanceof List)
            return getDeepCopyOf((List)object);
        if (object instanceof Set)
            return getDeepCopyOf((Set)object);
        if (object instanceof Cloneable)
            return ReflectionUtils.invokeMethod(object, "clone");
        throw new IllegalArgumentException("dunno how to clone " + object.toString());
    }

}

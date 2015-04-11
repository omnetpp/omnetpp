/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.unimod.generator.cpp.util;

import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.collections.CollectionUtils;
import org.apache.commons.collections.Predicate;
import org.apache.commons.collections.Transformer;
import org.apache.commons.lang.math.NumberUtils;

import com.evelopers.unimod.core.stateworks.Event;
import com.evelopers.unimod.core.stateworks.Guard;
import com.evelopers.unimod.core.stateworks.State;
import com.evelopers.unimod.core.stateworks.StateType;
import com.evelopers.unimod.core.stateworks.Transition;

public class CollectionsTool {
    private static Map cache = new HashMap();

    public static Predicate stableStatePredicate() {
        if (! cache.containsKey("stableStatePredicate")) {
            cache.put("stableStatePredicate", new Predicate() {
                public boolean evaluate(Object o) {
                    State s = (State) o;
                    return !StateType.INITIAL.equals(s.getType())
                            && !s.isComposite();
                }
            });
        }
        return (Predicate) cache.get("stableStatePredicate");
    }

    public static Predicate compositeStatePredicate() {
        if (! cache.containsKey("compositeStatePredicate")) {
            cache.put("compositeStatePredicate", new Predicate() {
                public boolean evaluate(Object o) {
                    State s = (State) o;
                    return s.isComposite();
                }
            });
        }
        return (Predicate) cache.get("compositeStatePredicate");
    }

    public static Predicate notInitialStatePredicate() {
        if (! cache.containsKey("notInitialStatePredicate")) {
            cache.put("notInitialStatePredicate", new Predicate() {
                public boolean evaluate(Object o) {
                    State s = (State) o;
                    return !StateType.INITIAL.equals(s.getType());
                }
            });
        }
        return (Predicate) cache.get("notInitialStatePredicate");
    }

    public static Predicate realEventPredicate() {
        if (! cache.containsKey("realEventPredicate")) {
            cache.put("realEventPredicate", new Predicate() {
                public boolean evaluate(Object o) {
                    Event e = (Event) o;
                    return !Event.ANY.equals(e) && !Event.NO_EVENT.equals(e);
                }
            });
        }
        return (Predicate) cache.get("realEventPredicate");
    }

    public static Predicate trueTransitionPredicate() {
        if (! cache.containsKey("trueTransitionPredicate")) {
            cache.put("trueTransitionPredicate", new Predicate() {
                public boolean evaluate(Object o) {
                    Transition t = (Transition) o;
                    return Guard.TRUE.equals(t.getGuard());
                }
            });
        }
        return (Predicate) cache.get("trueTransitionPredicate");
    }

    public static Comparator stateComparator() {
        if (! cache.containsKey("stateComparator")) {
            final Pattern STATE_NUMBER_PATTERN = Pattern.compile("(\\d+)");
            cache.put("stateComparator", new Comparator() {
                public int compare(Object o1, Object o2) {
                    State s1 = (State) o1;
                    State s2 = (State) o2;
                    return getNumber(s1) - getNumber(s2);
                }

                private int getNumber(State state) {
                    Matcher m = STATE_NUMBER_PATTERN.matcher(state.getName());
                    int d = Integer.MAX_VALUE;
                    if (m.find()) {
                        return NumberUtils.stringToInt(m.group(1), d);
                    } else {
                        return d;
                    }
                }
            });
        }
        return (Comparator) cache.get("stateComparator");
    }

    public static Comparator eventComparator() {
        if (! cache.containsKey("eventComparator")) {
            cache.put("eventComparator", new Comparator() {
                public int compare(Object o1, Object o2) {
                    Event e1 = (Event) o1;
                    Event e2 = (Event) o2;
                    return e1.getName().compareTo(e2.getName());
                }
            });
        }
        return (Comparator) cache.get("eventComparator");
    }

    public static Transformer transitionToEventTransformer() {
        if (! cache.containsKey("transitionToEventTransformer")) {
            cache.put("transitionToEventTransformer", new Transformer() {
                public Object transform(Object o) {
                    Transition t = (Transition) o;
                    return t.getEvent();
                }
            });
        }
        return (Transformer) cache.get("transitionToEventTransformer");
    }

    public static List filter(Collection c, Predicate p) {
        List l = new LinkedList(c);
        CollectionUtils.filter(l, p);
        return l;
    }

    public static List filter(Collection c, Predicate p, Comparator cmp) {
        List l = new LinkedList(c);
        CollectionUtils.filter(l, p);
        Collections.sort(l, cmp);
        return l;
    }

    public static List filter(Collection c, Transformer t, Predicate p, Comparator cmp) {
        List l = new LinkedList(c);
        CollectionUtils.transform(l, t);
        CollectionUtils.filter(l, p);
        Collections.sort(l, cmp);
        return l;
    }


}

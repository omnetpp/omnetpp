package org.omnetpp.sequencechart.widgets;

import java.util.ArrayList;
import java.util.HashMap;

import org.omnetpp.common.engine.JavaMatchableObject;
import org.omnetpp.common.engine.MatchExpression;
import org.omnetpp.common.util.Pair;

public class MatchExpressionFallback<Key, Value>
{
    protected JavaMatchableObject javaMatchableObject = new JavaMatchableObject();
    protected ArrayList<String> expressions = new ArrayList<String>();
    protected ArrayList<MatchExpression> matchers = new ArrayList<MatchExpression>();
    protected ArrayList<Value> values = new ArrayList<Value>();
    protected Value defaultValue = null;
    protected int cacheLimit = 0;
    protected HashMap<Key, Value> cache = new HashMap<Key, Value>();

    public MatchExpressionFallback(int cacheLimit, Value defaultValue) {
        this.cacheLimit = cacheLimit;
        this.defaultValue = defaultValue;
    }

    public void setFallback(Pair<String, Value>[] fallback) {
        removeAllFallbacks();
        for (Pair<String, Value> element : fallback)
            addFallback(element.first, element.second);
    }

    public Pair<String, Value>[] getFallback() {
        @SuppressWarnings("unchecked")
        Pair<String, Value>[] fallback = new Pair[matchers.size()];
        for (int i = 0; i < matchers.size(); i++) {
            fallback[i].first = expressions.get(i);
            fallback[i].second = values.get(i);
        }
        return fallback;
    }

    public void addFallback(String expression, Value value) {
        expressions.add(expression);
        matchers.add(new MatchExpression(expression, false, false, true));
        values.add(value);
        cache.clear();
    }

    public void removeAllFallbacks() {
        expressions.clear();
        matchers.clear();
        values.clear();
        cache.clear();
    }

    public Value resolveFallback(Key key) {
        if (matchers.size() != 0) {
            Value value = cache.get(key);
            if (value != null)
                return value;
            else {
                javaMatchableObject.setJavaObject(key);
                for (int i = 0; i < matchers.size(); i++) {
                    MatchExpression matcher = matchers.get(i);
                    if (matcher.matches(javaMatchableObject)) {
                        cache.put(key, value);
                        if (cache.size() == cacheLimit)
                            cache.clear();
                        javaMatchableObject.setJavaObject(null);
                        return values.get(i);
                    }
                }
                javaMatchableObject.setJavaObject(null);
            }
        }
        return defaultValue;
    }
}

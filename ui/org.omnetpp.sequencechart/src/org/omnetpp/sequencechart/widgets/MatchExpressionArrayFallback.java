package org.omnetpp.sequencechart.widgets;

import java.lang.reflect.Array;

import org.omnetpp.common.engineext.IMatchableObject;

public class MatchExpressionArrayFallback<Key, Value> extends MatchExpressionFallback<Key, Value[]>
{
    private String indexAttribute;

    public MatchExpressionArrayFallback(int cacheLimit, Value defaultValue) {
        this(cacheLimit, defaultValue, null);
    }

    @SuppressWarnings("unchecked")
    public MatchExpressionArrayFallback(int cacheLimit, Value defaultValue, String indexAttribute) {
        super(cacheLimit, (Value[])getDefaultArrayValue(defaultValue.getClass(), defaultValue));
        this.indexAttribute = indexAttribute;
    }

    private static Object[] getDefaultArrayValue(Class<?> clazz, Object defaultValue) {
        Object[] defaultArrayValue = (Object[])Array.newInstance(clazz, 1);
        defaultArrayValue[0] = defaultValue;
        return defaultArrayValue;
    }

    public Value resolveArrayFallback(Key key) {
        Value[] values = resolveFallback(key);
        IMatchableObject matchableObject = (IMatchableObject)key;
        String indexAttributeValue = indexAttribute != null ? matchableObject.getAsString(indexAttribute) : matchableObject.getAsString();
        return values[Math.abs(indexAttributeValue.hashCode()) % values.length];
    }
}

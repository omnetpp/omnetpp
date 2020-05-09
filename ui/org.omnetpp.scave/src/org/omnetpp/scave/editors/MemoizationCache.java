package org.omnetpp.scave.editors;

import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.Map;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.ByteVector;
import org.omnetpp.scave.engine.ResultFileManager;

/**
 * Memoization cache for methods of ResultProvider.
 *
 * @author andras
 */
public class MemoizationCache {
    private ResultFileManager rfm;
    private int currentSerial = 0;
    private long memoryLimit;
    private long memoryUsed = 0;

    public static class Key {
        private String method;
        private Object[] args;

        public Key(String method, Object... args) {
            this.method = method;
            this.args = args;
        }

        public String getMethodName() {
            return method;
        }

        @Override
        public int hashCode() {
            return method.hashCode() + 31 * Arrays.deepHashCode(args);
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;
            Key other = (Key) obj;
            return method.equals(other.method) && Arrays.deepEquals(args, other.args);
        }

        @Override
        public String toString() {
            return "Key(" + method + ": " + StringUtils.join(args, ",") + ")";
        }
    }

    private Map<Key,ByteVector> cache = new LinkedHashMap<>(); // for FIFO order

    public MemoizationCache(ResultFileManager rfm) {
        this(rfm, Long.MAX_VALUE);
    }

    public MemoizationCache(ResultFileManager rfm, long memoryLimit) {
        this.rfm = rfm;
        this.currentSerial = rfm.getSerial();
        this.memoryLimit = memoryLimit;
    }

    private void checkSerial() {
        if (currentSerial != rfm.getSerial()) {
            cache.clear();
            memoryUsed = 0;
            currentSerial = rfm.getSerial();
        }
    }

    public boolean contains(Key key) {
        checkSerial();
        return cache.containsKey(key);
    }

    public ByteVector get(Key key) {
        checkSerial();
        return cache.get(key);
    }

    public boolean isCacheable(Key key, long replySize) {
        return replySize <= memoryLimit;
    }

    public void put(Key key, ByteVector reply) {
        // note: using this API, memory consumption can temporarily exceed memoryLimit
        checkSerial();
        Assert.isTrue(reply.size() <= memoryLimit);
        memoryUsed += reply.size();
        cache.put(key, reply);
        while (memoryUsed > memoryLimit)
            discardFirst();
    }

    private void discardFirst() {
        Map.Entry<Key,ByteVector> entry = cache.entrySet().iterator().next();
        memoryUsed -= entry.getValue().size();
        cache.remove(entry.getKey());
    }

    public void clear() {
        cache.clear();
        memoryUsed = 0;
    }

}

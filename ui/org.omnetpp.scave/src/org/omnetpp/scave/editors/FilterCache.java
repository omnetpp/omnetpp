package org.omnetpp.scave.editors;

import java.util.HashMap;
import java.util.Map;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Result filtering cache for methods of ResultProvider.
 *
 * @author andras
 */
public class FilterCache {
    private ResultFileManager manager;
    private int currentSerial;

    private static class Key {
        private int resultTypes;
        private String filterExpression; // may include isfield and type filters, based on UI state
        private boolean includeFields;   // is false if resultTypes doesn't include SCALAR 

        /** includeFields is ignored if resultTypes doesn't include SCALAR. */
        public Key(int resultTypes, String filterExpression, boolean includeFields) {
            this.resultTypes = resultTypes;
            this.filterExpression = filterExpression;
            this.includeFields = ((resultTypes & ResultFileManager.SCALAR) != 0) && includeFields;
        }

        @Override
        public int hashCode() {
            return resultTypes + 31 * filterExpression.hashCode() + (includeFields ? 1023 : 127);
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;
            Key other = (Key) obj;
            return resultTypes == other.resultTypes && filterExpression.equals(other.filterExpression) && includeFields == other.includeFields;
        }

        @Override
        public String toString() {
            return "FilterKey(" + ScaveModelUtil.getResultTypesAsString(resultTypes) + ": " + filterExpression + ", " + (includeFields ? "with" : "without") + " fields)";
        }
    }

    private Map<Key,IDList> filterCache = new HashMap<>();

    public FilterCache(ResultFileManager manager) {
        this.manager = manager;
        this.currentSerial = manager.getSerial();
    }

    private void checkSerial() {
        if (currentSerial != manager.getSerial()) {
            filterCache.clear();
            currentSerial = manager.getSerial();
        }
    }

    public IDList getFilterResult(int resultTypes, String filterExpression, boolean includeFields) {
        checkSerial();
        return filterCache.get(new Key(resultTypes, filterExpression, includeFields));
    }

    public IDList getFilterResult(int resultTypes, String filterExpression) {
        return getFilterResult(resultTypes, filterExpression, false);
    }

    public void putFilterResult(int resultTypes, String filterExpression, boolean includeFields, IDList result) {
        checkSerial();
        filterCache.put(new Key(resultTypes, filterExpression, includeFields), result);
    }

    public void putFilterResult(int resultTypes, String filterExpression, IDList result) {
        putFilterResult(resultTypes, filterExpression, false, result);
    }

    public void clear() {
        filterCache.clear();
    }

}

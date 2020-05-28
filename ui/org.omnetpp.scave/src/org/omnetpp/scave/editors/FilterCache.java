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

        public Key(int resultTypes, String filterExpression) {
            this.resultTypes = resultTypes;
            this.filterExpression = filterExpression;
        }

        @Override
        public int hashCode() {
            return resultTypes + 31 * filterExpression.hashCode();
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;
            Key other = (Key) obj;
            return resultTypes == other.resultTypes && filterExpression.equals(other.filterExpression);
        }

        @Override
        public String toString() {
            return "FilterKey(" + ScaveModelUtil.getResultTypesAsString(resultTypes) + ": " + filterExpression + ")";
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

    public IDList getFilterResult(int resultTypes, String filterExpression) {
        checkSerial();
        return filterCache.get(new Key(resultTypes, filterExpression));
    }

    public void putFilterResult(int resultTypes, String filterExpression, IDList result) {
        checkSerial();
        filterCache.put(new Key(resultTypes, filterExpression), result);
    }

    public void clear() {
        filterCache.clear();
    }

}

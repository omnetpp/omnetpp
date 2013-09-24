package org.omnetpp.common.eventlog;

import org.omnetpp.common.collections.IEnumerator;
import org.omnetpp.common.collections.RangeSet;

public class EventNumberRangeSet extends RangeSet<Long> {
    private static class EventNumberEnumerator implements IEnumerator<Long> {
        @Override
        public int compare(Long o1, Long o2) {
            return (int)(o1 - o2);
        }

        @Override
        public Long getPrevious(Long value) {
            return value - 1;
        }

        @Override
        public Long getNext(Long value) {
            return value + 1;
        }

        @Override
        public int getApproximateDelta(Long o1, Long o2) {
            return (int)(o2 - o1);
        }

        @Override
        public boolean isExact() {
            return true;
        }
    };

    public EventNumberRangeSet() {
        super(new EventNumberEnumerator());
    }
}

package org.omnetpp.eventlog;

import java.util.TreeMap;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.engine.LineTokenizer;
import org.omnetpp.common.engine.PStringVector;
import org.omnetpp.eventlog.engine.FileReader;

/**
 * Allows random access of an eventlog file, i.e. positioning on arbitrary event numbers and simulation times.
 * TODO: throw out entries from cache to free memory. This is not that urgent because the cache will be quite
 * small unless the file is linearly read through which is not supposed to happen.
 */
public abstract class EventLogIndex extends EventLogBase
{
    protected FileReader reader;
    protected LineTokenizer tokenizer;

    protected long firstEventOffset = -1;
    protected long lastEventOffset = -1;
    protected long firstEventNumber = -1;
    protected long lastEventNumber = -1;
    protected BigDecimal firstSimulationTime = null;
    protected BigDecimal lastSimulationTime = null;

    /**
     * Subsequent events in an eventlog file may not have subsequent event numbers,
     * therefore it is insufficient to store the file offset only in the cache.
     */
    protected TreeMap<Long, CacheEntry> eventNumberToCacheEntryMap = new TreeMap<Long, CacheEntry>();

    /**
     * Subsequent events in an eventlog file may have the same simulation time,
     * therefore it is insufficient to store the file offset only in the cache.
     */
    protected TreeMap<BigDecimal, CacheEntry> simulationTimeToCacheEntryMap = new TreeMap<BigDecimal, CacheEntry>();

    protected static final class RefObject<T>
    {
        public T argValue;

        public RefObject(T refArg) {
            argValue = refArg;
        }
    }

    public EventLogIndex(FileReader reader)
    {
        this.reader = reader;
        this.tokenizer = new LineTokenizer((int)reader.getMaxLineSize() + 1);
    }

    public void cacheEntry(long eventNumber, BigDecimal simulationTime, long beginOffset, long endOffset) {
        var eventNumberCacheEntry = eventNumberToCacheEntryMap.ceilingEntry(eventNumber);
        if (eventNumberCacheEntry != null && eventNumberCacheEntry.getKey() == eventNumber)
            eventNumberCacheEntry.getValue().include(eventNumber, simulationTime, beginOffset, endOffset);
        else
            eventNumberToCacheEntryMap.put(eventNumber, new CacheEntry(eventNumber, simulationTime, beginOffset, endOffset));
        var simulationTimeCacheEntry = simulationTimeToCacheEntryMap.ceilingEntry(simulationTime);
        if (simulationTimeCacheEntry != null && simulationTimeCacheEntry.getKey().equals(simulationTime))
            simulationTimeCacheEntry.getValue().include(eventNumber, simulationTime, beginOffset, endOffset);
        else
            simulationTimeToCacheEntryMap.put(simulationTime, new CacheEntry(eventNumber, simulationTime, beginOffset, endOffset));
    }

    /**
     * Search for the file offset based on the key with the given match kind.
     * The key is either an event number or a simulation time.
     */
    protected final <T extends Comparable<T>> long searchForOffset(TreeMap<T, CacheEntry> map, T key, MatchKind matchKind) {
        T defaultValue = getKey(key, -1L, BigDecimal.getMinusOne());
        T lowerKey = defaultValue;
        T upperKey = defaultValue;
        long foundOffset = -1;
        long lowerOffset = -1;
        long upperOffset = -1;
        // first try to look up it the cache, this may result in an exact offset or a range around the offset being searched
        RefObject<T> tempRef_lowerKey = new RefObject<T>(lowerKey);
        RefObject<T> tempRef_upperKey = new RefObject<T>(upperKey);
        RefObject<Long> tempRef_foundOffset = new RefObject<Long>(foundOffset);
        RefObject<Long> tempRef_lowerOffset = new RefObject<Long>(lowerOffset);
        RefObject<Long> tempRef_upperOffset = new RefObject<Long>(upperOffset);
        boolean found = cacheSearchForOffset(map, key, matchKind, tempRef_lowerKey, tempRef_upperKey, tempRef_foundOffset, tempRef_lowerOffset, tempRef_upperOffset);
        upperOffset = tempRef_upperOffset.argValue;
        lowerOffset = tempRef_lowerOffset.argValue;
        foundOffset = tempRef_foundOffset.argValue;
        upperKey = tempRef_upperKey.argValue;
        lowerKey = tempRef_lowerKey.argValue;

        // no events in cache (not even the first and last which is always cached)
        // so there are no events at all
        if (map.isEmpty())
            foundOffset = -1;
        else if (!found) {
            Assert.isTrue(lowerKey.compareTo(key) <= 0 && key.compareTo(upperKey) <= 0);
            Assert.isTrue(foundOffset == -1 || (lowerOffset <= foundOffset && foundOffset <= upperOffset));
            // if we still have a key range then use a binary search to look up the closest match
            if (foundOffset == -1 || !lowerKey.equals(upperKey)) {
                RefObject<T> tempRef_lowerKey2 = new RefObject<T>(lowerKey);
                RefObject<T> tempRef_upperKey2 = new RefObject<T>(upperKey);
                RefObject<Long> tempRef_lowerOffset2 = new RefObject<Long>(lowerOffset);
                RefObject<Long> tempRef_upperOffset2 = new RefObject<Long>(upperOffset);
                foundOffset = binarySearchForOffset(key, matchKind, tempRef_lowerKey2, tempRef_upperKey2, tempRef_lowerOffset2, tempRef_upperOffset2);
                upperOffset = tempRef_upperOffset2.argValue;
                lowerOffset = tempRef_lowerOffset2.argValue;
                upperKey = tempRef_upperKey2.argValue;
                lowerKey = tempRef_lowerKey2.argValue;
            }

            boolean exactMatchFound = lowerKey.equals(key) && upperKey.equals(key);
            // finally use linear search to find the requested offset
            if (matchKind == MatchKind.EXACT) {
                if (foundOffset != -1 && key instanceof BigDecimal) {
                    // check if there are multiple events with the same simulation time
                    long firstOffset = linearSearchForOffset(key, foundOffset, false, true);
                    long lastOffset = linearSearchForOffset(key, foundOffset, true, true);

                    if (foundOffset != lastOffset || foundOffset != firstOffset)
                        throw new RuntimeException("Found non unique simulation time when exact match is requested");
                }
            }
            else {
                boolean forward;
                long searchOffset = -1;
                if (exactMatchFound) {
                    forward = matchKind == MatchKind.LAST_OR_NEXT || matchKind == MatchKind.LAST_OR_PREVIOUS;
                    searchOffset = foundOffset;
                }
                else {
                    forward = matchKind == MatchKind.FIRST_OR_NEXT || matchKind == MatchKind.LAST_OR_NEXT;
                    if (forward)
                        searchOffset = lowerOffset;
                    else {
                        reader.seekTo(upperOffset);
                        reader.getNextLine();
                        searchOffset = reader.getCurrentLineStartOffset();
                    }
                }
                foundOffset = linearSearchForOffset(key, searchOffset, forward, exactMatchFound);
            }
            Assert.isTrue(foundOffset == -1 || isEventBeginOffset(foundOffset));
        }
        return foundOffset;
    }

    /**
     * Search the internal cache finding the file offset(s and keys) for the given key with the given match kind.
     * Sets lower and upper keys and offsets to the closest appropriate values found in the cache.
     * Lower is less than or equal to key, while upper is greater than or equal that could theoretically be found in the eventlog file.
     * Sets found offset or returns false if the offset cannot be exactly determined.
     */
    protected final <T> boolean cacheSearchForOffset(TreeMap<T, CacheEntry> map, T key, MatchKind matchKind, RefObject<T> lowerKey, RefObject<T> upperKey, RefObject<Long> foundOffset, RefObject<Long> lowerOffset, RefObject<Long> upperOffset) {
        ensureFirstEventAndLastEventCached();
        T keyValue = (T)key;
        var it = map.ceilingEntry(keyValue); // greater or equal
        // if exact match found
        if (it != null && it.getKey().equals(keyValue)) {
            CacheEntry cacheEntry = it.getValue();
            // for event numbers there can be only one exact match so we can safely return it independently of matchKind
            if (key instanceof Long) {
                foundOffset.argValue = cacheEntry.beginOffset;
                return true;
            }
            else {
                // for simulation times we must consider whether the cache entry is complete or not by looking around it
                var itUpper = it;
                var itLower = it;
                itUpper = map.higherEntry(itUpper.getKey());
                if (!itLower.equals(map.firstEntry()))
                    itLower = map.lowerEntry(itLower.getKey());
                else
                    itLower = null;
                // subsequent events may or may not have subsequent event numbers in the eventlog file
                // the end offset for the nth event is less than or equal to the begin offset of the nth + 1 event
                boolean completeBegin = itLower == null || (itLower.getValue().endOffset == cacheEntry.beginOffset || itLower.getValue().endEventNumber + 1 == cacheEntry.beginEventNumber);
                boolean completeEnd = itUpper != null && (itUpper.getValue().beginOffset == cacheEntry.endOffset || itUpper.getValue().beginEventNumber == cacheEntry.endEventNumber + 1);
                // dispatching on match kind is required
                switch (matchKind) {
                    case EXACT:
                        if (completeBegin && completeEnd) {
                            Assert.isTrue(cacheEntry.beginEventNumber == cacheEntry.endEventNumber);
                            foundOffset.argValue = cacheEntry.beginOffset;
                            return true;
                        }
                        break;
                    case FIRST_OR_PREVIOUS:
                    case FIRST_OR_NEXT:
                        if (completeBegin) {
                            foundOffset.argValue = cacheEntry.beginOffset;
                            return true;
                        }
                        break;

                    case LAST_OR_PREVIOUS:
                    case LAST_OR_NEXT:
                        if (completeEnd) {
                            foundOffset.argValue = cacheEntry.endEventBeginOffset;
                            return true;
                        }
                        break;
                }
                // cannot exactly determine from cache
                Assert.isTrue(key != null);
                lowerKey.argValue = key;
                lowerOffset.argValue = cacheEntry.beginOffset;
                upperKey.argValue = key;
                upperOffset.argValue = cacheEntry.endOffset;
                // an event's begin offset must be returned
                foundOffset.argValue = cacheEntry.beginOffset;
                return false;
            }
        }
        else {
            // upper iterator refers to the closest element after the key
            var itUpper = it;
            if (itUpper != null) {
                CacheEntry cacheEntry = itUpper.getValue();
                upperKey.argValue = cacheEntry.getBeginKey(upperKey.argValue);
                upperOffset.argValue = cacheEntry.beginOffset;
            }
            else {
                upperKey.argValue = getKey(key, getLastEventNumber(), getLastSimulationTime());
                upperOffset.argValue = reader.getFileSize(); // this has to match last event's end offset
            }
            // lower iterator refers to the closest element before the key
            var itLower = it;
            if (itLower != null && !map.isEmpty() && !itLower.equals(map.firstEntry())) {
                itLower = map.lowerEntry(itLower.getKey());
                CacheEntry cacheEntry = itLower.getValue();
                Assert.isTrue(cacheEntry.getEndKey(lowerKey.argValue) != null);
                lowerKey.argValue = cacheEntry.getEndKey(lowerKey.argValue);
                lowerOffset.argValue = cacheEntry.endOffset;
                Assert.isTrue(lowerKey.argValue != null);
            }
            else {
                long firstEventNumber = getFirstEventNumber();
                if (firstEventNumber != -1) {
                    lowerKey.argValue = getKey(key, firstEventNumber, getFirstSimulationTime());
                    lowerOffset.argValue = getFirstEventOffset();
                    Assert.isTrue(lowerKey.argValue != null);
                }
            }
            // if the closest element before and after are subsequent elements
            // then the cache is complete around the key so the exact offset can be determined
            if (lowerOffset.argValue.equals(upperOffset.argValue)) {
                switch (matchKind) {
                    case EXACT:
                        // we did not get an exact match in the first place (see above)
                        foundOffset.argValue = -1L;
                        break;
                    case FIRST_OR_PREVIOUS:
                    case LAST_OR_PREVIOUS:
                        if (itLower == null)
                            foundOffset.argValue = -1L;
                        else
                            foundOffset.argValue = itLower.getValue().endEventBeginOffset;
                        break;
                    case FIRST_OR_NEXT:
                    case LAST_OR_NEXT:
                        if (itUpper == null)
                            foundOffset.argValue = -1L;
                        else
                            foundOffset.argValue = itUpper.getValue().beginOffset;
                        break;
                }
                return true;
            }
            else {
                // not possible to determine exact offset from cache
                foundOffset.argValue = -1L;
                return false;
            }
        }
    }

    /**
     * Binary search through the eventlog file finding the file offset for the given key with the given match kind.
     * Sets the closest lower and upper keys and offsets around the key found in the eventlog file.
     * Returns -1 if the offset cannot be determined.
     */
    protected final <T extends Comparable<T>> long binarySearchForOffset(T key, MatchKind matchKind, RefObject<T> lowerKey, RefObject<T> upperKey, RefObject<Long> lowerOffset, RefObject<Long> upperOffset)
    {
        Assert.isTrue(key.compareTo(getKey(key, 0L, BigDecimal.getZero())) >= 0);
        long foundOffset = -1;
        long middleEventBeginOffset = -1;
//        long middleEventEndOffset = -1;
        long middleEventNumber = -1;
        BigDecimal middleSimulationTime;
        // Binary search
        //
        // IMPORTANT NOTE: lowerOffset will always be exactly on an "E" line
        // (that of lowerKey), but upperOffset will NOT! This is necessary
        // for the distance between lowerOffset and upperOffset to shrink properly.
//        int stepCount = 0;
        while (true) {
//            stepCount++;
            long middleOffset = (upperOffset.argValue + lowerOffset.argValue) / 2;
            // System.out.println("step %d: offsets: lo=%ld, up=%ld, middle=%ld \t\tkey#: lo=#%ld", stepCount, lowerOffset, upperOffset, middleOffset, lowerKey);
            ReadToEventLineResult result = new ReadToEventLineResult();
            boolean success = readToEventLine(true, middleOffset, result);
            middleEventNumber = result.eventNumber;
            middleSimulationTime = result.simulationTime;
            middleEventBeginOffset = result.lineBeginOffset;
//            middleEventEndOffset = result.lineEndOffset;
            if (success) {
                // System.out.println("  found event #%ld at offset=%ld", middleEventNumber, middleEventBeginOffset);
                T middleKey = getKey(key, middleEventNumber, middleSimulationTime);
                // assign "middle" to "lower" or "upper"
                if (middleKey.compareTo(key) < 0) {
                    lowerKey.argValue = middleKey;
                    lowerOffset.argValue = lowerOffset.argValue == middleEventBeginOffset ? middleEventBeginOffset + 1 : middleEventBeginOffset;
                }
                else if (middleKey.compareTo(key) > 0) {
                    upperKey.argValue = middleKey;
                    upperOffset.argValue = upperOffset.argValue == middleOffset ? middleOffset - 1 : middleOffset;
                }
                // stopping condition
                else if (middleKey.equals(key)) {
                    lowerKey.argValue = upperKey.argValue = key;
                    foundOffset = lowerOffset.argValue = upperOffset.argValue = middleEventBeginOffset;
                    break;
                }
            }
            else {
                // System.out.println("  NOTHING found, decreasing upperOffset");
                // no key found -- we must be at the very end of the file.
                // try again finding an "E" line from a bit earlier point in the file.
                upperOffset.argValue = middleOffset;
            }
            if (lowerOffset.argValue >= upperOffset.argValue) {
                foundOffset = -1;
                break;
            }
        }
//        System.out.println("Binary search steps: " + stepCount);
        return foundOffset;
    }

    /**
     * Linear search through the eventlog file finding the file offset for the given key.
     * The search starts from begin offset and continues in the direction specified by the forward flag.
     * Returns -1 if no such event found otherwise the last exact match or the first non exact match depending
     * on the exact match required flag.
     */
    protected final <T extends Comparable<T>> long linearSearchForOffset(T key, long beginOffset, boolean forward, boolean exactMatchRequired) {
        Assert.isTrue(beginOffset >= 0);
        long eventNumber = -1;
        BigDecimal simulationTime;
        long lineBeginOffset = -1;
        long lineEndOffset = -1;
        long previousOffset = beginOffset;
        while (beginOffset != -1) {
            ReadToEventLineResult result = new ReadToEventLineResult();
            boolean success = readToEventLine(forward, beginOffset, result);
            eventNumber = result.eventNumber;
            simulationTime = result.simulationTime;
            lineBeginOffset = result.lineBeginOffset;
            lineEndOffset = result.lineEndOffset;
            if (!success) {
                if (exactMatchRequired)
                    return previousOffset;
                else
                    return -1;
            }
            T readKey = getKey(key, eventNumber, simulationTime);
            if (!exactMatchRequired) {
                if (forward) {
                    if (readKey.compareTo(key) > 0)
                        return lineBeginOffset;
                }
                else {
                    if (readKey.compareTo(key) < 0)
                        return lineBeginOffset;
                }
            }
            else if (!readKey.equals(key))
                return previousOffset;
            previousOffset = lineBeginOffset;
            if (forward)
                beginOffset = lineEndOffset;
            else
                beginOffset = lineBeginOffset;
        }
        return -1;
    }

    protected void clearInternalState() {
        super.clearInternalState();
        firstEventNumber = EventNumberKind.EVENT_NOT_YET_CALCULATED;
        lastEventNumber = EventNumberKind.EVENT_NOT_YET_CALCULATED;
        firstSimulationTime = BigDecimal.getMinusOne();
        lastSimulationTime = BigDecimal.getMinusOne();
        firstEventOffset = -1;
        lastEventOffset = -1;
        eventNumberToCacheEntryMap.clear();
        simulationTimeToCacheEntryMap.clear();
    }

    protected final boolean isEventBeginOffset(long offset) {
        reader.seekTo(offset);
        String line = reader.getNextLine();
        return line != null && line.charAt(0) == 'E';
    }

    public void synchronize(int change) {
        if (change != FileReader.FileChange.UNCHANGED)
        {
            reader.synchronize(change);
            switch (change)
            {
                case FileReader.FileChange.OVERWRITTEN:
                    clearInternalState();
                    break;
                case FileReader.FileChange.APPENDED:
                    eventNumberToCacheEntryMap.remove(lastEventNumber);
                    simulationTimeToCacheEntryMap.remove(lastSimulationTime);
                    lastEventNumber = EventNumberKind.EVENT_NOT_YET_CALCULATED;
                    lastSimulationTime = BigDecimal.getMinusOne();
                    lastEventOffset = -1;
                    break;
                default:
                    throw new RuntimeException("Unknown file change");
            }
        }
    }

    public final long getFirstEventNumber() {
        if (firstEventNumber == EventNumberKind.EVENT_NOT_YET_CALCULATED) {
            ReadToEventLineResult result = new ReadToEventLineResult();
            if (readToEventLine(true, 0, result)) {
                firstEventNumber = result.eventNumber;
                firstSimulationTime = result.simulationTime;
                firstEventOffset = result.lineBeginOffset;
            }
        }
        return firstEventNumber;
    }

    public final long getLastEventNumber() {
        if (lastEventNumber == EventNumberKind.EVENT_NOT_YET_CALCULATED) {
            ReadToEventLineResult result = new ReadToEventLineResult();
            if (readToEventLine(false, reader.getFileSize(), result)) {
                lastEventNumber = result.eventNumber;
                lastSimulationTime = result.simulationTime;
                lastEventOffset = result.lineBeginOffset;
                cacheEntry(lastEventNumber, lastSimulationTime, lastEventOffset, reader.getFileSize());
            }
        }
        return lastEventNumber;
    }

    public final BigDecimal getFirstSimulationTime() {
        getFirstEventNumber();
        return firstSimulationTime;
    }

    public final BigDecimal getLastSimulationTime() {
        getLastEventNumber();
        return lastSimulationTime;
    }

    public final long getFirstEventOffset() {
        getFirstEventNumber();
        return firstEventOffset;
    }

    public final long getLastEventOffset() {
        getLastEventNumber();
        return lastEventOffset;
    }

    public final void ensureFirstEventAndLastEventCached() {
        getFirstEventNumber();
        getLastEventNumber();
    }

    /**
     * Returns the begin file offset of an event reading linearly backward from its end file offset.
     */
    public final long getBeginOffsetForEndOffset(long endOffset) {
        Assert.isTrue(endOffset >= 0);
        ReadToEventLineResult result = new ReadToEventLineResult();
        if (readToEventLine(false, endOffset, result))
            return result.lineBeginOffset;
        else
            return -1;
    }

    /**
     * Returns the end file offset of an event reading linearly forward from its begin file offset.
     */
    public final long getEndOffsetForBeginOffset(long beginOffset) {
        Assert.isTrue(beginOffset >= 0);
        ReadToEventLineResult result = new ReadToEventLineResult();
        if (readToEventLine(true, beginOffset + 1, result))
            return result.lineBeginOffset;
        else
            return reader.getFileSize();
    }

    /**
     * Returns the begin file offset of the requested event. See MatchKind for details.
     */
    public long getOffsetForEventNumber(long eventNumber, MatchKind matchKind) {
        Assert.isTrue(eventNumber >= 0);
        long offset = searchForOffset(eventNumberToCacheEntryMap, eventNumber, matchKind);
//        System.out.println("Found event number: " + eventNumber + " for match kind: " + matchKind + " at offset: " + offset);
        return offset;
    }

    /**
     * Returns the begin file offset of the requested simulation time. See MatchKind for details.
     */
    public long getOffsetForSimulationTime(BigDecimal simulationTime, MatchKind matchKind) {
        Assert.isTrue(simulationTime.greaterOrEqual(BigDecimal.getZero()));
        long offset = searchForOffset(simulationTimeToCacheEntryMap, simulationTime, matchKind);
//        System.out.println("Found simulation time: " + simulationTime + " for match kind: " + matchKind + " at offset: " + offset);
        return offset;
    }

    /**
     * Returns true if OK, false if no "E" line found till the end of file in the given direction.
     * Reads the first event line in the given direction starting from the given offset.
     */
    public static class ReadToEventLineResult {
        long eventNumber;
        BigDecimal simulationTime;
        long lineBeginOffset;
        long lineEndOffset;
    }

    boolean readToEventLine(boolean forward, long readStartOffset, ReadToEventLineResult result) {
        Assert.isTrue(readStartOffset >= 0);
        result.eventNumber = -1;
        result.simulationTime = BigDecimal.getMinusOne();
        reader.seekTo(readStartOffset);
//        System.out.println("Reading to first event line from offset: " + readStartOffset + " in direction: " + (forward ? "forward" : "backward"));
        String line;
        // find first "E" line, return false if none found
        while (true) {
            if (forward)
                line = reader.getNextLine();
            else
                line = reader.getPreviousLine();
            if (line == null)
                return false;
            if (line.charAt(0) == 'E' && line.charAt(1) == ' ')
                break;
        }
        // find event number and simulation time in line ("# 12345 t 1.2345")
        tokenizer.tokenize(line, (int)reader.getCurrentLineLength());
        result.lineBeginOffset = reader.getCurrentLineStartOffset();
        result.lineEndOffset = reader.getCurrentLineEndOffset();
        int numTokens = tokenizer.numTokens();
        PStringVector tokens = tokenizer.tokensVector();
        for (int i = 1; i < numTokens - 1; i += 2) {
            String token = tokens.get(i);
            if (token.length() != 1)
                continue;
            if (token.charAt(0) == '#')
                result.eventNumber = EventLogEntry.parseEventNumber(tokens.get(i + 1));
            else if (token.charAt(0) == 't')
                result.simulationTime = EventLogEntry.parseSimulationTime(tokens.get(i + 1));
        }
        if (result.eventNumber != -1) {
            Assert.isTrue(!result.simulationTime.equals(BigDecimal.getMinusOne()));
            cacheEntry(result.eventNumber, result.simulationTime, result.lineBeginOffset, result.lineEndOffset);
            return true;
        }
        throw new RuntimeException("Wrong file format: No event number in 'E' line, line " + reader.getNumReadLines());
    }

    @SuppressWarnings("unchecked")
    private <T> T getKey(T key, long eventNumber, BigDecimal simulationTime) {
        if (key instanceof Long)
            return (T)(Long)eventNumber;
        else if (key instanceof BigDecimal)
            return (T)simulationTime;
        else
            throw new RuntimeException();
    }

    /**
     * An entry stores information for a simulation time and the corresponding event number and file offset ranges.
     * It actually describes a range of lines in the eventlog file.
     * The range might be temporarily smaller than what is actually present in the file for the given
     * simulation time or event number. The range gets closer and finally reaches the exact values by subsequent search operations.
     */
    protected static class CacheEntry
    {
        public BigDecimal simulationTime = null;
        public long beginEventNumber = -1; // begin event with simulation time
        public long endEventNumber = -1; // end event with simulation time
        public long beginOffset = -1; // begin offset of begin event
        public long endEventBeginOffset = -1; // begin offset of end event
        public long endOffset = -1; // end offset of end event

        public CacheEntry(long eventNumber, BigDecimal simulationTime, long beginOffset, long endOffset) {
            this.simulationTime = simulationTime;
            this.beginEventNumber = eventNumber;
            this.endEventNumber = eventNumber;
            this.beginOffset = beginOffset;
            this.endEventBeginOffset = beginOffset;
            this.endOffset = endOffset;
        }

        public void include(long eventNumber, BigDecimal simulationTime, long beginOffset, long endOffset) {
            Assert.isTrue(this.simulationTime.equals(simulationTime));
            this.beginEventNumber = Math.min(beginEventNumber, eventNumber);
            this.endEventNumber = Math.max(endEventNumber, eventNumber);
            this.beginOffset = Math.min(this.beginOffset, beginOffset);
            this.endEventBeginOffset = Math.max(this.endEventBeginOffset, beginOffset);
            this.endOffset = Math.max(this.endOffset, endOffset);
        }

        @SuppressWarnings("unchecked")
        public <T> T getBeginKey(T key) {
            if (key instanceof Long)
                return (T)(Long)beginEventNumber;
            else if (key instanceof BigDecimal)
                return (T)simulationTime;
            else
                return null;
        }

        @SuppressWarnings("unchecked")
        public <T> T getEndKey(T key) {
            if (key instanceof Long)
                return (T)(Long)endEventNumber;
            else if (key instanceof BigDecimal)
                return (T)simulationTime;
            else
                return null;
        }
    }
}

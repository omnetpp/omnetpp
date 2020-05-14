package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.List;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.editors.MemoizationCache.Key;
import org.omnetpp.scave.engine.ByteVector;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultsPickler;
import org.omnetpp.scave.engine.ShmSendBuffer;
import org.omnetpp.scave.engine.ShmSendBufferManager;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.pychart.IScaveResultsPickleProvider;
import org.omnetpp.scave.pychart.PythonProcess;

import net.razorvine.pickle.PickleException;

/**
 * Implements the internal parts needed for the functionality of the
 * omnetpp.scave.results Python module provided to chart scripts.
 *
 * But really, this is just a proxy for the _actual_ implementation
 * in C++, in the nativelibs package, through SWIG.
 *
 * It also takes care of storing the SHM object names (and sizes)
 * created by the C++ part into the associated PythonProcess.
 *
 * @author attila, andras
 */
public class ResultsProvider implements IScaveResultsPickleProvider {
    private ResultFileManager manager;
    private ShmSendBufferManager sendBufferManager;
    private ResultsPickler pickler;
    private MemoizationCache memoizationCache;
    private InterruptedFlag interrupted;

    public ResultsProvider(ResultFileManager rfm, PythonProcess proc, MemoizationCache mc) {
        this.manager = rfm;
        this.sendBufferManager = proc.getShmSendBufferManager();
        this.interrupted = proc.getInterruptedFlag();
        this.pickler = new ResultsPickler(rfm, proc.getShmSendBufferManager(), interrupted);
        this.memoizationCache = mc;
    }

    private interface PicklerFunction {
        ShmSendBuffer call() throws PickleException, IOException;
    };

    private String memoize(Key key, PicklerFunction pickler) throws PickleException, IOException {
        ByteVector cachedReply = memoizationCache.get(key);
        if (cachedReply != null) {
            Debug.println("ResultsProvider." + key.getMethodName() + ": returning memoized reply");
            return sendBufferManager.create("memoized", cachedReply).getNameAndSize();
        }
        else {
            Debug.println("ResultsProvider." + key.getMethodName() + ": computing and memoizing reply");
            ShmSendBuffer pickle = Debug.timed("ResultsProvider." + key.getMethodName(), 100, () -> pickler.call());
            memoizationCache.put(key, pickle.getContentCopy());
            return pickle.getNameAndSize();
        }
    }

    public int getSerial() {
        return manager.getSerial();
    }

    public String getRunsPickle(String filterExpression) throws PickleException, IOException {
        Key key = new Key("getRunsPickle", filterExpression);
        return memoize(key, () -> pickler.getRunsPickle(filterExpression));
    }

    public String getRunAttrsPickle(String filterExpression) throws PickleException, IOException {
        Key key = new Key("getRunAttrsPickle", filterExpression);
        return memoize(key, () -> pickler.getRunattrsPickle(filterExpression));
    }

    @Override
    public String getRunAttrsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        Key key = new Key("getRunAttrsPickle", runIDs);
        return memoize(key, () -> pickler.getRunattrsForRunsPickle(toStringVector(runIDs)));
    }

    @Override
    public String getItervarsPickle(String filterExpression) throws PickleException, IOException {
        Key key = new Key("getItervarsPickle", filterExpression);
        return memoize(key, () -> pickler.getItervarsPickle(filterExpression));
    }

    @Override
    public String getItervarsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        Key key = new Key("getItervarsPickle", runIDs);
        return memoize(key, () -> pickler.getItervarsForRunsPickle(toStringVector(runIDs)));
    }

    @Override
    public String getParamAssignmentsPickle(String filterExpression) throws PickleException, IOException {
        Key key = new Key("getParamAssignmentsPickle", filterExpression);
        return memoize(key, () -> pickler.getParamAssignmentsPickle(filterExpression));
    }

    @Override
    public String getParamAssignmentsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        Key key = new Key("getParamAssignmentsPickle", runIDs);
        return memoize(key, () -> pickler.getParamAssignmentsForRunsPickle(toStringVector(runIDs)));
    }

    @Override
    public String getConfigEntriesPickle(String filterExpression) throws PickleException, IOException {
        Key key = new Key("getConfigEntriesPickle", filterExpression);
        return memoize(key, () -> pickler.getConfigEntriesPickle(filterExpression));
    }

    @Override
    public String getConfigEntriesForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        Key key = new Key("getConfigEntriesForRunsPickle", runIDs);
        return memoize(key, () -> pickler.getConfigEntriesForRunsPickle(toStringVector(runIDs)));
    }

    public String getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        //TODO: cannot memoize due to the embedded vecx/vecy shmems (as we don't know about them)
        IDList idList = memoizationCache.getCachedFilterResult(filterExpression);
        if (idList == null) {
            idList = manager.filterIDList(manager.getAllParameters(), filterExpression);
            memoizationCache.putCachedFilterResult(filterExpression, idList);
        }
        return pickler.getCsvResultsPickle(idList, toStringVector(rowTypes), omitUnusedColumns, simTimeStart, simTimeEnd).getNameAndSize();
    }

    @Override
    public String getParamValuesPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getParamValuesPickle", filterExpression, includeAttrs);
        return memoize(key, () -> {
            IDList idList = memoizationCache.getCachedFilterResult(filterExpression);
            if (idList == null)
                idList = manager.filterIDList(manager.getAllParameters(), filterExpression); // no need to cache, as result will be (likely) memoized
            return pickler.getParamValuesPickle(idList, includeAttrs);
        });
    }

    @Override
    public String getScalarsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getScalarsPickle", filterExpression, includeAttrs);
        return memoize(key, () -> {
            IDList idList = memoizationCache.getCachedFilterResult(filterExpression);
            if (idList == null)
                idList = manager.filterIDList(manager.getAllScalars(true), filterExpression, -1, interrupted); // no need to cache, as result will be (likely) memoized
            return pickler.getScalarsPickle(idList, includeAttrs);
        });
    }

    @Override
    public String getVectorsPickle(String filterExpression, boolean includeAttrs, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        //TODO: cannot memoize due to the embedded vecx/vecy shmems (as we don't know about them)
        IDList idList = memoizationCache.getCachedFilterResult(filterExpression);
        if (idList == null) {
            idList = manager.filterIDList(manager.getAllParameters(), filterExpression, -1, interrupted);
            memoizationCache.putCachedFilterResult(filterExpression, idList);
        }
        return pickler.getVectorsPickle(filterExpression, includeAttrs, simTimeStart, simTimeEnd).getNameAndSize();
    }

    @Override
    public String getStatisticsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getStatisticsPickle", filterExpression, includeAttrs);
        return memoize(key, () -> {
            IDList idList = memoizationCache.getCachedFilterResult(filterExpression);
            if (idList == null)
                idList = manager.filterIDList(manager.getAllStatistics(), filterExpression, -1, interrupted); // no need to cache, as result will be (likely) memoized
            return pickler.getStatisticsPickle(idList, includeAttrs);
        });
    }

    @Override
    public String getHistogramsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getHistogramsPickle", filterExpression, includeAttrs);
        return memoize(key, () -> {
            IDList idList = memoizationCache.getCachedFilterResult(filterExpression);
            if (idList == null)
                idList = manager.filterIDList(manager.getAllHistograms(), filterExpression, -1, interrupted); // no need to cache, as result will be (likely) memoized
            return pickler.getHistogramsPickle(idList, includeAttrs);
        });
    }

    protected static StringVector toStringVector(List<String> runIDs) {
        return StringVector.fromArray(runIDs.toArray(new String[] {}));
    }
}
package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.Debug;
import org.omnetpp.scave.editors.MemoizationCache.Key;
import org.omnetpp.scave.engine.ByteVector;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultsPickler;
import org.omnetpp.scave.engine.ShmSendBuffer;
import org.omnetpp.scave.engine.ShmSendBufferManager;
import org.omnetpp.scave.engine.ShmSendBufferVector;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.pychart.IScaveResultsPickleProvider;
import org.omnetpp.scave.pychart.PythonProcess;

import net.razorvine.pickle.PickleException;

/**
 * Implements the internal parts needed for the functionality of the
 * omnetpp.scave.results Python module provided to chart scripts.
 *
 * It takes advantage of a MemoizationCache to make both result filtering
 * (mostly after the user plotted a set of selected result items),
 * and result pickling faster by reusing previous pickles.
 *
 * If no cached result is found, the pickled data is generated using
 * the ResultsPickler (implemented in C++) from the nativelibs package.
 *
 * @author attila, andras
 */
public class ResultsProvider implements IScaveResultsPickleProvider {
    private ResultFileManager manager;
    private ShmSendBufferManager sendBufferManager;
    private ResultsPickler pickler;
    private MemoizationCache memoizationCache;
    private FilterCache filterCache;
    private InterruptedFlag interrupted;

    public ResultsProvider(ResultFileManager rfm, PythonProcess proc, MemoizationCache mc, FilterCache fc) {
        this.manager = rfm;
        this.sendBufferManager = proc.getShmSendBufferManager();
        this.interrupted = proc.getInterruptedFlag();
        this.pickler = new ResultsPickler(rfm, proc.getShmSendBufferManager(), interrupted);
        this.memoizationCache = mc;
        this.filterCache = fc;
    }

    private interface PicklerFunction {
        ShmSendBuffer call() throws PickleException, IOException;
    };

    private interface PicklerFunctionVec {
        ShmSendBufferVector call() throws PickleException, IOException;
    };

    private String memoize(Key key, PicklerFunction pickler) throws PickleException, IOException {
        List<ByteVector> cachedReply = memoizationCache.get(key);

        if (cachedReply != null) {
            Assert.isTrue(cachedReply.size() == 1);
            Debug.println("ResultsProvider." + key.getMethodName() + ": returning memoized reply");
            return sendBufferManager.create("memoized", cachedReply.get(0)).getNameAndTotalSize();
        }
        else {
            Debug.println("ResultsProvider." + key.getMethodName() + ": computing and memoizing reply");
            ShmSendBuffer pickle = Debug.timed("ResultsProvider." + key.getMethodName(), 100, () -> pickler.call());
            memoizationCache.put(key, pickle.getContentCopy());
            return pickle.getNameAndTotalSize();
        }
    }

    private List<String> memoize(Key key, PicklerFunctionVec pickler) throws PickleException, IOException {
        List<ByteVector> cachedReply = memoizationCache.get(key);
        List<String> result = new ArrayList<String>();
        if (cachedReply != null) {
            Debug.println("ResultsProvider." + key.getMethodName() + ": returning memoized reply");
            for (int i = 0; i < cachedReply.size(); ++i)
                result.add(sendBufferManager.create("memoized", cachedReply.get(i)).getNameAndTotalSize());
        }
        else {
            Debug.println("ResultsProvider." + key.getMethodName() + ": computing and memoizing reply");
            ShmSendBufferVector pickles = Debug.timed("ResultsProvider." + key.getMethodName(), 100, () -> pickler.call());
            List<ByteVector> intoCache = new ArrayList<>();
            for (int i = 0; i < pickles.size(); ++i) {
                ShmSendBuffer buf = pickles.get(i);
                result.add(buf.getNameAndTotalSize());
                intoCache.add(buf.getContentCopy());
            }
            memoizationCache.put(key, intoCache);
        }
        return result;
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

    public List<String> getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, boolean includeFieldsAsScalars, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        Key key = new Key("getResultsPickle", filterExpression, rowTypes, omitUnusedColumns, includeFieldsAsScalars, simTimeStart, simTimeEnd);
        List<String> names = memoize(key, (PicklerFunctionVec) () -> {
            int allTypes = ResultFileManager.PARAMETER | ResultFileManager.SCALAR | ResultFileManager.VECTOR | ResultFileManager.STATISTICS | ResultFileManager.HISTOGRAM;
            IDList idList = filterCache.getFilterResult(allTypes, filterExpression, includeFieldsAsScalars);
            if (idList == null)
                idList = manager.filterIDList(manager.getAllItems(includeFieldsAsScalars), filterExpression); // no need to cache, as result will be (likely) memoized
            return pickler.getCsvResultsPickle(idList, toStringVector(rowTypes), omitUnusedColumns, simTimeStart, simTimeEnd);
        });
        return names;
    }

    @Override
    public String getParamValuesPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getParamValuesPickle", filterExpression, includeAttrs);
        return memoize(key, () -> {
            IDList idList = filterCache.getFilterResult(ResultFileManager.PARAMETER, filterExpression);
            if (idList == null)
                idList = manager.filterIDList(manager.getAllParameters(), filterExpression); // no need to cache, as result will be (likely) memoized
            return pickler.getParamValuesPickle(idList, includeAttrs);
        });
    }

    @Override
    public String getScalarsPickle(String filterExpression, boolean includeAttrs, boolean includeFields) throws PickleException, IOException {
        Key key = new Key("getScalarsPickle", filterExpression, includeAttrs, includeFields);
        return memoize(key, () -> {
            IDList idList = filterCache.getFilterResult(ResultFileManager.SCALAR, filterExpression, includeFields);
            if (idList == null)
                idList = manager.filterIDList(manager.getAllScalars(includeFields), filterExpression, -1, interrupted); // no need to cache, as result will be (likely) memoized
            return pickler.getScalarsPickle(idList, includeAttrs);
        });
    }

    @Override
    public List<String> getVectorsPickle(String filterExpression, boolean includeAttrs, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        Key key = new Key("getVectorsPickle", filterExpression, includeAttrs, simTimeStart, simTimeEnd);
        List<String> names = memoize(key, (PicklerFunctionVec) () -> {
            IDList idList = filterCache.getFilterResult(ResultFileManager.VECTOR, filterExpression);
            if (idList == null)
                idList = manager.filterIDList(manager.getAllVectors(), filterExpression, -1, interrupted); // no need to cache, as result will be (likely) memoized
            return pickler.getVectorsPickle(idList, includeAttrs, simTimeStart, simTimeEnd);
        });
        return names;
    }

    @Override
    public String getStatisticsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getStatisticsPickle", filterExpression, includeAttrs);
        return memoize(key, () -> {
            IDList idList = filterCache.getFilterResult(ResultFileManager.STATISTICS | ResultFileManager.HISTOGRAM, filterExpression);
            if (idList != null)
                    idList = idList.filterByTypes(ResultFileManager.STATISTICS);
            if (idList == null)
                idList = manager.filterIDList(manager.getAllStatistics(), filterExpression, -1, interrupted); // no need to cache, as result will be (likely) memoized
            return pickler.getStatisticsPickle(idList, includeAttrs);
        });
    }

    @Override
    public String getHistogramsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getHistogramsPickle", filterExpression, includeAttrs);
        return memoize(key, () -> {
            IDList idList = filterCache.getFilterResult(ResultFileManager.HISTOGRAM | ResultFileManager.HISTOGRAM, filterExpression);
            if (idList != null)
                idList = idList.filterByTypes(ResultFileManager.HISTOGRAM);
            if (idList == null)
                idList = manager.filterIDList(manager.getAllHistograms(), filterExpression, -1, interrupted); // no need to cache, as result will be (likely) memoized
            return pickler.getHistogramsPickle(idList, includeAttrs);
        });
    }

    protected static StringVector toStringVector(List<String> runIDs) {
        return StringVector.fromArray(runIDs.toArray(new String[] {}));
    }
}
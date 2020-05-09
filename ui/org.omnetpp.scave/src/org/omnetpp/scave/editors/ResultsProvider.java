package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.List;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.editors.MemoizationCache.Key;
import org.omnetpp.scave.engine.ByteVector;
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
 * @author attila
 */
public class ResultsProvider implements IScaveResultsPickleProvider {
    private ResultFileManager rfm;
    private ShmSendBufferManager sbm;
    private ResultsPickler rp;
    private MemoizationCache mc;

    public ResultsProvider(ResultFileManager rfm, PythonProcess proc, MemoizationCache mc) {
        this.rfm = rfm;
        this.sbm = proc.getShmSendBufferManager();
        this.rp = new ResultsPickler(rfm, proc.getShmSendBufferManager(), proc.getInterruptedFlag());
        this.mc = mc;
    }

    private interface PicklerFunction {
        ShmSendBuffer call() throws PickleException, IOException;
    };

    private String memoize(Key key, PicklerFunction pickler) throws PickleException, IOException {
        ByteVector cachedReply = mc.get(key);
        if (cachedReply != null) {
            Debug.println("ResultsProvider: returning memoized reply");
            return sbm.create("memoized", cachedReply).getNameAndSize();
        }
        else {
            Debug.println("ResultsProvider: computing and memoizing reply");
            ShmSendBuffer pickle = Debug.timed("ResultsProvider." + key.getMethodName(), 100, () -> pickler.call());
            mc.put(key, pickle.getContentCopy());
            return pickle.getNameAndSize();
        }
    }

    public int getSerial() {
        return rfm.getSerial();
    }

    public String getRunsPickle(String filterExpression) throws PickleException, IOException {
        Key key = new Key("getRunsPickle", filterExpression);
        return memoize(key, () -> rp.getRunsPickle(filterExpression));
    }

    public String getRunAttrsPickle(String filterExpression) throws PickleException, IOException {
        Key key = new Key("getRunAttrsPickle", filterExpression);
        return memoize(key, () -> rp.getRunattrsPickle(filterExpression));
    }

    @Override
    public String getRunAttrsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        Key key = new Key("getRunAttrsPickle", runIDs);
        return memoize(key, () -> rp.getRunattrsForRunsPickle(toStringVector(runIDs)));
    }

    @Override
    public String getItervarsPickle(String filterExpression) throws PickleException, IOException {
        Key key = new Key("getItervarsPickle", filterExpression);
        return memoize(key, () -> rp.getItervarsPickle(filterExpression));
    }

    @Override
    public String getItervarsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        Key key = new Key("getItervarsPickle", runIDs);
        return memoize(key, () -> rp.getItervarsForRunsPickle(toStringVector(runIDs)));
    }

    @Override
    public String getParamAssignmentsPickle(String filterExpression) throws PickleException, IOException {
        Key key = new Key("getParamAssignmentsPickle", filterExpression);
        return memoize(key, () -> rp.getParamAssignmentsPickle(filterExpression));
    }

    @Override
    public String getParamAssignmentsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        Key key = new Key("getParamAssignmentsPickle", runIDs);
        return memoize(key, () -> rp.getParamAssignmentsForRunsPickle(toStringVector(runIDs)));
    }

    @Override
    public String getConfigEntriesPickle(String filterExpression) throws PickleException, IOException {
        Key key = new Key("getConfigEntriesPickle", filterExpression);
        return memoize(key, () -> rp.getConfigEntriesPickle(filterExpression));
    }

    @Override
    public String getConfigEntriesForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        Key key = new Key("getConfigEntriesForRunsPickle", runIDs);
        return memoize(key, () -> rp.getConfigEntriesForRunsPickle(toStringVector(runIDs)));
    }

    public String getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        Key key = new Key("getResultsPickle", rowTypes, omitUnusedColumns, simTimeStart, simTimeEnd);
        return memoize(key, () -> rp.getCsvResultsPickle(filterExpression, toStringVector(rowTypes), omitUnusedColumns, simTimeStart, simTimeEnd));
    }

    @Override
    public String getParamValuesPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getParamValuesPickle", filterExpression, includeAttrs);
        return memoize(key, () -> rp.getParamValuesPickle(filterExpression, includeAttrs));
    }

    @Override
    public String getScalarsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getScalarsPickle", filterExpression, includeAttrs);
        return memoize(key, () -> rp.getScalarsPickle(filterExpression, includeAttrs));
    }

    @Override
    public String getVectorsPickle(String filterExpression, boolean includeAttrs, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        //TODO: currently does not work because the embedded vecx/vecy shmems are not memoized
        //Key key = new Key("getVectorsPickle", filterExpression, includeAttrs, simTimeStart, simTimeEnd);
        //return memoize(key, () -> rp.getVectorsPickle(filterExpression, includeAttrs, simTimeStart, simTimeEnd));
        return rp.getVectorsPickle(filterExpression, includeAttrs, simTimeStart, simTimeEnd).getNameAndSize();
    }

    @Override
    public String getStatisticsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getStatisticsPickle", filterExpression, includeAttrs);
        return memoize(key, () -> rp.getStatisticsPickle(filterExpression, includeAttrs));
    }

    @Override
    public String getHistogramsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        Key key = new Key("getHistogramsPickle", filterExpression, includeAttrs);
        return memoize(key, () -> rp.getHistogramsPickle(filterExpression, includeAttrs));
    }

    protected static StringVector toStringVector(List<String> runIDs) {
        return StringVector.fromArray(runIDs.toArray(new String[] {}));
    }
}
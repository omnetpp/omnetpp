package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.List;

import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultsPickler;
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
    private ResultsPickler rp;

    public ResultsProvider(ResultFileManager rfm, PythonProcess proc) {
        this.rp = new ResultsPickler(rfm, proc.getShmSendBufferManager(), proc.getInterruptedFlag());
    }

    public int getSerial() {
        return rfm.getSerial();
    }

    public String getRunsPickle(String filterExpression) throws PickleException, IOException {
        return rp.getRunsPickle(filterExpression).getNameAndSize();
    }

    public String getRunAttrsPickle(String filterExpression) throws PickleException, IOException {
        return rp.getRunattrsPickle(filterExpression).getNameAndSize();
    }

    @Override
    public String getRunAttrsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return rp.getRunattrsForRunsPickle(toStringVector(runIDs)).getNameAndSize();
    }

    @Override
    public String getItervarsPickle(String filterExpression) throws PickleException, IOException {
        return rp.getItervarsPickle(filterExpression).getNameAndSize();
    }

    @Override
    public String getItervarsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return rp.getItervarsForRunsPickle(toStringVector(runIDs)).getNameAndSize();
    }

    @Override
    public String getParamAssignmentsPickle(String filterExpression) throws PickleException, IOException {
        return rp.getParamAssignmentsPickle(filterExpression).getNameAndSize();
    }

    @Override
    public String getParamAssignmentsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return rp.getParamAssignmentsForRunsPickle(toStringVector(runIDs)).getNameAndSize();
    }

    @Override
    public String getConfigEntriesPickle(String filterExpression) throws PickleException, IOException {
        return rp.getConfigEntriesPickle(filterExpression).getNameAndSize();
    }

    @Override
    public String getConfigEntriesForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return rp.getConfigEntriesForRunsPickle(toStringVector(runIDs)).getNameAndSize();
    }

    public String getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        return rp.getCsvResultsPickle(filterExpression, toStringVector(rowTypes), omitUnusedColumns, simTimeStart, simTimeEnd).getNameAndSize();
    }

    @Override
    public String getParamValuesPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return rp.getParamValuesPickle(filterExpression, includeAttrs).getNameAndSize();
    }

    @Override
    public String getScalarsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return rp.getScalarsPickle(filterExpression, includeAttrs).getNameAndSize();
    }

    @Override
    public String getVectorsPickle(String filterExpression, boolean includeAttrs, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        return rp.getVectorsPickle(filterExpression, includeAttrs, simTimeStart, simTimeEnd).getNameAndSize();
    }

    @Override
    public String getStatisticsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return rp.getStatisticsPickle(filterExpression, includeAttrs).getNameAndSize();
    }

    @Override
    public String getHistogramsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return rp.getHistogramsPickle(filterExpression, includeAttrs).getNameAndSize();
    }

    protected static StringVector toStringVector(List<String> runIDs) {
        return StringVector.fromArray(runIDs.toArray(new String[] {}));
    }
}
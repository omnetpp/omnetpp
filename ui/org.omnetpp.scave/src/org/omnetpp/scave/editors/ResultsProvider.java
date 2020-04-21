package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.List;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultsPickler;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.pychart.IScaveResultsPickleProvider;

import net.razorvine.pickle.PickleException;

public class ResultsProvider implements IScaveResultsPickleProvider {
    ResultFileManager rfm;
    ResultsPickler rp;
    InterruptedFlag interruptedFlag;

    public ResultsProvider(ResultFileManager rfm, InterruptedFlag interruptedFlag) {
        this.rfm = rfm;
        this.rp = new ResultsPickler(rfm);
        this.interruptedFlag = interruptedFlag;
    }

    public String getRunsPickle(String filterExpression) throws PickleException, IOException {
        return Debug.timed("getRunsPickle", 0, () -> rp.getRunsPickle(filterExpression, interruptedFlag));
    }

    public String getRunAttrsPickle(String filterExpression) throws PickleException, IOException {
        return Debug.timed("getRunAttrsPickle", 0, () -> rp.getRunattrsPickle(filterExpression, interruptedFlag));
    }

    @Override
    public String getRunAttrsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return Debug.timed("getRunAttrsForRunsPickle", 0, () -> rp.getRunattrsForRunsPickle(StringVector.fromArray(runIDs.toArray(new String[] {})), interruptedFlag));
    }

    @Override
    public String getItervarsPickle(String filterExpression) throws PickleException, IOException {
        return Debug.timed("getItervarsPickle", 0, () -> rp.getItervarsPickle(filterExpression, interruptedFlag));
    }

    @Override
    public String getItervarsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return Debug.timed("getItervarsForRunsPickle", 0, () -> rp.getItervarsForRunsPickle(StringVector.fromArray(runIDs.toArray(new String[] {})), interruptedFlag));
    }

    @Override
    public String getParamAssignmentsPickle(String filterExpression) throws PickleException, IOException {
        return Debug.timed("getParamAssignmentsPickle", 0, () -> rp.getParamAssignmentsPickle(filterExpression, interruptedFlag));
    }
    @Override
    public String getParamAssignmentsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return Debug.timed("getParamAssignmentsForRunsPickle", 0, () -> rp.getParamAssignmentsForRunsPickle(StringVector.fromArray(runIDs.toArray(new String[] {})), interruptedFlag));
    }

    @Override
    public String getConfigEntriesPickle(String filterExpression) throws PickleException, IOException {
        return Debug.timed("getConfigEntriesPickle", 0, () -> rp.getConfigEntriesPickle(filterExpression, interruptedFlag));
    }

    @Override
    public String getConfigEntriesForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return Debug.timed("getConfigEntriesForRunsPickle", 0, () -> rp.getConfigEntriesForRunsPickle(StringVector.fromArray(runIDs.toArray(new String[] {})), interruptedFlag));
    }

    public String getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        StringVector sv = new StringVector();
        for (String s : rowTypes)
            sv.add(s);
        return Debug.timed("getResultsPickle", 0, () -> rp.getCsvResultsPickle(filterExpression, sv, omitUnusedColumns, simTimeStart, simTimeEnd, interruptedFlag));
    }

    @Override
    public String getParamValuesPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return Debug.timed("getParamValuesPickle", 0, () -> rp.getParamValuesPickle(filterExpression, includeAttrs, interruptedFlag));
    }

    @Override
    public String getScalarsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return Debug.timed("getScalarsPickle", 0, () -> rp.getScalarsPickle(filterExpression, includeAttrs, interruptedFlag));
    }

    @Override
    public String getVectorsPickle(String filterExpression, boolean includeAttrs, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        return Debug.timed("getVectorsPickle", 0, () -> rp.getVectorsPickle(filterExpression, includeAttrs, simTimeStart, simTimeEnd, interruptedFlag));
    }

    @Override
    public String getStatisticsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return Debug.timed("getStatisticsPickle", 0, () -> rp.getStatisticsPickle(filterExpression, includeAttrs, interruptedFlag));
    }

    @Override
    public String getHistogramsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return Debug.timed("getHistogramsPickle", 0, () -> rp.getHistogramsPickle(filterExpression, includeAttrs, interruptedFlag));
    }

}
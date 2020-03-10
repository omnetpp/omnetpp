package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.List;

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
        return rp.getRunsPickle(filterExpression, interruptedFlag);
    }

    public String getRunAttrsPickle(String filterExpression) throws PickleException, IOException {
        return rp.getRunattrsPickle(filterExpression, interruptedFlag);
    }

    @Override
    public String getRunAttrsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return rp.getRunattrsForRunsPickle(StringVector.fromArray(runIDs.toArray(new String[] {})), interruptedFlag);
    }

    @Override
    public String getItervarsPickle(String filterExpression) throws PickleException, IOException {
        return rp.getItervarsPickle(filterExpression, interruptedFlag);
    }

    @Override
    public String getItervarsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return rp.getItervarsForRunsPickle(StringVector.fromArray(runIDs.toArray(new String[] {})), interruptedFlag);
    }

    @Override
    public String getParamAssignmentsPickle(String filterExpression) throws PickleException, IOException {
        return rp.getParamAssignmentsPickle(filterExpression, interruptedFlag);
    }
    @Override
    public String getParamAssignmentsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return rp.getParamAssignmentsForRunsPickle(StringVector.fromArray(runIDs.toArray(new String[] {})), interruptedFlag);
    }

    @Override
    public String getConfigEntriesPickle(String filterExpression) throws PickleException, IOException {
        return rp.getConfigEntriesPickle(filterExpression, interruptedFlag);
    }

    @Override
    public String getConfigEntriesForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return rp.getConfigEntriesForRunsPickle(StringVector.fromArray(runIDs.toArray(new String[] {})), interruptedFlag);
    }

    public String getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        StringVector sv = new StringVector();
        for (String s : rowTypes)
            sv.add(s);
        return rp.getCsvResultsPickle(filterExpression, sv, omitUnusedColumns, simTimeStart, simTimeEnd, interruptedFlag);
    }

    @Override
    public String getParamValuesPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return rp.getParamValuesPickle(filterExpression, includeAttrs, interruptedFlag);
    }

    @Override
    public String getScalarsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return rp.getScalarsPickle(filterExpression, includeAttrs, interruptedFlag);
    }

    @Override
    public String getVectorsPickle(String filterExpression, boolean includeAttrs, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        return rp.getVectorsPickle(filterExpression, includeAttrs, simTimeStart, simTimeEnd, interruptedFlag);
    }

    @Override
    public String getStatisticsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return rp.getStatisticsPickle(filterExpression, includeAttrs, interruptedFlag);
    }

    @Override
    public String getHistogramsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException {
        return rp.getHistogramsPickle(filterExpression, includeAttrs, interruptedFlag);
    }

}
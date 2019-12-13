package org.omnetpp.scave.pychart;

import java.io.IOException;
import java.util.List;

import net.razorvine.pickle.PickleException;

public interface IScaveResultsPickleProvider {
    // metadata about runs
    byte[] getRunsPickle(String filter) throws PickleException, IOException;

    byte[] getRunAttrsPickle(String filter) throws PickleException, IOException;
    byte[] getItervarsPickle(String filter) throws PickleException, IOException;

    byte[] getConfigEntriesPickle(String filter) throws PickleException, IOException;
    byte[] getParamAssignmentsPickle(String filter) throws PickleException, IOException;

    byte[] getItervarsForRunsPickle(List<String> runIDs) throws PickleException, IOException;
    byte[] getRunAttrsForRunsPickle(List<String> runIDs) throws PickleException, IOException;
    byte[] getParamAssignmentsForRunsPickle(List<String> runIDs) throws PickleException, IOException;
    byte[] getConfigEntriesForRunsPickle(List<String> runIDs) throws PickleException, IOException;


    // CSV format
    byte[] getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd) throws PickleException, IOException;

    // simple format
    byte[] getScalarsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException;
    byte[] getVectorsPickle(String filterExpression, boolean includeAttrs, double simTimeStart, double simTimeEnd) throws PickleException, IOException;
    byte[] getStatisticsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException;
    byte[] getHistogramsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException;

    byte[] getParamValuesPickle(String filter, boolean include_attrs) throws PickleException, IOException;
}

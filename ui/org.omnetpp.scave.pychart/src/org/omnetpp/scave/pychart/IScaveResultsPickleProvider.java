package org.omnetpp.scave.pychart;

import java.io.IOException;
import java.util.List;

import net.razorvine.pickle.PickleException;

public interface IScaveResultsPickleProvider {
    // metadata about runs
    String getRunsPickle(String filter) throws PickleException, IOException;

    String getRunAttrsPickle(String filter) throws PickleException, IOException;
    String getItervarsPickle(String filter) throws PickleException, IOException;

    String getConfigEntriesPickle(String filter) throws PickleException, IOException;
    String getParamAssignmentsPickle(String filter) throws PickleException, IOException;

    String getItervarsForRunsPickle(List<String> runIDs) throws PickleException, IOException;
    String getRunAttrsForRunsPickle(List<String> runIDs) throws PickleException, IOException;
    String getParamAssignmentsForRunsPickle(List<String> runIDs) throws PickleException, IOException;
    String getConfigEntriesForRunsPickle(List<String> runIDs) throws PickleException, IOException;

    // CSV format
    String getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd) throws PickleException, IOException;

    // simple format
    String getScalarsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException;
    String getVectorsPickle(String filterExpression, boolean includeAttrs, double simTimeStart, double simTimeEnd) throws PickleException, IOException;
    String getStatisticsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException;
    String getHistogramsPickle(String filterExpression, boolean includeAttrs) throws PickleException, IOException;
    String getParamValuesPickle(String filter, boolean include_attrs) throws PickleException, IOException;
}

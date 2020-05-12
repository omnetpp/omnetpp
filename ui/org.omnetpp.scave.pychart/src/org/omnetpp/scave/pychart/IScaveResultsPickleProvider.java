/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.io.IOException;
import java.util.List;

import net.razorvine.pickle.PickleException;

/**
 * Used by the omnetpp.scave.results Python module (through Py4J) to
 * gain access to the results items loaded into a ResultFileManager.
 *
 *  Every method returns a string which is a space-separated pair of a
 *  shared memory name and its size in bytes (written in decimal form).
 */
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

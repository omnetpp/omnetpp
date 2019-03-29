package org.omnetpp.scave.pychart;

import java.io.IOException;
import java.util.List;

import net.razorvine.pickle.PickleException;

public interface IScaveResultsPickleProvider {
    byte[] getRunsPickle(String filter, boolean includeRunattrs, boolean includeItervars) throws PickleException, IOException;
    byte[] getRunAttrsPickle(String filter) throws PickleException, IOException;
    // TODO merge the one below into the one above, with an extra parameter
    byte[] getRunAttrsForItervarsPickle(String filter) throws PickleException, IOException;
    byte[] getItervarsPickle(String filter) throws PickleException, IOException;
    // TODO: getItervarsForRunAttrsPickle, similar to getRunAttrsForItervarsPickle

    // CSV format
    byte[] getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns) throws PickleException, IOException;

    // simple format
    byte[] getScalarsPickle(String filterExpression, boolean include_attrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException;
    byte[] getVectorsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException;
    byte[] getStatisticsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException;
    byte[] getHistogramsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException;

    byte[] getResultAttrsPickle(String filter) throws PickleException, IOException;
}

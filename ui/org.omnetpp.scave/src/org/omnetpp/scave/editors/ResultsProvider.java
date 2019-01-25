package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.List;

import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.pychart.IScaveResultsPickleProvider;
import org.omnetpp.scave.python.CsvResultsPickler;
import org.omnetpp.scave.python.HistogramResultsPickler;
import org.omnetpp.scave.python.IterVarsPickler;
import org.omnetpp.scave.python.ResultAttrsPickler;
import org.omnetpp.scave.python.ResultPicklingUtils;
import org.omnetpp.scave.python.RunAttrsPickler;
import org.omnetpp.scave.python.RunsPickler;
import org.omnetpp.scave.python.ScalarResultsPickler;
import org.omnetpp.scave.python.StatisticsResultsPickler;
import org.omnetpp.scave.python.VectorResultsPickler;

import net.razorvine.pickle.PickleException;

class ResultsProvider implements IScaveResultsPickleProvider {
    ResultFileManager rfm;

    public ResultsProvider(ResultFileManager rfm) {
        this.rfm = rfm;
    }

    @Override
    public byte[] getRunsPickle(String filterExpression, boolean includeRunattrs, boolean includeItervars) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new RunsPickler(filterExpression, includeRunattrs, includeItervars));
    }

    @Override
    public byte[] getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new CsvResultsPickler(filterExpression, rowTypes, omitUnusedColumns));
    }

    @Override
    public byte[] getResultAttrsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new ResultAttrsPickler(filter));
    }

    @Override
    public byte[] getRunAttrsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new RunAttrsPickler(filter, RunAttrsPickler.FilterMode.FILTER_RUNATTRS));
    }

    @Override
    public byte[] getItervarsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new IterVarsPickler(filter, IterVarsPickler.FilterMode.FILTER_ITERVARS));
    }

    @Override
    public byte[] getScalarsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new ScalarResultsPickler(filterExpression, includeAttrs, includeRunattrs, includeItervars, mergeModuleAndName));
    }

    @Override
    public byte[] getVectorsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new VectorResultsPickler(filterExpression, includeAttrs, includeRunattrs, includeItervars, mergeModuleAndName));
    }

    @Override
    public byte[] getStatisticsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new StatisticsResultsPickler(filterExpression, includeAttrs, includeRunattrs, includeItervars, mergeModuleAndName));
    }

    @Override
    public byte[] getHistogramsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new HistogramResultsPickler(filterExpression, includeAttrs, includeRunattrs, includeItervars, mergeModuleAndName));
    }
}
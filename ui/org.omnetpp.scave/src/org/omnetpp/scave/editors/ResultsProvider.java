package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.List;

import org.omnetpp.scave.engine.InterruptedFlag;
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

public class ResultsProvider implements IScaveResultsPickleProvider {
    ResultFileManager rfm;
    InterruptedFlag interruptedFlag;

    public ResultsProvider(ResultFileManager rfm, InterruptedFlag interruptedFlag) {
        this.rfm = rfm;
        this.interruptedFlag = interruptedFlag;
    }

    @Override
    public byte[] getRunsPickle(String filterExpression, boolean includeRunattrs, boolean includeItervars) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new RunsPickler(filterExpression, includeRunattrs, includeItervars, interruptedFlag));
    }

    @Override
    public byte[] getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new CsvResultsPickler(filterExpression, rowTypes, omitUnusedColumns, simTimeStart, simTimeEnd, interruptedFlag));
    }

    @Override
    public byte[] getResultAttrsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new ResultAttrsPickler(filter, interruptedFlag));
    }

    @Override
    public byte[] getRunAttrsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new RunAttrsPickler(filter, RunAttrsPickler.FilterMode.FILTER_RUNATTRS, interruptedFlag));
    }

    @Override
    public byte[] getRunAttrsForItervarsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new RunAttrsPickler(filter, RunAttrsPickler.FilterMode.FILTER_ITERVARS, interruptedFlag));
    }

    @Override
    public byte[] getItervarsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new IterVarsPickler(filter, IterVarsPickler.FilterMode.FILTER_ITERVARS, interruptedFlag));
    }

    @Override
    public byte[] getScalarsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new ScalarResultsPickler(filterExpression, includeAttrs, includeRunattrs, includeItervars, mergeModuleAndName, interruptedFlag));
    }

    @Override
    public byte[] getVectorsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new VectorResultsPickler(filterExpression, includeAttrs, includeRunattrs, includeItervars, mergeModuleAndName, simTimeStart, simTimeEnd, interruptedFlag));
    }

    @Override
    public byte[] getStatisticsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new StatisticsResultsPickler(filterExpression, includeAttrs, includeRunattrs, includeItervars, mergeModuleAndName, interruptedFlag));
    }

    @Override
    public byte[] getHistogramsPickle(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new HistogramResultsPickler(filterExpression, includeAttrs, includeRunattrs, includeItervars, mergeModuleAndName, interruptedFlag));
    }
}
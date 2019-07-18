package org.omnetpp.scave.editors;

import java.io.IOException;
import java.util.List;

import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.pychart.IScaveResultsPickleProvider;
import org.omnetpp.scave.python.ConfigEntriesPickler;
import org.omnetpp.scave.python.CsvResultsPickler;
import org.omnetpp.scave.python.HistogramResultsPickler;
import org.omnetpp.scave.python.IterVarsPickler;
import org.omnetpp.scave.python.ParamAssignmentsPickler;
import org.omnetpp.scave.python.ParamValuesPickler;
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
    public byte[] getRunsPickle(String filterExpression) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new RunsPickler(filterExpression, interruptedFlag));
    }

    @Override
    public byte[] getResultsPickle(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new CsvResultsPickler(filterExpression, rowTypes, omitUnusedColumns, simTimeStart, simTimeEnd, interruptedFlag));
    }

    @Override
    public byte[] getRunAttrsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new RunAttrsPickler(filter, interruptedFlag));
    }

    @Override
    public byte[] getItervarsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new IterVarsPickler(filter, interruptedFlag));
    }

    @Override
    public byte[] getItervarsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new IterVarsPickler(runIDs, interruptedFlag));
    }

    @Override
    public byte[] getRunAttrsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new RunAttrsPickler(runIDs, interruptedFlag));
    }

    @Override
    public byte[] getParamAssignmentsForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new ParamAssignmentsPickler(runIDs, interruptedFlag));
    }

    @Override
    public byte[] getConfigEntriesForRunsPickle(List<String> runIDs) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new ConfigEntriesPickler(runIDs, interruptedFlag));
    }


    @Override
    public byte[] getScalarsPickle(String filterExpression, boolean includeAttrs, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new ScalarResultsPickler(filterExpression, includeAttrs, mergeModuleAndName, interruptedFlag));
    }

    @Override
    public byte[] getVectorsPickle(String filterExpression, boolean includeAttrs, boolean mergeModuleAndName, double simTimeStart, double simTimeEnd) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new VectorResultsPickler(filterExpression, includeAttrs, mergeModuleAndName, simTimeStart, simTimeEnd, interruptedFlag));
    }

    @Override
    public byte[] getStatisticsPickle(String filterExpression, boolean includeAttrs, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new StatisticsResultsPickler(filterExpression, includeAttrs, mergeModuleAndName, interruptedFlag));
    }

    @Override
    public byte[] getHistogramsPickle(String filterExpression, boolean includeAttrs, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new HistogramResultsPickler(filterExpression, includeAttrs, mergeModuleAndName, interruptedFlag));
    }

    @Override
    public byte[] getConfigEntriesPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new ConfigEntriesPickler(filter, interruptedFlag));
    }


    @Override
    public byte[] getParamAssignmentsPickle(String filter) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new ParamAssignmentsPickler(filter, interruptedFlag));
    }


    @Override
    public byte[] getParamValuesPickle(String filterExpression, boolean includeAttrs, boolean mergeModuleAndName) throws PickleException, IOException {
        return ResultPicklingUtils.pickleResultsUsing(rfm, new ParamValuesPickler(filterExpression, includeAttrs, mergeModuleAndName, interruptedFlag));
    }
}
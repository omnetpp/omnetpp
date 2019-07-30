package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.Histogram;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.Statistics;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class HistogramResultsPickler implements IObjectPickler {

    void pickleHistogramResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        HistogramResult result = resultManager.getHistogram(ID);
        Statistics stats = result.getStatistics();
        Histogram hist = result.getHistogram();
        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save(result.getModuleName());
            if (mergeModuleAndName)
                pickler.save(result.getModuleName() + "." + result.getName());
            else
                pickler.save(result.getName());

            pickler.save(stats.getCount());
            pickler.save(stats.getSumWeights());
            pickler.save(stats.getMean());
            pickler.save(stats.getStddev());
            pickler.save(stats.getMin());
            pickler.save(stats.getMax());

            pickler.save(hist.getUnderflows());
            pickler.save(hist.getOverflows());

            ResultPicklingUtils.pickleDoubleArray(hist.getBinEdges().toArray(), out);
            ResultPicklingUtils.pickleDoubleArray(hist.getBinValues().toArray(), out);
        }
        out.write(Opcodes.TUPLE);
    }

    void pickleScalarResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        ScalarResult result = resultManager.getScalar(ID);

        // runID, module, name, value
        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save(result.getModuleName());
            if (mergeModuleAndName)
                pickler.save(result.getModuleName() + "." + result.getName());
            else
                pickler.save(result.getName());
            pickler.save(result.getValue());
        }
        out.write(Opcodes.TUPLE);
    }

    String filterExpression;
    boolean includeAttrs;
    boolean includeRunattrs;
    boolean includeItervars;
    boolean mergeModuleAndName;
    InterruptedFlag interruptedFlag;

    public HistogramResultsPickler(String filterExpression, boolean includeAttrs, boolean includeRunattrs, boolean includeItervars, boolean mergeModuleAndName, InterruptedFlag interruptedFlag) {
        this.filterExpression = filterExpression;
        this.includeAttrs = includeAttrs;
        this.includeRunattrs = includeRunattrs;
        this.includeItervars = includeItervars;
        this.mergeModuleAndName = mergeModuleAndName;
        this.interruptedFlag = interruptedFlag;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        out.write(Opcodes.MARK);
        {
            out.write(Opcodes.MARK);
            if (filterExpression != null && !filterExpression.trim().isEmpty()) {
                IDList histograms = resultManager.getAllHistograms();
                histograms = resultManager.filterIDList(histograms, filterExpression, interruptedFlag);

                if (ResultPicklingUtils.debug)
                    Debug.println("pickling " + histograms.size() + " histograms");

                for (int i = 0; i < histograms.size(); ++i) {
                    pickleHistogramResult(resultManager, histograms.get(i), pickler, out);

                    if (i % 10 == 0 && interruptedFlag.getFlag())
                        throw new RuntimeException("Result pickling interrupted");
                }
            }
            out.write(Opcodes.LIST);

            if (includeAttrs) // TODO: add this type(...) to all column groups in all picklers
                new ResultAttrsPickler("type(histogram) AND (" + filterExpression + ")", interruptedFlag).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);

            if (includeRunattrs)
                new RunAttrsPickler(filterExpression, RunAttrsPickler.FilterMode.FILTER_RESULTS, interruptedFlag).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);

            if (includeItervars)
                new IterVarsPickler(filterExpression, IterVarsPickler.FilterMode.FILTER_RESULTS, interruptedFlag).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);
        }
        out.write(Opcodes.TUPLE);
    }
}

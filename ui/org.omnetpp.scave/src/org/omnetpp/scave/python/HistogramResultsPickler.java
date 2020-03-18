package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.Histogram;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
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

    String filterExpression;
    boolean includeAttrs;
    InterruptedFlag interruptedFlag;

    public HistogramResultsPickler(String filterExpression, boolean includeAttrs, InterruptedFlag interruptedFlag) {
        this.filterExpression = filterExpression;
        this.includeAttrs = includeAttrs;
        this.interruptedFlag = interruptedFlag;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        out.write(Opcodes.MARK);
        {
            IDList histograms = null;

            out.write(Opcodes.MARK);
            if (filterExpression != null && !filterExpression.trim().isEmpty()) {
                histograms = resultManager.getAllHistograms();
                histograms = resultManager.filterIDList(histograms, filterExpression, -1, interruptedFlag);

                if (ResultPicklingUtils.debug)
                    Debug.println("pickling " + histograms.size() + " histograms");

                for (int i = 0; i < histograms.size(); ++i) {
                    pickleHistogramResult(resultManager, histograms.get(i), pickler, out);

                    if (i % 10 == 0 && interruptedFlag.getFlag())
                        throw new RuntimeException("Result pickling interrupted");
                }
            }
            out.write(Opcodes.LIST);

            if (histograms != null && includeAttrs)
                new ResultAttrsPickler(histograms, interruptedFlag).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);
        }
        out.write(Opcodes.TUPLE);
    }
}

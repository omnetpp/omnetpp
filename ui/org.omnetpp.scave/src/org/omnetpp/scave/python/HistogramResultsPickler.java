package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.Histogram;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Statistics;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class HistogramResultsPickler implements IObjectPickler {

    static void pickleHistogramResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        HistogramResult result = resultManager.getHistogram(ID);
        Statistics stats = result.getStatistics();
        Histogram hist = result.getHistogram();
        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save("histogram");
            pickler.save(result.getModuleName());
            pickler.save(result.getName());
            pickler.save(null); // attrname
            pickler.save(null); // value

            pickler.save(stats.getCount());
            pickler.save(stats.getSumWeights());
            pickler.save(stats.getMean());
            pickler.save(stats.getStddev());
            pickler.save(stats.getMin());
            pickler.save(stats.getMax());

            ResultPicklingUtils.pickleDoubleArray(hist.getBinLowerBounds().toArray(), out);
            ResultPicklingUtils.pickleDoubleArray(hist.getBinValues().toArray(), out);
        }
        out.write(Opcodes.TUPLE);

        ResultPicklingUtils.pickleResultAttributes(result, pickler, out);
    }

    public static void pickleHistogramsFiltered(ResultFileManager resultManager, String filter, Pickler pickler,
            OutputStream out) throws IOException {
        out.write(Opcodes.MARK);
        {
            IDList histograms = resultManager.getAllHistograms();

            histograms = resultManager.filterIDList(histograms, filter);
            Debug.println("pickling " + histograms.size() + " histograms");

            ResultPicklingUtils.pickleRunsOfResults(resultManager, histograms, pickler, out);

            for (int i = 0; i < histograms.size(); ++i)
                pickleHistogramResult(resultManager, histograms.get(i), pickler, out);
        }
        out.write(Opcodes.LIST);
    }

    String filter;

    public HistogramResultsPickler(String filter) {
        this.filter = filter;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        pickleHistogramsFiltered((ResultFileManager) obj, filter, pickler, out);
    }
}

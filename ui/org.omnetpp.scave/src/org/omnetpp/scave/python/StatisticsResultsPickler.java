package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.engine.StatisticsResult;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class StatisticsResultsPickler implements IObjectPickler {

    static void pickleStatisticsResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        StatisticsResult result = resultManager.getStatistics(ID);
        Statistics stats = result.getStatistics();
        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save("statistic");
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
        }
        out.write(Opcodes.TUPLE);

        ResultPicklingUtils.pickleResultAttributes(result, pickler, out);
    }

    public static void pickleStatisticsFiltered(ResultFileManager resultManager, String filter, Pickler pickler,
            OutputStream out) throws IOException {
        out.write(Opcodes.MARK);
        {
            IDList statistics = resultManager.getAllStatistics();

            statistics = resultManager.filterIDList(statistics, filter);
            System.out.println("pickling " + statistics.size() + " statistics");

            ResultPicklingUtils.pickleRunsOfResults(resultManager, statistics, pickler, out);

            for (int i = 0; i < statistics.size(); ++i)
                pickleStatisticsResult(resultManager, statistics.get(i), pickler, out);
        }
        out.write(Opcodes.LIST);
    }

    String filter;

    public StatisticsResultsPickler(String filter) {
        this.filter = filter;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        pickleStatisticsFiltered((ResultFileManager) obj, filter, pickler, out);
    }
}

package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.engine.StatisticsResult;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class StatisticsResultsPickler implements IObjectPickler {

    void pickleStatisticsResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        StatisticsResult result = resultManager.getStatistics(ID);
        Statistics stats = result.getStatistics();
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
        }
        out.write(Opcodes.TUPLE);
    }

    String filterExpression;
    boolean includeAttrs;
    InterruptedFlag interruptedFlag;

    public StatisticsResultsPickler(String filterExpression, boolean includeAttrs, InterruptedFlag interruptedFlag) {
        this.filterExpression = filterExpression;
        this.includeAttrs = includeAttrs;
        this.interruptedFlag = interruptedFlag;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        out.write(Opcodes.MARK);
        {
            IDList statistics = null;

            out.write(Opcodes.MARK);
            if (filterExpression != null && !filterExpression.trim().isEmpty()) {
                statistics = resultManager.getAllStatistics();
                statistics = resultManager.filterIDList(statistics, filterExpression, -1, interruptedFlag);

                if (ResultPicklingUtils.debug)
                    Debug.println("pickling " + statistics.size() + " statistics");

                for (int i = 0; i < statistics.size(); ++i) {
                    pickleStatisticsResult(resultManager, statistics.get(i), pickler, out);

                    if (i % 10 == 0 && interruptedFlag.getFlag())
                        throw new RuntimeException("Result pickling interrupted");
                }
            }
            out.write(Opcodes.LIST);

            if (statistics != null && includeAttrs)
                new ResultAttrsPickler(statistics, interruptedFlag).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);
        }
        out.write(Opcodes.TUPLE);
    }
}

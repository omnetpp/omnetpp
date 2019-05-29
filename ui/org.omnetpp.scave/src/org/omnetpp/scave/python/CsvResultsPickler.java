package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;
import java.util.List;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.charting.dataset.VectorDataLoader;
import org.omnetpp.scave.charting.dataset.XYVector;
import org.omnetpp.scave.engine.Histogram;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.OrderedKeyValueList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.engine.StatisticsResult;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringPair;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.VectorResult;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class CsvResultsPickler implements IObjectPickler {

    protected String filterExpression;
    protected List<String> rowTypes;
    protected boolean omitUnusedColumns;
    protected double simTimeStart;
    protected double simTimeEnd;
    protected InterruptedFlag interruptedFlag;

    public CsvResultsPickler(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd, InterruptedFlag interruptedFlag) {
        this.filterExpression = filterExpression;
        this.rowTypes = rowTypes;
        this.omitUnusedColumns = omitUnusedColumns;
        this.simTimeStart = simTimeStart;
        this.simTimeEnd = simTimeEnd;
        this.interruptedFlag = interruptedFlag;
    }

    public void pickleResultAttributes(ResultItem result, Pickler pickler, OutputStream out) throws IOException {
        StringMap attrs = result.getAttributes();
        StringVector attrKeys = attrs.keys();
        for (int i = 0; i < attrKeys.size(); ++i) {
            String key = attrKeys.get(i);

            out.write(Opcodes.MARK);
            {
                pickler.save(result.getRun().getRunName());
                pickler.save("attr");
                pickler.save(result.getModuleName());
                pickler.save(result.getName());
                pickler.save(key);
                pickler.save(attrs.get(key));
            }
            out.write(Opcodes.TUPLE);
        }
    }

    public void pickleRunsOfResults(ResultFileManager resultManager, IDList results, Pickler pickler,
            OutputStream out) throws IOException {
        RunList runs = resultManager.getUniqueRuns(results);

        // runID, type, module, name, attrname, attrvalue, value, count, sumweights, mean, stddev, min, max, binedges, binvalues, vectime, vecvalue

        for (Run r : runs.toArray()) {

            // Run attributes
            StringMap attrs = r.getAttributes();
            StringVector attrKeys = attrs.keys();
            for (int i = 0; i < attrKeys.size(); ++i) {
                String key = attrKeys.get(i);

                out.write(Opcodes.MARK);
                {
                    pickler.save(r.getRunName());
                    pickler.save("runattr");
                    pickler.save(null); // module
                    pickler.save(null); // name
                    pickler.save(key);
                    pickler.save(attrs.get(key));

                    for (int j = 0; j < 11; ++j)
                        pickler.save(null);
                }
                out.write(Opcodes.TUPLE);
            }

            // Iteration variables
            StringMap itervars = r.getIterationVariables();
            StringVector itervarKeys = itervars.keys();
            for (int i = 0; i < itervarKeys.size(); ++i) {
                String key = itervarKeys.get(i);

                out.write(Opcodes.MARK);
                {
                    pickler.save(r.getRunName());
                    pickler.save("itervar");
                    pickler.save(null); // module
                    pickler.save(null); // name
                    pickler.save(key);
                    pickler.save(itervars.get(key));

                    for (int j = 0; j < 11; ++j)
                        pickler.save(null);
                }
                out.write(Opcodes.TUPLE);
            }

            // Parameter assignments
            OrderedKeyValueList params = r.getParamAssignments();
            for (int i = 0; i < params.size(); ++i) {
                StringPair pair = params.get(i);
                out.write(Opcodes.MARK);
                {
                    pickler.save(r.getRunName());
                    pickler.save("param");
                    pickler.save(null); // module
                    pickler.save(null); // name
                    pickler.save(pair.getFirst());
                    pickler.save(pair.getSecond());

                    for (int j = 0; j < 11; ++j)
                        pickler.save(null);
                }
                out.write(Opcodes.TUPLE);
            }

            if (interruptedFlag.getFlag())
                throw new RuntimeException("Result pickling interrupted");
        }
    }

    protected void pickleScalarResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        ScalarResult result = resultManager.getScalar(ID);

        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save("scalar");
            pickler.save(result.getModuleName());
            pickler.save(result.getName());
            pickler.save(null); // attrname
            pickler.save(null); // attrvalue
            pickler.save(result.getValue());

            for (int j = 0; j < 10; ++j)
                pickler.save(null);
        }
        out.write(Opcodes.TUPLE);

        pickleResultAttributes(result, pickler, out);
    }


    protected void pickleVectorResult(ResultFileManager resultManager, long ID, Pickler pickler,
            OutputStream out) throws PickleException, IOException {
        VectorResult result = resultManager.getVector(ID);
        IDList idAsList = new IDList();
        idAsList.add(ID);
        XYVector data = VectorDataLoader.getDataOfVectors(resultManager, idAsList, simTimeStart, simTimeEnd, interruptedFlag)[0];

        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save("vector");
            pickler.save(result.getModuleName());
            pickler.save(result.getName());

            for (int j = 0; j < 11; ++j)
                pickler.save(null);

            ResultPicklingUtils.pickleDoubleArray(data.x, out);
            ResultPicklingUtils.pickleDoubleArray(data.y, out);
        }
        out.write(Opcodes.TUPLE);

        data = null;
        System.gc();

        pickleResultAttributes(result, pickler, out);
    }

    protected void pickleStatisticsResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        StatisticsResult result = resultManager.getStatistics(ID);
        Statistics stats = result.getStatistics();
        // runID, type, module, name, attrname, attrvalue, value, count, sumweights, mean, stddev, min, max, binedges, binvalues, vectime, vecvalue
        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save("statistic");
            pickler.save(result.getModuleName());
            pickler.save(result.getName());
            pickler.save(null); // attrname
            pickler.save(null); // attrvalue
            pickler.save(null); // value

            pickler.save(stats.getCount());
            pickler.save(stats.getSumWeights());
            pickler.save(stats.getMean());
            pickler.save(stats.getStddev());
            pickler.save(stats.getMin());
            pickler.save(stats.getMax());

            for (int j = 0; j < 4; ++j)
                pickler.save(null);

        }
        out.write(Opcodes.TUPLE);

        pickleResultAttributes(result, pickler, out);
    }

    protected void pickleHistogramResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
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
            pickler.save(null); // attrvalue
            pickler.save(null); // value

            pickler.save(stats.getCount());
            pickler.save(stats.getSumWeights());
            pickler.save(stats.getMean());
            pickler.save(stats.getStddev());
            pickler.save(stats.getMin());
            pickler.save(stats.getMax());

            ResultPicklingUtils.pickleDoubleArray(hist.getBinLowerBounds().toArray(), out);
            ResultPicklingUtils.pickleDoubleArray(hist.getBinValues().toArray(), out);

            pickler.save(null); // vectime
            pickler.save(null); // vecvalue
        }
        out.write(Opcodes.TUPLE);

        pickleResultAttributes(result, pickler, out);
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        // TODO: rowTypes and omitUnusedColumns is currently ignored here, filtering is done in Python.
        out.write(Opcodes.MARK);
        if (filterExpression != null && !filterExpression.trim().isEmpty()) {
            IDList results = resultManager.getAllItems();
            results = resultManager.filterIDList(results, filterExpression, interruptedFlag);

            if (ResultPicklingUtils.debug)
                Debug.println("pickling " + results.size() + " items");

            pickleRunsOfResults(resultManager, results, pickler, out);

            for (int i = 0; i < results.size(); ++i) {
                if (ResultFileManager.getTypeOf(results.get(i)) == ResultFileManager.SCALAR)
                    pickleScalarResult(resultManager, results.get(i), pickler, out);
                if (i % 10 == 0 && interruptedFlag.getFlag())
                    throw new RuntimeException("Result pickling interrupted");
            }

            for (int i = 0; i < results.size(); ++i) {
                if (ResultFileManager.getTypeOf(results.get(i)) == ResultFileManager.VECTOR)
                    pickleVectorResult(resultManager, results.get(i), pickler, out);
                if (i % 10 == 0 && interruptedFlag.getFlag())
                    throw new RuntimeException("Result pickling interrupted");
            }

            for (int i = 0; i < results.size(); ++i) {
                if (ResultFileManager.getTypeOf(results.get(i)) == ResultFileManager.STATISTICS)
                    pickleStatisticsResult(resultManager, results.get(i), pickler, out);
                if (i % 10 == 0 && interruptedFlag.getFlag())
                    throw new RuntimeException("Result pickling interrupted");
            }

            for (int i = 0; i < results.size(); ++i) {
                if (ResultFileManager.getTypeOf(results.get(i)) == ResultFileManager.HISTOGRAM)
                    pickleHistogramResult(resultManager, results.get(i), pickler, out);
                if (i % 10 == 0 && interruptedFlag.getFlag())
                    throw new RuntimeException("Result pickling interrupted");
            }
        }
        out.write(Opcodes.LIST);
    }
}

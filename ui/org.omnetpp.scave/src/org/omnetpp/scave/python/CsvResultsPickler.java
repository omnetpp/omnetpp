package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;
import java.util.List;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.charting.dataset.VectorDataLoader;
import org.omnetpp.scave.engine.Histogram;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.OrderedKeyValueList;
import org.omnetpp.scave.engine.ParameterResult;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.ScaveEngine;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.engine.StatisticsResult;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringPair;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class CsvResultsPickler implements IObjectPickler {

    protected String filterExpression;

    boolean addRunAttrs, addIterVars, addConfigEntries, addScalars, addVectors, addStatistics, addHistograms, addParams, addAttrs;

    protected boolean omitUnusedColumns; // done in python at the moment
    protected double simTimeStart;
    protected double simTimeEnd;
    protected InterruptedFlag interruptedFlag;

    public CsvResultsPickler(String filterExpression, List<String> rowTypes, boolean omitUnusedColumns, double simTimeStart, double simTimeEnd, InterruptedFlag interruptedFlag) {
        this.filterExpression = filterExpression;

        this.omitUnusedColumns = omitUnusedColumns;
        this.simTimeStart = simTimeStart;
        this.simTimeEnd = simTimeEnd;
        this.interruptedFlag = interruptedFlag;

        addRunAttrs = rowTypes.contains("runattr");
        addIterVars = rowTypes.contains("itervar");
        addConfigEntries = rowTypes.contains("config");
        addScalars = rowTypes.contains("scalar");
        addVectors = rowTypes.contains("vector");
        addStatistics = rowTypes.contains("statistic");
        addHistograms = rowTypes.contains("histogram");
        addParams = rowTypes.contains("param");
        addAttrs = rowTypes.contains("attr");
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

                for (int j = 0; j < 13; ++j)
                    pickler.save(null);
            }
            out.write(Opcodes.TUPLE);
        }
    }

    public void pickleRunsOfResults(ResultFileManager resultManager, IDList results, Pickler pickler,
            OutputStream out) throws IOException {
        RunList runs = resultManager.getUniqueRuns(results);

        for (Run r : runs.toArray()) {

            if (addRunAttrs) {
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

                        for (int j = 0; j < 13; ++j)
                            pickler.save(null);
                    }
                    out.write(Opcodes.TUPLE);
                }
            }

            if (addIterVars) {
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

                        for (int j = 0; j < 13; ++j)
                            pickler.save(null);
                    }
                    out.write(Opcodes.TUPLE);
                }
            }

            if (addConfigEntries) {
                // Config entries
                OrderedKeyValueList entries = r.getConfigEntries();
                for (int i = 0; i < entries.size(); ++i) {
                    StringPair pair = entries.get(i);
                    out.write(Opcodes.MARK);
                    {
                        pickler.save(r.getRunName());
                        pickler.save("config");
                        pickler.save(null); // module
                        pickler.save(null); // name
                        pickler.save(pair.getFirst());
                        pickler.save(pair.getSecond());

                        for (int j = 0; j < 13; ++j)
                            pickler.save(null);
                    }
                    out.write(Opcodes.TUPLE);
                }
            }

            if (interruptedFlag.getFlag())
                throw new RuntimeException("Result pickling interrupted");
        }
    }

    protected void pickleScalarResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        ScalarResult result = resultManager.getScalar(ID);

        if (addScalars) {
            out.write(Opcodes.MARK);
            {
                pickler.save(result.getRun().getRunName());
                pickler.save("scalar");
                pickler.save(result.getModuleName());
                pickler.save(result.getName());
                pickler.save(null); // attrname
                pickler.save(null); // attrvalue
                pickler.save(result.getValue());

                for (int j = 0; j < 12; ++j)
                    pickler.save(null);
            }
            out.write(Opcodes.TUPLE);
        }

        if (addAttrs)
            pickleResultAttributes(result, pickler, out);
    }

    protected void pickleParameterResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        ParameterResult result = resultManager.getParameter(ID);

        if (addParams) {
            out.write(Opcodes.MARK);
            {
                pickler.save(result.getRun().getRunName());
                pickler.save("param");
                pickler.save(result.getModuleName());
                pickler.save(result.getName());
                pickler.save(null); // attrname
                pickler.save(null); // attrvalue
                pickler.save(result.getValue());

                for (int j = 0; j < 12; ++j)
                    pickler.save(null);
            }
            out.write(Opcodes.TUPLE);
        }

        if (addAttrs)
            pickleResultAttributes(result, pickler, out);
    }

    protected void pickleVectorResult(ResultFileManager resultManager, long ID, Pickler pickler,
            OutputStream out) throws PickleException, IOException {
        VectorResult result = resultManager.getVector(ID);

        if (addVectors) {
            IDList idAsList = new IDList();
            idAsList.add(ID);
            // TODO optimize: load all the vectors at once
            XYArray data = VectorDataLoader.getDataOfVectors(resultManager, idAsList, simTimeStart, simTimeEnd, interruptedFlag).get(0);

            out.write(Opcodes.MARK);
            {
                pickler.save(result.getRun().getRunName());
                pickler.save("vector");
                pickler.save(result.getModuleName());
                pickler.save(result.getName());

                for (int j = 0; j < 13; ++j)
                    pickler.save(null);

                ResultPicklingUtils.pickleXYArray(data, out);
            }
            out.write(Opcodes.TUPLE);

            data.delete();
            data = null;
            //System.gc(); // NOT NEEDED, SLOW, and actually BREAKS some internal parts of Py4J...
            ScaveEngine.malloc_trim(); // so the std::vector buffers (in data) are released to the operating system
        }

        if (addAttrs)
            pickleResultAttributes(result, pickler, out);
    }

    protected void pickleStatisticsResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        StatisticsResult result = resultManager.getStatistics(ID);

        if (addStatistics) {
            Statistics stats = result.getStatistics();
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

                for (int j = 0; j < 6; ++j)
                    pickler.save(null);
            }
            out.write(Opcodes.TUPLE);
        }

        if (addAttrs)
            pickleResultAttributes(result, pickler, out);
    }

    protected void pickleHistogramResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {

        HistogramResult result = resultManager.getHistogram(ID);

        if (addHistograms) {
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

                pickler.save(hist.getUnderflows());
                pickler.save(hist.getOverflows());

                ResultPicklingUtils.pickleDoubleArray(hist.getBinEdges().toArray(), out);
                ResultPicklingUtils.pickleDoubleArray(hist.getBinValues().toArray(), out);

                pickler.save(null); // vectime
                pickler.save(null); // vecvalue
            }
            out.write(Opcodes.TUPLE);
        }

        if (addAttrs)
            pickleResultAttributes(result, pickler, out);
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        // the columns are always:
        // runID, type, module, name, attrname, attrvalue, value, count, sumweights, mean, stddev, min, max, underflows, overflows, binedges, binvalues, vectime, vecvalue

        // TODO: omitUnusedColumns is currently ignored here, dropping them is done in Python.
        out.write(Opcodes.MARK);
        if (filterExpression != null && !filterExpression.trim().isEmpty()) {
            IDList results = resultManager.getAllItems(false);
            results = resultManager.filterIDList(results, filterExpression, -1, interruptedFlag);

            if (ResultPicklingUtils.debug)
                Debug.println("pickling " + results.size() + " items");

            pickleRunsOfResults(resultManager, results, pickler, out);

            if (addScalars || addAttrs)
                for (int i = 0; i < results.size(); ++i) {
                    if (ResultFileManager.getTypeOf(results.get(i)) == ResultFileManager.SCALAR)
                        pickleScalarResult(resultManager, results.get(i), pickler, out);
                    if (i % 10 == 0 && interruptedFlag.getFlag())
                        throw new RuntimeException("Result pickling interrupted");
                }

            if (addVectors || addAttrs)
                for (int i = 0; i < results.size(); ++i) {
                    if (ResultFileManager.getTypeOf(results.get(i)) == ResultFileManager.VECTOR)
                        pickleVectorResult(resultManager, results.get(i), pickler, out);
                    if (i % 10 == 0 && interruptedFlag.getFlag())
                        throw new RuntimeException("Result pickling interrupted");
                }

            if (addStatistics || addAttrs)
                for (int i = 0; i < results.size(); ++i) {
                    if (ResultFileManager.getTypeOf(results.get(i)) == ResultFileManager.STATISTICS)
                        pickleStatisticsResult(resultManager, results.get(i), pickler, out);
                    if (i % 10 == 0 && interruptedFlag.getFlag())
                        throw new RuntimeException("Result pickling interrupted");
                }

            if (addHistograms || addAttrs)
                for (int i = 0; i < results.size(); ++i) {
                    if (ResultFileManager.getTypeOf(results.get(i)) == ResultFileManager.HISTOGRAM)
                        pickleHistogramResult(resultManager, results.get(i), pickler, out);
                    if (i % 10 == 0 && interruptedFlag.getFlag())
                        throw new RuntimeException("Result pickling interrupted");
                }

            if (addParams || addAttrs)
                for (int i = 0; i < results.size(); ++i) {
                    if (ResultFileManager.getTypeOf(results.get(i)) == ResultFileManager.PARAMETER)
                        pickleParameterResult(resultManager, results.get(i), pickler, out);
                    if (i % 10 == 0 && interruptedFlag.getFlag())
                        throw new RuntimeException("Result pickling interrupted");
                }
        }
        out.write(Opcodes.LIST);
    }
}

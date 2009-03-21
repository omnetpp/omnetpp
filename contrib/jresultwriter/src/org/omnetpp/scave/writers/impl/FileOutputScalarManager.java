package org.omnetpp.scave.writers.impl;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Map;

import org.omnetpp.scave.writers.IHistogramSummary;
import org.omnetpp.scave.writers.IOutputScalarManager;
import org.omnetpp.scave.writers.IStatisticalSummary;
import org.omnetpp.scave.writers.IStatisticalSummary2;

/**
 * An output scalar manager that writes OMNeT++ scalar (".sca") files.
 *  
 * @author Andras
 */
public class FileOutputScalarManager extends OutputFileManager implements IOutputScalarManager {
    public static final int FILE_VERSION = 2;
    
    protected String runID;
    protected File file;
    protected PrintStream out;
    
    public FileOutputScalarManager(String fileName) {
        file = new File(fileName);
    }

    public void open(String runID, Map<String, String> runAttributes) throws IOException {
        this.runID = runID;
        out = new PrintStream(file);

        out.println("version " + FILE_VERSION);
        out.println();
        writeRunHeader(out, runID, runAttributes);

        flushAndCheck();
    }

    public void close() throws IOException {
        if (out != null) {
            flushAndCheck();
            out.close();
        }
    }

    public void flush() throws IOException {
        if (out != null)
            flushAndCheck();
    }

    protected void flushAndCheck() throws IOException {
        if (out.checkError()) // implies flush()
            throw new IOException("Cannot write output scalar file " + file.getPath());
    }

    public String getFileName() {
        return file.getPath();
    }

    public void recordScalar(String componentPath, String name, double value, Map<String, String> attributes) {
        out.println("scalar " + q(componentPath) + " " + q(name) + " " + value);
        writeAttributes(out, attributes);
    }

    public void recordScalar(String componentPath, String name, Number value, Map<String, String> attributes) {
        out.println("scalar " + q(componentPath) + " " + q(name) + " " + value.toString());
        writeAttributes(out, attributes);
    }

    public void recordStatistic(String componentPath, String name, IStatisticalSummary statistic, Map<String, String> attributes) {
        out.println("statistic " + q(componentPath) + " " + q(name));
  
        writeField("count", statistic.getN());
        writeField("mean", statistic.getMean());
        writeField("stddev", statistic.getStandardDeviation());
        writeField("sum", statistic.getSum());
        writeField("sqrsum", statistic.getSqrSum());
        writeField("min", statistic.getMin());
        writeField("max", statistic.getMax());
        
        if (statistic instanceof IStatisticalSummary2) {
            IStatisticalSummary2 statistic2 = (IStatisticalSummary2)statistic;
            if (statistic2.isWeighted())
            {
                writeField("weights", statistic2.getWeights());
                writeField("weightedSum", statistic2.getWeightedSum());
                writeField("sqrSumWeights", statistic2.getSqrSumWeights());
                writeField("weightedSqrSum", statistic2.getWeightedSqrSum());
            }
        }

        writeAttributes(out, attributes);

        if (statistic instanceof IHistogramSummary) {
            IHistogramSummary histogram = (IHistogramSummary)statistic;
            int n = histogram.getNumCells();
            if (n > 0) {
                out.println("bin -INF " + histogram.getUnderflowCell());
                for (int i=0; i<n; i++)
                    out.print("bin " + histogram.getBasepoint(i) + " " + histogram.getCellValue(i));
                out.println("bin " + histogram.getBasepoint(n) + histogram.getOverflowCell());
            }
        }
    }

    protected void writeField(String name, double value) {
        if (!Double.isNaN(value))
            out.println("field " + q(name) + " " + value);
    }
}

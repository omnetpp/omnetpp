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
public class FileOutputScalarManager implements IOutputScalarManager {
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
        out.println("run " + q(runID));
        writeAttributes(runAttributes);
        out.println();

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
        writeAttributes(attributes);
    }

    public void recordScalar(String componentPath, String name, Number value, Map<String, String> attributes) {
        out.println("scalar " + q(componentPath) + " " + q(name) + " " + value.toString());
        writeAttributes(attributes);
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

        writeAttributes(attributes);

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

    protected void writeAttributes(Map<String, String> attributes) {
        if (attributes != null)
            for (String attr : attributes.keySet())
                out.println("attr " + q(attr) + " " + q(attributes.get(attr)));
    }
    
    /**
     * Quotes the given string if needed.
     */
    protected static String q(String s) {
        if (s == null || s.length() == 0)
            return "\"\"";

        boolean needsQuotes = false;
        for (int i=0; i<s.length(); i++) {
            char ch = s.charAt(i);
            if (ch == '\\' || ch == '"' || Character.isWhitespace(ch) || Character.isISOControl(ch)) {
                needsQuotes = true;
                break;
            }
        }
        
        if (needsQuotes) {
            StringBuilder buf = new StringBuilder();
            buf.append('"');
            for (int i=0; i<s.length(); i++) {
                char ch = s.charAt(i);
                switch (ch)
                {
                    case '\b': buf.append("\\b"); break;
                    case '\f': buf.append("\\f"); break;
                    case '\n': buf.append("\\n"); break;
                    case '\r': buf.append("\\r"); break;
                    case '\t': buf.append("\\t"); break;
                    case '"':  buf.append("\\\""); break;
                    case '\\': buf.append("\\\\"); break;
                    default: 
                        if (!Character.isISOControl(ch))
                            buf.append(ch);
                        else
                            buf.append(String.format("\\x%02.2x", ch));
                }
            }
            buf.append('"');
            s = buf.toString();
        }
        return s;
    }
}

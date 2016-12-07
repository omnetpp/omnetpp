package org.omnetpp.scave.writers.impl;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.omnetpp.scave.writers.IOutputVector;
import org.omnetpp.scave.writers.IOutputVectorManager;
import org.omnetpp.scave.writers.ISimulationTimeProvider;
import org.omnetpp.scave.writers.ResultRecordingException;

/**
 * An output vector manager that writes OMNeT++ vector (".vec") files.
 * Recording event numbers ("ETV" vectors) is not supported, because it is
 * practically only useful for sequence charts.
 *
 * This class does not support filtering (of vectors or recorded data)
 * at all -- this functionality may be added via subclasses.
 *
 * @author Andras
 */
public class FileOutputVectorManager extends OutputFileManager implements IOutputVectorManager {
    public static final int FILE_VERSION = 2;

    protected String runID;
    protected Map<String, String> runAttributes;
    protected File file;
    protected FileOutputStream stream;
    protected PrintStream out;
    protected File indexFile;
    protected FileOutputStream indexStream;
    protected PrintStream indexOut;

    protected ISimulationTimeProvider simtimeProvider;

    protected int perVectorLimit = 1000;
    protected int totalLimit = 1000000;

    protected int lastId = 0;
    protected int nbuffered = 0;

    protected Set<OutputVector> vectors = new HashSet<OutputVector>();

    class OutputVector implements IOutputVector {
        int id;
        byte[] header;

        int n = 0;
        Number[] times = new Number[10];
        double[] values = new double[10];

        Number blockStartTime = 0;
        Number blockEndTime = 0;
        double min = Double.NaN;
        double max = Double.NaN;
        double sum = 0;
        double sqrSum = 0;

        public OutputVector(int id, String componentPath, String vectorName, Map<String, String> attributes) {
            this.id = id;

            // postpone writing out vector declaration until there's actually something to record
            ByteArrayOutputStream bos = new ByteArrayOutputStream();
            PrintStream tmp = new PrintStream(bos);
            tmp.println("vector " + id + " " + q(componentPath) + " " + q(vectorName) + " TV");
            writeAttributes(tmp, attributes);
            tmp.close();
            header = bos.toByteArray();
        }

        protected void writeVectorHeader() throws IOException {
            if (out == null)
                open();
            out.write(header);
            indexOut.write(header);
            header = null;
        }

        public void close() {
            flush();
            vectors.remove(this);
            id = -1; // i.e. dead object
        }

        public void flush() {
            if (id == -1)
                throw new IllegalStateException("Output vector already closed");

            writeBlock();  // implies file flushing as well
        }

        public boolean record(double value) {
            if (simtimeProvider == null)
                throw new IllegalStateException("Simtime provider not yet specified");

            return record(simtimeProvider.getSimulationTime(), value);
        }

        public boolean record(Number time, double value) {
            if (id == -1)
                throw new IllegalStateException("Attempt to write to an output vector that's already closed");

            if (time.doubleValue() < blockEndTime.doubleValue())
                throw new IllegalStateException("Vector data must be recorded in increasing timestamp order (t="+time+ "tprev="+blockEndTime+")");

            if (n == times.length) {
                int newSize = (n * 3) / 2;
                Number[] newTimes = new Number[newSize];
                double[] newValues = new double[newSize];
                System.arraycopy(times, 0, newTimes, 0, n);
                System.arraycopy(values, 0, newValues, 0, n);
                times = newTimes;
                values = newValues;
            }

            // store
            times[n] = time;
            values[n] = value;
            if (n == 0)
                blockStartTime = time;
            blockEndTime = time;
            n++;

            // update statistics
            if (min > value || Double.isNaN(min))
                min = value;
            if (max < value || Double.isNaN(max))
                max = value;
            sum += value;
            sqrSum += value*value;

            ++nbuffered;

            // flush if needed
            changed(this);

            return false;
        }

        protected void writeBlock() {
            try {
                // write out vector declaration if not yet done
                if (header != null)
                    writeVectorHeader();

                // write data
                long blockOffset = stream.getChannel().position();
                for (int i=0; i<n; i++)
                    out.println(id + " " + times[i] + " " + values[i]);
                long blockSize = stream.getChannel().position() - blockOffset;

                // make sure that the offsets referred to by the index file are exists in the vector file
                // so the index can be used to access the vector file while it is being written
                out.flush();
                if (out.checkError())
                    throw new ResultRecordingException("Cannot write output vector file " + file.getPath());

                // write index
                indexOut.println(id + " " + blockOffset + " " + blockSize + " " +
                        blockStartTime + " " + blockEndTime + " " +
                        n + " " + min + " " + max + " " + sum + " " + sqrSum);


                // reset block
                nbuffered -= n;
                n = 0;
                min = Double.NaN;
                max = Double.NaN;
                sum = 0;
                sqrSum = 0;
                times = new Number[10];
                values = new double[10];
            }
            catch (IOException e) {
                throw new ResultRecordingException("Error recording vector results:" + e.getMessage(), e);
            }
        }
    }

    public FileOutputVectorManager(String fileName) {
        file = new File(fileName);
        if (file.exists() && !file.delete())
            throw new ResultRecordingException("Cannot delete old output vector file " + file.getPath());

        String indexFileName = fileName.replaceFirst("\\.[^./\\:]*$", "") + ".vci";
        indexFile = new File(indexFileName);
        if (indexFile.exists() && !indexFile.delete())
            throw new ResultRecordingException("Cannot delete old output vector index file " + indexFile.getPath());
    }

    public ISimulationTimeProvider getSimtimeProvider() {
        return simtimeProvider;
    }

    public void setSimtimeProvider(ISimulationTimeProvider simtimeProvider) {
        this.simtimeProvider = simtimeProvider;
    }

    public int getPerVectorBufferLimit() {
        return perVectorLimit;
    }

    public void setPerVectorBufferLimit(int count) {
        this.perVectorLimit = count;
    }

    public int getTotalBufferLimit() {
        return totalLimit;
    }

    public void setTotalBufferLimit(int count) {
        this.totalLimit = count;
    }

    public void open(String runID, Map<String, String> runAttributes) {
        this.runID = runID;
        this.runAttributes = runAttributes;
    }

    protected void open() {
        try {
            stream = new FileOutputStream(file);
        }
        catch (FileNotFoundException e) {
            throw new ResultRecordingException("Cannot open output vector file " + file.getPath(), e);
        }
        try {
            indexStream = new FileOutputStream(indexFile);
        }
        catch (FileNotFoundException e) {
            throw new ResultRecordingException("Cannot open output vector index file " + indexFile.getPath(), e);
        }

        out = new PrintStream(stream);
        out.println("version " + FILE_VERSION);
        out.println();
        writeRunHeader(out, runID, runAttributes);

        indexOut = new PrintStream(indexStream);
        indexOut.format("%64s\n", " "); // room for "file ...." line
        indexOut.println("version " + FILE_VERSION);
        indexOut.println();
        writeRunHeader(indexOut, runID, runAttributes);

        flushAndCheck();
    }

    public void close() {
        if (out != null) {
            flush();
            out.close();

            // record size and timestamp of the vector file, for up-to-date checks
            try {
                indexStream.getChannel().position(0);
            }
            catch (IOException e) {
                throw new ResultRecordingException("Cannot rewind output vector index file " + indexFile.getPath(), e);
            }
            indexOut.print("file " + file.length() + " " + file.lastModified()/1000);
            if (indexOut.checkError())
                throw new ResultRecordingException("Cannot write output vector index file " + indexFile.getPath());
            indexOut.close();
        }
        vectors.clear();
    }

    public void flush() {
        for (OutputVector v : vectors)
            v.writeBlock();

        if (out != null)
            flushAndCheck();
    }

    protected void flushAndCheck() {
        if (out.checkError()) // implies flush()
            throw new ResultRecordingException("Cannot write output vector file " + file.getPath());
        if (indexOut.checkError())
            throw new ResultRecordingException("Cannot write output vector index file " + indexFile.getPath());
    }

    public String getFileName() {
        return file.getPath();
    }

    public IOutputVector createVector(String componentPath, String vectorName, Map<String, String> attributes) {
        int id = ++lastId;
        OutputVector vector = new OutputVector(id, componentPath, vectorName, attributes);
        vectors.add(vector);
        return vector;
    }

    protected void changed(OutputVector vector) {
        if (vector.n > perVectorLimit)
            vector.writeBlock();
        else if (nbuffered > totalLimit) {
            for (OutputVector v : vectors)
                v.writeBlock();
        }
    }
}

package org.omnetpp.scave.writers.impl;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.omnetpp.scave.writers.IOutputVector;
import org.omnetpp.scave.writers.IOutputVectorManager;
import org.omnetpp.scave.writers.ISimulationTimeProvider;

/**
 * An output vector manager that writes OMNeT++ vector (".vec") files.
 *  
 * @author Andras
 */
//XXX eletciklust tisztazni
//XXX ETV-t is tamogatni
//XXX too many "throws IOException"? wrap!!!
//XXX csak azokat a vektorokat irja ki, amibe irtak is!
//XXX elejen torolje a fajlt, es csak akkor krealja ha irtak bele
public class FileOutputVectorManager extends OutputFileManager implements IOutputVectorManager {
    public static final int FILE_VERSION = 2;
    
    protected String runID;
    protected File file;
    protected FileOutputStream stream;
    protected PrintStream out;
    protected File indexFile;
    protected FileOutputStream indexStream;
    protected PrintStream indexOut;
    
    protected ISimulationTimeProvider simtimeProvider;
    protected boolean recordEventNumbers = true;
    
    protected int perVectorLimit = 1000;
    protected int totalLimit = 1000000;
    
    protected int lastId = 0;
    protected int nbuffered = 0;

    protected Set<OutputVector> vectors = new HashSet<OutputVector>();
    
    class OutputVector implements IOutputVector {
        int id;
        int n = 0;
        Number[] times = new Number[10];
        double[] values = new double[10];
        
        Number blockStartTime = 0;
        Number blockEndTime = 0;
        double min = Double.NaN;
        double max = Double.NaN;
        double sum = 0;
        double sqrSum = 0;
        
        public OutputVector(int id) {
            this.id = id;
        }

        public void close() throws IOException {
            flush();
            vectors.remove(this);
            id = -1; // i.e. dead object
        }

        public void flush() throws IOException {
            if (id == -1)
                throw new IllegalStateException("Output vector already closed");

            writeBlock();  // implies file flushing as well
        }

        public boolean record(double value) throws IOException {
            return record(simtimeProvider.getSimulationTime(), value);
        }

        public boolean record(Number time, double value) throws IOException {
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

        protected void writeBlock() throws IOException {
            // write data
            long blockOffset = stream.getChannel().position();
            for (int i=0; i<n; i++)
                out.println(id + " " + times[i] + " " + values[i]);
            long blockSize = stream.getChannel().position() - blockOffset;

            // make sure that the offsets referred to by the index file are exists in the vector file
            // so the index can be used to access the vector file while it is being written
            out.flush();  //TODO checkError

            // write index
            indexOut.println(id + " " + blockOffset + " " + blockSize + " " + 
                    blockStartTime + " " + blockEndTime + " " + 
                    n + " " + min + " " + max + " " + sum + " " + sqrSum);
            
            // reset block
            nbuffered -= n;
            n = 0;
            times = new Number[10];
            values = new double[10];
            
        }
    }

    public FileOutputVectorManager(String fileName) {
        file = new File(fileName);
        
        String indexFileName = fileName.replaceFirst("\\.[^./\\:]*$", "") + ".vci";
        indexFile = new File(indexFileName);
    }

    public ISimulationTimeProvider getSimtimeProvider() {
        return simtimeProvider;
    }

    public void setSimtimeProvider(ISimulationTimeProvider simtimeProvider) {
        this.simtimeProvider = simtimeProvider;
    }

    public boolean getRecordEventNumbers() {
        return recordEventNumbers;
    }

    public void setRecordEventNumbers(boolean recordEventNumbers) {
        this.recordEventNumbers = recordEventNumbers;
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

    public void open(String runID, Map<String, String> runAttributes) throws IOException {
        this.runID = runID;
        stream = new FileOutputStream(file);
        out = new PrintStream(stream);
        
        out.println("version " + FILE_VERSION);
        out.println();
        writeRunHeader(out, runID, runAttributes);

        indexStream = new FileOutputStream(indexFile);
        indexOut = new PrintStream(indexStream);
        indexOut.format("%64s\n", " "); // room for "file ...." line
        indexOut.println("version " + FILE_VERSION);
        indexOut.println();
        writeRunHeader(indexOut, runID, runAttributes);
        
        flushAndCheck();
    }

    public void close() throws IOException {
        if (out != null) {
            flush();
            out.close();
           
            // record size and timestamp of the vector file, for up-to-date checks 
            indexStream.getChannel().position(0);
            indexOut.print("file " + file.length() + " " + file.lastModified()/1000);
            indexOut.close();
        }
        vectors.clear();
    }

    public void flush() throws IOException {
        for (OutputVector v : vectors)
            v.writeBlock();

        if (out != null)
            flushAndCheck();
    }

    protected void flushAndCheck() throws IOException {
        if (out.checkError()) // implies flush()
            throw new IOException("Cannot write output vector file " + file.getPath());
        if (indexOut.checkError())
            throw new IOException("Cannot write output vector index file " + indexFile.getPath());
    }

    public String getFileName() {
        return file.getPath();
    }

    public IOutputVector createVector(String componentPath, String vectorName, Map<String, String> attributes) {
        int id = ++lastId;

        String vectorDecl = "vector " + id + " " + q(componentPath) + " " + q(vectorName) + " TV"; //XXX recordEventNumber!
        out.println(vectorDecl);  
        writeAttributes(out, attributes);
        indexOut.println(vectorDecl);
        writeAttributes(indexOut, attributes);

        OutputVector vector = new OutputVector(id);
        vectors.add(vector);
        return vector;
    }

    protected void changed(OutputVector vector) throws IOException {
        if (vector.n > perVectorLimit)
            vector.writeBlock();
        else if (nbuffered > totalLimit) {
            for (OutputVector v : vectors)
                v.writeBlock();
        }
    }
}

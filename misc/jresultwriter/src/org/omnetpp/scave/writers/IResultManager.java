package org.omnetpp.scave.writers;

import java.util.Map;

/**
 * Contains common methods for IOutputScalarManager and IOutputVectorManager.
 * Methods throw ResultRecordingException on errors.
 *
 * @author Andras
 */
public interface IResultManager extends IResultAttributes {
    /**
     * Opens collecting. This should be called at the beginning of a simulation run.
     *
     * The runID will be used to identify the run with during result analysis,
     * so, while making sense to the user, it should also be globally unique with
     * a reasonable probability. A recommended runID format is the following:
     * <i>&lt;networkname&gt;-&lt;runnumber&gt;-&lt;datetime&gt;-&lt;hostname&gt;</i>.
     *
     * Attributes contain metadata about the simulation run; see IResultAttributes
     * for attribute names. Many of run attributes are useful for the result
     * analysis process.
     *
     * @param runID  reasonably globally unique string to identify the run.
     * @param attributes  run attributes; may be null. See IResultAttributes
     *      for potential attribute names.
     */
    void open(String runID, Map<String,String> runAttributes);

    /**
     * Close collecting. This should be called at the end of a simulation run.
     */
    void close();

    /**
     * Force writing out all buffered output.
     */
    void flush();

    /**
     * Returns the name of the output scalar/vector file. Returns null if this
     * object is not producing file output.
     */
    String getFileName();
}

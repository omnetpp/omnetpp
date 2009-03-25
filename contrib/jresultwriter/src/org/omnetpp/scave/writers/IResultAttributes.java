package org.omnetpp.scave.writers;


/**
 * Provides constants for IOutputScalarManager and IOutputVectorManager.
 * 
 * @author Andras
 */
public interface IResultAttributes {

    // --- run attributes ---

    /**
     * Run attribute: The sequence number of the simulation run among the total
     * number of runs in a parameter study.
     */
    static final String ATTR_RUNNUMBER   = "runnumber";

    /**
     * Run attribute: The sequence number of the simulation run among the total
     * number of runs in a parameter study.
     */
    static final String ATTR_NETWORK     = "network";

    /**
     * Run attribute: name of the experiment (=parameter study).
     */
    static final String ATTR_EXPERIMENT  = "experiment";

    /**
     * Run attribute: identifies the measurement within the experiment (=parameter study).
     * A recommended measurement label is the concatenation of the inputs, e.g.
     * <code>"nhosts=10,mean=0.2s"</code>. That is, the value of ATTR_ITERATIONVARS
     * may be used as measurement label, if present.
     */
    static final String ATTR_MEASUREMENT = "measurement";

    /**
     * Run attribute: label of the given trial within the measurement. The recommended
     * value is "#" plus the value of ATTR_REPETITION, if present.
     */
    static final String ATTR_REPLICATION = "replication";
    static final String ATTR_ITERATIONVARS = "iterationvars";
    static final String ATTR_ITERATIONVARS2 = "iterationvars2";
    static final String ATTR_DATETIME    = "datetime";
    static final String ATTR_PROCESSID   = "processid";
    static final String ATTR_RESULTDIR   = "resultdir";

    /**
     * Run attribute: serial number of the given trial within the measurement.
     */
    static final String ATTR_REPETITION  = "repetition";
    
    static final String ATTR_SEEDSET     = "seedset";

    /**
     * Run attribute; name of the (main) configuration file of the simulation,
     * if such thing exists.
     */
    static final String ATTR_INIFILE     = "inifile";

    /**
     * Name of the active configuration in the config file of the simulation,
     * if such thing exists.
     */
    static final String ATTR_CONFIGNAME  = "configname";
    
    // --- vector/scalar attributes ---
    
    /**
     * Vector/scalar attribute: unit of measurement in which the numeric value(s)
     * of the scalar or vector are understood.
     */
    static final String ATTR_UNIT = "unit";
    
    /**
     * Vector/scalar attribute: data type of the scalar or vector. Potential values
     * are provided as TYPE_xxx constants.
     */
    static final String ATTR_TYPE = "type";
    
    /**
     * Vector/scalar attribute: if the data type of the scalar or vector is enum,
     * what is the name of the enum.
     */
    static final String ATTR_ENUMNAME = "enumname";

    /**
     * Vector/scalar attribute: defines symbolic names for numeric values.
     * Syntax for the attribute value: <code>"NAME1=value1,NAME2=value2,NAME3=value3"</code>
     */
    static final String ATTR_ENUM = "enum";  
    
    /**
     * Vector attribute: defines how values in the vector should be plotted:
     * with linear interpolation, sample-hold, backward sample-hold, or no
     * interpolation (i.e. dots should not be connected). Values are provided
     * as IM_xxx constants. 
     */
    static final String ATTR_INTERPOLATIONMODE = "interpolationmode";

    /**
     * Vector attribute: declares the theoretical lower bound of the values 
     * in the vector. This value may be used as a hint during result analysis.
     */
    static final String ATTR_MIN = "min";

    /**
     * Vector attribute: declares the theoretical upper bound of the values
     * in the vector. This value may be used as a hint during result analysis.
     */
    static final String ATTR_MAX = "max";

    // values for ATTR_TYPE
    static final String TYPE_INT = "int";
    static final String TYPE_DOUBLE = "double"; 
    static final String TYPE_ENUM = "enum";

    // values for ATTR_INTERPOLATIONMODE
    static final String IM_NONE = "none";
    static final String IM_LINEAR = "linear"; 
    static final String IM_SAMPLE_HOLD = "sample-hold";
    static final String IM_BACKWARD_SAMPLE_HOLD = "backward-sample-hold";
}

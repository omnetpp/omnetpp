package org.omnetpp.scave.writers;


/**
 * Provides constants for IOutputScalarManager and IOutputVectorManager.
 * 
 * @author Andras
 */
public interface IResultAttributes {
    // run attributes
    static final String ATTR_INIFILE     = "inifile";
    static final String ATTR_CONFIGNAME  = "configname";
    static final String ATTR_RUNNUMBER   = "runnumber";
    static final String ATTR_NETWORK     = "network";
    static final String ATTR_EXPERIMENT  = "experiment";
    static final String ATTR_MEASUREMENT = "measurement";
    static final String ATTR_REPLICATION = "replication";
    static final String ATTR_DATETIME    = "datetime";
    static final String ATTR_PROCESSID   = "processid";
    static final String ATTR_RESULTDIR   = "resultdir";
    static final String ATTR_REPETITION  = "repetition";
    static final String ATTR_SEEDSET     = "seedset";
    static final String ATTR_ITERATIONVARS = "iterationvars";
    static final String ATTR_ITERATIONVARS2 = "iterationvars2";
    
    // vector/scalar attributes
    static final String ATTR_UNIT = "unit";
    static final String ATTR_ENUMNAME = "enumname";  // name of the enum
    static final String ATTR_ENUM = "enum";  // defines symbolic names for numeric values; syntax for the attribute value: "NAME1=value1, NAME2=value2, NAME3=value3"
    static final String ATTR_TYPE = "type";
    
    // vector-only attributes
    static final String ATTR_INTERPOLATIONMODE = "interpolationmode";
    static final String ATTR_MIN = "min";
    static final String ATTR_MAX = "max";

    // values for ATTR_TYPE
    static final String TYPE_INT = "int";
    static final String TYPE_DOUBLE = "double"; 
    static final String TYPE_ENUM = "enum";

    // values for ATTR_INTERPOLATIONMODE
    static final String NONE = "none";
    static final String LINEAR = "linear"; 
    static final String SAMPLE_HOLD = "sample-hold";
    static final String BACKWARD_SAMPLE_HOLD = "backward-sample-hold";
}

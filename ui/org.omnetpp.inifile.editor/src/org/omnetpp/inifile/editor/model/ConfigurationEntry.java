package org.omnetpp.inifile.editor.model;

/**
 * One entry in the ConfigurationRegistry. Describes a configuration entry. 
 * 
 * @author Andras
 */
public class ConfigurationEntry {
    /** Configuration entry types */
    public enum Type {
      CFG_BOOL,
      CFG_INT,
      CFG_DOUBLE,
      CFG_STRING,
      CFG_FILENAME,
      CFG_FILENAMES,
      CFG_CUSTOM
    };

    private String name;      // e.g. "sim-time-limit"
    private boolean isGlobal; // if true, it cannot occur in [Run X] sections
    private Type type;        // entry type
    private String unit;      // if numeric, its unit ("s") or empty string
    private Object defaultValue; // the default value
    private String description;  // help text

    /**
     * Constructor.
     */
    ConfigurationEntry(String name, boolean isGlobal, Type type,
                 String unit, Object defaultValue, String description) {
        this.name = name;
        this.isGlobal = isGlobal;
        this.type = type;
        this.unit = unit;
        this.defaultValue = defaultValue;
        this.description = description;
    }

    public String getKey() {
    	return name;
    }
    
    public boolean isGlobal() {
    	return isGlobal;
    }
    
    public Type getType() {
    	return type;
    }
        
	public Object getDefaultValue() {
		return defaultValue;
	}

	public String getDescription() {
		return description;
	}

	public String getUnit() {
		return unit;
	}
}

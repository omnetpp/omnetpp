package org.omnetpp.inifile.editor.model;

/**
 * Classifies inifile keys; see getKeyType().
 */
public enum KeyType {
	CONFIG, // contains no dot (like sim-time-limit=, etc)
	PARAM,  // contains dot, but no hyphen: parameter setting (like **.app.delay)
	PER_OBJECT_CONFIG;

    /**
     * Classify an inifile key, based on its syntax.
     */
    public static KeyType getKeyType(String key) {
    	if (!key.contains("."))
    		return CONFIG;  // contains no dot
    	else if (!key.contains("-"))
    		return PARAM; // contains dot, but no hyphen
    	else
    		return PER_OBJECT_CONFIG; // contains both dot and hyphen
    } // dotted, and contains hyphen (like **.partition-id, rng mapping, vector configuration, etc)
}
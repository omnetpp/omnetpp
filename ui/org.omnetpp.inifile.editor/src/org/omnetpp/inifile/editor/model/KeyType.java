package org.omnetpp.inifile.editor.model;

/**
 * Classifies inifile keys; see getKeyType().
 */
public enum KeyType {
    CONFIG, // contains no dot (like sim-time-limit=, etc)
    PARAM,  // contains dot but no hyphen: parameter setting (like **.app.delay)
    PER_OBJECT_CONFIG; // with dot and hyphen (exception: **.typename)

    /**
     * Classify an inifile key, based on its syntax.
     */
    public static KeyType getKeyType(String key) {
        int indexOfLastDot = InifileUtils.findLastDot(key);
        if (indexOfLastDot == -1)
            return CONFIG;  // contains no dot
        else if (!key.contains("-") && !key.endsWith("."+ConfigRegistry.TYPENAME))
            return PARAM; // contains dot, but no hyphen
        else
            return PER_OBJECT_CONFIG; // contains both dot and hyphen
    }
}
package org.omnetpp.inifile.editor.model;

/**
 * Specifies a fixed amount of time allocated for a computation.
 * Use it as:
 *     someLongComputation(..., new Timeout(1000))
 *     
 * @author tomi
 */
public class Timeout implements ITimeout {

    private long timeout;
    private long endTime;
    
    public Timeout(long millis) {
        this.timeout = millis;
        this.endTime = System.currentTimeMillis() + millis;
    }
    
    public long getRemainingTime() {
        return endTime - System.currentTimeMillis();
    }
    
    public String toString() {
        return String.format("Timeout(%dms)", timeout);
    }
}

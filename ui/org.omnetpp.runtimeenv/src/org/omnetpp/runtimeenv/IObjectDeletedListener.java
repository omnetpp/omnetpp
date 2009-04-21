/**
 * 
 */
package org.omnetpp.runtimeenv;

import org.omnetpp.experimental.simkernel.swig.cObject;


/**
 * 
 * @author Andras
 */
public interface IObjectDeletedListener {
    void objectDeleted(cObject obj);
    //TODO void objectsDeleted(PointerSet deletedObjects);
}
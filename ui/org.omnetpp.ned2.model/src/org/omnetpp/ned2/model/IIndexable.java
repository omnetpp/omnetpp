package org.omnetpp.ned2.model;

/**
 * @author rhornig
 * Objects that can have an optional index (ie. they can be vector or non vector)
 */
public interface IIndexable extends INamed {
    
    public String getVectorSize();
    
    public void setVectorSize(String indexstring);
    
    public String getNameWithIndex();
    
}

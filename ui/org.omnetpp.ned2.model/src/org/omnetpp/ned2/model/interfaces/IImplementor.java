package org.omnetpp.ned2.model.interfaces;

import java.util.List;

import org.omnetpp.ned2.model.pojo.InterfaceNameNode;

/**
 * @author rhornig
 * Objects that can implement other interfaces
 */
public interface IImplementor {

    /**
     * @return All ned elements (interface nodes) that are implemented by this object
     */
    public List<InterfaceNameNode> getAllInterfaces();
}

package org.omnetpp.ned.model.interfaces;

import java.util.List;

import org.omnetpp.ned.model.pojo.InterfaceNameNode;

/**
 * @author rhornig
 * Objects that can implement other interfaces
 */
public interface IHasInterfaces {

    /**
     * @return All ned elements (interface nodes) that are implemented by this object
     */
    public List<InterfaceNameNode> getAllInterfaces();
}

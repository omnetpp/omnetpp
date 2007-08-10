package org.omnetpp.ned.model.interfaces;

import java.util.List;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.pojo.InterfaceNameNode;

/**
 * Objects that can implement other interfaces
 *
 * @author rhornig
 */
public interface IHasInterfaces extends INEDElement {

    /**
     * Returns the list of all ned elements (interface nodes) that are implemented by this object
     */
    public List<InterfaceNameNode> getAllInterfaces();
}

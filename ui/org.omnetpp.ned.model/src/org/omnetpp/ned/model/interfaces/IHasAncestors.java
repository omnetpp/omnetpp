package org.omnetpp.ned.model.interfaces;

import java.util.List;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.pojo.ExtendsNode;

/**
 * Objects that can extend other objects, i.e. they are derived objects.
 *
 * @author rhornig
 */
//XXX merge into INedTypeNode? --Andras
public interface IHasAncestors extends INEDElement {

    /**
     * Returns the base object's name (ONLY the first extends node name returned)
     */
    public String getFirstExtends();

    /**
     * @param ext The object's name that is extended
     */
    public void setFirstExtends(String ext);

    /**
     * Returns the TTypeInfo object of the base object of this component. ie this method checks the base type
     * of this element and looks up the typeinfo object to that. NOTE that this check only the
     * FIRST extends node, so it returns the first extends child for ModuleInterface and ChannelInterface
     * (Special handling is needed for these types)
     */
    public INEDTypeInfo getFirstExtendsNEDTypeInfo();

    /**
     * Returns the model element that represents the base object of this element
     *
     * NOTE that this check only the FIRST extends node, so it returns the first
     * "extends" child for ModuleInterface and ChannelInterface. 
     * (Special handling is needed for these types)
     */
    public INEDElement getFirstExtendsRef();

    /**
     * Returns the list of all ned elements that used as base (usually only a single element,
     * but can be more than that)
     */
    public List<ExtendsNode> getAllExtends();
}

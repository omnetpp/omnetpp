package org.omnetpp.ned.model.interfaces;

import java.util.List;

import org.omnetpp.ned.model.pojo.ExtendsElement;


/**
 * A marker interface for elements that can be used as toplevel elements
 * in a NED file, except property definitions and includes.
 *
 * @author rhornig
 */
public interface INedTypeElement extends IHasName, IHasDisplayString, IHasParameters {
	
	/**
	 * Returns the typeinfo belonging to this NED type. This can be trusted
	 * to be NOT null for all NED types, including duplicate and invalid ones.
	 * 
	 * Only null for detached trees that haven't been seen at all by NEDResources 
	 * (i.e. not part of any NED file).   
	 */
	public INEDTypeInfo getNEDTypeInfo();

	/**
	 * Sets the associated component type info. To be called from the NED type 
	 * resolver (NEDResources) only.
	 */
	public void setNEDTypeInfo(INEDTypeInfo typeInfo);

    /**
     * Returns the base object's name (ONLY the first extends node name returned)
     */
    public String getFirstExtends();

    /**
     * Sets the first "extends" node to the given NED type name
     */
    public void setFirstExtends(String name);

    /**
     * Returns the TypeInfo object of the base object of this component. I.e. this method checks the base type
     * of this element and looks up the typeinfo object to that. NOTE that this check only the
     * FIRST extends node, so it returns the first extends child for ModuleInterface and ChannelInterface
     * (Special handling is needed for these types)
     */
    public INEDTypeInfo getFirstExtendsNEDTypeInfo();

    /**
     * Returns the model element that represents the base object of this element.
     *
     * NOTE that this checks only the FIRST "extends" node, so it doesn't return full 
     * inheritance info for ModuleInterface and ChannelInterface. 
     */
    public INedTypeElement getFirstExtendsRef();

    /**
     * Returns the list of all ned elements that used as base (usually only a single element,
     * but can be more than that)
     */
    //FIXME needed? get it from typeInfo --Andras
    public List<ExtendsElement> getAllExtends();
}

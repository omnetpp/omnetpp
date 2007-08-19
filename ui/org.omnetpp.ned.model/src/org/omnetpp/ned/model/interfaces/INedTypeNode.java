package org.omnetpp.ned.model.interfaces;


/**
 * A marker interface for elements that can be used as toplevel elements
 * in a NED file, except property definitions and includes.
 *
 * @author rhornig
 */
public interface INedTypeNode extends IHasName, IHasAncestors, IHasDisplayString, IHasParameters {
	
	/**
	 * Returns the typeInfo belonging to this NED type.
	 * 
	 * Returns null if the NED type is duplicated or invalid.
	 * FIXME it may still have an INedTypeInfo, only not available via the resolver, or??  
	 */
	public INEDTypeInfo getNEDTypeInfo();

	/**
	 * Sets the associated component type info. To be called from the NED type 
	 * resolver (NEDResources) only.
	 */
	public void setNEDTypeInfo(INEDTypeInfo typeInfo);
}

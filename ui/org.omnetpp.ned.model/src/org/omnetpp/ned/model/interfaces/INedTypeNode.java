package org.omnetpp.ned.model.interfaces;


/**
 * A marker interface for elements that can be used as toplevel elements
 * in a NED file, except property definitions and includes.
 *
 * @author rhornig
 */
public interface INedTypeNode extends IHasName, IHasAncestors, IHasDisplayString, IHasParameters {
	
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
}

package org.omnetpp.ned.model.interfaces;

import org.omnetpp.common.displaymodel.IHasDisplayString;

/**
 * Common interface for SimpleModuleNodeEx and CompoundModuleNodeEx.
 * 
 * @author Andras
 */
public interface IModuleTypeNode extends INedTypeNode, 
	IHasAncestors, IHasInterfaces, IHasParameters, IHasGates,
	IHasDisplayString, IHasParent, IHasName {
}

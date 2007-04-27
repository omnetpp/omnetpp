package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INEDElement;

/**
 * A marker interface to mark all elements that can be used as top level in a nedfile
 * @author rhornig
 */
//XXX extend IHasDisplayString and IHasParameters too?
public interface ITopLevelElement extends INEDElement, 
	IHasParent, IHasName, IHasAncestors {

}

package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INEDElement;

/**
 * A marker interface for elements that can be used as toplevel elements
 * in a NED file, except property definitions and includes.
 *
 * @author rhornig
 */
//XXX extend IHasDisplayString and IHasParameters too?
public interface INedTypeNode extends INEDElement, IHasName, IHasAncestors {

}

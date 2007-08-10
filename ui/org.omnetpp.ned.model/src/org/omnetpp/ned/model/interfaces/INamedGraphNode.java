package org.omnetpp.ned.model.interfaces;

import org.omnetpp.common.displaymodel.IHasDisplayString;

/**
 * Az element that has name, display string, can be connected (submodule and compound module)
 *
 * @author rhornig
 */
public interface INamedGraphNode extends IHasDisplayString, IHasConnections, IHasParent, IHasName  {

}

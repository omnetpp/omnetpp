package org.omnetpp.ned2.model.interfaces;

import org.omnetpp.common.displaymodel.IDisplayStringProvider;

/**
 * Az element that has name, displaystring, can be connected (submodule and compound module)
 * @author rhornig
 */
public interface INamedGraphNode extends IDisplayStringProvider, IConnectable, IParentable, INamed  {

}

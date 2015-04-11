/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INedElement;

/**
 * Can provide a NED Model. Allows that we access the model from any selection
 * without depending on the GEF plugin. GEF EditParts should implement this, as an
 * IStructuredSelection contains GEF EditParts in the graphical NED editor.
 *
 * @author rhornig
 */
public interface INedModelProvider {
    public INedElement getModel();
}

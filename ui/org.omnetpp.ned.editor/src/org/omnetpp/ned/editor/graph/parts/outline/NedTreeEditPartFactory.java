/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts.outline;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;

/**
 * Creates edit parts for the NEDElements used in outline view
 *
 * @author rhornig
 */
public class NedTreeEditPartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, Object model) {
        return new NedTreeEditPart(model);
    }

}
/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts.outline;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.eclipse.gef.editparts.AbstractTreeEditPart;
import org.omnetpp.ned.model.INedElement;

/**
 * Creates edit parts for the NEDElements used in outline view
 *
 * @author rhornig
 */
public class NedTreeEditPartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, final Object model) {
        if (model instanceof INedElement)
            return new NedTreeEditPart(model);
        else
            return new AbstractTreeEditPart() {
            @Override
            protected String getText() {
                return model.toString();
            }
        };
    }

}
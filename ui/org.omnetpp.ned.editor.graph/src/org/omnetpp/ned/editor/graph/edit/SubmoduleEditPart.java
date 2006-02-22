/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.edit;

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.AccessibleAnchorProvider;
import org.eclipse.gef.AccessibleEditPart;
import org.eclipse.swt.accessibility.AccessibleEvent;
import org.omnetpp.ned.editor.graph.figures.SimpleModuleFigure;
import org.omnetpp.ned.editor.graph.model.SubmoduleModel;

/**
 * EditPart for holding gates in the Logic Example.
 */
public class SubmoduleEditPart extends NedNodeEditPart {

    /**
     * Returns a newly created Figure of this.
     * 
     * @return A new Figure of this.
     */
    protected IFigure createFigure() {
        
        return new SimpleModuleFigure();
    }

    /**
     * Returns the Figure for this as an SimpleModuleFigure.
     * 
     * @return Figure of this as a SimpleModuleFigure
     */
    protected SimpleModuleFigure getSimpleModuleFigure() {
        return (SimpleModuleFigure) getFigure();
    }

    /**
     * Returns the model of this as a SimpleModule
     * 
     * @return Model of this as a SimpleModule
     */
    protected SubmoduleModel getSimpleModule() {
        return (SubmoduleModel) getModel();
    }
    
    public Object getAdapter(Class key) {
        if (key == AccessibleAnchorProvider.class) return new DefaultAccessibleAnchorProvider() {
            public List getSourceAnchorLocations() {
                List list = new ArrayList();
                Vector sourceAnchors = getNedFigure().getSourceConnectionAnchors();
                for (int i = 0; i < sourceAnchors.size(); i++) {
                    ConnectionAnchor anchor = (ConnectionAnchor) sourceAnchors.get(i);
                    list.add(anchor.getReferencePoint().getTranslated(0, -3));
                }
                return list;
            }

            public List getTargetAnchorLocations() {
                List list = new ArrayList();
                Vector targetAnchors = getNedFigure().getTargetConnectionAnchors();
                for (int i = 0; i < targetAnchors.size(); i++) {
                    ConnectionAnchor anchor = (ConnectionAnchor) targetAnchors.get(i);
                    list.add(anchor.getReferencePoint());
                }
                return list;
            }
        };

        // FIXME this is a duplicate. Should be deleted?
        if (key == AccessibleAnchorProvider.class) return new DefaultAccessibleAnchorProvider() {
            public List getSourceAnchorLocations() {
                List list = new ArrayList();
                Vector sourceAnchors = getNedFigure().getSourceConnectionAnchors();
                for (int i = 0; i < sourceAnchors.size(); i++) {
                    ConnectionAnchor anchor = (ConnectionAnchor) sourceAnchors.get(i);
                    list.add(anchor.getReferencePoint().getTranslated(0, -3));
                }
                return list;
            }

        };
        return super.getAdapter(key);
    }

}

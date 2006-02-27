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

import java.beans.PropertyChangeEvent;
import java.util.List;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.requests.DropRequest;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedNodeEditPolicy;
import org.omnetpp.ned.editor.graph.figures.NedFigure;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayCalloutSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayInfoTextSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayNameSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayQueueSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayRangeSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayShapeSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayTooltipSupport;
import org.omnetpp.ned.editor.graph.misc.ColorFactory;
import org.omnetpp.ned.editor.graph.misc.ImageFactory;
import org.omnetpp.ned.editor.graph.model.INedComponent;
import org.omnetpp.ned.editor.graph.properties.DisplayPropertySource;

/**
 * Base abstract controller for NedModel and NedFigures. Provides support for 
 * connection handling and common display attributes.
 */
abstract public class NedNodeEditPart extends ContainerEditPart {

    protected void createEditPolicies() {
        super.createEditPolicies();
        installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
        installEditPolicy(EditPolicy.GRAPHICAL_NODE_ROLE, new NedNodeEditPolicy());
    }


    /**
     * Returns a list of connections for which this is the source.
     * 
     * @return List of connections.
     */
    protected List getModelSourceConnections() {
        return ((INedComponent)getNEDModel()).getSrcConnections();
    }

    /**
     * Returns a list of connections for which this is the target.
     * 
     * @return List of connections.
     */
    protected List getModelTargetConnections() {
        return ((INedComponent)getNEDModel()).getDestConnections();
    }

    /**
     * Returns the Figure of this, as a NedFigure.
     * 
     * @return Figure as a NedFigure.
     */
    protected NedFigure getNedFigure() {
        return (NedFigure) getFigure();
    }

    /**
     * Returns the connection anchor for the given ConnectionEditPart's source.
     * 
     * @return ConnectionAnchor.
     */
//    public ConnectionAnchor getSourceConnectionAnchor(ConnectionEditPart connEditPart) {
//    	ConnectionNodeEx conn = (ConnectionNodeEx) connEditPart.getModel();
//        return getNedFigure().getConnectionAnchor(conn.getSourceGate());
//    }

    /**
     * Returns the connection anchor of a source connection which is at the
     * given point.
     * 
     * @return ConnectionAnchor.
     */
    public ConnectionAnchor getSourceConnectionAnchor(Request request) {
        Point pt = new Point(((DropRequest) request).getLocation());
        return getNedFigure().getSourceConnectionAnchorAt(pt);
    }

    /**
     * Returns the connection anchor for the given ConnectionEditPart's target.
     * 
     * @return ConnectionAnchor.
     */
//    public ConnectionAnchor getTargetConnectionAnchor(ConnectionEditPart connEditPart) {
//        ConnectionNodeEx conn = (ConnectionNodeEx) connEditPart.getModel();
//        return getNedFigure().getConnectionAnchor(conn.getTargetGate());
//    }

    /**
     * Returns the connection anchor of a terget connection which is at the
     * given point.
     * 
     * @return ConnectionAnchor.
     */
    public ConnectionAnchor getTargetConnectionAnchor(Request request) {
        Point pt = new Point(((DropRequest) request).getLocation());
        return getNedFigure().getTargetConnectionAnchorAt(pt);
    }

    /**
     * Returns the gate associated with a the given connection anchor.
     * 
     * @return The name of the ConnectionAnchor as a String.
     */
    final public String getGate(ConnectionAnchor c) {
        return getNedFigure().getGate(c);
    }

    /**
     * Updates the visual aspect of this.
     */
    // FIXME most of this stuff should go to SubmoduleEditPart.refreshVisuls()
    protected void refreshVisuals() {
        
        // define the properties that determine the visual appearence
        
    	INedComponent model = (INedComponent)getNEDModel();
    	
        // parse a dispaly string, so it's easier to get values from it.
    	String displayStr = model.getDisplayString();
        DisplayPropertySource dps = 
        	new DisplayPropertySource(displayStr);
        
        // setup the figure's properties

        Integer x = dps.getIntegerPropertyDef(DisplayPropertySource.PROP_X);
        Integer y = dps.getIntegerPropertyDef(DisplayPropertySource.PROP_Y);
        Integer w = dps.getIntegerPropertyDef(DisplayPropertySource.PROP_W);
        Integer h = dps.getIntegerPropertyDef(DisplayPropertySource.PROP_H);
        // FIXME hardcoded
        if(x == null) x = 100;
        if(y == null) y = 100;
        if(h == null) h = -1;
        if(w == null) w = -1;
        // set the location and size using the models helper methods
        Rectangle constraint = new Rectangle(x, y, w, h);
        ((GraphicalEditPart) getParent()).setLayoutConstraint(this, getFigure(), constraint);
        // check if the figure supports the name decoration
        if(getNedFigure() instanceof DisplayNameSupport)
            ((DisplayNameSupport)getNedFigure()).setName(model.getName());
        // range support
        if(getNedFigure() instanceof DisplayRangeSupport)
            ((DisplayRangeSupport)getNedFigure()).setRange(
                    dps.getIntPropertyDef(DisplayPropertySource.PROP_RANGE, -1),
                    ColorFactory.asColor(dps.getStringPropertyDef(DisplayPropertySource.PROP_RANGEFILLCOL)),
                    ColorFactory.asColor(dps.getStringPropertyDef(DisplayPropertySource.PROP_RANGEBORDERCOL)),
                    dps.getIntPropertyDef(DisplayPropertySource.PROP_RANGEBORDERWIDTH, -1));
        // tooltip support
        if(getNedFigure() instanceof DisplayTooltipSupport)
            ((DisplayTooltipSupport)getNedFigure()).setTooltipText(
                    dps.getStringPropertyDef(DisplayPropertySource.PROP_TOOLTIP));
        // queue length support
        if(getNedFigure() instanceof DisplayQueueSupport)
            ((DisplayQueueSupport)getNedFigure()).setQueueText(
                    dps.getStringPropertyDef(DisplayPropertySource.PROP_QUEUE));
        // additional text support
        if(getNedFigure() instanceof DisplayInfoTextSupport)
            ((DisplayInfoTextSupport)getNedFigure()).setInfoText(
                    dps.getStringPropertyDef(DisplayPropertySource.PROP_TEXT), 
                    dps.getStringPropertyDef(DisplayPropertySource.PROP_TEXTPOS),
                    ColorFactory.asColor(dps.getStringPropertyDef(DisplayPropertySource.PROP_TEXTCOLOR)));
        // shape support
        if(getNedFigure() instanceof DisplayShapeSupport) {
            String imgSize = dps.getStringPropertyDef(DisplayPropertySource.PROP_IMAGESIZE);
            Image img = ImageFactory.getImage(
                    dps.getStringPropertyDef(DisplayPropertySource.PROP_IMAGE), 
                    imgSize,
                    ColorFactory.asRGB(dps.getStringPropertyDef(DisplayPropertySource.PROP_IMAGECOLOR)),
                    dps.getIntPropertyDef(DisplayPropertySource.PROP_IMAGECOLORPCT,0));
            
            // set the figure properties
            ((DisplayShapeSupport)getNedFigure()).setShape(img, 
                    dps.getStringPropertyDef(DisplayPropertySource.PROP_SHAPE), 
                    dps.getIntPropertyDef(DisplayPropertySource.PROP_W, -1), 
                    dps.getIntPropertyDef(DisplayPropertySource.PROP_H, -1),
                    ColorFactory.asColor(dps.getStringPropertyDef(DisplayPropertySource.PROP_FILLCOL)),
                    ColorFactory.asColor(dps.getStringPropertyDef(DisplayPropertySource.PROP_BORDERCOL)),
                    dps.getIntPropertyDef(DisplayPropertySource.PROP_BORDERWIDTH, -1));
            // set the decoration image properties
            ((DisplayShapeSupport)getNedFigure()).setImageDecoration(
                    ImageFactory.getImage(
                            dps.getStringPropertyDef(DisplayPropertySource.PROP_OVIMAGE),
                            null,
                            ColorFactory.asRGB(dps.getStringPropertyDef(DisplayPropertySource.PROP_OVIMAGECOLOR)),
                            dps.getIntPropertyDef(DisplayPropertySource.PROP_OVIMAGECOLORPCT,0)));

        }
        
        // XXX callout bubble. just for testing
        if(getNedFigure() instanceof DisplayCalloutSupport) {
            if (x != null) {
                ((DisplayCalloutSupport)getNedFigure()).addCallout("Yes Sir, my position is: "+x+","+y );
                if (x>70) ((DisplayCalloutSupport)getNedFigure()).clearCallout();
            }
        } 
    }
    
    

}

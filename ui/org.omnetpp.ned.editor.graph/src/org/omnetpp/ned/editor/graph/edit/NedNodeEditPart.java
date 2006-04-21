package org.omnetpp.ned.editor.graph.edit;

import java.util.List;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.requests.DropRequest;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageFactory;
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
import org.omnetpp.ned2.model.DisplayString;
import org.omnetpp.ned2.model.INedModule;

/**
 * Base abstract controller for NedModel and NedFigures. Provides support for 
 * connection handling and common display attributes.
 */
abstract public class NedNodeEditPart extends ContainerEditPart {

    @Override
    protected void createEditPolicies() {
        super.createEditPolicies();
        installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
        installEditPolicy(EditPolicy.GRAPHICAL_NODE_ROLE, new NedNodeEditPolicy());
    }


    /**
     * Returns a list of connections for which this is the srcModule.
     * 
     * @return List of connections.
     */
    @Override
    protected List getModelSourceConnections() {
        return ((INedModule)getNEDModel()).getSrcConnections();
    }

    /**
     * Returns a list of connections for which this is the destModule.
     * 
     * @return List of connections.
     */
    @Override
    protected List getModelTargetConnections() {
        return ((INedModule)getNEDModel()).getDestConnections();
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
     * Returns the connection anchor for the given ConnectionEditPart's srcModule.
     * 
     * @return ConnectionAnchor.
     */
//    public ConnectionAnchor getSourceConnectionAnchor(ConnectionEditPart connEditPart) {
//    	ConnectionNodeEx conn = (ConnectionNodeEx) connEditPart.getModel();
//        return getNedFigure().getConnectionAnchor(conn.getSourceGate());
//    }

    /**
     * Returns the connection anchor of a srcModule connection which is at the
     * given point.
     * 
     * @return ConnectionAnchor.
     */
    public ConnectionAnchor getSourceConnectionAnchor(Request request) {
        Point pt = new Point(((DropRequest) request).getLocation());
        return getNedFigure().getSourceConnectionAnchorAt(pt);
    }

    /**
     * Returns the connection anchor for the given ConnectionEditPart's destModule.
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
    @Override
    protected void refreshVisuals() {
        
        // define the properties that determine the visual appearence
        
    	INedModule model = (INedModule)getNEDModel();
    	
        // parse a dispaly string, so it's easier to get values from it.
        DisplayString dps = model.getDisplayString();
        
        // setup the figure's properties

        Integer x = dps.getAsIntDef(DisplayString.Prop.X, 0);
        Integer y = dps.getAsIntDef(DisplayString.Prop.Y, 0);
        Integer w = dps.getAsIntDef(DisplayString.Prop.WIDTH, -1);
        Integer h = dps.getAsIntDef(DisplayString.Prop.HEIGHT, -1);
        // set the location and size using the models helper methods
        Rectangle constraint = new Rectangle(x, y, w, h);
        ((GraphicalEditPart) getParent()).setLayoutConstraint(this, getFigure(), constraint);
        // check if the figure supports the name decoration
        if(getNedFigure() instanceof DisplayNameSupport)
            ((DisplayNameSupport)getNedFigure()).setName(model.getName());
        // range support
        if(getNedFigure() instanceof DisplayRangeSupport)
            ((DisplayRangeSupport)getNedFigure()).setRange(
                    dps.getAsIntDef(DisplayString.Prop.RANGE, -1),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.RANGEFILLCOL)),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.RANGEBORDERCOL)),
                    dps.getAsIntDef(DisplayString.Prop.RANGEBORDERWIDTH, -1));
        // tooltip support
        if(getNedFigure() instanceof DisplayTooltipSupport)
            ((DisplayTooltipSupport)getNedFigure()).setTooltipText(
                    dps.getAsStringDef(DisplayString.Prop.TOOLTIP));
        // queue length support
        if(getNedFigure() instanceof DisplayQueueSupport)
            ((DisplayQueueSupport)getNedFigure()).setQueueText(
                    dps.getAsStringDef(DisplayString.Prop.QUEUE));
        // additional text support
        if(getNedFigure() instanceof DisplayInfoTextSupport)
            ((DisplayInfoTextSupport)getNedFigure()).setInfoText(
                    dps.getAsStringDef(DisplayString.Prop.TEXT), 
                    dps.getAsStringDef(DisplayString.Prop.TEXTPOS),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.TEXTCOLOR)));
        // shape support
        if(getNedFigure() instanceof DisplayShapeSupport) {
            String imgSize = dps.getAsStringDef(DisplayString.Prop.IMAGESIZE);
            Image img = ImageFactory.getImage(
                    dps.getAsStringDef(DisplayString.Prop.IMAGE), 
                    imgSize,
                    ColorFactory.asRGB(dps.getAsStringDef(DisplayString.Prop.IMAGECOLOR)),
                    dps.getAsIntDef(DisplayString.Prop.IMAGECOLORPCT,0));
            
            // set the figure properties
            ((DisplayShapeSupport)getNedFigure()).setShape(img, 
                    dps.getAsStringDef(DisplayString.Prop.SHAPE), 
                    dps.getAsIntDef(DisplayString.Prop.WIDTH, -1), 
                    dps.getAsIntDef(DisplayString.Prop.HEIGHT, -1),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.FILLCOL)),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.BORDERCOL)),
                    dps.getAsIntDef(DisplayString.Prop.BORDERWIDTH, -1));
            // set the decoration image properties
            ((DisplayShapeSupport)getNedFigure()).setImageDecoration(
                    ImageFactory.getImage(
                            dps.getAsStringDef(DisplayString.Prop.OVIMAGE),
                            null,
                            ColorFactory.asRGB(dps.getAsStringDef(DisplayString.Prop.OVIMAGECOLOR)),
                            dps.getAsIntDef(DisplayString.Prop.OVIMAGECOLORPCT,0)));

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

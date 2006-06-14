package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.figures.SubmoduleFigure;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayCalloutSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayInfoTextSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayNameSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayQueueSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayRangeSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayShapeSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayTooltipSupport;
import org.omnetpp.ned2.model.DisplayString;
import org.omnetpp.ned2.model.INamedGraphNode;
import org.omnetpp.ned2.model.SubmoduleDisplayString;

public class SubmoduleEditPart extends ModuleEditPart {

    /** 
     * Returns a newly created Figure of this.
     * 
     * @return A new Figure of this.
     */
    @Override
    protected IFigure createFigure() {
        
        return new SubmoduleFigure();
    }

    /**
     * Returns the Figure for this as an SimpleModuleFigure.
     * 
     * @return Figure of this as a SimpleModuleFigure
     */
    protected SubmoduleFigure getSimpleModuleFigure() {
        return (SubmoduleFigure) getFigure();
    }

    /**
     * Updates the visual aspect of this.
     */
    @Override
    protected void refreshVisuals() {
        
        // define the properties that determine the visual appearence
        
    	INamedGraphNode model = (INamedGraphNode)getNEDModel();
    	
        // parse a dispaly string, so it's easier to get values from it.
        SubmoduleDisplayString dps = (SubmoduleDisplayString)model.getDisplayString();
        
        // setup the figure's properties

        // set the location and size using the models helper methods
        Point loc = dps.getLocation();
        // TODO get the location from the autolayouting engine if exact position is not specified
        if (loc == null) loc = new Point(0,0);
        
        Rectangle constraint = new Rectangle(loc, dps.getSize());
        ((GraphicalEditPart) getParent()).setLayoutConstraint(this, getFigure(), constraint);
        // check if the figure supports the name decoration
        if(getModuleFigure() instanceof DisplayNameSupport)
            ((DisplayNameSupport)getModuleFigure()).setName(model.getName());
        // range support
        if(getModuleFigure() instanceof DisplayRangeSupport)
            ((DisplayRangeSupport)getModuleFigure()).setRange(
                    dps.getRange(),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.RANGEFILLCOL)),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.RANGEBORDERCOL)),
                    dps.getAsIntDef(DisplayString.Prop.RANGEBORDERWIDTH, -1));
        // tooltip support
        if(getModuleFigure() instanceof DisplayTooltipSupport)
            ((DisplayTooltipSupport)getModuleFigure()).setTooltipText(
                    dps.getAsStringDef(DisplayString.Prop.TOOLTIP));
        // queue length support
        if(getModuleFigure() instanceof DisplayQueueSupport)
            ((DisplayQueueSupport)getModuleFigure()).setQueueText(
                    dps.getAsStringDef(DisplayString.Prop.QUEUE));
        // additional text support
        if(getModuleFigure() instanceof DisplayInfoTextSupport)
            ((DisplayInfoTextSupport)getModuleFigure()).setInfoText(
                    dps.getAsStringDef(DisplayString.Prop.TEXT), 
                    dps.getAsStringDef(DisplayString.Prop.TEXTPOS),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.TEXTCOLOR)));
        // shape support
        if(getModuleFigure() instanceof DisplayShapeSupport) {
            String imgSize = dps.getAsStringDef(DisplayString.Prop.IMAGESIZE);
            Image img = ImageFactory.getImage(
                    dps.getAsStringDef(DisplayString.Prop.IMAGE), 
                    imgSize,
                    ColorFactory.asRGB(dps.getAsStringDef(DisplayString.Prop.IMAGECOLOR)),
                    dps.getAsIntDef(DisplayString.Prop.IMAGECOLORPCT,0));
            
            // set the figure properties
            ((DisplayShapeSupport)getModuleFigure()).setShape(img, 
                    dps.getAsStringDef(DisplayString.Prop.SHAPE), 
                    constraint.width, 
                    constraint.height,
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.FILLCOL)),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.BORDERCOL)),
                    dps.getAsIntDef(DisplayString.Prop.BORDERWIDTH, -1));
            // set the decoration image properties
            ((DisplayShapeSupport)getModuleFigure()).setImageDecoration(
                    ImageFactory.getImage(
                            dps.getAsStringDef(DisplayString.Prop.OVIMAGE),
                            null,
                            ColorFactory.asRGB(dps.getAsStringDef(DisplayString.Prop.OVIMAGECOLOR)),
                            dps.getAsIntDef(DisplayString.Prop.OVIMAGECOLORPCT,0)));

        }
        
        // XXX callout bubble. just for testing
        if(getModuleFigure() instanceof DisplayCalloutSupport) {
                if (dps.getLocation()!=null && dps.getLocation().x >70) ((DisplayCalloutSupport)getModuleFigure()).clearCallout();
                	else ((DisplayCalloutSupport)getModuleFigure()).addCallout("Yes Sir, my position is: "+dps.getLocation() );

        } 
    }
    
	@Override
	public String[] getInputGateNames() {
		String gates[] = {"IN1", "IN2", "IN3", "IN4"}; 
		return gates; 
	}

	@Override
	public String[] getOutputGateNames() {
		String gates[] = {"OUT1", "OUT2", "OUT3", "OUT4"}; 
		return gates; 
	}
    
}

package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
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
import org.omnetpp.ned2.model.INedModule;

public class SubmoduleEditPart extends NedNodeEditPart {

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

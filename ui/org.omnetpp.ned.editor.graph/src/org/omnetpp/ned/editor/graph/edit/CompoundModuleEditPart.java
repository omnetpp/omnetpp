package org.omnetpp.ned.editor.graph.edit;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.AutoexposeHelper;
import org.eclipse.gef.CompoundSnapToHelper;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.ExposeHelper;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.MouseWheelHelper;
import org.eclipse.gef.SnapToGeometry;
import org.eclipse.gef.SnapToGrid;
import org.eclipse.gef.SnapToGuides;
import org.eclipse.gef.SnapToHelper;
import org.eclipse.gef.editparts.ViewportAutoexposeHelper;
import org.eclipse.gef.editparts.ViewportExposeHelper;
import org.eclipse.gef.editparts.ViewportMouseWheelHelper;
import org.eclipse.gef.editpolicies.SnapFeedbackPolicy;
import org.eclipse.gef.rulers.RulerProvider;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.edit.policies.CompoundModuleLayoutEditPolicy;
import org.omnetpp.ned.editor.graph.figures.CompoundModuleFigure;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayBackgroundSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayShapeSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayTitleSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayTooltipSupport;
import org.omnetpp.ned2.model.CompoundModuleDisplayString;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.DisplayString;
import org.omnetpp.ned2.model.INamedGraphNode;
import org.omnetpp.ned2.model.DisplayString.Prop;

public class CompoundModuleEditPart extends ModuleEditPart {


    @Override
    protected void createEditPolicies() {
        super.createEditPolicies();
        installEditPolicy(EditPolicy.LAYOUT_ROLE, new CompoundModuleLayoutEditPolicy((XYLayout) getContentPane()
                .getLayoutManager()));
        installEditPolicy("Snap Feedback", new SnapFeedbackPolicy()); //$NON-NLS-1$
    }

    /**
     * Creates a new Module Figure and returns it.
     * 
     * @return Figure representing the module.
     */
    @Override
    protected IFigure createFigure() {
        return new CompoundModuleFigure();
    }

    /**
     * Returns the Figure of this as a ModuleFigure.
     * 
     * @return ModuleFigure of this.
     */
    protected CompoundModuleFigure getCompoundModuleFigure() {
        return (CompoundModuleFigure) getFigure();
    }

    @Override
    public IFigure getContentPane() {
        return getCompoundModuleFigure().getContentsPane();
    }

    @Override
    public Object getAdapter(Class key) {
        
        if (key == AutoexposeHelper.class) return new ViewportAutoexposeHelper(this);
        
        if (key == ExposeHelper.class) return new ViewportExposeHelper(this);

        if (key == MouseWheelHelper.class) return new ViewportMouseWheelHelper(this);

        // snap to grig/guide adaptor
        if (key == SnapToHelper.class) {
            List snapStrategies = new ArrayList();
            Boolean val = (Boolean) getViewer().getProperty(RulerProvider.PROPERTY_RULER_VISIBILITY);
            if (val != null && val.booleanValue()) snapStrategies.add(new SnapToGuides(this));
            val = (Boolean) getViewer().getProperty(SnapToGeometry.PROPERTY_SNAP_ENABLED);
            if (val != null && val.booleanValue()) snapStrategies.add(new SnapToGeometry(this));
            val = (Boolean) getViewer().getProperty(SnapToGrid.PROPERTY_GRID_ENABLED);
            if (val != null && val.booleanValue()) snapStrategies.add(new SnapToGrid(this));

            if (snapStrategies.size() == 0) return null;
            if (snapStrategies.size() == 1) return snapStrategies.get(0);

            SnapToHelper ss[] = new SnapToHelper[snapStrategies.size()];
            for (int i = 0; i < snapStrategies.size(); i++)
                ss[i] = (SnapToHelper) snapStrategies.get(i);
            return new CompoundSnapToHelper(ss);
        }
        
        return super.getAdapter(key);
    }

    @Override
    protected List getModelChildren() {
    	return ((CompoundModuleNodeEx)getNEDModel()).getSubmodules();
    }
    
    @Override
	public void propertyChanged(Prop changedProp) {
    	// if the scaling has changed we must refresh the children too
    	if (changedProp == Prop.MODULE_SCALE) {
    		refreshChildrenVisuals();
    	}
    	// refresh only ourselves
    	refreshVisuals();
    	System.out.println("Compound module notification: "+changedProp);
	}

	/**
     * Updates the visual aspect of this.
     */
    @Override
    protected void refreshVisuals() {
        
        // define the properties that determine the visual appearence
    	INamedGraphNode model = (INamedGraphNode)getNEDModel();
    	
        // parse a dispaly string, so it's easier to get values from it.
    	CompoundModuleDisplayString dps = (CompoundModuleDisplayString)model.getDisplayString();
        
        // setup the figure's properties
        // set the location and size using the models helper methods
        // if the siez is specified in the displaystring we should set it as preferred size
        // otherwise getPreferredSize should return the size calculated from the children
        if (dps.getSize().height > 0 || dps.getSize().width > 0) 
        	getNedFigure().setPreferredSize(dps.getSize());
        else 
        	getNedFigure().setPreferredSize(null);
        
        // check if the figure supports the name decoration
        if(getNedFigure() instanceof DisplayTitleSupport) {
        	// set the name and type + other inof on compound module
            ((DisplayTitleSupport)getNedFigure()).setName(model.getName());
            // set the icon showing the default representation in the titlebar
            Image img = ImageFactory.getImage(
                    dps.getAsStringDef(DisplayString.Prop.IMAGE), 
                    dps.getAsStringDef(DisplayString.Prop.IMAGESIZE),
                    ColorFactory.asRGB(dps.getAsStringDef(DisplayString.Prop.IMAGECOLOR)),
                    dps.getAsIntDef(DisplayString.Prop.IMAGECOLORPCT,0));
            ((DisplayTitleSupport)getNedFigure()).setDefaultShape(img, 
                    dps.getAsStringDef(DisplayString.Prop.SHAPE), 
                    dps.getAsIntDef(DisplayString.Prop.WIDTH, -1), 
                    dps.getAsIntDef(DisplayString.Prop.HEIGHT, -1),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.FILLCOL)),
                    ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.BORDERCOL)),
                    dps.getAsIntDef(DisplayString.Prop.BORDERWIDTH, -1));
        }
        
        // tooltip support
        if(getNedFigure() instanceof DisplayTooltipSupport)
            ((DisplayTooltipSupport)getNedFigure()).setTooltipText(
                    dps.getAsStringDef(DisplayString.Prop.TOOLTIP));
        
        // background color / image
        if(getNedFigure() instanceof DisplayBackgroundSupport) {
            Image img = ImageFactory.getImage(
                    dps.getAsStringDef(DisplayString.Prop.MODULE_IMAGE),
                    null, null, 0);
            
            // decode the image arrangement
            String imageArrangementStr = dps.getAsStringDef(DisplayString.Prop.MODULE_IMAGEARRANGEMENT);
            
            // set the background
            ((DisplayBackgroundSupport)getNedFigure()).setBackgorund(
            		img, 
            		imageArrangementStr, 
            		ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.MODULE_FILLCOL)), 
            		ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.MODULE_BORDERCOL)), 
            		dps.getAsIntDef(DisplayString.Prop.MODULE_BORDERWIDTH, -1));
            
            // grid support
            ((DisplayBackgroundSupport)getNedFigure()).setGrid(
            		dps.getGridTickDistance(), 
            		dps.getAsIntDef(DisplayString.Prop.MODULE_TICKNUMBER, -1), 
            		ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.MODULE_GRIDCOL)));
            
            // scaling support
            ((DisplayBackgroundSupport)getNedFigure()).setScale(
            		dps.getAsFloatDef(DisplayString.Prop.MODULE_SCALE, 1),
            		dps.getAsStringDef(DisplayString.Prop.MODULE_UNIT));
        }
        
        // default icon / shape support
        if(getNedFigure() instanceof DisplayShapeSupport) {
            Image img = ImageFactory.getImage(
                    dps.getAsStringDef(DisplayString.Prop.IMAGE), 
                    dps.getAsStringDef(DisplayString.Prop.IMAGESIZE),
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

        }
        
    }

	@Override
	String[] getInputGateNames() {
		String gates[] = {"IN1", "IN2", "IN3", "IN4"}; 
		return gates; 
	}

	@Override
	String[] getOutputGateNames() {
		String gates[] = {"OUT1", "OUT2", "OUT3", "OUT4"}; 
		return gates; 
	}
}

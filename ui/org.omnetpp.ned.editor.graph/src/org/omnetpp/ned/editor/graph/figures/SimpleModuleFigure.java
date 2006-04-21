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
package org.omnetpp.ned.editor.graph.figures;


import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.Shape;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayCalloutSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayInfoTextSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayNameSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayQueueSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayRangeSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayShapeSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayTooltipSupport;
import org.omnetpp.ned.editor.graph.figures.properties.LayerSupport;

public class SimpleModuleFigure extends NedFigure implements HandleBounds, 
    DisplayRangeSupport, DisplayNameSupport, DisplayTooltipSupport, DisplayQueueSupport, 
    DisplayInfoTextSupport, DisplayCalloutSupport, DisplayShapeSupport {

    protected Layer foregroundLayer;
    protected Layer backgroundLayer;
    
    protected Shape currShape;
    protected RectangleFigure rectShapeFigure = new RectangleFigure();
    protected Ellipse ovalShapeFigure = new Ellipse();
    protected RoundedRectangle rrectShapeFigure = new RoundedRectangle();
    protected RegularPolygon polygonShapeFigure = new RegularPolygon(6,0);
    
    protected ImageFigure imageFigure = new ImageFigure();
    protected ImageFigure decoratorImageFigure = new ImageFigure();
    protected Label nameFigure = new Label();
    protected AttachedLayer textAttachLayer;
    protected Label textFigure = new Label();
    protected Label queueFigure = new Label();
    protected String queueName = "";
    protected TooltipFigure tooltipFigure = new TooltipFigure();
    protected Shape rangeFigure = new Ellipse();
    protected CalloutFigure calloutFigure = new CalloutFigure();

    public SimpleModuleFigure() {
        // anchors TEMPORARY
        PinnableNoncentralChopboxAnchor outputConnectionAnchor = new PinnableNoncentralChopboxAnchor(this);
        outputConnectionAnchor.setRelRefPoint(0.5, 0.8);
        addSourceConnectionAnchor(outputConnectionAnchor, "OUT");

        PinnableNoncentralChopboxAnchor inputConnectionAnchorA, inputConnectionAnchorB;
        inputConnectionAnchorA = new PinnableNoncentralChopboxAnchor(this, 0.2, 0.0);
        inputConnectionAnchorA.setPinnedDown(false);
        addTargetConnectionAnchor(inputConnectionAnchorA, "IN1");
        inputConnectionAnchorB = new PinnableNoncentralChopboxAnchor(this, 0.8, 0.0);
        inputConnectionAnchorB.setPinnedDown(false);
        addTargetConnectionAnchor(inputConnectionAnchorB, "IN2");
  
        setLayoutManager(new StackLayout());

        rectShapeFigure.setVisible(false);
        rrectShapeFigure.setVisible(false);
        rrectShapeFigure.setCornerDimensions(new Dimension(30, 30));
        ovalShapeFigure.setVisible(false);
        polygonShapeFigure.setVisible(false);
        rangeFigure.setOpaque(false);
        
        // XX just debugging
//        LineBorder lb = new LineBorder();
//        lb.setColor(ColorConstants.green);
//        setBorder(lb);
//        lb = new LineBorder();
//        lb.setColor(ColorConstants.blue);
//        imageFigure.setBorder(lb);
//        lb = new LineBorder();
//        lb.setColor(ColorConstants.red);
//        rectShapeFigure.setBorder(lb);
    }

    public void addNotify() {
        // functions here need to access the parent or ancestor figures, so these setup
        // procedures cannot be done in the constructor

        // look for decorator layers among the ancestor
        foregroundLayer = getAncestorLayer(LayerSupport.FRONT_DECORATION_LAYER);
        backgroundLayer = getAncestorLayer(LayerSupport.BACK_DECORATION_LAYER);
        
        // add mainf figures
        add(rectShapeFigure);
        add(rrectShapeFigure);
        add(ovalShapeFigure);
        add(polygonShapeFigure);
        add(imageFigure);
        
        // setup decorations belongig to the background decoration layer
        if(backgroundLayer != null) {
            backgroundLayer.add(new AttachedLayer(this, PositionConstants.CENTER, 
                                                     rangeFigure, PositionConstants.CENTER)); 
        }

        if(foregroundLayer != null) {
            // image decoration
            foregroundLayer.add(new AttachedLayer(this, new PrecisionPoint(1.0, 0.0), 
                    decoratorImageFigure, new PrecisionPoint(0.75, 0.25), null));
            //name decoration
            foregroundLayer.add(new AttachedLayer(this, PositionConstants.SOUTH, 
                                        nameFigure, PositionConstants.NORTH)); 
            // text comment decoration
            foregroundLayer.add(
                    textAttachLayer = new AttachedLayer(this, PositionConstants.NORTH, 
                                            textFigure, PositionConstants.SOUTH, 0, -1)); 
            // queue description
            foregroundLayer.add(new AttachedLayer(this, PositionConstants.SOUTH_EAST, 
                    queueFigure, PositionConstants.SOUTH_WEST, 2, 0));
            // callout (above the text)
            // TODO should be placed in the TOPMOST layer even above the connections
            foregroundLayer.add(new AttachedLayer(this, PositionConstants.CENTER, 
                    calloutFigure, PositionConstants.SOUTH_WEST));
        }

        super.addNotify();
    }

    public void setName(String name) {
        if (nameFigure == null) return;
        nameFigure.setVisible(name != null && !"".equals(name));
        nameFigure.setText(name);
    }
    
    public void setRange(int radius, Color fillColor, Color borderColor, int borderWidth) {
        if (rangeFigure == null) return;
        if(radius > 0) {
            int rad = radius;
            rangeFigure.setVisible(true);
            rangeFigure.setSize(rad*2, rad*2);
        }
        else rangeFigure.setVisible(false);
        rangeFigure.setFill(fillColor != null);
        rangeFigure.setBackgroundColor(fillColor);
        rangeFigure.setOutline(borderColor != null &&  borderWidth > 0);
        rangeFigure.setForegroundColor(borderColor);
        rangeFigure.setLineWidth(borderWidth);
    }
    
    public void setTooltipText(String tttext) {
        if(tttext == null || "".equals(tttext)) {
            setToolTip(null);
        } else {
            setToolTip(tooltipFigure);
            tooltipFigure.setText(tttext);
        }
    }
    
    public void setQueueText(String qtext) {
        if (queueFigure == null) return;
        queueFigure.setVisible(qtext != null && !"".equals(qtext));
        queueFigure.setText(qtext);
    }
    
    public void setInfoText(String text, String alignment, Color color) {
        if (textFigure == null || textAttachLayer == null) return;
        textFigure.setVisible(text != null && !"".equals(text));
        textFigure.setText(text);
        textFigure.setForegroundColor(color);
        // set alignment
        if(alignment == null) return;
        if("t".equals(alignment.toLowerCase())) {
            textFigure.setTextAlignment(PositionConstants.CENTER);
            textAttachLayer.setRefPoints(PositionConstants.NORTH, PositionConstants.SOUTH);
            textAttachLayer.setDeltaXY(0, -1);
        } else if ("l".equals(alignment.toLowerCase())) {
            textFigure.setTextAlignment(PositionConstants.RIGHT);
            textAttachLayer.setRefPoints(PositionConstants.NORTH_WEST, PositionConstants.NORTH_EAST);
            textAttachLayer.setDeltaXY(-2, 0);
        } else if ("r".equals(alignment.toLowerCase())) {
            textFigure.setTextAlignment(PositionConstants.LEFT);
            textAttachLayer.setRefPoints(PositionConstants.NORTH_EAST, PositionConstants.NORTH_WEST);
            textAttachLayer.setDeltaXY(2, 0);
        }
      if(textFigure.getParent() != null) textFigure.getParent().revalidate();
    }
    
    public void addCallout(String text) {
        calloutFigure.addCallout(new Label(text));
    }
    
    public void clearCallout() {
        calloutFigure.clearCallout();
    }
    
    public void setShape(Image img, 
            String shape, int shapeWidth, int shapeHeight, 
            Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {
        // handle defaults. if no size is given and no icon is present use a default icon
        if(shapeWidth <= 0 && shapeHeight <= 0 && img == null)
            img = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);
        // if image changed update it
        if (img != imageFigure.getImage()) {
            imageFigure.setImage(img);
            imageFigure.setVisible(img != null);
            Dimension imgSize = (img == null) ? 
                    new Dimension(0,0) : new Dimension(img.getImageData().width, img.getImageData().height);
            // we set the image minimum size, so it will never be clipped        
            imageFigure.setPreferredSize(imgSize);
        }
        setPreferredSize(imageFigure.getPreferredSize());
        
        // creating the shape
        currShape = null;
        rectShapeFigure.setVisible(false);
        rrectShapeFigure.setVisible(false);
        ovalShapeFigure.setVisible(false);
        polygonShapeFigure.setVisible(false);
        if (shape == null) return;

        if ("rect".equals(shape.toLowerCase())) {
            currShape = rectShapeFigure;
        } else if ("rrect".equals(shape.toLowerCase())) {
            currShape = rrectShapeFigure;
        } else if ("oval".equals(shape.toLowerCase())) {
            currShape = ovalShapeFigure;
        } else if ("rect2".equals(shape.toLowerCase())) {
            currShape = polygonShapeFigure;
            polygonShapeFigure.setGeometry(4, 0);
        } else if ("tri".equals(shape.toLowerCase())) {
            currShape = polygonShapeFigure;
            polygonShapeFigure.setGeometry(3, 0);
        } else if ("tri2".equals(shape.toLowerCase())) {
            currShape = polygonShapeFigure;
            polygonShapeFigure.setGeometry(3, 180);
        } else if ("hex".equals(shape.toLowerCase())) {
            currShape = polygonShapeFigure;
            polygonShapeFigure.setGeometry(6, 30);
        } else if ("hex2".equals(shape.toLowerCase())) {
            currShape = polygonShapeFigure;
            polygonShapeFigure.setGeometry(6, 0);
        } else
            return;

        currShape.setVisible(true);
        currShape.setOpaque(false);
        currShape.setFill(shapeFillColor != null);
        currShape.setBackgroundColor(shapeFillColor);
        currShape.setOutline(shapeBorderColor != null && shapeBorderWidth > 0);
        currShape.setForegroundColor(shapeBorderColor);
        currShape.setLineWidth(shapeBorderWidth);
        
        // if both coordinates < 0 then the icon should be used as a size reference
        Dimension shapeSize = new Dimension(shapeWidth, shapeHeight);
        if (shapeSize.width < 0 && shapeSize.height < 0) {
            shapeSize.setSize(imageFigure.getPreferredSize());
        }
        // set the requested size of the object
        currShape.setSize(shapeSize);
        // get back the real dimension (may differ from the width,height if one of them was < 0
        // in this case that one is computed in a way that the figure's aspect ratio is unchanged
        Dimension actualSize = currShape.getSize();
        // if only one of them is less than 0 then take the other dimension as reference (rectangular bounding)
        if (actualSize.width < 0) actualSize.width = actualSize.height;
        if (actualSize.height < 0) actualSize.height = actualSize.width;
        // set the preferred size of the parent, so the child's 
        currShape.setPreferredSize(actualSize);
        
        // ensure  that the size is not smaller than the visible image's preferred size
        if(imageFigure.isVisible())
            actualSize.union(imageFigure.getPreferredSize());
        setPreferredSize(actualSize);
    }
    
    public void setImageDecoration(Image img) {
        if(img != decoratorImageFigure.getImage())
            decoratorImageFigure.setImage(img);
        
        decoratorImageFigure.setVisible(img != null);
    }

    public Rectangle getHandleBounds() {
        return getBounds();
    }

}
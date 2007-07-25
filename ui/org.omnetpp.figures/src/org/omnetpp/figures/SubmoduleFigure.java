package org.omnetpp.figures;


import org.eclipse.draw2d.*;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;

import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.ILayerSupport.LayerID;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.figures.misc.IDirectEditSupport;
import org.omnetpp.figures.misc.LabelCellEditorLocator;

public class SubmoduleFigure extends ModuleFigure
                implements HandleBounds, IDirectEditSupport {

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
	private AttachedLayer rangeAttachLayer;
    protected Label textFigure = new Label();
    protected Label queueFigure = new Label();
    protected String queueName = "";
    protected boolean shapeVisible;
    protected TooltipFigure tooltipFigure;
    protected Shape rangeFigure = new RangeFigure();
    protected float scale = 1.0f;

    public SubmoduleFigure() {
        setLayoutManager(new StackLayout());
        // set line antialiasing on for all drawing n submodules
        rectShapeFigure.setVisible(false);
        rrectShapeFigure.setVisible(false);
        rrectShapeFigure.setCornerDimensions(new Dimension(30, 30));
        ovalShapeFigure.setVisible(false);
        polygonShapeFigure.setVisible(false);
        rangeFigure.setOpaque(false);
        shapeVisible = false;
    }

    @Override
    public void paint(Graphics graphics) {
        graphics.pushState();
        // set antialiasing on content and child/derived figures
        if(NedFileFigure.antialias != SWT.DEFAULT)
            graphics.setAntialias(NedFileFigure.antialias);
        super.paint(graphics);
        graphics.popState();
    }

    /**
     * Returns the requested layer from the first ancestor that supports multiple layers for decorations
     * and contains the a layer with the given id
     * @param id Layer id
     * @return The layer with the given id from the first ancestor that implements the ILayerSupport IF
     */
    protected Layer getAncestorLayer(LayerID id) {
        IFigure figureIter = getParent();
        // look for a parent who is an instance of ILayerSupport and get the layer from it
        while (figureIter != null) {
            if(figureIter instanceof ILayerSupport) {
                Layer layer = ((ILayerSupport)figureIter).getLayer(id);
                if (layer != null)
                    return layer;
            }
            figureIter = figureIter.getParent();
        }
        return null;
    }

    @Override
    public void addNotify() {
        // functions here need to access the parent or ancestor figures, so these setup
        // procedures cannot be done in the constructor

        // look for decorator layers among the ancestor (compound module figure)
        foregroundLayer = getAncestorLayer(ILayerSupport.LayerID.FRONT_DECORATION);
        backgroundLayer = getAncestorLayer(ILayerSupport.LayerID.BACKGROUND_DECORATION);

        // add main figures
        // TODO figures should be added and created only ON DEMAND!!!
        add(rectShapeFigure);
        add(rrectShapeFigure);
        add(ovalShapeFigure);
        add(polygonShapeFigure);
        add(imageFigure);

        // setup decorations belonging to the background decoration layer
        if(backgroundLayer != null) {
            backgroundLayer.add(rangeAttachLayer = new AttachedLayer(this, PositionConstants.CENTER,
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
        }

        super.addNotify();
    }

    public void setName(String name) {
        if (nameFigure == null) return;
        nameFigure.setVisible(name != null && !"".equals(name));
        nameFigure.setText(name);
        figureName = name;
        invalidate();
    }

    protected void setRange(int radius, Color fillColor, Color borderColor, int borderWidth) {
        if (rangeFigure == null) return;
        if(radius > 0) {
            int rad = radius;
            rangeFigure.setVisible(true);
            rangeFigure.setSize(rad*2, rad*2);
        }
        else {
        	rangeFigure.setVisible(false);
            rangeFigure.setSize(0, 0);
        }
        rangeFigure.setFill(fillColor != null);
        rangeFigure.setBackgroundColor(fillColor);
        rangeFigure.setOutline(borderColor != null &&  borderWidth > 0);
        rangeFigure.setForegroundColor(borderColor);
        rangeFigure.setLineWidth(borderWidth);

        if (rangeAttachLayer != null)
        	rangeAttachLayer.revalidate();

        invalidate();
    }

    public void setTooltipText(String tttext) {
        if(tttext == null || "".equals(tttext)) {
            setToolTip(null);
            tooltipFigure = null;
        } else {
            tooltipFigure = new TooltipFigure();
            setToolTip(tooltipFigure);
            tooltipFigure.setText(tttext);
            invalidate();
        }
    }

    protected void setQueueText(String qtext) {
        if (queueFigure == null) return;
        queueFigure.setVisible(qtext != null && !"".equals(qtext));
        queueFigure.setText(qtext);
        invalidate();
    }

    protected void setInfoText(String text, String alignment, Color color) {
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
      invalidate();
    }

    protected void setShape(Image image,
            String shape, int shapeWidth, int shapeHeight,
            Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {

        Image img = image;
        // handle defaults. if no size is given and no icon is present use a default icon
        if(StringUtils.isEmpty(shape) && img == null && shapeWidth == -1 && shapeHeight == -1)
            img = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);
        // if image changed update it
        if (img != imageFigure.getImage()) {
            imageFigure.setImage(img);
            imageFigure.setVisible(img != null);
            Dimension imgSize = img == null ?
                    new Dimension(0,0) : new Dimension(img.getImageData().width, img.getImageData().height);
            // we set the image minimum size, so it will never be clipped
            imageFigure.setPreferredSize(imgSize);
        }
        setPreferredSize(imageFigure.getPreferredSize());

        // if only size is specified the default shape is rectangle
        if (StringUtils.isEmpty(shape) && (shapeHeight>0 || shapeWidth>0))
            shape = "rect";
        // if nor image nor size is specified, the default size is 40x24
        if (img == null && shapeWidth == -1 && shapeHeight == -1) {
            shapeWidth = 40;
            shapeHeight = 24;
        }


        // creating the shape
        currShape = null;
        rectShapeFigure.setVisible(false);
        rrectShapeFigure.setVisible(false);
        ovalShapeFigure.setVisible(false);
        polygonShapeFigure.setVisible(false);
        shapeVisible = false;
        if (shape == null || "".equals(shape) || shapeHeight==-1 && shapeWidth==-1)
            return;

        if ("rrect".equals(shape.toLowerCase())) {
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
            currShape = rectShapeFigure;

        shapeVisible = true;
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
        invalidate();
    }

    /**
     * Sets the external image decoration (for pins)
     * @param img
     */
    public void setImageDecoration(Image img) {
        if(img != decoratorImageFigure.getImage())
            decoratorImageFigure.setImage(img);

        decoratorImageFigure.setVisible(img != null);
        invalidate();
    }

    public Rectangle getHandleBounds() {
        return getBounds();
    }

    /**
     * Sets the layout constraint for this submodule figure
     * @param constr
     */
    public void setConstraint(SubmoduleConstraint constr) {
        getParent().setConstraint(this, constr);
    }

    /**
     * @return The constraint object belonging to this figure
     */
    public SubmoduleConstraint getConstraint() {
        return (SubmoduleConstraint)getParent().getLayoutManager().getConstraint(this);
    }

	/**
	 * Adjusts the image properties using a displayString object (except the location and size)
	 * @param dps
	 */
	@Override
    public void setDisplayString(IDisplayString dps) {
		lastDisplayString = dps;
        // range support
        setRange(
        		dps.getRange(getScale()),
        		ColorFactory.asColor(dps.getAsStringDef(IDisplayString.Prop.RANGEFILLCOL)),
        		ColorFactory.asColor(dps.getAsStringDef(IDisplayString.Prop.RANGEBORDERCOL)),
        		dps.getAsIntDef(IDisplayString.Prop.RANGEBORDERWIDTH, -1));
        // tooltip support
        setTooltipText(dps.getAsStringDef(IDisplayString.Prop.TOOLTIP));

        // queue length support
        setQueueText(dps.getAsStringDef(IDisplayString.Prop.QUEUE));

        // additional text support
        setInfoText(dps.getAsStringDef(IDisplayString.Prop.TEXT),
        		dps.getAsStringDef(IDisplayString.Prop.TEXTPOS),
        		ColorFactory.asColor(dps.getAsStringDef(IDisplayString.Prop.TEXTCOLOR)));

        // shape support
        String imgSize = dps.getAsStringDef(IDisplayString.Prop.IMAGESIZE);
        Image img = ImageFactory.getImage(
        		dps.getAsStringDef(IDisplayString.Prop.IMAGE),
        		imgSize,
        		ColorFactory.asRGB(dps.getAsStringDef(IDisplayString.Prop.IMAGECOLOR)),
        		dps.getAsIntDef(IDisplayString.Prop.IMAGECOLORPCT,0));

        // set the figure properties
        setShape(img,
        		dps.getAsStringDef(IDisplayString.Prop.SHAPE),
        		dps.getSize(getScale()).width,
        		dps.getSize(getScale()).height,
        		ColorFactory.asColor(dps.getAsStringDef(IDisplayString.Prop.FILLCOL)),
        		ColorFactory.asColor(dps.getAsStringDef(IDisplayString.Prop.BORDERCOL)),
        		dps.getAsIntDef(IDisplayString.Prop.BORDERWIDTH, -1));

        // set the decoration image properties
        setImageDecoration(
        		        ImageFactory.getImage(
        				dps.getAsStringDef(IDisplayString.Prop.OVIMAGE),
        				null,
        				ColorFactory.asRGB(dps.getAsStringDef(IDisplayString.Prop.OVIMAGECOLOR)),
        				dps.getAsIntDef(IDisplayString.Prop.OVIMAGECOLORPCT,0)));

        invalidate();
	}


    /**
     * @return The current scaling factor for the submodule
     */
    public float getScale() {
        return scale;
    }

    /**
     * Sets the scale factor
     * @param scale
     */
    public void setScale(float scale) {
        this.scale = scale;
        invalidate();
    }

    public CellEditorLocator getDirectEditCellEditorLocator() {
        // create a center aligned label locator
        return new LabelCellEditorLocator(nameFigure, true);
    }

    public String getDirectEditText() {
        return nameFigure.getText();
    }

    public void setDirectEditTextVisible(boolean visible) {
        nameFigure.setVisible(visible);
    }

    /* (non-Javadoc)
     * @see org.eclipse.draw2d.Figure#containsPoint(int, int)
     * override it to include also the nameFigure, so clicking on submodule name would
     * be counted also as a selection event.
     */
    @Override
    public boolean containsPoint(int x, int y) {
        // if the name label contains this point we consider it as our part and report true
        if (nameFigure.containsPoint(x, y))
            return true;
        // otherwise use the default implementation
        return super.containsPoint(x, y);
    }

    /**
     * @return Whether the figure displays a shape
     */
    public boolean isShapeVisible() {
        return shapeVisible;
    }

}
package org.omnetpp.figures;


import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.*;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.draw2d.text.FlowPage;
import org.eclipse.draw2d.text.TextFlow;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;

import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.misc.AttachedLayer;
import org.omnetpp.figures.misc.ILayerSupport;
import org.omnetpp.figures.misc.ILayerSupport.LayerID;

/**
 * Figure representing a submodule inside a compound module figure. Contains several figures attached
 * to the main figure. They are placed on a different foreground and background layer, so they are
 * not included when the boundary calculation is done. getPreferredLocation can be
 * queried after setDisplaystring is called so an appropriate constraint can be created for a layouter.
 *
 * @author rhornig
 */
// FIXME change this so that figures are created on demand.
// TODO maybe we should use locators to place the different figures relative to the main figure
// instead of using attachedLayers
public class SubmoduleFigure extends NedFigure implements HandleBounds {
	// parent figures for attached figures
    protected Layer foregroundLayer;
    protected Layer backgroundLayer;

    // state info, from the display string. Note: much of the state is stored inside sub-figures
    private Point preferredLocation;
    protected boolean shapeVisible;
    protected float scale = 1.0f;
    protected String queueName = "";

    // figures which are part of the submodule figure. NOTE: all of them are created, the
    // unused ones being set to invisible. This has to be optimized in the future.
    protected Shape shapeFigure;
    protected RectangleFigure rectShapeFigure = new RectangleFigure();
    protected Ellipse ovalShapeFigure = new Ellipse();
    protected RoundedRectangle rrectShapeFigure = new RoundedRectangle();
    protected RegularPolygon polygonShapeFigure = new RegularPolygon(6,0);

    protected ImageFigure imageFigure = new ImageFigure();
    protected ImageFigure decoratorImageFigure = new ImageFigure();
    protected ImageFigure pinFigure = new ImageFigure(ImageFactory.getImage(ImageFactory.DEFAULT_PIN));
    protected AttachedLayer textAttachLayer;
	private AttachedLayer rangeAttachLayer;
    protected TextFlow textFigure = new TextFlow();
    protected FlowPage textFlowPage = new FlowPage();
    protected Label queueFigure = new Label();
    protected Shape rangeFigure = new RangeFigure();


    public SubmoduleFigure() {
        setLayoutManager(new StackLayout());
        rectShapeFigure.setVisible(false);
        rrectShapeFigure.setVisible(false);
        rrectShapeFigure.setCornerDimensions(new Dimension(30, 30));
        ovalShapeFigure.setVisible(false);
        polygonShapeFigure.setVisible(false);
        rangeFigure.setOpaque(false);
        shapeVisible = false;
        textFlowPage.add(textFigure);
        // use center alignment under the icon for the name
        nameFigure.setTextAlignment(PositionConstants.CENTER);
        nameFigure.setTextPlacement(PositionConstants.SOUTH);
    }

    @Override
    public void paint(Graphics graphics) {
        graphics.pushState();
        // set antialiasing on content and child/derived figures
        if (NedFileFigure.antialias != SWT.DEFAULT)
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
            if (figureIter instanceof ILayerSupport) {
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
        if (backgroundLayer != null) {
            backgroundLayer.add(rangeAttachLayer = new AttachedLayer(this, PositionConstants.CENTER,
                                                     rangeFigure, PositionConstants.CENTER));
        }

        if (foregroundLayer != null) {
            // image decoration
            foregroundLayer.add(new AttachedLayer(this, new PrecisionPoint(1.0, 0.0),
                    decoratorImageFigure, new PrecisionPoint(0.75, 0.25), null));
            // problem marker image
            foregroundLayer.add(new AttachedLayer(this, new PrecisionPoint(0.0, 0.0),
            		problemMarkerFigure, new PrecisionPoint(0.35, 0.35), null));
            // "pin" image
            foregroundLayer.add(new AttachedLayer(this, new PrecisionPoint(1.0, 0.0),
            		pinFigure, new PrecisionPoint(0.5, 0.5), null));
            // name decoration
            foregroundLayer.add(new AttachedLayer(this, PositionConstants.SOUTH,
                                        nameFigure, PositionConstants.NORTH));
            // text comment decoration
            foregroundLayer.add(
                    textAttachLayer = new AttachedLayer(this, PositionConstants.NORTH,
                                            textFlowPage, PositionConstants.SOUTH, 0, -1));
            // queue description
            foregroundLayer.add(new AttachedLayer(this, PositionConstants.SOUTH_EAST,
                    queueFigure, PositionConstants.SOUTH_WEST, 2, 0));
        }

        super.addNotify();
    }

    protected void setRange(int radius, Color fillColor, Color borderColor, int borderWidth) {
        if (radius > 0) {
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

    protected void setQueueText(String qtext) {
        queueFigure.setVisible(qtext != null && !"".equals(qtext));
        queueFigure.setText(qtext);
        invalidate();
    }

    protected void setInfoText(String text, String alignment, Color color) {
        textFigure.setVisible(text != null && !"".equals(text));
        textFigure.setText(text);
        textFigure.setForegroundColor(color);
        // set alignment
        if (alignment == null) return;
        if ("t".equals(alignment.toLowerCase())) {
            textFlowPage.setHorizontalAligment(PositionConstants.CENTER);
            textAttachLayer.setRefPoints(PositionConstants.NORTH, PositionConstants.SOUTH);
            textAttachLayer.setDeltaXY(0, -1);
        }
        else if ("l".equals(alignment.toLowerCase())) {
            textFlowPage.setHorizontalAligment(PositionConstants.RIGHT);
            textAttachLayer.setRefPoints(PositionConstants.NORTH_WEST, PositionConstants.NORTH_EAST);
            textAttachLayer.setDeltaXY(-2, 0);
        }
        else if ("r".equals(alignment.toLowerCase())) {
            textFlowPage.setHorizontalAligment(PositionConstants.LEFT);
            textAttachLayer.setRefPoints(PositionConstants.NORTH_EAST, PositionConstants.NORTH_WEST);
            textAttachLayer.setDeltaXY(2, 0);
        }
        if (textFigure.getParent() != null)
        	textFigure.getParent().revalidate();
        invalidate();
    }

    protected void setShape(Image image, String shapeName, int shapeWidth, int shapeHeight,
            Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {

        // handle defaults. if no size is given and no icon is present, use a default icon
        if (StringUtils.isEmpty(shapeName) && image == null)
            image = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);

        // if image changed, update the figure
        if (image != imageFigure.getImage()) {
            imageFigure.setImage(image);
            imageFigure.setVisible(image != null);
        }
        Dimension imageSize = image == null ?
                new Dimension(0,0) : new Dimension(image.getBounds().width, image.getBounds().height);

        // we set the image minimum size, so it will never be clipped
        imageFigure.setPreferredSize(imageSize);

        setPreferredSize(imageFigure.getPreferredSize());

        // creating the shape
        shapeFigure = null;
        rectShapeFigure.setVisible(false);
        rrectShapeFigure.setVisible(false);
        ovalShapeFigure.setVisible(false);
        polygonShapeFigure.setVisible(false);
        shapeVisible = false;

        // if there is no shape, we are done
        if (StringUtils.isEmpty(shapeName))
            return;

        if ("rrect".equals(shapeName.toLowerCase())) {
            shapeFigure = rrectShapeFigure;
        }
        else if ("oval".equals(shapeName.toLowerCase())) {
            shapeFigure = ovalShapeFigure;
        }
        else if ("rect2".equals(shapeName.toLowerCase())) {
            shapeFigure = polygonShapeFigure;
            polygonShapeFigure.setGeometry(4, 0);
        }
        else if ("tri".equals(shapeName.toLowerCase())) {
            shapeFigure = polygonShapeFigure;
            polygonShapeFigure.setGeometry(3, 0);
        }
        else if ("tri2".equals(shapeName.toLowerCase())) {
            shapeFigure = polygonShapeFigure;
            polygonShapeFigure.setGeometry(3, 180);
        }
        else if ("hex".equals(shapeName.toLowerCase())) {
            shapeFigure = polygonShapeFigure;
            polygonShapeFigure.setGeometry(6, 30);
        }
        else if ("hex2".equals(shapeName.toLowerCase())) {
            shapeFigure = polygonShapeFigure;
            polygonShapeFigure.setGeometry(6, 0);
        }
        else {
            shapeFigure = rectShapeFigure;
        }

        shapeVisible = true;
        shapeFigure.setVisible(true);
        shapeFigure.setOpaque(false);
        shapeFigure.setFill(shapeFillColor != null);
        shapeFigure.setBackgroundColor(shapeFillColor);
        shapeFigure.setOutline(shapeBorderColor != null && shapeBorderWidth > 0);
        shapeFigure.setForegroundColor(shapeBorderColor);
        shapeFigure.setLineWidth(shapeBorderWidth);

        // set the requested size of the object
        Assert.isTrue(shapeHeight != -1 || shapeWidth != -1);
        Dimension shapeSize = new Dimension(shapeWidth, shapeHeight);
        shapeFigure.setSize(shapeSize);

        // get back the real dimension (may differ from the width,height if one of them was < 0
        // in this case that one is computed in a way that the figure's aspect ratio is unchanged
        Dimension actualSize = shapeFigure.getSize();

        // if only one of them is less than 0 then take the other dimension as reference (rectangular bounding)
        if (actualSize.width < 0) actualSize.width = actualSize.height;
        if (actualSize.height < 0) actualSize.height = actualSize.width;

        // ensure  that the size is not smaller than the visible image's preferred size
        if (imageFigure.isVisible())
            actualSize.union(imageFigure.getPreferredSize());

        setPreferredSize(actualSize);
        invalidate();
    }

    /**
     * Sets the external image decoration ("i2" tag)
     */
    protected void setDecorationImage(Image img) {
        if (img != decoratorImageFigure.getImage())
            decoratorImageFigure.setImage(img);

        decoratorImageFigure.setVisible(img != null);
        invalidate();
    }

    public Rectangle getHandleBounds() {
        return getBounds();
    }

    /**
     * The bounds of the name label figure
     */
    public Rectangle getLabelBounds() {
        return nameFigure.getBounds();
    }
	/**
	 * Adjusts the image properties using a displayString object (except the location and size)
	 */
	@Override
    public void setDisplayString(IDisplayString dps) {
        // get the position from the display string. When a coordinate is missing
		// in the display string, use Integer.MIN_VALUE. (means: it's UNPINNED)
		preferredLocation = dps.getLocation(scale);
        if (preferredLocation == null)
        	preferredLocation = new Point(Integer.MIN_VALUE, Integer.MIN_VALUE);

		// range support
        setRange(
        		dps.getRange(getScale()),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.RANGEFILLCOL)),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.RANGEBORDERCOL)),
        		dps.getAsInt(IDisplayString.Prop.RANGEBORDERWIDTH, -1));
        // tooltip support
        setTooltipText(dps.getAsString(IDisplayString.Prop.TOOLTIP));

        // queue length support
        setQueueText(dps.getAsString(IDisplayString.Prop.QUEUE));

        // additional text support
        setInfoText(dps.getAsString(IDisplayString.Prop.TEXT),
        		dps.getAsString(IDisplayString.Prop.TEXTPOS),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.TEXTCOLOR)));

        // shape support
        String imgSize = dps.getAsString(IDisplayString.Prop.IMAGESIZE);
        Image img = ImageFactory.getImage(
        		dps.getAsString(IDisplayString.Prop.IMAGE),
        		imgSize,
        		ColorFactory.asRGB(dps.getAsString(IDisplayString.Prop.IMAGECOLOR)),
        		dps.getAsInt(IDisplayString.Prop.IMAGECOLORPERCENTAGE,0));

        // rectangle ("b" tag)
        Dimension size = dps.getSize(scale);  // falls back to size in EMPTY_DEFAULTS
        boolean widthExist = dps.containsProperty(IDisplayString.Prop.WIDTH);
        boolean heightExist = dps.containsProperty(IDisplayString.Prop.HEIGHT);

        // if both are missing, use values from EMPTY_DEFAULTS, otherwise substitute
        // -1 for missing coordinate
        if (!widthExist && heightExist)
            size.width = -1;
        if (widthExist && !heightExist)
            size.height = -1;

        String shape = dps.getAsString(IDisplayString.Prop.SHAPE);
        if (!dps.containsTag(IDisplayString.Tag.b))
        	shape = "";
        setShape(img, shape,
                size.width,
                size.height,
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.FILLCOL)),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.BORDERCOL)),
        		dps.getAsInt(IDisplayString.Prop.BORDERWIDTH, -1));

        // set the decoration image properties
        setDecorationImage(
        		        ImageFactory.getImage(
        				dps.getAsString(IDisplayString.Prop.OVIMAGE),
        				null,
        				ColorFactory.asRGB(dps.getAsString(IDisplayString.Prop.OVIMAGECOLOR)),
        				dps.getAsInt(IDisplayString.Prop.OVIMAGECOLORPCT,0)));

        invalidate();
	}

    /**
     * The current scaling factor for the submodule.
     */
    public float getScale() {
        return scale;
    }

    /**
     * Sets the scale factor used for drawing.
     */
    public void setScale(float scale) {
        this.scale = scale;
        invalidate();
    }

	/**
	 * Returns the position where the submodule wants to be (based on
	 * display string contents). If the display string does not specify
	 * a coordinate, Integer.MIN_VALUE is returned there. Valid only
	 * after a setDisplayString() call.
	 */
    public Point getPreferredLocation() {
		return preferredLocation;
	}


    /* (non-Javadoc)
     * @see org.eclipse.draw2d.Figure#containsPoint(int, int)
     * We override it to include also the nameFigure, so clicking on submodule name would
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
     * Whether the figure displays a shape
     */
    public boolean isShapeVisible() {
        return shapeVisible;
    }

    /**
     * Turns the "pin" icon (which shows whether the submodule has a
     * user-specified position) on/off
     */
    public void setPinDecoration(boolean enabled) {
        pinFigure.setVisible(enabled);
        invalidate(); //XXX needed?
    }
}
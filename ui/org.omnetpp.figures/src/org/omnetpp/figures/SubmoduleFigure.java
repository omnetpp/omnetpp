/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;


import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.TextUtilities;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.CompoundModuleFigure.SubmoduleLayer;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.figures.misc.AttachedLayer;

/**
 * Figure representing a submodule inside a compound module figure. Contains several figures attached
 * to the main figure. They are placed on a different foreground and background layer, so they are
 * not included when the boundary calculation is done. getPreferredLocation can be
 * queried after setDisplaystring is called so an appropriate constraint can be created for a layouter.
 *
 * @author andras
 */
//FIXME support multiple texts: t/t1/t2/t3/t4
public class SubmoduleFigure extends NedFigure {
    // supported shape types
	protected static final int SHAPE_NONE = 0;
	protected static final int SHAPE_OVAL = 1;
	protected static final int SHAPE_RECT = 2;
	protected static final int SHAPE_RECT2 = 3;
	protected static final int SHAPE_TRI = 4;
	protected static final int SHAPE_TRI2 = 5;
	protected static final int SHAPE_HEX = 6;
	protected static final int SHAPE_HEX2 = 7;

	protected static final int TEXTPOS_LEFT = 1;
	protected static final int TEXTPOS_RIGHT = 2;
	protected static final int TEXTPOS_TOP = 3;

    protected static final Image IMG_PIN = ImageFactory.getImage(ImageFactory.DEFAULT_PIN);
	
    // state info, from the display string
    protected int centerX, centerY;
    protected float scale = 1.0f;
    protected int shape;
    protected int shapeWidth;
    protected int shapeHeight;
    protected Color shapeFillColor; 
    protected Color shapeBorderColor;
    protected int shapeBorderWidth;
    protected Image image;
    protected Image decoratorImage;
    protected boolean pinVisible;
    protected String text;
    protected int textPos;
    protected Color textColor;
    protected String queueText;
    private AttachedLayer rangeAttachLayer;
    protected RangeFigure rangeFigure = new RangeFigure();  //FIXME make this on demand as well!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


    public SubmoduleFigure() {
        rangeAttachLayer = new AttachedLayer(this, PositionConstants.CENTER, rangeFigure, PositionConstants.CENTER);
    }

    @Override
    public void addNotify() {
        // functions here need to access the parent or ancestor figures, so these setup
        // procedures cannot be done in the constructor
    	Assert.isTrue(getParent() instanceof SubmoduleLayer, "SubmoduleFigure can be added directly only to a SubmoduleLayer (inside of a CompoundModuleFigure)");

        // look for decorator layers among the ancestor (compound module figure)
        Layer foregroundLayer = ((SubmoduleLayer)getParent()).getCompoundModuleFigure().getForegroundDecorationLayer();
        Layer backgroundLayer = ((SubmoduleLayer)getParent()).getCompoundModuleFigure().getBackgroundDecorationLayer();

        // setup decorations belonging to the background decoration layer
        backgroundLayer.add(rangeAttachLayer);

        // problem marker image
        foregroundLayer.add(new AttachedLayer(this, new PrecisionPoint(0.0, 0.0),
        		problemMarkerFigure, new PrecisionPoint(0.35, 0.35), null));

        super.addNotify();
    }

    protected void setRange(int radius, Color fillColor, Color borderColor, int borderWidth) {
        if (radius > 0) {
            rangeFigure.setVisible(true);
            rangeFigure.setSize(radius*2, radius*2);
        }
        else {
        	rangeFigure.setVisible(false);
            rangeFigure.setSize(0, 0); //XXX minek?
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

    public void setQueueText(String qtext) {
    	queueText = qtext;
        invalidate();
    }

    protected void setInfoText(String text, String pos, Color color) {
    	this.text = text;
    	this.textPos = pos.equalsIgnoreCase("l") ? TEXTPOS_LEFT : pos.equalsIgnoreCase("r") ? TEXTPOS_RIGHT : TEXTPOS_TOP;
    	this.textColor = color;
        invalidate();
    }

    protected void setShape(Image image, String shapeName, int shapeWidth, int shapeHeight,
            Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {

        if (StringUtils.isEmpty(shapeName))
        	shape = SHAPE_NONE;
        else if (shapeName.equalsIgnoreCase("oval"))
        	shape = SHAPE_OVAL;
        else if (shapeName.equalsIgnoreCase("rect"))
            shape = SHAPE_RECT;
        else if (shapeName.equalsIgnoreCase("rrect"))
            shape = SHAPE_RECT2;
        else if (shapeName.equalsIgnoreCase("tri"))
            shape = SHAPE_TRI;
        else if (shapeName.equalsIgnoreCase("tri2"))
            shape = SHAPE_TRI2;
        else if (shapeName.equalsIgnoreCase("hex"))
            shape = SHAPE_HEX;
        else if (shapeName.equalsIgnoreCase("hex2"))
            shape = SHAPE_HEX2;
        else
        	shape = SHAPE_NONE;

        Assert.isTrue(shapeHeight != -1 || shapeWidth != -1);
        this.shapeWidth = shapeWidth;
        this.shapeHeight = shapeHeight;

        this.image = image;
        
        if (image == null && shape == SHAPE_NONE)
            image = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);

        invalidate();
    }

    protected void calculateBounds() {
        Rectangle bounds = getShapeBounds().union(getNameBounds()); //FIXME also the text, decoration image etc etc!!!
        super.setBounds(bounds);
    }
    
	protected Rectangle getShapeBounds() {
		int width = shapeWidth>0 ? shapeWidth : shapeHeight;
		int height = shapeHeight>0 ? shapeHeight : shapeWidth;
        if (image != null) {
            width = Math.max(width, image.getBounds().width);
            height = Math.max(height, image.getBounds().height);
        }
		return new Rectangle(centerX-width/2, centerY-height/2, width, height);
	}

	/**
	 * Sets the external image decoration ("i2" tag)
	 */
	protected void setDecorationImage(Image img) {
		decoratorImage = img;
		invalidate();
	}

    @Override
    public void setBounds(Rectangle rect) {
    	throw new UnsupportedOperationException(); // call setCenterLocation/setShape instead
    }
    
    @Override
    public void setLocation(Point p) {
    	throw new UnsupportedOperationException(); // call setCenterLocation/setShape instead
    }
    
    @Override
    public void setSize(int w, int h) {
    	throw new UnsupportedOperationException(); // call setCenterLocation/setShape instead
    }

    @Override
    public void setPreferredSize(Dimension size) {
    	throw new UnsupportedOperationException(); // call setCenterLocation/setShape instead
    }
    
    @Override
    public Dimension getPreferredSize(int hint, int hint2) {
    	throw new UnsupportedOperationException(); // call setCenterLocation/setShape instead
    }

    public Rectangle getHandleBounds() {
        return getShapeBounds();
    }

    /**
     * The bounds of the name label
     */
    public Rectangle getNameBounds() {
        Rectangle shapeBounds = getShapeBounds();
        Dimension textSize = TextUtilities.INSTANCE.getTextExtents(text, getFont());
        return new Rectangle(centerX-textSize.width/2, shapeBounds.bottom(), textSize.width, textSize.height);
    }

    /**
	 * Adjusts the image properties using a displayString object (except the location and size)
	 */
	@Override
    public void setDisplayString(IDisplayString displayString) {
		// range support
        setRange(
        		displayString.getRange(getScale()),
        		ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.RANGEFILLCOL)),
        		ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.RANGEBORDERCOL)),
        		displayString.getAsInt(IDisplayString.Prop.RANGEBORDERWIDTH, -1));
        
        // tooltip support
        setTooltipText(displayString.getAsString(IDisplayString.Prop.TOOLTIP));

        // additional text support
        setInfoText(displayString.getAsString(IDisplayString.Prop.TEXT),
        		displayString.getAsString(IDisplayString.Prop.TEXTPOS),
        		ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.TEXTCOLOR)));

        // shape support
        String imageSize = displayString.getAsString(IDisplayString.Prop.IMAGESIZE);
        Image img = ImageFactory.getImage(
        		displayString.getAsString(IDisplayString.Prop.IMAGE),
        		imageSize,
        		ColorFactory.asRGB(displayString.getAsString(IDisplayString.Prop.IMAGECOLOR)),
        		displayString.getAsInt(IDisplayString.Prop.IMAGECOLORPERCENTAGE,0));

        // rectangle ("b" tag)
        Dimension size = displayString.getSize(scale);  // falls back to size in EMPTY_DEFAULTS
        boolean widthExist = displayString.containsProperty(IDisplayString.Prop.WIDTH);
        boolean heightExist = displayString.containsProperty(IDisplayString.Prop.HEIGHT);

        // if both are missing, use values from EMPTY_DEFAULTS, otherwise substitute
        // -1 for missing coordinate
        if (!widthExist && heightExist)
            size.width = -1;
        if (widthExist && !heightExist)
            size.height = -1;

        String shape = displayString.getAsString(IDisplayString.Prop.SHAPE);
        if (!displayString.containsTag(IDisplayString.Tag.b))
        	shape = "";
        setShape(img, shape,
                size.width,
                size.height,
        		ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.FILLCOL)),
        		ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.BORDERCOL)),
        		displayString.getAsInt(IDisplayString.Prop.BORDERWIDTH, -1));

        // set the decoration image properties
        setDecorationImage(
        		        ImageFactory.getImage(
        				displayString.getAsString(IDisplayString.Prop.OVIMAGE),
        				null,
        				ColorFactory.asRGB(displayString.getAsString(IDisplayString.Prop.OVIMAGECOLOR)),
        				displayString.getAsInt(IDisplayString.Prop.OVIMAGECOLORPCT,0)));

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

    /* (non-Javadoc)
     * @see org.eclipse.draw2d.Figure#containsPoint(int, int)
     * We override it to include also the nameFigure, so clicking on submodule name would
     * be counted also as a selection event.
     */
    @Override
    public boolean containsPoint(int x, int y) {
    	return getShapeBounds().contains(x, y) || nameFigure.containsPoint(x, y);
    }

    /**
     * Whether the figure displays a shape
     */
    public boolean isShapeVisible() {
        return shape != SHAPE_NONE;
    }

    /**
     * Turns the "pin" icon (which shows whether the submodule has a
     * user-specified position) on/off
     */
    public void setPinDecoration(boolean enabled) {
    	pinVisible = enabled;
        invalidate();
    }

    public boolean isPinned() {
        return pinVisible;
    }
    
    @Override
    public void paint(Graphics graphics) {
    	super.paint(graphics);
    	
    	// draw shape
    	if (shape != SHAPE_NONE) {
    		graphics.pushState();
    		graphics.setForegroundColor(shapeBorderColor);
    		graphics.setBackgroundColor(shapeFillColor);
    		graphics.setLineWidth(shapeBorderWidth);
    		int left = centerX - shapeWidth/2;
    		int top = centerY - shapeHeight/2;
    		if (shape == SHAPE_OVAL) {
    			graphics.drawOval(left, top, left+shapeWidth, top+shapeHeight);
    		}
    		else if (shape == SHAPE_RECT) {
    			graphics.drawRectangle(left, top, left+shapeWidth, top+shapeHeight);
    		}
    		else {
    			Assert.isTrue(false, "NOT IMPLEMENTED YET"); //XXX
    		}
    		graphics.popState();
    	}
    	
    	// draw image
        if (image != null) {
        	org.eclipse.swt.graphics.Rectangle imageBounds = image.getBounds();
        	graphics.drawImage(image, centerX - imageBounds.width/2, centerY - imageBounds.height/2);
        }
        
        // draw text
        if (!StringUtils.isEmpty(text)) {
    		graphics.pushState();
    		int x, y;
    		Dimension textSize = TextUtilities.INSTANCE.getTextExtents(text, getFont());
    		Rectangle shapeBounds = getShapeBounds();
    		if (textPos == TEXTPOS_LEFT) {
    			x = shapeBounds.x - textSize.width; 
    			y = shapeBounds.y; 
    		}
    		if (textPos == TEXTPOS_RIGHT) {
    			x = shapeBounds.right(); 
    			y = shapeBounds.y; 
    		}
    		else {  // TEXTPOS_TOP
    			x = centerX - textSize.width/2; 
    			y = shapeBounds.x - textSize.height; 
    		}
    		graphics.drawText(text, x, y);
    		graphics.popState();
        }
        
        // draw decoration image
        if (decoratorImage != null) {
    		Rectangle shapeBounds = getShapeBounds();
    		org.eclipse.swt.graphics.Rectangle decoratorImageBounds = decoratorImage.getBounds();
			graphics.drawImage(decoratorImage, shapeBounds.x-decoratorImageBounds.width, shapeBounds.y - decoratorImageBounds.height/2);
        }
        	
        if (pinVisible) {
    		Rectangle shapeBounds = getShapeBounds();
    		org.eclipse.swt.graphics.Rectangle pinImageBounds = IMG_PIN.getBounds();
			graphics.drawImage(IMG_PIN, shapeBounds.x-pinImageBounds.width, shapeBounds.y - pinImageBounds.height/2);
        }
    	
    }
    
    /**
     * The constraint belonging to this figure. Used by the layout manager to arrange the
     * submodule figures.
     */
    public SubmoduleConstraint getSubmoduleConstraint() {
    	LayoutManager layoutManager = getParent().getLayoutManager();
		return layoutManager != null ? (SubmoduleConstraint)layoutManager.getConstraint(this) : null;
    }
    
    public void setSubmoduleConstraint(SubmoduleConstraint constraint) {
    	getParent().setConstraint(this, constraint);
    }
}
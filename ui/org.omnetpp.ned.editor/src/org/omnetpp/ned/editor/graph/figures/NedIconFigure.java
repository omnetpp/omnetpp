/*--------------------------------------------------------------*
  Copyright (C) 2010 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;


import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Figure representing the graphical part of a ModuleTypeFigure 
 *
 * @author andras
 */
public class NedIconFigure extends Figure {
    
    // we try to be this size (modulo decoration image)
	private int targetWidth = 40;
	private int targetHeight = 40;
	
    // supported shape types
	protected static final int SHAPE_NONE = 0;
	protected static final int SHAPE_OVAL = 1;
	protected static final int SHAPE_RECT = 2;

	// appearance
	protected int alpha = 255;
	protected int shape;
	protected int shapeWidth;
	protected int shapeHeight;
	protected Color shapeFillColor;
	protected Color shapeBorderColor;
	protected int shapeBorderWidth;
	protected Image image;
	protected Image decoratorImage;
    private int decoratorImageXOffset;
    private int decoratorImageYOffset;

	public NedIconFigure() {
	}

	public void setTargetSize(int w, int h) {
	    targetWidth = w;
	    targetHeight = h;
	    image = ImageUtils.fitImage(image, w, h);
	}
	
    public void setAlpha(int alpha) {
        this.alpha = alpha;
    }

    /**
     * Sets the image  ("i" tag or default icon)
     */
    public void setImage(Image image) {
        this.image = ImageUtils.fitImage(image, targetWidth, targetHeight);
    }

    /**
     * Sets the decoration image  ("i2" tag)
     */
    public void setDecorationImage(Image image) {
        decoratorImage = image;
        if (image == null) {
            decoratorImageYOffset = decoratorImageXOffset = 0;
        } else {
            org.eclipse.swt.graphics.Rectangle imageBounds = decoratorImage.getBounds();
            decoratorImageXOffset = imageBounds.width/4;
            decoratorImageYOffset = imageBounds.height/4;
        }
    }

    /**
     * Sets the shape ("b" tag)
     */
    public void setShape(String shapeName, int shapeWidth, int shapeHeight, Color shapeFillColor, 
            Color shapeBorderColor, int shapeBorderWidth) {
		Assert.isNotNull(shapeFillColor);
		Assert.isNotNull(shapeBorderColor);

		// rescale the shape to exactly fit into targetWidth/targetHeight, keeping aspect ratio 
		double hMult = targetWidth / (double)shapeWidth;
		double vMult = targetHeight / (double)shapeHeight;
		double mult = Math.min(hMult, vMult);
		shapeHeight = (int)(shapeHeight*mult);
		shapeWidth = (int)(shapeWidth*mult);
		if (shapeHeight==0) shapeHeight = 1;
		if (shapeWidth==0) shapeWidth = 1;
		
		if (StringUtils.isEmpty(shapeName))
			shape = SHAPE_NONE;
		else {
			shapeName = IDisplayString.Prop.SHAPE.getEnumSpec().getNameFor(shapeName);

			if (StringUtils.isEmpty(shapeName))
				shape = SHAPE_RECT; // unknown -> rectangle
			else if (shapeName.equalsIgnoreCase("oval"))
				shape = SHAPE_OVAL;
			else if (shapeName.equalsIgnoreCase("rectangle"))
				shape = SHAPE_RECT;
			else
				Assert.isTrue(false); // unhandled shape
		}

		this.shapeWidth = Math.max(shapeWidth, 0);
		this.shapeHeight = Math.max(shapeHeight, 0);
		this.shapeBorderColor = shapeBorderColor;
		this.shapeFillColor = shapeFillColor;
		this.shapeBorderWidth = shapeBorderWidth;
	}

    @Override
    public Dimension getPreferredSize(int wHint, int hHint) {
        return new Dimension(targetWidth + decoratorImageXOffset, targetHeight + decoratorImageYOffset);
    }
    
	@Override
	public void paint(Graphics graphics) {
	    graphics.setAlpha(alpha);
		super.paint(graphics);

		Point centerLoc = getBounds().getCenter();
		centerLoc.y += decoratorImageYOffset>0 ?decoratorImageYOffset-1 : 0; // the -1 is needed because of a rounding error causing clipping at the bottom
		
		// draw shape
		if (shape != SHAPE_NONE) {
			graphics.pushState();
			graphics.setForegroundColor(shapeBorderColor);
			graphics.setBackgroundColor(shapeFillColor);
			graphics.setLineWidth(shapeBorderWidth);
			int left = centerLoc.x - shapeWidth/2;
			int top = centerLoc.y - shapeHeight/2;
			if (shape == SHAPE_OVAL) {
				if (2*shapeBorderWidth > shapeWidth || 2*shapeBorderWidth > shapeHeight) {
					// special case: border is wider than the shape itself
					graphics.setBackgroundColor(shapeBorderColor);
					graphics.fillOval(left, top, shapeWidth, shapeHeight);
				}
				else {
					graphics.fillOval(left, top, shapeWidth, shapeHeight);
					if (shapeBorderWidth > 0) {
						graphics.drawOval(left+shapeBorderWidth/2,
								top+shapeBorderWidth/2,
								shapeWidth-Math.max(1, shapeBorderWidth),
								shapeHeight-Math.max(1, shapeBorderWidth));
					}
				}
			}
			else if (shape == SHAPE_RECT) {
				if (2*shapeBorderWidth > shapeWidth || 2*shapeBorderWidth > shapeHeight) {
					// special case: border is wider than the shape itself
					graphics.setBackgroundColor(shapeBorderColor);
					graphics.fillRectangle(left, top, shapeWidth, shapeHeight);
				}
				else {
					graphics.fillRectangle(left, top, shapeWidth, shapeHeight);
					if (shapeBorderWidth > 0) {
						graphics.drawRectangle(left+shapeBorderWidth/2,
								top+shapeBorderWidth/2,
								shapeWidth-Math.max(1, shapeBorderWidth),
								shapeHeight-Math.max(1, shapeBorderWidth));
					}
				}
			}
			else {
				Assert.isTrue(false, "NOT IMPLEMENTED YET");
			}
			graphics.popState();
		}

		// draw image
		if (image != null) {
			org.eclipse.swt.graphics.Rectangle imageBounds = image.getBounds();
			graphics.drawImage(image, centerLoc.x - imageBounds.width/2, centerLoc.y - imageBounds.height/2);
		}

		// draw decoration image
		if (decoratorImage != null) {
			graphics.drawImage(decoratorImage, targetWidth + decoratorImageXOffset - decoratorImage.getBounds().width, 0);
		}
	}
}

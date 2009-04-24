/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.figures;


import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.TextUtilities;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.CompoundModuleFigure.SubmoduleLayer;
import org.omnetpp.figures.anchors.IAnchorBounds;
import org.omnetpp.figures.layout.ISubmoduleConstraint;
import org.omnetpp.figures.misc.ISelectionHandleBounds;

/**
 * Figure representing a submodule inside a compound module figure.
 *
 * @author andras
 */
//FIXME support multiple texts: t/t1/t2/t3/t4
//FIXME alignment of multi-line text
public class SubmoduleFigure extends Figure implements ISubmoduleConstraint, IAnchorBounds, 
ISelectionHandleBounds, ITooltipTextProvider, IProblemDecorationSupport {
	// supported shape types
	protected static final int SHAPE_NONE = 0;
	protected static final int SHAPE_OVAL = 1;
	protected static final int SHAPE_RECT = 2;
	//	protected static final int SHAPE_RECT2 = 3;
	//	protected static final int SHAPE_TRI = 4;
	//	protected static final int SHAPE_TRI2 = 5;
	//	protected static final int SHAPE_HEX = 6;
	//	protected static final int SHAPE_HEX2 = 7;

	protected static final int TEXTPOS_LEFT = 1;
	protected static final int TEXTPOS_RIGHT = 2;
	protected static final int TEXTPOS_TOP = 3;

	protected static final Image IMG_PIN = ImageFactory.getImage(ImageFactory.DEFAULT_PIN);
	protected static final Image IMG_DEFAULT = ImageFactory.getImage(ImageFactory.DEFAULT);

	// input for the layouting
	protected float scale = 1.0f;
	protected Point baseLoc;
	protected Object vectorIdentifier;
	protected int vectorSize;
	protected int vectorIndex;
	protected VectorArrangement vectorArrangement;
	protected int vectorArrangementPar1, vectorArrangementPar2, vectorArrangementPar3;

	// result of layouting
	protected Point centerLoc;

	// appearance
	protected String nameText;
	protected String tooltipText;
	protected ITooltipTextProvider problemMarkerTextProvider;
	protected Image problemMarkerImage;
	protected int shape;
	protected int shapeWidth;
	protected int shapeHeight;
	protected Color shapeFillColor; 
	protected Color shapeBorderColor;
	protected int shapeBorderWidth;
	protected Image image;
	protected Image decoratorImage;
	protected boolean pinVisible;
	protected boolean nameVisible = true;
	protected String text;
	protected int textPos;
	protected Color textColor;
	protected String queueText;
	protected RangeFigure rangeFigure = null;
	private String oldDisplayString = null;

	public SubmoduleFigure() {
	}

	/**
	 * Adjust the properties using a display string object
	 */
	public void setDisplayString(float scale, IDisplayString displayString) {
		// optimization: do not change anything if the display string has not changed
		String newDisplayString = displayString.toString();
		if (this.scale == scale && newDisplayString.equals(oldDisplayString))
			return;

		Assert.isNotNull(getFont()); // font must be set on the figure explicitly, otherwise it'll recursively go up to get it from the canvas every time

		this.scale = scale;
		this.oldDisplayString = newDisplayString;
		
		Rectangle oldShapeBounds = getShapeBounds();  // to compare at the end
		
		// range support
		setRange(
				displayString.getRange(scale),
				ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.RANGEFILLCOL)),
				ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.RANGEBORDERCOL)),
				displayString.getAsInt(IDisplayString.Prop.RANGEBORDERWIDTH, -1));

		// tooltip support
		setTooltipText(displayString.getAsString(IDisplayString.Prop.TOOLTIP));

		// additional text support
		setInfoText(displayString.getAsString(IDisplayString.Prop.TEXT),
				displayString.getAsString(IDisplayString.Prop.TEXTPOS),
				ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.TEXTCOLOR), ColorFactory.RED));

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

		// if one of the dimensions is missing use the other dimension instead
		if (!widthExist && heightExist)
			size.width = size.height;
		else if (widthExist && !heightExist)
			size.height = size.width;

		String shape = displayString.getAsString(IDisplayString.Prop.SHAPE);
		if (!displayString.containsTag(IDisplayString.Tag.b))
			shape = null;
		setShape(img, shape,
				size.width,
				size.height,
				ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.FILLCOL), ColorFactory.RED),
				ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.BORDERCOL), ColorFactory.RED),
				displayString.getAsInt(IDisplayString.Prop.BORDERWIDTH, -1));

		// set the decoration image properties
		setDecorationImage(
				ImageFactory.getImage(
						displayString.getAsString(IDisplayString.Prop.OVIMAGE),
						null,
						ColorFactory.asRGB(displayString.getAsString(IDisplayString.Prop.OVIMAGECOLOR)),
						displayString.getAsInt(IDisplayString.Prop.OVIMAGECOLORPCT,0)));

		// set the layouter input
		String layout = displayString.getAsString(IDisplayString.Prop.LAYOUT);
		VectorArrangement arrangement = ISubmoduleConstraint.VectorArrangement.none;
		if (StringUtils.isNotEmpty(layout)) {
			layout = IDisplayString.Prop.LAYOUT.getEnumSpec().getNameFor(layout);
			if (StringUtils.isNotEmpty(layout))
				arrangement = ISubmoduleConstraint.VectorArrangement.valueOf(layout);
		}
		setBaseLocation(
				displayString.getLocation(scale),
				arrangement,
				displayString.unit2pixel(displayString.getAsInt(IDisplayString.Prop.LAYOUT_PAR1, Integer.MIN_VALUE), scale),
				displayString.unit2pixel(displayString.getAsInt(IDisplayString.Prop.LAYOUT_PAR2, Integer.MIN_VALUE), scale),
				displayString.unit2pixel(displayString.getAsInt(IDisplayString.Prop.LAYOUT_PAR3, Integer.MIN_VALUE), scale)
		);

		// if the shapeBounds has changed, we should trigger the layouting
		if (!getShapeBounds().equals(oldShapeBounds)) 
			revalidate();
		// update the bounds, so that repaint works correctly (nothing gets clipped off)
		if (centerLoc != null)
			updateBounds();  // note: re-layouting does not guarantee that updateBounds() gets called!
		repaint();
	}

	protected void setTooltipText(String tooltipText) {
		this.tooltipText = tooltipText;
	}

	/**
	 * Store range figure parameters. fillColor and/or borderColor can be null, 
	 * meaning that no background or outline should be drawn.
	 */
	protected void setRange(int radius, Color fillColor, Color borderColor, int borderWidth) {
		if (radius <= 0) {
			if (rangeFigure != null)
				rangeFigure.setVisible(false);
		}
		else {
			if (rangeFigure == null) {
				rangeFigure = new RangeFigure();
				Layer backgroundLayer = ((SubmoduleLayer)getParent()).getCompoundModuleFigure().getBackgroundDecorationLayer();
				backgroundLayer.add(rangeFigure);
			}
			rangeFigure.setVisible(true);
			rangeFigure.setFill(fillColor != null);
			rangeFigure.setBackgroundColor(fillColor);
			rangeFigure.setOutline(borderColor != null &&  borderWidth > 0);
			rangeFigure.setForegroundColor(borderColor);
			rangeFigure.setLineWidth(borderWidth);

			if (centerLoc != null)
				rangeFigure.setBounds(new Rectangle(centerLoc.x, centerLoc.y, 0, 0).expand(radius,radius));
			else 
				rangeFigure.setBounds(new Rectangle(0, 0, 0, 0).expand(radius,radius));

			rangeFigure.repaint(); 
		}
	}

	public void setProblemDecoration(int maxSeverity, ITooltipTextProvider textProvider) {
		problemMarkerImage = NedFigure.getProblemImageFor(maxSeverity);
		problemMarkerTextProvider = textProvider;
		if (centerLoc != null)
			updateBounds();
		repaint();
	}

	public void setQueueText(String queueText) {
		if (!StringUtils.equals(this.queueText, queueText)) {
			this.queueText = queueText;
			if (centerLoc != null)
				updateBounds();
			repaint();
		}
	}

	protected void setInfoText(String text, String pos, Color color) {
		// only called as part of setDisplayString() -- no invalidate() or repaint() needed here 
		Assert.isNotNull(color);
		this.text = text;
		if (!StringUtils.isEmpty(pos))
			pos = IDisplayString.Prop.TEXTPOS.getEnumSpec().getNameFor(pos);
		this.textPos = pos.equals("left") ? TEXTPOS_LEFT : pos.equals("right") ? TEXTPOS_RIGHT : TEXTPOS_TOP;
		this.textColor = color;
	}

	protected void setShape(Image image, String shapeName, int shapeWidth, int shapeHeight,
			Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {
		// only called as part of setDisplayString() -- no invalidate() or repaint() needed here 
		Assert.isNotNull(shapeFillColor);
		Assert.isNotNull(shapeBorderColor);

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

		// else if (shapeName.equalsIgnoreCase("rrect"))
		// shape = SHAPE_RECT2;
		// else if (shapeName.equalsIgnoreCase("tri"))
		// shape = SHAPE_TRI;
		// else if (shapeName.equalsIgnoreCase("tri2"))
		// shape = SHAPE_TRI2;
		// else if (shapeName.equalsIgnoreCase("hex"))
		// shape = SHAPE_HEX;
		// else if (shapeName.equalsIgnoreCase("hex2"))
		// shape = SHAPE_HEX2;

		this.shapeWidth = Math.max(shapeWidth, 0);
		this.shapeHeight = Math.max(shapeHeight, 0);
		this.shapeBorderColor = shapeBorderColor;
		this.shapeFillColor = shapeFillColor;
		this.shapeBorderWidth = shapeBorderWidth;

		this.image = image;

		if (image == null && shape == SHAPE_NONE)
			this.image = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);
	}

	public void setSubmoduleVectorIndex(Object vectorIdentifier, int vectorSize, int vectorIndex) {
		// clear centerLoc iff something's changed
		if ((this.vectorIdentifier==null ? vectorIdentifier!=null : !this.vectorIdentifier.equals(vectorIdentifier)) ||
				this.vectorSize != vectorSize || this.vectorIndex != vectorIndex) {
			this.vectorIdentifier = vectorIdentifier;
			this.vectorSize = vectorSize;
			this.vectorIndex = vectorIndex;
			centerLoc = null;
			revalidate();  // invalidate() not enough here. Layout must be triggered 
		}
	}

	protected void setBaseLocation(Point loc, VectorArrangement vectorArrangement, 
			int vectorArrangementPar1, int vectorArrangementPar2, int vectorArrangementPar3) {
		// clear centerLoc iff something's changed
		if ((baseLoc==null ? loc!=null : !baseLoc.equals(loc)) || 
				this.vectorArrangement != vectorArrangement ||
				this.vectorArrangementPar1 != vectorArrangementPar1 ||
				this.vectorArrangementPar2 != vectorArrangementPar2 ||
				this.vectorArrangementPar3 != vectorArrangementPar3) {
			this.baseLoc = loc;
			this.vectorArrangement = vectorArrangement;
			this.vectorArrangementPar1 = vectorArrangementPar1;
			this.vectorArrangementPar2 = vectorArrangementPar2;
			this.vectorArrangementPar3 = vectorArrangementPar3;
			centerLoc = null; // force re-layout of this module
			revalidate();  // invalidate() not enough here 
		}
	}

	protected Rectangle computeBounds(Rectangle shapeBounds) {
		// calculate name bounds, ASSUMING the given shapeBounds; MUST NOT use centerLoc!
		Rectangle bounds = shapeBounds.getCopy();
		Rectangle tmp;
		if ((tmp = getNameBounds(shapeBounds)) != null)
			bounds.union(tmp);
		if ((tmp = getTextBounds(shapeBounds)) != null)
			bounds.union(tmp);
		if ((tmp = getQueueTextBounds(shapeBounds)) != null)
			bounds.union(tmp);
		if ((tmp = getPinImageBounds(shapeBounds)) != null)
			bounds.union(tmp);
		if ((tmp = getProblemMarkerImageBounds(shapeBounds)) != null)
			bounds.union(tmp);
		if ((tmp = getDecorationImageBounds(shapeBounds)) != null)
			bounds.union(tmp);
		return bounds;
	}

	public Rectangle getShapeBounds() {
		// when centerLoc==null, assume (0,0)
		int width = shape==SHAPE_NONE ? 0 : shapeWidth;
		int height = shape==SHAPE_NONE ? 0 : shapeHeight;
		if (image != null) {
			width = Math.max(width, image.getBounds().width);
			height = Math.max(height, image.getBounds().height);
		}
		return centerLoc==null ? new Rectangle(0,0,width,height) : new Rectangle(centerLoc.x-width/2, centerLoc.y-height/2, width, height);
	}

	/**
	 * The bounds of the name label
	 */
	public Rectangle getNameBounds() {
		return getNameBounds(getShapeBounds());
	}

	protected Rectangle getNameBounds(Rectangle shapeBounds) {
		// calculate name bounds, ASSUMING the given shapeBounds; MUST NOT use centerLoc!
		if (StringUtils.isEmpty(nameText)) 
			return null;
		Dimension textSize = TextUtilities.INSTANCE.getTextExtents(nameText, getFont());
		return new Rectangle(shapeBounds.x + shapeBounds.width/2 - textSize.width/2, shapeBounds.bottom(), textSize.width, textSize.height);
	}

	protected Rectangle getTextBounds(Rectangle shapeBounds) {
		// calculate text bounds, ASSUMING the given shapeBounds; MUST NOT use centerLoc!
		if (StringUtils.isEmpty(text)) 
			return null;
		int x, y;
		Dimension textSize = TextUtilities.INSTANCE.getTextExtents(text, getFont());
		if (textPos == TEXTPOS_LEFT) {
			x = shapeBounds.x - textSize.width; 
			y = shapeBounds.y; 
		}
		else if (textPos == TEXTPOS_RIGHT) {
			x = shapeBounds.right(); 
			y = shapeBounds.y; 
		}
		else {  // TEXTPOS_TOP
			x = shapeBounds.x + shapeBounds.width/2 - textSize.width/2; 
			y = shapeBounds.y - textSize.height; 
		}
		return new Rectangle(x, y, textSize.width, textSize.height);
	}

	protected Rectangle getQueueTextBounds(Rectangle shapeBounds) {
		// calculate queue text bounds, ASSUMING the given shapeBounds; MUST NOT use centerLoc!
		if (StringUtils.isEmpty(queueText)) 
			return null;
		int x, y;
		Dimension textSize = TextUtilities.INSTANCE.getTextExtents(queueText, getFont());
		x = shapeBounds.right(); 
		y = shapeBounds.bottom()-textSize.height; 
		return new Rectangle(x, y, textSize.width, textSize.height);
	}

	protected Rectangle getDecorationImageBounds(Rectangle shapeBounds) {
		// calculate icon2 bounds, ASSUMING the given shapeBounds; MUST NOT use centerLoc!
		if (decoratorImage == null)
			return null;
		org.eclipse.swt.graphics.Rectangle imageBounds = decoratorImage.getBounds();
		return new Rectangle(shapeBounds.right()-3*imageBounds.width/4, shapeBounds.y - imageBounds.height/4, imageBounds.width, imageBounds.height);
	}

	protected Rectangle getPinImageBounds(Rectangle shapeBounds) {
		// calculate pin image bounds, ASSUMING the given shapeBounds; MUST NOT use centerLoc!
		if (!pinVisible)
			return null;
		org.eclipse.swt.graphics.Rectangle imageBounds = IMG_PIN.getBounds();
		return new Rectangle(shapeBounds.right()-imageBounds.width/2, shapeBounds.y - imageBounds.height/2, imageBounds.width, imageBounds.height);
	}


	protected Rectangle getProblemMarkerImageBounds(Rectangle shapeBounds) {
		// calculate marker image bounds, ASSUMING the given shapeBounds; MUST NOT use centerLoc!
		if (problemMarkerImage == null)
			return null;
		org.eclipse.swt.graphics.Rectangle imageBounds = problemMarkerImage.getBounds();
		return new Rectangle(shapeBounds.x-4, shapeBounds.y-4, imageBounds.width, imageBounds.height);
	}

	/**
	 * Sets the external image decoration ("i2" tag)
	 */
	protected void setDecorationImage(Image img) {
		decoratorImage = img;
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

	public Rectangle getAnchorBounds() {
		return getShapeBounds();
	}

	public void setName(String text) {
		if (!StringUtils.equals(this.nameText, text)) {
			nameText = text;
			if (centerLoc != null)
				updateBounds();
			repaint();
		}
	}

	public String getName() {
		return nameText;
	}

	public String getTooltipText(int x, int y) {
		// if there is a problem marker and an associated tooltip text provider
		// and the cursor is over the marker, delegate to the problemmarker text provider
		if (problemMarkerTextProvider != null && problemMarkerImage != null) {
			Rectangle markerBounds = getProblemMarkerImageBounds(getShapeBounds());
			translateToAbsolute(markerBounds);
			if (markerBounds.contains(x, y)) {
				String text = problemMarkerTextProvider.getTooltipText(x, y);
				if (text != null)
					return text;
			}
		}
		return tooltipText;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.draw2d.Figure#containsPoint(int, int)
	 * We override it to include also the nameBounds, so clicking on submodule name would
	 * be counted also as a selection event.
	 */
	@Override
	public boolean containsPoint(int x, int y) {
		return getShapeBounds().contains(x, y) || getNameBounds().contains(x, y);
	}

	/**
	 * Whether the figure displays a shape (and not image)
	 */
	public boolean isShapeVisible() {
		return shape != SHAPE_NONE;
	}

	/**
	 * Turns the "pin" icon (which shows whether the submodule has a
	 * user-specified position) on/off
	 */
	public void setPinVisible(boolean visible) {
		if (pinVisible != visible) {
			pinVisible = visible;
			if (centerLoc != null)
				updateBounds();
			repaint();
		}
	}

	public boolean isPinVisible() {
		return pinVisible;
	}

	/**
	 * Hides/shows the name of the module. NOTE that the NameBounds is still calculated correctly
	 * just the paint method does not paint it.
	 */
	public void setNameVisible(boolean visible) {
		if (nameVisible != visible) {
			nameVisible = visible;
			if (centerLoc != null)
				updateBounds();
			repaint();
		}
	}

	public boolean isNameVisible() {
		return nameVisible;
	}

	public Point getCenterLocation() {
		return centerLoc;
	}

	public void setCenterLocation(Point loc) {
		// set the center of the image, and update bounds if changed. 
		// note: this method may ONLY be called from the layouter! 
		if (loc == null) {
			centerLoc = null;
		}
		else if (!loc.equals(centerLoc)) {
			this.centerLoc = loc;
			updateBounds();
			if (rangeFigure != null) 
				rangeFigure.setBounds(rangeFigure.getBounds().setLocation(centerLoc.x-rangeFigure.getSize().width/2, 
						centerLoc.y - rangeFigure.getSize().height/2));
		}
	}

	protected void updateBounds() {
		super.setBounds(computeBounds(getShapeBounds()));
	}

	public Point getBaseLocation() {
		return baseLoc;
	}

	public Object getVectorIdentifier() {
		return vectorIdentifier;
	}

	public VectorArrangement getVectorArrangement() {
		return vectorArrangement;
	}

	public int getVectorIndex() {
		return vectorIndex;
	}

	public int getVectorSize() {
		return vectorSize;
	}

	public int getVectorArrangementPar1() {
		return vectorArrangementPar1;
	}

	public int getVectorArrangementPar2() {
		return vectorArrangementPar2;
	}

	public int getVectorArrangementPar3() {
		return vectorArrangementPar3;
	}

	@Override
	public void paint(Graphics graphics) {
		super.paint(graphics);
//		System.out.println(this+": paint(): centerLoc==" + centerLoc);
		Assert.isNotNull(centerLoc, "setCenterLoc() must be called before painting");

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
				Assert.isTrue(false, "NOT IMPLEMENTED YET"); //XXX
			}
			graphics.popState();
		}

		// draw image
		if (image != null) {
			org.eclipse.swt.graphics.Rectangle imageBounds = image.getBounds();
			graphics.drawImage(image, centerLoc.x - imageBounds.width/2, centerLoc.y - imageBounds.height/2);
		}

		Rectangle shapeBounds = null;  // do it on demand

		// draw decoration image
		if (decoratorImage != null) {
			if (shapeBounds == null) shapeBounds = getShapeBounds();
			Rectangle r = getDecorationImageBounds(shapeBounds);
			graphics.drawImage(decoratorImage, r.x, r.y);
		}

		// draw text
		if (!StringUtils.isEmpty(text)) {
			if (shapeBounds == null) shapeBounds = getShapeBounds();
			Rectangle r = getTextBounds(shapeBounds);
			graphics.setForegroundColor(textColor);
			graphics.drawText(text, r.x, r.y);
		}

		// draw queueText
		if (!StringUtils.isEmpty(queueText)) {
			if (shapeBounds == null) shapeBounds = getShapeBounds();
			Rectangle r = getQueueTextBounds(shapeBounds);
			graphics.setForegroundColor(ColorFactory.BLACK);
			graphics.drawText(queueText, r.x, r.y);
		}

		// draw pin
		if (pinVisible) {
			if (shapeBounds == null) shapeBounds = getShapeBounds();
			Rectangle r = getPinImageBounds(shapeBounds);
			graphics.drawImage(IMG_PIN, r.x, r.y);
		}

		// draw pin
		if (problemMarkerImage != null) {
			if (shapeBounds == null) shapeBounds = getShapeBounds();
			Rectangle r = getProblemMarkerImageBounds(shapeBounds);
			graphics.drawImage(problemMarkerImage, r.x, r.y);
		}

		// draw name string
		if (!StringUtils.isEmpty(nameText) && nameVisible) {
			if (shapeBounds == null) shapeBounds = getShapeBounds();
			Rectangle r = getNameBounds(shapeBounds);
			graphics.setForegroundColor(ColorFactory.BLACK);
			graphics.drawText(nameText, r.x, r.y);
		}
	}

	@Override
	public String toString() {
		return getClass().getSimpleName()+" "+nameText;
	}
}

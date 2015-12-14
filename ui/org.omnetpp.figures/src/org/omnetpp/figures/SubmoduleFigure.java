/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.figures;


import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.AssertionFailedException;
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
import org.omnetpp.common.displaymodel.DimensionF;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.PointF;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.anchors.IAnchorBounds;
import org.omnetpp.figures.layout.ISubmoduleConstraint;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.figures.layout.ISubmoduleConstraint.VectorArrangement;
import org.omnetpp.figures.layout.VectorArrangementParameters;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.figures.misc.ISelectableFigure;
import org.omnetpp.figures.misc.ISelectionHandleBounds;

/**
 * Figure representing a submodule inside a compound module figure.
 *
 * @author andras
 */
//FIXME support multiple texts: t/t1/t2/t3/t4
//FIXME alignment of multi-line text
public class SubmoduleFigure extends Figure implements IAnchorBounds, ISelectionHandleBounds, ITooltipTextProvider, IProblemDecorationSupport, ISelectableFigure {
    // supported shape types
    protected static final int SHAPE_NONE = 0;
    protected static final int SHAPE_OVAL = 1;
    protected static final int SHAPE_RECT = 2;

    protected static final int TEXTPOS_LEFT = 1;
    protected static final int TEXTPOS_RIGHT = 2;
    protected static final int TEXTPOS_TOP = 3;

    protected static final Image IMG_PIN = ImageFactory.global().getImage(ImageFactory.DEFAULT_PIN);
    protected static final Image IMG_DEFAULT = ImageFactory.global().getImage(ImageFactory.DEFAULT);

    protected SubmoduleConstraint constraint = new SubmoduleConstraint();

    protected float lastScale = 1.0f;
    protected float lastIconScale = 1.0f;

    // appearance
    protected Point centerPos;
    protected int alpha = 255;
    protected String nameText;
    protected String tooltipText;
    protected ITooltipTextProvider problemMarkerTextProvider;
    protected Image problemMarkerImage;
    protected int shape;
    protected int shapeWidth; // zero if unspec
    protected int shapeHeight; // zero if unspec
    protected Color shapeFillColor;
    protected Color shapeBorderColor;
    protected int shapeBorderWidth;
    protected Image image;
    protected Image decoratorImage;
    protected int imageSizePercentage = 100;  // currently unused
    protected boolean pinVisible;
    protected boolean nameVisible = true;
    protected String text;
    protected int textPos;
    protected Color textColor;
    protected String queueText;
    protected Layer rangeFigureLayer;
    protected RangeFigure rangeFigure = null;
    private int lastCumulativeHashCode;
    private boolean isSelected;

    public SubmoduleFigure() {
    }

    public ISubmoduleConstraint getLayoutConstraint() {
        return constraint;
    }

    public boolean isSelected() {
        return isSelected;
    }

    public void setSelected(boolean isSelected) {
        if (isSelected == this.isSelected)
            return;
        else {
            this.isSelected = isSelected;
            repaint();
        }
    }

    protected int unitToPixel(float d, float scale) {
        return (int)(scale * d);
    }

    /**
     * Adjust the properties using a display string object
     */
    public void setDisplayString(IDisplayString displayString, float scale, float iconScale, IProject project) {
        // optimization: do not change anything if the display string has not changed
        int cumulativeHashCode = displayString.cumulativeHashCode();
        if (lastScale == scale && lastIconScale == iconScale && lastCumulativeHashCode != 0 && cumulativeHashCode == lastCumulativeHashCode)
            return;

        // font must be set on the figure explicitly, otherwise it'll recursively go up to get it from the canvas every time
        setFont(getFont());

        this.lastScale = scale;
        this.lastIconScale = iconScale;
        this.lastCumulativeHashCode = cumulativeHashCode;

        Rectangle oldShapeBounds = getShapeBounds();  // to compare at the end

        // range support
        setRange(
                unitToPixel(displayString.getRange(), scale),
                ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.RANGE_FILL_COLOR)),
                ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.RANGE_BORDER_COLOR)),
                displayString.getAsInt(IDisplayString.Prop.RANGE_BORDER_WIDTH, -1));

        // tooltip support
        setTooltipText(displayString.getAsString(IDisplayString.Prop.TOOLTIP));

        // additional text support
        setInfoText(displayString.getAsString(IDisplayString.Prop.TEXT),
                displayString.getAsString(IDisplayString.Prop.TEXT_POS),
                ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.TEXT_COLOR), ColorFactory.RED));

        // image support
        String imageSize = displayString.getAsString(IDisplayString.Prop.IMAGE_SIZE);
        imageSizePercentage = (int)(100.0f * iconScale);
        Image image = ImageFactory.of(project).getImage(
                displayString.getAsString(IDisplayString.Prop.IMAGE),
                imageSize,
                ColorFactory.asRGB(displayString.getAsString(IDisplayString.Prop.IMAGE_COLOR)),
                displayString.getAsInt(IDisplayString.Prop.IMAGE_COLOR_PERCENTAGE,0));

        // rectangle ("b" tag)
        DimensionF size = displayString.getSize();
        String shape = displayString.getAsString(IDisplayString.Prop.SHAPE);
        if (!displayString.containsTag(IDisplayString.Tag.b))
            shape = null;
        constraint.setShapeSize(shape == null ? null : size);
        setShape(image, shape,
                (int)(size.width * scale),
                (int)(size.height * scale),
                ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.SHAPE_FILL_COLOR), ColorFactory.RED),
                ColorFactory.asColor(displayString.getAsString(IDisplayString.Prop.SHAPE_BORDER_COLOR), ColorFactory.RED),
                displayString.getAsInt(IDisplayString.Prop.SHAPE_BORDER_WIDTH, -1));

        // set the decoration image properties
        setDecorationImage(
                ImageFactory.of(project).getImage(
                        displayString.getAsString(IDisplayString.Prop.IMAGE2),
                        null,
                        ColorFactory.asRGB(displayString.getAsString(IDisplayString.Prop.IMAGE2_COLOR)),
                        displayString.getAsInt(IDisplayString.Prop.IMAGE2_COLOR_PERCENTAGE,0)));

        // set the layouter input
        String layout = displayString.getAsString(IDisplayString.Prop.LAYOUT);
        VectorArrangement arrangement = ISubmoduleConstraint.VectorArrangement.none;
        if (StringUtils.isNotEmpty(layout)) {
            layout = IDisplayString.Prop.LAYOUT.getEnumSpec().getNameFor(layout);
            if (StringUtils.isNotEmpty(layout))
                arrangement = ISubmoduleConstraint.VectorArrangement.valueOf(layout);
        }

        setBaseLocation(displayString.getLocation());
        setVectorArrangement(arrangement,
                displayString.getAsString(IDisplayString.Prop.LAYOUT_PAR1),
                displayString.getAsString(IDisplayString.Prop.LAYOUT_PAR2),
                displayString.getAsString(IDisplayString.Prop.LAYOUT_PAR3)
        );

        // if the shapeBounds has changed, we should trigger the layouting
        if (!getShapeBounds().equals(oldShapeBounds))
            revalidate();
        // update the bounds, so that repaint works correctly (nothing gets clipped off)
        if (centerPos != null)
            updateBounds();  // note: re-layouting does not guarantee that updateBounds() gets called!
        repaint();
    }

    public void setAlpha(int alpha) {
        this.alpha = alpha;
    }

    protected void setTooltipText(String tooltipText) {
        this.tooltipText = tooltipText;
    }

    /**
     * Sets the layer to be used as parent for the range figure (see setRange()).
     */
    public void setRangeFigureLayer(Layer rangeFigureLayer) {
        this.rangeFigureLayer = rangeFigureLayer;
    }

    public Layer getRangeFigureLayer() {
        return rangeFigureLayer;
    }

    /**
     * Store range figure parameters. fillColor and/or borderColor can be null,
     * meaning that no background or outline should be drawn.
     */
    protected void setRange(int radius, Color fillColor, Color borderColor, int borderWidth) {
        if (radius <= 0) {
            if (rangeFigure != null) {
                getRangeFigureLayer().remove(rangeFigure);
                rangeFigure = null;
            }
        }
        else {
            if (rangeFigure == null) {
                rangeFigure = new RangeFigure();
                getRangeFigureLayer().add(rangeFigure);
            }
            rangeFigure.setVisible(true);
            rangeFigure.setFill(fillColor != null);
            rangeFigure.setBackgroundColor(fillColor);
            rangeFigure.setOutline(borderColor != null &&  borderWidth > 0);
            rangeFigure.setForegroundColor(borderColor);
            rangeFigure.setLineWidth(borderWidth);

            if (centerPos != null)
                rangeFigure.setBounds(new Rectangle(centerPos.x, centerPos.y, 0, 0).expand(radius,radius));
            else
                rangeFigure.setBounds(new Rectangle(0, 0, 0, 0).expand(radius,radius));

            rangeFigure.repaint();
        }
    }

    public void setProblemDecoration(int maxSeverity, ITooltipTextProvider textProvider) {
        problemMarkerImage = FigureUtils.getProblemImageFor(maxSeverity);
        problemMarkerTextProvider = textProvider;
        if (centerPos != null)
            updateBounds();
        repaint();
    }

    public void setQueueText(String queueText) {
        if (!StringUtils.equals(this.queueText, queueText)) {
            this.queueText = queueText;
            if (centerPos != null)
                updateBounds();
            repaint();
        }
    }

    protected void setInfoText(String text, String pos, Color color) {
        // only called as part of setDisplayString() -- no invalidate() or repaint() needed here
        Assert.isNotNull(color);
        this.text = text;
        if (!StringUtils.isEmpty(pos))
            pos = StringUtils.nullToEmpty(IDisplayString.Prop.TEXT_POS.getEnumSpec().getNameFor(pos));
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

        this.shapeWidth = Math.max(shapeWidth, 0);
        this.shapeHeight = Math.max(shapeHeight, 0);
        this.shapeBorderColor = shapeBorderColor;
        this.shapeFillColor = shapeFillColor;
        this.shapeBorderWidth = shapeBorderWidth;

        this.image = image;

        if (image == null && shape == SHAPE_NONE)
            this.image = ImageFactory.global().getImage(ImageFactory.DEFAULT_KEY);
    }

    public void setSubmoduleVectorIndex(Object vectorIdentifier, int vectorSize, int vectorIndex) {
        if (constraint.setSubmoduleVectorIndex(vectorIdentifier, vectorSize, vectorIndex))
            invalidateLocation();
    }

    protected void setBaseLocation(PointF loc) {
        if (constraint.setBaseLocation(loc)) {
            // If the module position is set, we do not need the temporary position (centerLoc)
            // used by the layouter as a temporary position. If we just unpinned
            // this module (loc == null) we have to keep the current position. The layouter will
            // use that as the current position (this will ensure that the module will not 'jump away'
            // when we unpin it.
            if (loc != null)
                invalidateLocation();
            revalidate();
        }
    }

    protected void setVectorArrangement(VectorArrangement vectorArrangement, String par1, String par2, String par3) {
        VectorArrangementParameters pars = makeVectorArrangementParameters(vectorArrangement, par1, par2, par3);
        if (constraint.setVectorArragement(vectorArrangement, pars))
            revalidate();
    }

    protected VectorArrangementParameters makeVectorArrangementParameters(VectorArrangement vectorArrangement, String par1, String par2, String par3) {
        if (vectorArrangement == VectorArrangement.none)
            return null;

        VectorArrangementParameters p = new VectorArrangementParameters();
        switch (vectorArrangement) {
        case none:
            break; // handled above
        case exact:
            p.x = parseFloat(par1);
            p.y = parseFloat(par2);
            break;
        case row:
            p.dx = parseFloat(par1);
            break;
        case column:
            p.dy = parseFloat(par1);
            break;
        case matrix:
            p.n = parseInt(par1);
            p.dx = parseFloat(par2);
            p.dy = parseFloat(par3);
            break;
        case ring:
            p.dx = parseFloat(par1);
            p.dy = parseFloat(par2);
            break;
        default:
            throw new AssertionFailedException("unhandled vector arrangement");
        }
        return p;
    }

    private float parseFloat(String value) {
        try {
            if (!StringUtils.isBlank(value))
                return Float.valueOf(value);
        } catch (NumberFormatException e) { }
        return Float.NaN;
    }

    private int parseInt(String value) {
        try {
            if (!StringUtils.isBlank(value))
                return Integer.valueOf(value);
        } catch (NumberFormatException e) { }
        return -1;
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
            org.eclipse.swt.graphics.Rectangle imageBounds = image.getBounds();
            width = Math.max(width, imageSizePercentage * imageBounds.width / 100);
            height = Math.max(height, imageSizePercentage * imageBounds.height / 100);
        }
        return centerPos==null ? new Rectangle(0,0,width,height) : new Rectangle(centerPos.x-width/2, centerPos.y-height/2, width, height);
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
        if (imageSizePercentage != 100) {
            imageBounds.width = imageSizePercentage * imageBounds.width / 100;
            imageBounds.height = imageSizePercentage * imageBounds.height / 100;
        }
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
        throw new UnsupportedOperationException(); // call setCenterPosition/setShape instead
    }

    @Override
    public void setLocation(Point p) {
        throw new UnsupportedOperationException(); // call setCenterPosition/setShape instead
    }

    @Override
    public void setSize(int w, int h) {
        throw new UnsupportedOperationException(); // call setCenterPosition/setShape instead
    }

    @Override
    public void setPreferredSize(Dimension size) {
        throw new UnsupportedOperationException(); // call setCenterPosition/setShape instead
    }

    @Override
    public Dimension getPreferredSize(int hint, int hint2) {
        throw new UnsupportedOperationException(); // call setCenterPosition/setShape instead
    }

    public Rectangle getHandleBounds() {
        return getShapeBounds();
    }

    public Rectangle getAnchorBounds() {
        return getShapeBounds();
    }


    /* (non-Javadoc)
     * @see org.eclipse.draw2d.Figure#containsPoint(int, int)
     * We override it to include also the nameBounds, so clicking on submodule name would
     * be counted also as a selection event.
     */
    @Override
    public boolean containsPoint(int x, int y) {
        if (getShapeBounds().contains(x, y))
            return true;
        Rectangle nameBounds = getNameBounds();
        return nameBounds != null && nameBounds.contains(x, y);
    }

    public void setName(String text) {
        if (!StringUtils.equals(this.nameText, text)) {
            nameText = text;
            if (centerPos != null)
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
            if (centerPos != null)
                updateBounds();
            repaint();
        }
    }

    public boolean isPinVisible() {
        return pinVisible;
    }

    /**
     * Hides/shows the name of the module. NOTE that the name bounds is still
     * calculated correctly and the name label counts in the figure bounds,
     * only the paint method does not paint it. This methods is used by the
     * direct edit support in the graphical editor.
     *
     * Note: an alternative is setName(null), which will exclude the name label
     * from the bounds.
     */
    public void setNameVisible(boolean visible) {
        if (nameVisible != visible) {
            nameVisible = visible;
            if (centerPos != null)
                updateBounds();
            repaint();
        }
    }

    public boolean isNameVisible() {
        return nameVisible;
    }

    public void setImageSizePercentage(int imageSizePercentage) {
        if (this.imageSizePercentage != imageSizePercentage) {
            this.imageSizePercentage = imageSizePercentage;
            if (centerPos != null)
                updateBounds();
            repaint();
        }
    }

    public int getImageSizePercentage() {
        return imageSizePercentage;
    }

    public Point getCenterPosition() {
        return centerPos;
    }

    /**
     * Set the center of the main area (icon/shape) of if the figure, and
     * update bounds if changed. Pass null to invalidate the position.
     */
    public void setCenterPosition(Point pos) {
        if (pos == null) {
            centerPos = null;
        }
        else if (!pos.equals(centerPos)) {
            this.centerPos = pos;
            updateBounds();
            if (rangeFigure != null)
                rangeFigure.setBounds(rangeFigure.getBounds().setLocation(centerPos.x-rangeFigure.getSize().width/2,
                        centerPos.y - rangeFigure.getSize().height/2));
        }
    }

    protected void invalidateLocation() {
        constraint.setLayoutedLocation(null);
        setCenterPosition(null);
    }

    protected void updateBounds() {
        super.setBounds(computeBounds(getShapeBounds()));
    }

    @Override
    public void paint(Graphics graphics) {
        graphics.setAlpha(alpha);
        super.paint(graphics);
        Assert.isNotNull(centerPos, "setCenterPos() must be called before painting");
        Assert.isNotNull(rangeFigureLayer, "setRangeFigureLayer() must be called before painting");

        // draw shape
        if (shape != SHAPE_NONE) {
            graphics.pushState();
            graphics.setForegroundColor(shapeBorderColor);
            graphics.setBackgroundColor(shapeFillColor);
            graphics.setLineWidthFloat(shapeBorderWidth*(float)graphics.getAbsoluteScale());
            int left = centerPos.x - shapeWidth/2;
            int top = centerPos.y - shapeHeight/2;
            // expand the clipping a bit so the anti-aliased sides would not be clipped
            graphics.setClip(graphics.getClip(Rectangle.SINGLETON).getExpanded(1, 1));
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
            if (imageSizePercentage == 100)
                graphics.drawImage(image, centerPos.x - imageBounds.width/2, centerPos.y - imageBounds.height/2);
            else {
                int scaledWidth = imageSizePercentage * imageBounds.width / 100;
                int scaledHeight = imageSizePercentage * imageBounds.height / 100;
                graphics.drawImage(image, 0, 0, imageBounds.width, imageBounds.height,
                        centerPos.x - scaledWidth/2, centerPos.y - scaledHeight/2, scaledWidth, scaledHeight);
            }
        }

        Rectangle shapeBounds = null;  // do it on demand

        // draw decoration image
        if (decoratorImage != null) {
            if (shapeBounds == null) shapeBounds = getShapeBounds();
            Rectangle r = getDecorationImageBounds(shapeBounds);
            if (imageSizePercentage == 100)
                graphics.drawImage(decoratorImage, r.x, r.y);
            else {
                org.eclipse.swt.graphics.Rectangle imageBounds = decoratorImage.getBounds();
                graphics.drawImage(decoratorImage, 0, 0, imageBounds.width, imageBounds.height,
                        r.x, r.y, r.width, r.height);
            }
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

        // draw error image
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

        if (isSelected) {
            graphics.setForegroundColor(ColorFactory.RED);
            Rectangle r = getHandleBounds();
            graphics.drawRectangle(r.x, r.y, r.width - 1, r.height - 1);
        }
    }

    @Override
    public String toString() {
        return getClass().getSimpleName()+" "+nameText;
    }
}

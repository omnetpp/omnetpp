package org.omnetpp.figures;

import org.eclipse.draw2d.AbstractLabeledBorder;
import org.eclipse.draw2d.Border;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;

public class CompoundModuleTitleBarBorder extends AbstractLabeledBorder {

	private static Color defaultColor = ColorConstants.menuBackgroundSelected;

	private Insets padding = new Insets(1, 3, 2, 2);
	private Color fillColor = defaultColor;

	private Insets imgInsets;
	private Image image;
	private Dimension imageSize;

	public CompoundModuleTitleBarBorder() { }

	public CompoundModuleTitleBarBorder(String s) {
		setLabel(s);
	}

	/**
	 * Calculates and returns the Insets for this border. 
	 * 
	 * @param figure the figure on which Insets calculations are based
	 * @return the calculated Insets
	 */
	protected Insets calculateInsets(IFigure figure) {
		int titleBarHeight = 0;
		// check the image height
		if (image != null)
			titleBarHeight = Math.max(titleBarHeight, image.getImageData().height + padding.getHeight());
			
		// check the text height
		titleBarHeight = Math.max(titleBarHeight, getTextExtents(figure).height + padding.getHeight());
		
		return new Insets(titleBarHeight, 0, 0, 0);
	}

	/**
	 * Returns the background Color of this TitleBarBorder.
	 * @return the background color
	 * @since 2.0
	 */
	protected Color getBackgroundColor() {
		return fillColor;
	}

	/**
	 * Returns this TitleBarBorder's padding. Padding provides spacing along the sides of the 
	 * TitleBarBorder. The default value is no padding along all sides.
	 * 
	 * @return the Insets representing the space along the sides of the TitleBarBorder
	 * @since 2.0 
	 */
	protected Insets getPadding() {
		return padding;
	}

	/**
	 * Returns <code>true</code> thereby filling up all the contents within its boundaries, 
	 * eleminating the need by the figure to clip the boundaries and do the same.
	 * 
	 * @see Border#isOpaque()
	 */
	public boolean isOpaque() {
		return true;
	}

	/**
	 * @see Border#paint(IFigure, Graphics, Insets)
	 */
	public void paint(IFigure figure, Graphics g, Insets insets) {
		int lx, ly, imgx, imgy;
		int titleBarHeight = 0;
		Rectangle tRect = new Rectangle(getPaintRectangle(figure, insets));
		lx = imgx = tRect.x;
		ly = imgy = tRect.y;
		// compute image position
		if (image != null) {
			imgx += padding.left;
			imgy += padding.top;
			lx = imgx + image.getImageData().width;
			titleBarHeight = Math.max(titleBarHeight, image.getImageData().height + padding.getHeight());
		}

		// compute label position
		titleBarHeight = Math.max(titleBarHeight, getTextExtents(figure).height + padding.getHeight());
		lx += padding.left;
		// position the text in the middle of the bar
		ly += padding.top + (titleBarHeight - getTextExtents(figure).height - padding.getHeight())/2;
		tRect.height = Math.min(tRect.height, titleBarHeight);

		g.clipRect(tRect);
		g.setFont(getFont(figure));
		g.setForegroundColor(getTextColor());
		if (fillColor != null) {
			g.setBackgroundColor(fillColor);
			g.fillRectangle(tRect);
		}
		g.drawString(getLabel(), lx, ly);
		if (getImage() != null) 
			g.drawImage(getImage(), imgx, imgy);
	}

	/**
	 * Sets the background color of the area within the boundaries of this border. This is 
	 * required as this border takes responsibility for filling up the region, as 
	 * TitleBarBorders are always opaque.
	 *
	 * @param color the background color
	 * @since 2.0
	 */
	public void setBackgroundColor(Color color) {
		if (color == null)
			color = ColorFactory.defaultBorder;
		fillColor = color;
	}

	/**
	 * Sets the padding space to be applied on all sides of the border. The default value is 
	 * no padding on all sides.
	 *
	 * @param all the value of the padding on all sides
	 * @since 2.0
	 */
	public void setPadding(int all) {
		padding = new Insets(all);
		invalidate();
	}

	/**
	 * Sets the padding space of this TitleBarBorder to the passed value. The default value is 
	 * no padding on all sides.
	 *
	 * @param pad the padding
	 * @since 2.0
	 */
	public void setPadding(Insets pad) {
		padding = pad; 
		invalidate();
	}

	public Image getImage() {
		return image;
	}

	public void setImage(Image img) {
		image = img;
		if (img == null)
			return;
		
		imageSize = new Dimension(image);
		imgInsets = new Insets();
		imgInsets.left = imageSize.width;
	}

}
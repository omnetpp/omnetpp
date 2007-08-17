package org.omnetpp.figures;

import org.eclipse.draw2d.CompoundBorder;
import org.eclipse.draw2d.LabeledBorder;
import org.eclipse.draw2d.TitleBarBorder;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class CompoundModuleBorder extends CompoundBorder implements LabeledBorder
{
	/**
	 * Constructs a FrameBorder with its label set to the name of the {@link TitleBarBorder}
	 * class.
	 *
	 * @since 2.0
	 */
	public CompoundModuleBorder() {
		inner = new CompoundModuleLineBorder();
		outer = new CompoundModuleTitleBarBorder();
	}

	/**
	 * Constructs a FrameBorder with the title set to the passed String.
	 *
	 * @param label  label or title of the frame.
	 * @since 2.0
	 */
	public CompoundModuleBorder(String label) {
		this();
		setLabel(label);
	}

	public CompoundModuleTitleBarBorder getTitleBorder() {
		return (CompoundModuleTitleBarBorder)outer;
	}

	public CompoundModuleLineBorder getLineBorder() {
		return (CompoundModuleLineBorder)inner;
	}

	/**
	 * Returns the label for this border
	 */
	public String getLabel() {
		return getTitleBorder().getLabel();
	}

	/**
	 * Sets the label for this border.
	 * @param label the label
	 */
	public void setLabel(String label) {
		getTitleBorder().setLabel(label);
	}

	/**
	 * Sets the font for this border's label.
	 * @param font the font
	 */
	public void setFont(Font font) {
		getTitleBorder().setFont(font);
	}

	public void setTitleBackgroundColor(Color color) {
		getTitleBorder().setBackgroundColor(color);
	}

	public void setBorderColor(Color color) {
		// null is not valid for title background use border color as default
		if (color == null)
			color = CompoundModuleFigure.ERROR_BORDER_COLOR;
		getLineBorder().setColor(color);
	}

	/**
	 * Sets the line width for this border.
	 * @param width The line width
	 */
	public void setBorderWidth(int width) {
		getLineBorder().setWidth(width);
	}

	public void setImage(Image img) {
		getTitleBorder().setImage(img);
	}

	public void setProblemDecorationImage(Image image) {
		getTitleBorder().setProblemDecorationImage(image);
	}

}

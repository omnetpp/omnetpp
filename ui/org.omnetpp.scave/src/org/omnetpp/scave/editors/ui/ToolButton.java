package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.omnetpp.common.color.ColorFactory;

/**
 * A button which looks like SWT's ToolItem (flat), but text is rendered to the right 
 * of the image. SWT's ToolItem puts text below the image and cannot be talked out of it,
 * while Button has the right layout but it is never rendered as flat. Oh well.
 * 
 * This is expected to be put on a light background; otherwise colors used for painting 
 * will have to be refined (probably it's best to dynamically calculate them, using 
 * the RGB class's HSB conversion.)
 * 
 * @author Andras
 */
public class ToolButton extends Canvas {
	private static final int R = 5;
	private static final int BORDER = 4;

	private Color normalInnerBorderColor = null;
	private Color normalOuterBorderColor = ColorFactory.GREY90;

	private Color activeBackgroundColor = ColorFactory.GREY96;
	private Color activeInnerBorderColor = ColorFactory.ORANGE;
	private Color activeOuterBorderColor = ColorFactory.GREY40;

	private Color pushedBackgroundColor = ColorFactory.GREY90;
	private Color pushedOuterBorderColor = ColorFactory.GREY40;
	private Color pushedInnerBorderColor = ColorFactory.GREY95;

	private Color disabledBackgroundColor = null;
	private Color disabledOuterBorderColor = ColorFactory.GREY90;
	private Color disabledInnerBorderColor = null;

	private List<SelectionListener> selectionListeners = new ArrayList<SelectionListener>();

	private Image image;
	private String text;
	private int textWidth = -1;
	private int textHeight= -1;

	private static int NORMAL_STATE = 0;
	private static int ACTIVE_STATE = 1;
	private static int PUSHED_STATE = 2;
	private int state = 0;

	public ToolButton(Composite parent, int style) {
		super(parent, style);
		setBackground(ColorFactory.WHITE);

		addPaintListener(new PaintListener() {
			public void paintControl(PaintEvent e) {
				_repaint(e.gc);
			}
		});

		addMouseTrackListener(new MouseTrackListener() {
			public void mouseEnter(MouseEvent e) {
				if (isEnabled()) {
					state = ACTIVE_STATE;
					redraw();
				}
			}
			public void mouseExit(MouseEvent e) {
				if (isEnabled()) {
					state = NORMAL_STATE;
					redraw();
				}
			}
			public void mouseHover(MouseEvent e) {
			}
		});
		
		addMouseListener(new MouseListener() {
			public void mouseDoubleClick(MouseEvent e) {
			}

			public void mouseDown(MouseEvent e) {
				if (isEnabled() && e.button==1) {
					// Note: when DragSource is installed on the button, the mouseDown event
					// apparently gets intercepted and delayed ~1s by DragSource
					state = PUSHED_STATE;
					redraw();
				}
			}

			public void mouseUp(MouseEvent e) {
				Point size = ToolButton.this.getSize();
				Rectangle bounds = new Rectangle(0, 0, size.x, size.y);
				if (isEnabled() && e.button==1 &&
						e.widget == ToolButton.this && bounds.contains(e.x, e.y)) {
					state = ACTIVE_STATE;
					redraw();

					// fire "button selected" event
					Event tmp = new Event();
					tmp.display = e.display;
					tmp.widget = e.widget;
					tmp.x = e.x;
					tmp.y = e.y;
					tmp.stateMask = e.stateMask;
					SelectionEvent selectionEvent = new SelectionEvent(tmp);
					for (SelectionListener listener : selectionListeners)
						listener.widgetSelected(selectionEvent);
				}
			}
		});
	}

	protected void _repaint(GC gc) {
		Rectangle r = getBounds();
		if (!isEnabled())
			fillBackground(gc, r, disabledBackgroundColor);
		else if (state==ACTIVE_STATE)
			fillBackground(gc, r, activeBackgroundColor);
		else if (state==PUSHED_STATE)
			fillBackground(gc, r, pushedBackgroundColor);

		if (image != null)
			gc.drawImage(image, BORDER, (r.height-image.getBounds().height+1)/2); //TODO paint disabled image
		if (text != null) {
			if (getFont() != null)
				gc.setFont(getFont());
			if (textHeight == -1)
				updateTextExtent(gc);
			gc.setForeground(isEnabled() ? ColorFactory.BLACK : ColorFactory.GREY50);
			gc.drawText(text, BORDER+image.getBounds().width+BORDER, (r.height-textHeight+1) / 2);
		}
		if (!isEnabled())
			drawBorder(gc, r, disabledOuterBorderColor, disabledInnerBorderColor);
		else if (state==NORMAL_STATE)
			drawBorder(gc, r, normalOuterBorderColor, normalInnerBorderColor);
		else if (state==ACTIVE_STATE)
			drawBorder(gc, r, activeOuterBorderColor, activeInnerBorderColor);
		else if (state==PUSHED_STATE)
			drawBorder(gc, r, pushedOuterBorderColor, pushedInnerBorderColor);
	}

	protected void fillBackground(GC gc, Rectangle r, Color color) {
		if (color != null) {
			gc.setBackground(color);
			gc.fillRoundRectangle(0, 0, r.width-1, r.height-1, R, R);
		}
	}

	protected void drawBorder(GC gc, Rectangle r, Color outerColor, Color innerColor) {
		gc.setAntialias(SWT.ON);
		if (outerColor != null) {
			gc.setForeground(outerColor);
			gc.drawRoundRectangle(0, 0, r.width-1, r.height-1, R, R);
		}
		if (innerColor != null) {
			gc.setForeground(innerColor);
			gc.drawRoundRectangle(1, 1, r.width-3, r.height-3, R-2, R-2);
		}
	}

	protected void updateTextExtent(GC gc) {
		if (getFont() != null)
			gc.setFont(getFont());
		Point size = gc.textExtent(text);
		textWidth = size.x;
		textHeight = size.y;
	}
	
	public Point computeSize(int wHint, int hHint, boolean changed) {
		checkWidget();
		if (textWidth == -1) {
			GC gc = new GC(this);
			updateTextExtent(gc);
			gc.dispose();
		}

		int border = getBorderWidth();
		int width = wHint, height = hHint;
		if (width == SWT.DEFAULT)
			width = border * 2 + BORDER * 3 + (image==null ? 0 : image.getBounds().width) + textWidth; 
		if (height == SWT.DEFAULT)
			height = border * 2 + BORDER * 2 + Math.max(image==null ? 0 : image.getBounds().height, textHeight); 
		return new Point(width, height);
	}

	public Image getImage() {
		checkWidget();
		return image;
	}

	public String getText() {
		checkWidget();
		return text;
	}

	public void setImage(Image image) {
		checkWidget();
		if (image != null && image.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
		this.image = image;
	}

	public void setText(String string) {
		checkWidget();
		if (string == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
		text = string;
		textWidth = textHeight = -1;
	}

	@Override
	public void setFont(Font font) {
		super.setFont(font);
		textWidth = textHeight = -1;
	}

	public void addSelectionListener(SelectionListener listener) {
		checkWidget();
		if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
		if (!selectionListeners.contains(listener))
			selectionListeners.add(listener);
	}

	public void removeSelectionListener(SelectionListener listener) {
		checkWidget();
		if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
		selectionListeners.remove(listener);
	}

}

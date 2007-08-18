/**
 * Copy of org.eclipse.ve.internal.cde.core.CustomSashForm from the
 * Visual Editor project. Check https://bugs.eclipse.org/bugs/show_bug.cgi?id=75619.
 * 
 * Fixed recomputeSashInfo() to make it work horizontally too.
 */

/*******************************************************************************
 * Copyright (c) 2001, 2005 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
//package org.eclipse.ve.internal.cde.core;
package org.omnetpp.common.ui;
/*
 *  $RCSfile: CustomSashForm.java,v $
 *  $Revision: 1.9 $  $Date: 2005/12/05 22:10:14 $ 
 */

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.draw2d.Cursors;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Sash;

/**
 * A SashForm that allows move to/from max controls on sash.
 * 
 * It only works with one sash (two children). It doesn't make sense
 * for the arrows when there is more than one sash. Things get confusing for
 * a restore position.
 * 
 * Currently only handle top/bottom orientation. Wouldn't take much to handle left/right.
 * @author richkulp
 */
public class CustomSashForm extends SashForm {

	public static final String copyright = "(c) Copyright IBM Corporation 2002."; //$NON-NLS-1$
	/**
	 * Custom style bits. They set whether max to one side of the other
	 * is not permitted. For example, if NO_MAX_UP, then there will be only
	 * one arrow. When not maxed, it will point down (and will do a max down),
	 * and when maxed down, it will point up (and will do a restore to the
	 * previous weight). There won't be a max to the top arrow.
	 */
	public static final int
		NO_MAX_LEFT = 0x1,			// Custom style bit for not allow max left
		NO_MAX_UP = NO_MAX_LEFT,	// Custom style bit for not allow max up
		NO_MAX_RIGHT = 0x2,			// Custom style bit for not allow max right
		NO_MAX_DOWN = NO_MAX_RIGHT;	// Custom style bit for not allow max down


	private static final int NO_WEIGHT = -1;	
	private static final int NO_ARROW = -1;
	private static class SashInfo {
		public Sash sash;
		public boolean enabled;	// Whether this sashinfo is enabled (i.e. if there is more than one, this will be disabled).
		public int weight = NO_WEIGHT;	// If slammed to an edge this is the restore weight. -1 means not slammed. This is the weight in the next form (i.e. sash[0] == weight[1].
		public int cursorOver = NO_ARROW;	// Which arrow is cursor over,
		public boolean sashBorderLeft;	// Draw sash border left/top
		public boolean sashBorderRight;	// Draw sash border right/bottom		  
		public int[][] sashLocs;	// There is one entry for each arrow, It is arrowType/arrowDrawn/x/y/height/width of the arrow area. 
									// There may not be a second entry, in which case we have only one arrow.
		public SashInfo(Sash sash) {
			this.sash = sash;
		}
	};
	
	public static interface ICustomSashFormListener{
		public void dividerMoved(int firstControlWeight, int secondControlWeight);
	}
	
	protected SashInfo currentSashInfo = null;	// When the sash goes away, its entry is made null.
	protected boolean inMouseClick = false;	// Because we can't stop drag even when we are in the arrow area, we need 
												// to know that mouse down is in process so that when drag is completed, we
												// know not to recompute our position because a mouse up is about to happen
												// and we want the correct arrow handled correctly.
					
	protected boolean sashBorders[];	// Whether cooresponding control needs a sash border
							
	protected boolean noMaxUp, noMaxDown;
	protected List<ICustomSashFormListener> customSashFormListeners = null;
	
	protected static final int 
		UP_ARROW = 0,
		UP_MAX_ARROW = 1,
		DOWN_ARROW = 2,
		DOWN_MAX_ARROW = 3,
		
		MAX_ARROWS = 4;
		
	protected static final int
		ARROW_TYPE_INDEX = 0,
		ARROW_DRAWN_INDEX = 1,
		X_INDEX = 2,
		Y_INDEX = 3,
		WIDTH_INDEX = 4,
		HEIGHT_INDEX = 5;
	
	/**
	 * Constructor for CustomSashForm.
	 * @param parent
	 * @param style
	 */
	public CustomSashForm(Composite parent, int style) {
		this(parent, style, SWT.NONE);
	}
	
	/**
	 * Constructor taking a custom style too.		
	 * Or in the Custom style bits defined above (e.g. NO_MAX_RIGHT,...)
	 */
	public CustomSashForm(Composite parent, int style, int customStyle) {	
		super(parent, style);
		
		// Need listener to force a layout
		this.addListener(SWT.Resize, new Listener() {
			public void handleEvent(Event e) {
				layout(true);
			}
		});
		
		noMaxUp = ((customStyle & NO_MAX_UP) != 0);
		noMaxDown = ((customStyle & NO_MAX_DOWN) != 0);
		
		if (noMaxUp & noMaxDown)
			return;	// If you can't max up or down, there there is no need for arrows.
				
		SASH_WIDTH = 3+ getOrientation() == SWT.VERTICAL ? ARROW_HEIGHT : ARROW_SIZE;
		
		arrowColor = new Color(parent.getDisplay(), 99, 101, 156);
		borderColor = new Color(parent.getDisplay(), 132, 130, 132);		
		
		addDisposeListener(new DisposeListener() {
			/**
			 * @see org.eclipse.swt.events.DisposeListener#widgetDisposed(DisposeEvent)
			 */
			public void widgetDisposed(DisposeEvent e) {
				arrowColor.dispose();
				borderColor.dispose();
				arrowColor = borderColor = null;
			}

		});	
	}
	
	/**
	 * Call to set to max up
	 */
	public void maxUp() {
		if (noMaxUp)
			return;

		if (currentSashInfo == null)
			currentSashInfo = new SashInfo(null);		
		upMaxClicked(currentSashInfo);
	}
	
	/**
	 * Call to set to max left
	 */
	public void maxLeft() {
		maxUp();
	}
	

	/**
	 * Call to set to max down
	 */
	public void maxDown() {
		if (noMaxDown)
			return;
			
		if (currentSashInfo == null)
			currentSashInfo = new SashInfo(null);		

		downMaxClicked(currentSashInfo);
	}
	
	/**
	 * Call to set to max left
	 */
	public void maxRight() {
		maxDown();
	}
	
	/**
	 * Set the need sash borders for the controls.
	 */
	public void setSashBorders(boolean[] sashBorders) {
		int[] weights = getWeights();	// KLUDGE This is a kludge just to see how many controls we have.
		if (weights.length != 2 || (sashBorders != null && sashBorders.length != 2)) {
			SWT.error(SWT.ERROR_INVALID_ARGUMENT);
		}		
		this.sashBorders = sashBorders;
	}
	
	/**
	 * @see org.eclipse.swt.widgets.Composite#layout(boolean)
	 */
	public void layout(boolean changed) {
		super.layout(changed);

		if (noMaxUp && noMaxDown)
			return;	// No arrows to handle in this case.
			
		if (getMaximizedControl() != null)
			return;	// We have a maximized control, so we don't need to worry about the sash.
			
		// Let's get the list of all sashes the sash form now has. If there is more than one then just disable the sashinfo.
		// If there is no current sash, and there is only one sash, then create the sashinfo for it.
		Control[] children = getChildren();
		Sash newSash = null;
		for (int i = 0; i < children.length; i++) {
			if (children[i] instanceof Sash)
				if (newSash == null) 
					newSash = (Sash) children[i];
				else {
					// We have more than one sash, so need to disable current sash, if we have one.
					if (currentSashInfo != null)
						currentSashInfo.enabled = false;
					return;	// Don't go on.
				}	
		}
		
		if (newSash == null)
			return;	// We have no sashes at all.
		
		// Now we need to see if this is a new sash.
		if (currentSashInfo == null || currentSashInfo.sash == null) {
			if (currentSashInfo == null)
				currentSashInfo = new SashInfo(newSash);
			else
				currentSashInfo.sash = newSash;
			newSash.addPaintListener(new PaintListener() { 
				/**
				 * @see org.eclipse.swt.events.PaintListener#paintControl(PaintEvent)
				 */
				public void paintControl(PaintEvent e) {
					// Need to find the index of the sash we're interested in.
					
					GC gc = e.gc;
					Color oldFg = gc.getForeground();
					Color oldBg = gc.getBackground();
					
					drawArrow(gc, currentSashInfo.sashLocs[0], currentSashInfo.cursorOver == 0);	// Draw first arrow
					if (currentSashInfo.sashLocs.length > 1)
						drawArrow(gc, currentSashInfo.sashLocs[1], currentSashInfo.cursorOver == 1);	// Draw second arrow					
					
					if (currentSashInfo.sashBorderLeft)
						drawSashBorder(gc, currentSashInfo.sash, true);
					if (currentSashInfo.sashBorderRight)
						drawSashBorder(gc, currentSashInfo.sash, false);
						
					gc.setForeground(oldFg);
					gc.setBackground(oldBg);
				}

			});
			
			newSash.addControlListener(new ControlListener() {
				/**
				 * @see org.eclipse.swt.events.ControlAdapter#controlMoved(ControlEvent)
				 */
				public void controlMoved(ControlEvent e) {
					recomputeSashInfo();
				}
				
				/**
				 * @see org.eclipse.swt.events.ControlAdapter#controlResized(ControlEvent)
				 */
				public void controlResized(ControlEvent e) {
					recomputeSashInfo();
				}
								

			});
			
			newSash.addDisposeListener(new DisposeListener() {
				/**
				 * @see org.eclipse.swt.events.DisposeListener#widgetDisposed(DisposeEvent)
				 */
				public void widgetDisposed(DisposeEvent e) {
					// Need to clear out the widget from current.
					currentSashInfo= null;
				}
			});
			
			// This is a kludge because we can't override the set cursor hit test.
			newSash.addMouseMoveListener(new MouseMoveListener() {
				/**
				 * @see org.eclipse.swt.events.MouseMoveListener#mouseMove(MouseEvent)
				 */
				public void mouseMove(MouseEvent e) {
					// See if within one of the arrows.
					int x = e.x;
					int y = e.y;
					for (int i=0; i<currentSashInfo.sashLocs.length; i++) {
						int[] locs = currentSashInfo.sashLocs[i];
						boolean vertical = getOrientation() == SWT.VERTICAL;
						int loc = vertical ? x : y;
						int locIndex = vertical ? X_INDEX : Y_INDEX;
						int sizeIndex = vertical ? WIDTH_INDEX : HEIGHT_INDEX;
						if (locs[locIndex] <= loc && loc <= locs[locIndex]+locs[sizeIndex]) {					
							if (currentSashInfo.cursorOver == NO_ARROW) {
								currentSashInfo.sash.setCursor(Cursors.ARROW);
							}
							if (currentSashInfo.cursorOver != i) {
								currentSashInfo.cursorOver = i;
								currentSashInfo.sash.redraw();
								switch (locs[ARROW_TYPE_INDEX]) {
									case UP_ARROW:
									case DOWN_ARROW:
										//currentSashInfo.sash.setToolTipText(CDEMessages.CustomSashForm_Restore); 
										currentSashInfo.sash.setToolTipText("Restore Previous Location");
										break;
									case UP_MAX_ARROW:
									case DOWN_MAX_ARROW:
										//currentSashInfo.sash.setToolTipText(CDEMessages.CustomSashForm_Maximize); 
										currentSashInfo.sash.setToolTipText("Maximize Pane"); 
										break;
								}
							}
							return;
						}
					}
					if (currentSashInfo.cursorOver != NO_ARROW) {
						currentSashInfo.sash.setCursor(null);
						currentSashInfo.cursorOver = NO_ARROW;
						currentSashInfo.sash.redraw();
						currentSashInfo.sash.setToolTipText(null);
					}
				}
				
			});
			
			// Need to know when we leave so that we can clear the cursor feedback if set.
			newSash.addMouseTrackListener(new MouseTrackAdapter() {
				/**
				 * @see org.eclipse.swt.events.MouseTrackAdapter#mouseExit(MouseEvent)
				 */
				public void mouseExit(MouseEvent e) {
					if (currentSashInfo.cursorOver != NO_ARROW) {
						// Undo the cursor.
						currentSashInfo.sash.setCursor(null);
						currentSashInfo.cursorOver = NO_ARROW;
						currentSashInfo.sash.redraw();
						currentSashInfo.sash.setToolTipText(null);
					}						
				}				
			});
			
			// Want to handle mouse down as a selection.
			newSash.addMouseListener(new MouseAdapter() {
				/**
				 * @see org.eclipse.swt.events.MouseAdapter#mouseDown(MouseEvent)
				 */
				public void mouseDown(MouseEvent e) {
					inMouseClick = true;
					// If we're within a button, then redraw to wipe out stipple and get button push effect.
					int x = e.x;
					int y = e.y;
					for (int i=0; i<currentSashInfo.sashLocs.length; i++) {
						int[] locs = currentSashInfo.sashLocs[i];
						boolean vertical = getOrientation() == SWT.VERTICAL;
						int loc = vertical ? x : y;
						int locIndex = vertical ? X_INDEX : Y_INDEX;
						int sizeIndex = vertical ? WIDTH_INDEX : HEIGHT_INDEX;						
						if (locs[locIndex] <= loc && loc <= locs[locIndex]+locs[sizeIndex]) {
							currentSashInfo.sash.redraw();
							break;
						}
					}
				}
				
				/**
				 * @see org.eclipse.swt.events.MouseListener#mouseDown(MouseEvent)
				 */
				public void mouseUp(MouseEvent e) {
					// See if within one of the arrows.
					inMouseClick = false;	// No longer in down click					
					int x = e.x;
					int y = e.y;
					for (int i=0; i<currentSashInfo.sashLocs.length; i++) {
						int[] locs = currentSashInfo.sashLocs[i];
						boolean vertical = getOrientation() == SWT.VERTICAL;
						int loc = vertical ? x : y;
						int locIndex = vertical ? X_INDEX : Y_INDEX;
						int sizeIndex = vertical ? WIDTH_INDEX : HEIGHT_INDEX;						
						if (locs[locIndex] <= loc && loc <= locs[locIndex]+locs[sizeIndex]) {
							// We found it.
							switch (locs[ARROW_TYPE_INDEX]) {
								case UP_ARROW:
									upClicked(currentSashInfo);
									break;
								case UP_MAX_ARROW:
									upMaxClicked(currentSashInfo);
									break;
								case DOWN_ARROW:
									downClicked(currentSashInfo);
									break;
								case DOWN_MAX_ARROW:
									downMaxClicked(currentSashInfo);
									break;
							}
							break;
						}
					}
					
					currentSashInfo.sash.redraw();	// Make sure stipple goes away from the mouse up if not over an arrow button.
					fireDividerMoved();
				}

			});
			recomputeSashInfo();	// Get initial setting			
		}
		
	}
	

	protected void recomputeSashInfo() {
		if (inMouseClick && currentSashInfo.cursorOver != NO_WEIGHT)
			return;	// Don't process because we are in the down mouse button on an arrow.
			
		// We need to refigure size for the sash arrows.
		int[] addArrows = null;
		int[] drawArrows = null;
		int[] weights = getWeights();	// This should be two entries only. We shouldn't of gotton here if there were more than two.
		boolean vertical = getOrientation() == SWT.VERTICAL;
		if (noMaxUp) {
			addArrows = new int[1];
			drawArrows = new int[1];
			if (weights[1] == 0) {
				// Slammed to the bottom
				addArrows[0] = UP_ARROW;
				drawArrows[0] = UP_ARROW;
				currentSashInfo.sashBorderLeft = sashBorders != null ? sashBorders[0] : false;
				currentSashInfo.sashBorderRight = false;
			}
			else {
				// Not slammed
				addArrows[0] = DOWN_MAX_ARROW;
				drawArrows[0] = DOWN_ARROW;			
				currentSashInfo.weight = NO_WEIGHT;	// Since we are in the middle, there is no weight. We've could of been dragged here.
				currentSashInfo.sashBorderLeft = sashBorders != null ? sashBorders[0] : false;
				currentSashInfo.sashBorderRight = sashBorders != null ? sashBorders[1] : false;
			}				
		}
		else if (noMaxDown) {
			addArrows = new int[1];
			drawArrows = new int[1];
			if (weights[0] == 0) {
				// Slammed to the top
				addArrows[0] = DOWN_ARROW;
				drawArrows[0] = DOWN_ARROW;
				currentSashInfo.sashBorderLeft = false;
				currentSashInfo.sashBorderRight = sashBorders != null ? sashBorders[1] : false;				
			}
			else {
				// Not slammed
				addArrows[0] = UP_MAX_ARROW;
				drawArrows[0] = UP_ARROW;			
				currentSashInfo.weight = NO_WEIGHT;	// Since we are in the middle, there is no weight. We've could of been dragged here.
				currentSashInfo.sashBorderLeft = sashBorders != null ? sashBorders[0] : false;
				currentSashInfo.sashBorderRight = sashBorders != null ? sashBorders[1] : false;				
			}				
		}
		else {
			addArrows = new int[2];
			drawArrows = new int[2];	
			// TODO: SashForm as changed the folllwing is a temporary kludge
			Rectangle sashBounds = currentSashInfo.sash.getBounds();
			Rectangle clientArea = getClientArea();
			final int DRAG_MINIMUM = 20; // TODO: kludge see SashForm.DRAG_MINIMUM 
			if (weights[0] == 0 || (currentSashInfo.weight != NO_WEIGHT && 
					(vertical && sashBounds.y <= DRAG_MINIMUM || !vertical && sashBounds.x <= DRAG_MINIMUM))) {  
				// Slammed to the top.
				addArrows[0] = DOWN_MAX_ARROW;
				drawArrows[0] = DOWN_MAX_ARROW;			
				addArrows[1] = DOWN_ARROW;
				drawArrows[1] = DOWN_ARROW;	
				currentSashInfo.sashBorderLeft = false;
				currentSashInfo.sashBorderRight = sashBorders != null ? sashBorders[1] : false;
			}
			else if (weights[1] == 0 || (currentSashInfo.weight != NO_WEIGHT &&
					(vertical && sashBounds.y+sashBounds.height >= clientArea.height-DRAG_MINIMUM ||
					 !vertical && sashBounds.x + sashBounds.width >= clientArea.width-DRAG_MINIMUM))) {
				// Slammed to the bottom
				addArrows[0] = UP_ARROW;
				drawArrows[0] = UP_ARROW;
				addArrows[1] = UP_MAX_ARROW;
				drawArrows[1] = UP_MAX_ARROW;
				currentSashInfo.sashBorderLeft = sashBorders != null ? sashBorders[0] : false;
				currentSashInfo.sashBorderRight = false;
			}
			else {
				// Not slammed
				addArrows[0] = UP_MAX_ARROW;
				drawArrows[0] = UP_ARROW;			
				addArrows[1] = DOWN_MAX_ARROW;
				drawArrows[1] = DOWN_ARROW;
				currentSashInfo.weight = NO_WEIGHT;	// Since we are in the middle, there is no weight. We've could of been dragged here.
				currentSashInfo.sashBorderLeft = sashBorders != null ? sashBorders[0] : false;
				currentSashInfo.sashBorderRight = sashBorders != null ? sashBorders[1] : false;				
			}
		}
		getNewSashArray(currentSashInfo, addArrows, drawArrows);
		
		currentSashInfo.sash.redraw();	// Need to schedule a redraw because it has already drawn the old ones during the set bounds in super layout.
	}

	protected void upClicked(SashInfo sashinfo) {
		// This means restore just the sash below weight and reduce the above weight by the right amount.
		int[] weights = getWeights();
		
		weights[0] = 1000-sashinfo.weight;	// Assume weights are always in units of 1000.
		weights[1] = sashinfo.weight;
		sashinfo.weight = NO_WEIGHT;
		
		setWeights(weights);	
		fireDividerMoved();
	}
	
	protected void upMaxClicked(SashInfo sashinfo) {
		int[] weights = getWeights();

		// Up max, so save the current weight of 1 into the sash info, and move to the top.
		if (currentSashInfo.weight == NO_WEIGHT)
			currentSashInfo.weight = weights[1];	// Not currently maxed, save position.

		weights[1] = 1000;
		weights[0] = 0;
					
		// If the upper panel has focus, flip focus to the lower panel because the upper panel is now hidden.
		Control[] children = getChildren();
		boolean upperFocus = isFocusAncestorA(children[0]);
		setWeights(weights);
		if (upperFocus)
			children[1].setFocus();	
		fireDividerMoved();
	}

	protected void downClicked(SashInfo sashinfo) {
		// This means restore just the sash below weight and increase the above weight by that amount.
		int[] weights = getWeights();
		
		weights[0] = 1000-sashinfo.weight;	// Assume weights are always in units of 1000.
		weights[1] = sashinfo.weight;
		sashinfo.weight = NO_WEIGHT;
		
		setWeights(weights);
		fireDividerMoved();
	}
	
	protected void downMaxClicked(SashInfo sashinfo) {
		int[] weights = getWeights();

		// Down max, so save the current weight of 1 into the sash info, and move to the bottom.
		if (currentSashInfo.weight == NO_WEIGHT) 
			currentSashInfo.weight = weights[1];	// Not currently maxed, save current weight.
		weights[0] = 1000;
		weights[1] = 0;
		
		// If the lower panel has focus, flip focus to the upper panel because the lower panel is now hidden.
		Control[] children = getChildren();
		boolean lowerFocus = isFocusAncestorA(children[1]);
		setWeights(weights);
		if (lowerFocus)
			children[0].setFocus();		
		fireDividerMoved();
	}
	
	/*
	 * This determines if the control or one of its children
	 * has the focus. It was hidden by SWT, but it is really useful.
	 */
	protected boolean isFocusAncestorA (Control control) {
		Display display = getDisplay ();
		Control focusControl = display.getFocusControl ();
		while (focusControl != null && focusControl != control) {
			focusControl = focusControl. getParent();
		}
		return control == focusControl;
	}	

	protected void getNewSashArray(SashInfo sashInfo, int[] addArrowTypes, int[] drawArrowTypes) {

		int[][] thisSash = sashInfo.sashLocs;
		if (thisSash == null) 
			thisSash = sashInfo.sashLocs = new int[addArrowTypes.length][];
		
		int aSize = ARROW_SIZE;	// Width of arrow
		int tSize = aSize+2*ARROW_MARGIN;		// Total Width (arrow + margin)
		int neededSize = tSize*addArrowTypes.length;
		
		boolean vertical = getOrientation() == SWT.VERTICAL;
		Point s = sashInfo.sash.getSize();
		int start = 0;
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
		if (vertical) {
			start = (s.x - neededSize) / 2;
			x = start;
			y = (s.y - ARROW_HEIGHT) / 2;	// Center vertically, no margin required.
			width = tSize;
			height = aSize;
		}
		else {
			start = (s.y - neededSize) / 2;
			y = start;
			x = (s.x - ARROW_HEIGHT) / 2;	// Center horizontally, no margin required.
			width = aSize;
			height = tSize;
		}
		for (int j=0; j<addArrowTypes.length; j++) {
			if (thisSash[j] == null)
				thisSash[j] = new int[] {addArrowTypes[j], drawArrowTypes[j], x, y, width, height};
			else {
				// Reuse the array
				thisSash[j][ARROW_TYPE_INDEX] = addArrowTypes[j];
				thisSash[j][ARROW_DRAWN_INDEX] = drawArrowTypes[j];				
				thisSash[j][X_INDEX] = x;
				thisSash[j][Y_INDEX] = y;
				thisSash[j][WIDTH_INDEX] = width;
				thisSash[j][HEIGHT_INDEX] = height;				
			}
			if (vertical)
				x+=tSize;				
			else
				y+=tSize;
		}		
	}

	protected void drawSashBorder(GC gc, Sash sash, boolean leftBorder) {
		gc.setForeground(borderColor);
		if (getOrientation() == SWT.VERTICAL) {
			Point s = sash.getSize();
			if (leftBorder)
				gc.drawLine(0, 0, s.x-1, 0);
			else
				gc.drawLine(0, s.y-1, s.x-1, s.y-1);
		}
		else {
			Point s = sash.getSize();
			if (leftBorder)
				gc.drawLine(0, 0, 0, s.y-1);
			else
				gc.drawLine(s.x-1, 0, s.x-1, s.y-1);			
		}
	}
	
	protected void drawArrow(GC gc, int[] sashLoc, boolean selected) {
		int indent = 0;
		if (selected) {
			if (!inMouseClick) {
				// Draw the selection box.
				Color highlightShadow = getDisplay().getSystemColor(SWT.COLOR_WIDGET_HIGHLIGHT_SHADOW);
				Color normalShadow = getDisplay().getSystemColor(SWT.COLOR_WIDGET_NORMAL_SHADOW);
				gc.setForeground(highlightShadow);
				gc.drawLine(sashLoc[X_INDEX], sashLoc[Y_INDEX]+sashLoc[HEIGHT_INDEX], sashLoc[X_INDEX], sashLoc[Y_INDEX]);
				gc.drawLine(sashLoc[X_INDEX], sashLoc[Y_INDEX], sashLoc[X_INDEX]+sashLoc[WIDTH_INDEX], sashLoc[Y_INDEX]);
				
				gc.setForeground(normalShadow);
				gc.drawLine(sashLoc[X_INDEX], sashLoc[Y_INDEX]+sashLoc[HEIGHT_INDEX], sashLoc[X_INDEX]+sashLoc[WIDTH_INDEX], sashLoc[Y_INDEX]+sashLoc[HEIGHT_INDEX]);
				gc.drawLine(sashLoc[X_INDEX]+sashLoc[WIDTH_INDEX], sashLoc[Y_INDEX]+sashLoc[HEIGHT_INDEX], sashLoc[X_INDEX]+sashLoc[WIDTH_INDEX], sashLoc[Y_INDEX]);			
			}
			else {
				// Draw pushed selection box.
				indent = 1;
				Color highlightShadow = getDisplay().getSystemColor(SWT.COLOR_WIDGET_HIGHLIGHT_SHADOW);
				Color normalShadow = getDisplay().getSystemColor(SWT.COLOR_WIDGET_NORMAL_SHADOW);
				gc.setForeground(normalShadow);
				gc.drawLine(sashLoc[X_INDEX], sashLoc[Y_INDEX]+sashLoc[HEIGHT_INDEX], sashLoc[X_INDEX], sashLoc[Y_INDEX]);
				gc.drawLine(sashLoc[X_INDEX], sashLoc[Y_INDEX], sashLoc[X_INDEX]+sashLoc[WIDTH_INDEX], sashLoc[Y_INDEX]);
				
				gc.setForeground(highlightShadow);
				gc.drawLine(sashLoc[X_INDEX], sashLoc[Y_INDEX]+sashLoc[HEIGHT_INDEX], sashLoc[X_INDEX]+sashLoc[WIDTH_INDEX], sashLoc[Y_INDEX]+sashLoc[HEIGHT_INDEX]);
				gc.drawLine(sashLoc[X_INDEX]+sashLoc[WIDTH_INDEX], sashLoc[Y_INDEX]+sashLoc[HEIGHT_INDEX], sashLoc[X_INDEX]+sashLoc[WIDTH_INDEX], sashLoc[Y_INDEX]);			
			}					
		}
		if (getOrientation() == SWT.VERTICAL) {
			switch (sashLoc[ARROW_DRAWN_INDEX]) {
				case UP_ARROW:
					drawUpArrow(gc, sashLoc[X_INDEX]+indent, sashLoc[Y_INDEX]+indent);
					break;
				case DOWN_ARROW:
					drawDownArrow(gc, sashLoc[X_INDEX]+indent, sashLoc[Y_INDEX]+indent);
					break;
				case UP_MAX_ARROW:
					drawUpMaxArrow(gc, sashLoc[X_INDEX]+indent, sashLoc[Y_INDEX]+indent);
					break;
				case DOWN_MAX_ARROW:
					drawDownMaxArrow(gc, sashLoc[X_INDEX]+indent, sashLoc[Y_INDEX]+indent);
					break;
			}
		}
		else {
			switch (sashLoc[ARROW_DRAWN_INDEX]) {
				case UP_ARROW:
					drawLeftArrow(gc, sashLoc[X_INDEX]+indent, sashLoc[Y_INDEX]+indent);
					break;
				case DOWN_ARROW:
					drawRightArrow(gc, sashLoc[X_INDEX]+indent, sashLoc[Y_INDEX]+indent);
					break;
				case UP_MAX_ARROW:
					drawLeftMaxArrow(gc, sashLoc[X_INDEX]+indent, sashLoc[Y_INDEX]+indent);
					break;
				case DOWN_MAX_ARROW:
					drawRightMaxArrow(gc, sashLoc[X_INDEX]+indent, sashLoc[Y_INDEX]+indent);
					break;
			}			
		}
	}
	
	// These are for the up/down arrow. Just swap them for left/right arrow.
	protected static final int 
		ARROW_SIZE = 8,	
		ARROW_HEIGHT = 8,
		ARROW_MARGIN = 3;	// Margin on each side of arrow
		
	protected Color arrowColor, borderColor;
		
	// Draw at the given x/y (upper left corner of arrow area).
	protected void drawUpArrow(GC gc, int x, int y) { 
		gc.setForeground(arrowColor);
	
		x+=ARROW_MARGIN;
		gc.drawLine(x+4, y+2, x+7, y+5);
		gc.drawLine(x+3, y+2, x+3, y+2);
		
		gc.drawLine(x+2, y+3, x+4, y+3);
		gc.drawLine(x+1, y+4, x+5, y+4);
		gc.drawLine(x,   y+5, x+6, y+5);		
	}
	
	// Draw at the given x/y (upper left corner of arrow area).
	protected void drawUpMaxArrow(GC gc, int x, int y) {
		gc.setForeground(arrowColor);
	
		x+=ARROW_MARGIN;
		gc.drawLine(x,   y,   x+7, y);
		gc.drawLine(x,   y+1, x+7, y+1);
		
		gc.drawLine(x+4, y+2, x+7, y+5);
		gc.drawLine(x+3, y+2, x+3, y+2);
		
		gc.drawLine(x+2, y+3, x+4, y+3);
		gc.drawLine(x+1, y+4, x+5, y+4);
		gc.drawLine(x,   y+5, x+6, y+5);		
	}	

	// Draw at the given x/y (upper left corner of arrow area).
	protected void drawDownArrow(GC gc, int x, int y) {
		gc.setForeground(arrowColor);

		x+=ARROW_MARGIN;
		gc.drawLine(x,   y+2, x+3, y+5);
		gc.drawLine(x+4, y+5, x+4, y+5);
		
		gc.drawLine(x+3, y+4, x+5, y+4);
		gc.drawLine(x+1, y+3, x+6, y+3);
		gc.drawLine(x+1, y+2, x+7, y+2);
	}	
	
	// Draw at the given x/y (upper left corner of arrow area).
	protected void drawDownMaxArrow(GC gc, int x, int y) {
		gc.setForeground(arrowColor);

		x+=ARROW_MARGIN;
		gc.drawLine(x,   y+6, x+7, y+6);
		gc.drawLine(x,   y+7, x+7, y+7);

		gc.drawLine(x,   y+2, x+3, y+5);
		gc.drawLine(x+4, y+5, x+4, y+5);
		
		gc.drawLine(x+3, y+4, x+5, y+4);
		gc.drawLine(x+1, y+3, x+6, y+3);
		gc.drawLine(x+1, y+2, x+7, y+2);
	}	

	// Draw at the given x/y (upper left corner of arrow area).
	protected void drawLeftArrow(GC gc, int x, int y) { 
		gc.setForeground(arrowColor);

		y+=ARROW_MARGIN;
		gc.drawLine(x+2, y+4, x+5, y+7);
		gc.drawLine(x+2, y+3, x+2, y+3);
		
		gc.drawLine(x+3, y+2, x+3, y+4);
		gc.drawLine(x+4, y+1, x+4, y+5);
		gc.drawLine(x+5, y,   x+5, y+6);		
	}
	
	// Draw at the given x/y (upper left corner of arrow area).
	protected void drawLeftMaxArrow(GC gc, int x, int y) {
		gc.setForeground(arrowColor);

		y+=ARROW_MARGIN;
		gc.drawLine(x,   y, x,   y+7);
		gc.drawLine(x+1, y, x+1, y+7);
		
		gc.drawLine(x+2, y+4, x+5, y+7);
		gc.drawLine(x+2, y+3, x+2, y+3);
		
		gc.drawLine(x+3, y+2, x+3, y+4);
		gc.drawLine(x+4, y+1, x+4, y+5);
		gc.drawLine(x+5, y,   x+5, y+6);		
	}	

	// Draw at the given x/y (upper left corner of arrow area).
	protected void drawRightArrow(GC gc, int x, int y) {
		gc.setForeground(arrowColor);

		y+=ARROW_MARGIN;
		gc.drawLine(x+2, y,   x+5, y+3);
		gc.drawLine(x+5, y+4, x+5, y+4);
		
		gc.drawLine(x+4, y+3, x+4, y+5);
		gc.drawLine(x+3, y+1, x+3, y+6);
		gc.drawLine(x+2, y+1, x+2, y+7);
	}	
	
	// Draw at the given x/y (upper left corner of arrow area).
	protected void drawRightMaxArrow(GC gc, int x, int y) {
		gc.setForeground(arrowColor);

		y+=ARROW_MARGIN;
		gc.drawLine(x+6, y,   x+6, y+7);
		gc.drawLine(x+7, y,   x+7, y+7);

		gc.drawLine(x+2, y,   x+5, y+3);
		gc.drawLine(x+5, y+4, x+5, y+4);
		
		gc.drawLine(x+4, y+3, x+4, y+5);
		gc.drawLine(x+3, y+1, x+3, y+6);
		gc.drawLine(x+2, y+1, x+2, y+7);
	}

	
	public int getSavedWeight() {
		if (currentSashInfo!=null)
			return currentSashInfo.weight;
		else
			return -1;
	}

	
	protected Sash getSash() {
		Control[] kids = getChildren();
		for (int i = 0; i < kids.length; i++) {
			if (kids[i] instanceof Sash)
				return (Sash)kids[i];			
		}
		return null;
	}
	
	public void setCurrentSavedWeight(int weight) {
		if (weight>=0 && currentSashInfo!=null) {
			recomputeSashInfo();
			currentSashInfo.weight=weight;
		}
	}
	
	/**
	 * Adds a custom sashform listener. This listener will be removed when 
	 * this control is disposed.
	 * 
	 * @param listener
	 * 
	 * @since 1.2.0
	 */
	public void addCustomSashFormListener(ICustomSashFormListener listener){
		if(customSashFormListeners==null)
			customSashFormListeners = new ArrayList<ICustomSashFormListener>();
		customSashFormListeners.add(listener);
	}
	
	/**
	 * Removes the custom sashform listener.
	 * 
	 * @param listener
	 * 
	 * @since 1.2.0
	 */
	public void removeCustomSashFormListener(ICustomSashFormListener listener){
		if(customSashFormListeners!=null){
			customSashFormListeners.remove(listener);
		}
	}
	
	@SuppressWarnings("unchecked")
	protected void fireDividerMoved(){
		if(customSashFormListeners!=null && customSashFormListeners.size()>0){
			int[] weights = getWeights();
			if(weights!=null && weights.length==2){
				int firstControlWeight = weights[0];
				int secondControlWeight = weights[1];
				for (Iterator listenerItr = customSashFormListeners.iterator(); listenerItr.hasNext();) {
					ICustomSashFormListener listener = (ICustomSashFormListener) listenerItr.next();
					listener.dividerMoved(firstControlWeight, secondControlWeight);
				}
			}
		}
	}
	
}

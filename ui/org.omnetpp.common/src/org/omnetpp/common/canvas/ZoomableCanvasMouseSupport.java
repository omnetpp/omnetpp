/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.canvas;

import org.eclipse.draw2d.Cursors;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Cursor;
import org.omnetpp.common.ui.CustomCursors;

/**
 * Adds mouse bindings to a ZoomableCachingCanvas for zooming and panning.
 *
 * There are two operation modes that can be selected with setMouseMode():
 * panning (aka "hand") and zoom.
 *
 * Bindings in pan mode:
 *   - mouse drag: panning
 *   - wheel: vertical scroll
 *   - shift+wheel: horizontal scroll
 *   - ctrl+drag: zoom to rectangle dragged out
 *   - ctrl+wheel: zoom in/out
 *   - ctrl+leftclick: zoom in
 *   - ctrl+shift+leftclick: zoom in
 *
 * Bindings in zoom mode:
 *   - drag: zoom to rectangle dragged out
 *   - wheel: zoom in/out
 *   - leftclick: zoom in
 *   - shift+leftclick: zoom out
 *   - ctrl+drag: panning
 *
 * Note that the opposite mode's bindings are always available by
 * holding down the ctrl key.
 *
 * @author Andras
 */
public class ZoomableCanvasMouseSupport {
    // the adapted canvas
    protected ZoomableCachingCanvas canvas;

    // mouse pointers
    protected static final Cursor PAN_CURSOR = Cursors.SIZEALL;
    protected static final Cursor ZOOMIN_CURSOR = CustomCursors.ZOOMIN;
    protected static final Cursor ZOOMOUT_CURSOR = CustomCursors.ZOOMOUT;

    protected RubberbandSupport rubberBand;

    public static final int PAN_MODE = 0;
    public static final int ZOOM_MODE = 1;
    private int mouseMode;

    // remembers the previous mouse position during dragging
    private int dragPrevX;
    private int dragPrevY;

    // used tell apart click from drag; initialized to true to prevent initial
    // stray button-up event (ie end of the double-click) to zoom the canvas
    private boolean mousedMoved = true;

    // remembered because MouseMove doesn't send it
    private int activeMouseButton;

    /**
     * Sets up mouse bindings on the given canvas.
     */
    public ZoomableCanvasMouseSupport(final ZoomableCachingCanvas canvas) {
        this.canvas = canvas;
        setupMouseHandling();
        rubberBand = new RubberbandSupport(canvas, 0) {
            @Override
            public void rubberBandSelectionMade(Rectangle r) {
                canvas.zoomToRectangle(new org.eclipse.draw2d.geometry.Rectangle(r));
            }
        };
        setMouseMode(PAN_MODE);
    }

    public int getMouseMode() {
        return mouseMode;
    }

    public void setMouseMode(int mouseMode) {
        this.mouseMode = mouseMode;
        updateCursor(mouseMode, activeMouseButton, 0); // XXX should store pressed modifiers
        canvas.setCursor(mouseMode == ZOOM_MODE ? ZOOMIN_CURSOR : null);
        rubberBand.setModifierKeys(mouseMode==ZOOM_MODE ? SWT.NONE : SWT.MOD1);
    }

    /**
     * Calculates and sets the new mouse cursor.
     * To be called when the mouseMode changed, a mouse button pressed/released or
     * a modifier key pressed/released.
     */
    private void updateCursor(int mouseMode, int mouseButton, int modifierKeys) {
        boolean ctrl = (modifierKeys & SWT.MOD1) != 0;
        boolean shift = (modifierKeys & SWT.SHIFT) != 0;
//      Debug.format("updateCursor(%s,%s,%s)%n",
//              (mouseMode == PAN_MODE ? "pan" : "zoom"),
//              (mouseButton == 0 ? "no button pressed" : "button pressed"),
//              (shift && ctrl ? "ctrl+shift" : ctrl ? "ctrl" : shift ? "shift" : "none"));

        Cursor cursor;
        boolean zoomCursor = (mouseMode == PAN_MODE) && ctrl || (mouseMode == ZOOM_MODE) && !ctrl;
        if (zoomCursor) {
            cursor = shift ? ZOOMOUT_CURSOR : ZOOMIN_CURSOR;
        }
        else {
            // show the pan cursor if a mouse button pressed (dragging)
            // or clear the cursor, so other component can check and set it (e.g. crosshair)
            cursor = mouseButton != 0 ? PAN_CURSOR : null;
        }
        canvas.setCursor(cursor);
    }

    protected void setupMouseHandling() {
        // ctrl/shift key
        canvas.addListener(SWT.KeyDown, (e) ->  {
            if (e.doit && (e.keyCode & (SWT.MOD1 | SWT.SHIFT)) != 0) {
                // e.stateMask contains the state of the modifier keys before shift/ctrl pressed
                int stateMask = e.stateMask | e.keyCode;
                updateCursor(mouseMode, activeMouseButton, stateMask);
            }
        });

        canvas.addListener(SWT.KeyUp, (e) ->  {
            if (e.doit && (e.keyCode & (SWT.MOD1 | SWT.SHIFT)) != 0) {
                // e.stateMask contains the state of the modifier keys before shift/ctrl released
                int stateMask = e.stateMask & (~e.keyCode);
                updateCursor(mouseMode, activeMouseButton, stateMask);
            }
        });

        // wheel
        canvas.addListener(SWT.MouseWheel, (event) -> {
            if (!event.doit)
                return;

            int modifier = event.stateMask & SWT.MODIFIER_MASK;
            if ((mouseMode==ZOOM_MODE && modifier==SWT.NONE) || (mouseMode==PAN_MODE && modifier==SWT.MOD1)) {
                // zoom in/out
                for (int i = 0; i < event.count; i++)
                    canvas.zoomBy(1.1);
                for (int i = 0; i < -event.count; i++)
                    canvas.zoomBy(1.0 / 1.1);
            }
            else if (modifier==SWT.SHIFT) {
                // if not zooming: shift+wheel does horizontal scroll
                // XXX on the gtk platform the vertical scroll bar
                //     is scrolled even if the SHIFT button is pressed.
                //     do not scroll horizontally to avoid diagonal scrolling
                if (!"gtk".equals(SWT.getPlatform()))
                    canvas.scrollHorizontalTo(canvas.getViewportLeft() - canvas.getViewportWidth() * event.count / 20);
            }
        });

        // mouse button down / up
        canvas.addListener(SWT.MouseDown, (event) -> {
            if (!event.doit)
                return;
            canvas.setFocus();
            activeMouseButton = event.button;
            if (event.button == 1) {
                int modifier = event.stateMask & SWT.MODIFIER_MASK;
                updateCursor(mouseMode, activeMouseButton, modifier);
                dragPrevX = event.x;
                dragPrevY = event.y;
                mousedMoved = false;
            }
        });

        canvas.addListener(SWT.MouseUp, (event) -> {
            if (!event.doit)
                return;
            dragPrevX = dragPrevY = -1;
            activeMouseButton = 0;
            int modifier = event.stateMask & SWT.MODIFIER_MASK;
            updateCursor(mouseMode, activeMouseButton, modifier);
            if (!mousedMoved) {  // just a click
                if (event.button==1) {
                    if ((mouseMode==ZOOM_MODE && modifier==SWT.NONE) || (mouseMode==PAN_MODE && modifier==SWT.MOD1))
                        canvas.zoomBy(2.0, event.x, event.y); // zoom in around mouse
                    if ((mouseMode==ZOOM_MODE && modifier==SWT.SHIFT) || (mouseMode==PAN_MODE && modifier==(SWT.MOD1|SWT.SHIFT)))
                        canvas.zoomBy(1/2.0, event.x, event.y); // zoom out around mouse
                }
            }
        });



        // dragging ("hand" cursor)
        canvas.addListener(SWT.MouseMove, (event) -> {
            if (!event.doit)
                return;

            int modifier = event.stateMask & SWT.MODIFIER_MASK;
            if (activeMouseButton==1) { // drag with left mouse button being held down
                if ((mouseMode==PAN_MODE && modifier==SWT.NONE) || (mouseMode==ZOOM_MODE && modifier==SWT.MOD1)) {

                    // drag the chart
                    if (dragPrevX!=-1 && dragPrevY!=-1) {
                        // scroll by the amount moved since last drag call
                        int dx = event.x - dragPrevX;
                        int dy = event.y - dragPrevY;
                        canvas.scrollHorizontalTo(canvas.getViewportLeft() - dx);
                        canvas.scrollVerticalTo(canvas.getViewportTop() - dy);
                        dragPrevX = event.x;
                        dragPrevY = event.y;
                    }

                }
                mousedMoved = true;
            }
            else if (activeMouseButton==0) { // plain mouse move (no mouse button pressed)
                // restore cursor at end of drag. (It is not enough to do it in the
                // "mouse released" event, because we don't receive it if user
                // releases mouse outside the canvas!)
                updateCursor(mouseMode, activeMouseButton, modifier);
            }
        });
    }

    public void drawRubberband(Graphics graphics) {
        rubberBand.drawRubberband(graphics);
    }
}

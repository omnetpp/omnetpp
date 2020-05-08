/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.PaletteData;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.omnetpp.scave.engine.ScaveEngine;

/**
 * This is an SWT Control which is "one half" of a matplotlib FigureCanvas,
 * as a part of our matplotlib backend - in backend_SWTAgg.py.
 *
 * Displays the rendered image coming from the Python process, and forwards
 * the input events (mouse move, click, resize...) back to matplotlib in Python.
 *
 * Also takes care of setting the appropriate cursor type, and drawing the
 * tool "rectangle" (for the rectangle zoom tool).
 *
 * See also: https://matplotlib.org/3.2.1/api/backend_bases_api.html#matplotlib.backend_bases.FigureCanvasBase
 *
 * Implementation details:
 *
 * - The pixels of the image are transferred in a
 *   shared memory buffer for improved performance. This shared memory region
 *   is reused across frames, and only recreated when it becomes too small -
 *   when the user sized the control bigger.
 *
 * - The mouse move and resize events are categorized into so-called
 *   "event streams". The point of these is to cancel the delivery of all
 *   previous events in a stream, when a newer one belonging to the same stream
 *   arrives. This reduces input lag on resize and navigation considerably.
 *
 * - Aside from full frame renders, matplotlib also supports blitting a smaller
 *   portion of the canvas image, which can potentially increase performance
 *   when only a small part of it changes, for example with some animations,
 *   custom cursor lines, interactively showing-hiding annotations, etc..
 *   At the moment, these "subimage" pixels are transferred as byte[] objects
 *   straight through the Py4J connection, without using shared memory.
 *
 * - It seems that SWT has to internally convert our pixels into a different
 *   format - at least when using Cairo. (Maybe even twice: when we submit them,
 *   and then when they are drawn on the screen.) This seems unavoidable, and
 *   most likely really hurts interactive performance. Sad. :(
 *   See: https://www.eclipse.org/lists/platform-swt-dev/msg07533.html
 *
 */
public class MatplotlibWidget extends Canvas implements IMatplotlibWidget {

    /** This is backed by the shared memory used to receive the image to show.
     * Contains raw RGBA pixels - in "some" channel ordering.
     */
    private ByteBuffer buf;

    /**
     * The last received image, plus the since blitted regions applied to it.
     * This might not always be the same size as this Control itself,
     * either because we are using the (internal) halfRes option; or because
     * a resize event is "in-flight": the Control has been resized, but the
     * new pixel data from Python has not arrived yet.
     */
    private ImageData imageData;

    /**
     * Used to draw the contents of imageData on screen.
     * The imageData is submitted to the SWT Display (and automatically
     * converted to the appropriate pixel format) through this.
     */
    private Image image;

    /**
     * Whether to draw the last received frame stretched during resizing,
     * until the new image with correct size arrives.
     * This seems to improve responsiveness by "cheating", but can sometimes
     * draw ugly glitches for a short time when resizing is too quick.
     * If set to false, the last frame will always be aligned on the top-left
     * corner without changing its size at all.
     */
    private boolean stretchOnResize = true;

    /**
     * Which matplotlib figure this canvas belongs to.
     */
    public int figureNumber = 0;

    /**
     * A reference to the "other half" (in Python) of this FigureCanvas.
     */
    private IMatplotlibFigureCanvas pyCanvas = null;

    /**
     * The Python process which hosts the other half of this FigureCanvas.
     */
    private PythonProcess pythonProcess;

    /**
     * Is true when the mouse pointer is currently within the area of this Canvas.
     * Controls whether the message is drawn on top of the image.
     */
    private boolean mouseIsOverMe;


    private boolean isRefreshing = false;

    /**
     * Stores the last received message (intended for the user).
     * This "message" is a standard part of any matplotlib backend.
     */
    private String message;

    /**
     * Stores the last received warning (intended for the user).
     * This is our custom addition, it has nothing to do with matplotlib.
     */
    private String warning;

    /**
     * Stores the cursor type last set by matplotlib.
     * Used to control whether the context menu should be shown on right-click,
     * so it does not interfere with the zoom actions, which use the right button.
     */
    private int cursorType = SWT.CURSOR_ARROW;

    /**
     * The context menu to show when requested. Set up externally and passed in.
     */
    private Menu contextMenu;

    /**
     * The currently drawn "tool rectangle" - used by rectangle zoom for example.
     */
    private ArrayList<Integer> rect = new ArrayList<Integer>();

    /**
     * Event stream identifier for mouse move events.
     * Passed to PythonCallerThread.asyncExec from SWT event handlers.
     */
    private static final int EVENTSTREAM_MOUSEMOVE = 1;

    /**
     * Event stream identifier for mouse move events.
     * Passed to PythonCallerThread.asyncExec from SWT event handlers.
     */
    private static final int EVENTSTREAM_RESIZE = 2;

    /**
     * Creates a new MatplotlibWidget in the given parent Control, with the
     * given style. The other half of this FigureCanvas, canvas, is in the
     * Python process proc. Mostly just sets up a bunch of event listeners.
     */
    public MatplotlibWidget(Composite parent, int style, PythonProcess proc, IMatplotlibFigureCanvas canvas) {
        super(parent, style);

        this.pyCanvas = canvas;
        this.pythonProcess = proc;

        addPaintListener(e -> {
            // draw the last stored frame (including blitted parts), stretching it if enabled and needed
            if (image != null) {
                int width = image.getImageData().width;
                int height = image.getImageData().height;

                if (stretchOnResize) {
                    if (width == getSize().x && height == getSize().y)
                        e.gc.drawImage(image, 0, 0); // this is simply an optimization
                    else
                        e.gc.drawImage(image, 0, 0, width, height, 0, 0, getSize().x, getSize().y);
                }
                else
                    e.gc.drawImage(image, 0, 0);
            }

            // draw the message, if any, only if the mouse is over the canvas
            if ((isRefreshing || mouseIsOverMe) && message != null && !message.isEmpty()) {
                e.gc.setFont(JFaceResources.getTextFont());
                Point textSize = e.gc.textExtent(message);
                e.gc.fillRectangle(6, getSize().y - 14 - textSize.y, textSize.x + 20, textSize.y + 8);
                e.gc.drawText(message, 16, getSize().y - 10 - textSize.y, true);
            }

            // draw the warning if there is one
            if (warning != null && !warning.isEmpty()) {
                e.gc.setFont(JFaceResources.getTextFont());
                e.gc.setForeground(new Color(e.gc.getDevice(), 255, 31, 0));
                e.gc.setAlpha(255);
                e.gc.drawText(warning, 16, 14, true);
            }

            // draw the tool rectangle if needed
            if (!rect.isEmpty()) {
                Color rectColor = getDisplay().getSystemColor(SWT.COLOR_LIST_SELECTION);

                e.gc.setBackground(rectColor);
                e.gc.setAlpha(64);
                e.gc.fillRectangle(rect.get(0), rect.get(1), rect.get(2), rect.get(3));

                e.gc.setForeground(rectColor);
                e.gc.setAlpha(255);
                e.gc.drawRectangle(rect.get(0), rect.get(1), rect.get(2), rect.get(3));
            }
        });

        // forward mouse move events to Python, in an "event stream"
        addMouseMoveListener(e -> {
            int sy = getSize().y;
            if (pythonProcess != null && pythonProcess.isAlive()) {
                pythonProcess.pythonCallerThread.asyncExec(() -> getCanvas().mouseMoveEvent(e.x, sy - e.y),
                        figureNumber * 100 + EVENTSTREAM_MOUSEMOVE);
            }
        });

        addMouseTrackListener(new MouseTrackListener() {
            @Override
            public void mouseHover(MouseEvent arg0) {
            }

            @Override
            public void mouseExit(MouseEvent arg0) {
                if (pythonProcess != null && pythonProcess.isAlive()) {
                    pythonProcess.pythonCallerThread.asyncExec(() -> getCanvas().leaveEvent());
                    mouseIsOverMe = false;
                    redraw();
                }
            }

            @Override
            public void mouseEnter(MouseEvent e) {
                if (pythonProcess != null && pythonProcess.isAlive()) {
                    pythonProcess.pythonCallerThread.asyncExec(() -> getCanvas().enterEvent(e.x, e.y));
                    mouseIsOverMe = true;
                    redraw();
                }
            }
        });

        addMouseListener(new MouseListener() {
            @Override
            public void mouseUp(MouseEvent e) {
                if (pythonProcess != null && pythonProcess.isAlive()) {
                    int sy = getSize().y;
                    pythonProcess.pythonCallerThread.asyncExec(() -> getCanvas().mouseReleaseEvent(e.x, sy - e.y, e.button));
                }
            }

            @Override
            public void mouseDown(MouseEvent e) {
                if (pythonProcess != null && pythonProcess.isAlive()) {
                    int sy = getSize().y;
                    if (e.button == 3 && cursorType == SWT.CURSOR_ARROW) {
                        if (contextMenu != null)
                            contextMenu.setVisible(true);
                    }
                    else
                        pythonProcess.pythonCallerThread.asyncExec(() -> getCanvas().mousePressEvent(e.x, sy - e.y, e.button));
                }
                setFocus();
            }

            @Override
            public void mouseDoubleClick(MouseEvent e) {
                if (pythonProcess != null && pythonProcess.isAlive()) {
                    int sy = getSize().y;
                    pythonProcess.pythonCallerThread
                            .asyncExec(() -> getCanvas().mouseDoubleClickEvent(e.x, sy - e.y, e.button));
                }
            }
        });

        addControlListener(new ControlListener() {
            @Override
            public void controlResized(ControlEvent e) {
                if (pythonProcess != null && pythonProcess.isAlive()) {
                    int x = getSize().x;
                    int y = getSize().y;
                    if (x > 2 && y > 2)
                        pythonProcess.pythonCallerThread.asyncExec(() -> getCanvas().resizeEvent(x, y), figureNumber * 100 + EVENTSTREAM_RESIZE);
                }
            }

            @Override
            public void controlMoved(ControlEvent arg0) {
            }
        });
    }

    @Override
    public void setSharedMemoryNameAndSize(String name, long size) {
        if (buf != null)
            ScaveEngine.unmapSharedMemory(buf);
        buf = ScaveEngine.mapSharedMemory(name, size);
    }

    @Override
    public void setPixelsShared(int w, int h) {
        if (isRefreshing)
            setMessage(null);
        isRefreshing = false;

        if (buf == null || buf.capacity() == 0)
            return; // avoids InvalidArgumentException

        // copy the data synchronously
        Display.getDefault().syncExec(() -> {
            if (imageData == null || w != imageData.width || h != imageData.height) {
                PaletteData palette = new PaletteData(0xFF000000, 0xFF0000, 0xFF00);
                imageData = new ImageData(w, h, 32, palette);
            }
            // Have to up-cast buf because Java 9 added a covariant
            // override of rewind to ByteBuffer, so we weren't always
            // compatible (got NoSuchMethodException).
            // see: https://github.com/eclipse/jetty.project/issues/3244
            ((Buffer)buf).rewind();
            buf.get(imageData.data);

        });

        // then update the image itself and redraw asynchronously, because this part takes longer
        Display.getDefault().asyncExec(() -> {
            if (!isDisposed()) {
                if (image != null)
                    image.dispose();

                // this is actually the slowest part... consider using BufferedImage or
                // something like that?
                image = new Image(getDisplay(), imageData);

                redraw();
                update();
            }

        });

    }

    @Override
    public void dispose() {
        if (buf != null)
            ScaveEngine.unmapSharedMemory(buf);
        if (image != null)
            image.dispose();
        super.dispose();
    }

    /**
     * This is not used by default at the moment.
     */
    @Override
    public void setPixels(byte[] pixels, int w, int h) {
        if (isRefreshing)
            setMessage(null);
        isRefreshing = false;

        Display.getDefault().asyncExec(() -> {
            if (!isDisposed()) {
                if (pixels.length == 0)
                    return; // avoids InvalidArgumentException

                PaletteData palette = new PaletteData(0xFF000000, 0xFF0000, 0xFF00);
                ImageData imageData = new ImageData(w, h, 32, palette, 1, pixels);

                if (image != null)
                    image.dispose();

                // this is actually the slowest part... consider using BufferedImage or
                // something like that?
                image = new Image(getDisplay(), imageData);

                redraw();
                update();
            }
        });
    }

    public void rebindToNewProcess(PythonProcess pythonProcess, IMatplotlibFigureCanvas canvas) {
        this.pythonProcess = pythonProcess;
        this.pyCanvas = canvas;
        int x = getSize().x;
        int y = getSize().y;
        isRefreshing = true;

        pythonProcess.pythonCallerThread.asyncExec(() -> getCanvas().resizeEvent(x, y), figureNumber * 100 + EVENTSTREAM_RESIZE);

        redraw();
        update();
    }

    @Override
    public void blit(byte[] pixels, int x, int y, int w, int h) {
        if (isRefreshing)
            setMessage(null);
        isRefreshing = false;
        Display.getDefault().asyncExec(() -> {
            if (!isDisposed()) {
                // TODO: could be optimized

                GC gc = new GC(image);

                // this is ARGB, and maybe a different endianness, so BGRA, which is
                // different than in setPixels
                PaletteData palette = new PaletteData(0xFF00, 0xFF0000, 0xFF000000);
                ImageData imageData = new ImageData(w, h, 32, palette, 1, pixels);

                Image im = new Image(getDisplay(), imageData);

                gc.drawImage(im, x, y);

                gc.dispose();

                redraw();
                update();
            }
        });
    }

    @Override
    public void setRect(ArrayList<Integer> rect) {
        this.rect = rect;
    }

    @Override
    public void clearRect() {
        this.rect.clear();
    }

    @Override
    public void setCursorType(int cursor) {
        // in matplotlib/backend_tools.py: HAND, POINTER, SELECT_REGION, MOVE, WAIT = range(5)
        int[] cursors = new int[] { SWT.CURSOR_HAND, SWT.CURSOR_ARROW, SWT.CURSOR_CROSS, SWT.CURSOR_SIZEALL, SWT.CURSOR_WAIT };
        cursorType = cursors[cursor];
        Display.getDefault().asyncExec(() -> setCursor(new Cursor(getDisplay(), cursorType)));
    }

    @Override
    public void setMessage(String s) {
        if (message == null || !message.equals(s)) {
            message = s;
            Display.getDefault().asyncExec(() -> { if (!isDisposed()) redraw(); } );
        }
    }

    @Override
    public void setWarning(String s) {
        if (warning == null || !warning.equals(s)) {
            warning = s;
            Display.getDefault().asyncExec(() -> { if (!isDisposed()) redraw(); } );
        }
    }

    public void setMenu(Menu menu) {
        // not calling super.setMenu, we will show it manually
        contextMenu = menu;
    }

    public PythonProcess getPythonProcess() {
        return pythonProcess;
    }

    public IMatplotlibFigureCanvas getCanvas() {
        return pyCanvas;
    }

    public Image getImage() {
        return image;
    }

    @Override
    protected void checkSubclass() {
    }

    @Override
    public Point computeSize(int wHint, int hHint) {
        return imageData == null ? new Point(800, 600) : new Point(imageData.width, imageData.height);
    }
    @Override
    public Point computeSize(int wHint, int hHint, boolean changed) {
        return imageData == null ? new Point(800, 600) : new Point(imageData.width, imageData.height);
    }
}

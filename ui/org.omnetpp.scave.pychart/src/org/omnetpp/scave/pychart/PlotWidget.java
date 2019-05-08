package org.omnetpp.scave.pychart;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.Arrays;

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

public class PlotWidget extends Canvas implements IPlotWidget {
    Image image;
    boolean imageIsHalfRes;

    // this seems to improve responsiveness by "cheating", but can sometimes
    // draw ugly glitches for a short time when resizing is too quick
    boolean stretchOnResize = true;

    public int figureNumber = 0;

    IPyFigureCanvas pyCanvas = null;
    PythonProcess pythonProcess;

    boolean mouseIsOverMe;
    boolean isRefreshing = false;
    String message;

    int cursorType = SWT.CURSOR_ARROW;
    Menu contextMenu;

    ArrayList<Integer> rect = new ArrayList<Integer>();

    // Event stream identifiers to pass to PythonCallerThread.asyncExec from SWT event handlers.
    static final int EVENTSTREAM_MOUSEMOVE = 1;
    static final int EVENTSTREAM_RESIZE = 2;

    private static byte[] intsToBytes(int[] ints) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(ints.length * 4);
        byteBuffer.asIntBuffer().put(ints);
        return byteBuffer.array();
    }

    private static int[] bytesToInts(byte[] bytes) {
        ByteBuffer byteBuffer = ByteBuffer.wrap(bytes);
        int[] ints = new int[bytes.length / 4];
        byteBuffer.asIntBuffer().get(ints);
        return ints;
    }

    public PlotWidget(Composite parent, int style, PythonProcess proc, IPyFigureCanvas canvas) {
        super(parent, style);

        pyCanvas = canvas;
        this.pythonProcess = proc;

        addPaintListener(e -> {
            if (image != null) {
                int width = image.getImageData().width;
                int height = image.getImageData().height;

                if (stretchOnResize) {
                    if (width == getSize().x && height == getSize().y)
                        e.gc.drawImage(image, 0, 0); // this is simply an optimization
                    else
                        e.gc.drawImage(image, 0, 0, width, height, 0, 0, getSize().x, getSize().y);
                }
                else {
                    if (imageIsHalfRes)
                        e.gc.drawImage(image, 0, 0, width, height, 0, 0, width * 2, height * 2);
                    else
                        e.gc.drawImage(image, 0, 0);
                }
            }

            if ((isRefreshing || mouseIsOverMe) && message != null && !message.isEmpty()) {
                e.gc.setFont(JFaceResources.getTextFont());
                Point textSize = e.gc.textExtent(message);
                e.gc.fillRectangle(6, getSize().y - 14 - textSize.y, textSize.x + 20, textSize.y + 8);
                e.gc.drawText(message, 16, getSize().y - 10 - textSize.y, true);
            }

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
            public void mouseEnter(MouseEvent arg0) {
                if (pythonProcess != null && pythonProcess.isAlive()) {
                    pythonProcess.pythonCallerThread.asyncExec(() -> getCanvas().enterEvent());
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
    public void setPixels(byte[] pixels, int w, int h, boolean rleCoded, boolean halfRes) {
        if (isRefreshing)
            setMessage(null);
        isRefreshing = false;

        Display.getDefault().asyncExec(() -> {
            if (!isDisposed()) {
                // just to capture, and be able to overwrite it
                byte[] m_pixels = pixels;

                if (rleCoded) {
                    int[] intPixels = bytesToInts(pixels);
                    IntBuffer decodedInts = IntBuffer.allocate(w * h);

                    // decoding RLE
                    for (int i = 0; i < intPixels.length / 2; ++i) {
                        // might be _slightly_ faster if we only allocated one big array
                        // once in the beginning, and always used that, with explicit length
                        // parameters to fill and put... but it's also more complicated.
                        // (measurements so far did not show any significant speedup)
                        int[] arr = new int[intPixels[i * 2]];
                        Arrays.fill(arr, intPixels[i * 2 + 1]);
                        decodedInts.put(arr);
                    }

                    m_pixels = intsToBytes(decodedInts.array());
                }

                if (m_pixels.length == 0)
                    return; // avoids InvalidArgumentException

                PaletteData palette = new PaletteData(0xFF000000, 0xFF0000, 0xFF00);
                ImageData imageData = new ImageData(w, h, 32, palette, 1, m_pixels);

                if (image != null)
                    image.dispose();

                // this is actually the slowest part... consider using BufferedImage or
                // something like that?
                image = new Image(getDisplay(), imageData);
                imageIsHalfRes = halfRes;

                redraw();
                update();
            }
        });
    }

    public void rebindToNewProcess(PythonProcess pythonProcess, IPyFigureCanvas canvas) {
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
        int[] cursors = new int[] { SWT.CURSOR_HAND, SWT.CURSOR_ARROW, SWT.CURSOR_CROSS, SWT.CURSOR_SIZEALL };
        cursorType = cursors[cursor];
        Display.getDefault().asyncExec(() -> setCursor(new Cursor(getDisplay(), cursorType)));
    }

    @Override
    public void setMessage(String s) {
        if (message == null || !message.equals(s)) {
            message = s;
            Display.getDefault().asyncExec(() -> redraw());
        }
    }

    public void setMenu(Menu menu) {
        // not calling super.setMenu, we will show it manually
        contextMenu = menu;
    }

    public PythonProcess getPythonProcess() {
        return pythonProcess;
    }

    public IPyFigureCanvas getCanvas() {
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
        return new Point(800, 600);
    }
    @Override
    public Point computeSize(int wHint, int hHint, boolean changed) {
        return new Point(800, 600);
    }
}

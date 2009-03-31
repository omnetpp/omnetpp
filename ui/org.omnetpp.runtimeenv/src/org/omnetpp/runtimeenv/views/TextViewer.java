package org.omnetpp.runtimeenv.views;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyledTextContent;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseWheelListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontMetrics;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.ScrollBar;
import org.omnetpp.common.color.ColorFactory;


/**
 * For efficient viewing of a large amount of text. We use this instead of 
 * StyledText plus a custom StyledTextContent class, because StyledTextRenderer 
 * is very slow. Its throughput is about 20..30 events/sec, while this class
 * has about 200..300 (counting with average 2 lines/event). Also, this class 
 * has constant memory consumption (StyleTextRenderer consumes minimum 8 bytes 
 * per line, see lineWidth[] and lineHeight[] arrays).
 * 
 * @author Andras
 */
public class TextViewer extends Canvas {
    protected StyledTextContent content = null;
    protected Font font;
    protected Color backgroundColor = ColorFactory.WHITE;
    protected Color foregroundColor = ColorFactory.BLACK;
    protected int leftMargin = 0;
    protected int lineHeight, averageCharWidth; // measured from font
    protected int topLineIndex;
    protected int horizontalScrollOffset; // in pixels
    protected boolean caretState;
    protected int caretLineIndex, caretColumn;
    protected int selectionStartLineIndex, selectionStartColumn;
    protected int selectionEndLineIndex, selectionEndColumn;

    // all-in-one listener
    protected class TextViewerListener implements PaintListener, KeyListener, MouseListener, MouseMoveListener, MouseWheelListener {
        @Override
        public void paintControl(PaintEvent e) {
            handlePaintEvent(e);
        }

        @Override
        public void keyPressed(KeyEvent e) {
            //TODO see StyledText: createKeyBindings(), invokeAction()
            caretState = true;  //XXX also restart blink timer
            switch (e.keyCode) {
                case SWT.ARROW_UP:
                    caretLineIndex = Math.max(0, caretLineIndex-1);
                    adjustTopIndex();
                    redraw();
                    adjustScrollbars();
                    break;
                case SWT.ARROW_DOWN:
                    caretLineIndex = Math.max(0, Math.min(content.getLineCount()-1, caretLineIndex+1));
                    adjustTopIndex();
                    redraw();
                    adjustScrollbars();
                    break;
                case SWT.ARROW_LEFT:
                    if (caretColumn > 0) 
                        caretColumn--;
                    else if (caretLineIndex > 0) {
                        caretLineIndex--;
                        caretColumn = content.getLine(caretLineIndex).length();
                    }
                    adjustTopIndex();
                    redraw();
                    adjustScrollbars();
                    break;
                case SWT.ARROW_RIGHT:
                    if (caretColumn < content.getLine(caretLineIndex).length())
                        caretColumn++;
                    else if (caretLineIndex < content.getLineCount()-1) {
                        caretLineIndex++;
                        caretColumn = 0;
                    }
                    adjustTopIndex();
                    redraw();
                    adjustScrollbars();
                    break;
                case SWT.PAGE_DOWN:
                    int pageLines = Math.max(1, getNumVisibleLines()-1);
                    caretLineIndex += pageLines;
                    topLineIndex += pageLines;
                    if (caretLineIndex >= content.getLineCount())
                        caretLineIndex = content.getLineCount()-1;
                    redraw();
                    adjustScrollbars();
                    break;
                case SWT.PAGE_UP:
                    int pageLines2 = Math.max(1, getNumVisibleLines()-1);
                    caretLineIndex -= pageLines2;
                    topLineIndex -= pageLines2;
                    if (caretLineIndex < 0)
                        caretLineIndex = 0;
                    redraw();
                    adjustScrollbars();
                    break;
                case SWT.HOME:
                    caretColumn = 0;
                    redraw();
                    adjustScrollbars();
                    break;
                case SWT.END:
                    caretColumn = content.getLine(caretLineIndex).length();
                    redraw();
                    adjustScrollbars();
                    break;
            }
        }

        @Override
        public void keyReleased(KeyEvent e) {
            // TODO Auto-generated method stub
        }

        @Override
        public void mouseDoubleClick(MouseEvent e) {
            // TODO Auto-generated method stub
        }

        @Override
        public void mouseDown(MouseEvent e) {
            Point pos = getLineColumnAt(e.x, e.y);
            caretLineIndex = pos.y;
            caretColumn = pos.x;
            redraw();
        }

        @Override
        public void mouseUp(MouseEvent e) {
            // TODO Auto-generated method stub
        }

        @Override
        public void mouseMove(MouseEvent e) {
            // TODO Auto-generated method stub
        }

        @Override
        public void mouseScrolled(MouseEvent e) {
            // TODO Auto-generated method stub
        }
    }
    
    public TextViewer(Composite parent, int style) {
        super(parent, style | SWT.H_SCROLL | SWT.V_SCROLL | SWT.NO_REDRAW_RESIZE | SWT.DOUBLE_BUFFERED | SWT.NO_BACKGROUND);
        
        setFont(JFaceResources.getTextFont());
        
        installListeners();
        
        // cursor blinking
        //XXX disable if not focused
        Display.getCurrent().timerExec(500, new Runnable() {
            @Override
            public void run() {
                if (!isDisposed()) {
                    caretState = !caretState;
                    redraw();
                    Display.getCurrent().timerExec(500, this);
                }
            }
        });
        
    }

    protected void installListeners() {
        TextViewerListener listener = new TextViewerListener();
        addPaintListener(listener);
        addKeyListener(listener);
        addMouseListener(listener);
        addMouseMoveListener(listener);
        addMouseWheelListener(listener);
        
        ScrollBar verticalBar = getVerticalBar();
        if (verticalBar != null) {
            verticalBar.addListener(SWT.Selection, new Listener() {
                public void handleEvent(Event event) {
                    handleVerticalScroll(event);
                }
            });
        }
        ScrollBar horizontalBar = getHorizontalBar();
        if (horizontalBar != null) {
            horizontalBar.addListener(SWT.Selection, new Listener() {
                public void handleEvent(Event event) {
                    handleHorizontalScroll(event);
                }
            });
        }

    }

    public void setContent(StyledTextContent content) {
        this.content = content;
        redraw();
    }

    public StyledTextContent getContent() {
        return content;
    }
    
    public void setFont(Font font) {
        this.font = font;
        
        GC gc = new GC(Display.getCurrent());
        gc.setFont(font);
        FontMetrics fontMetrics = gc.getFontMetrics();
        lineHeight = fontMetrics.getHeight();
        averageCharWidth = fontMetrics.getAverageCharWidth();
        gc.dispose();
        
        redraw();
    }

    public Font getFont() {
        return font;
    }

    public void setBackgroundColor(Color backgroundColor) {
        this.backgroundColor = backgroundColor;
        redraw();
    }

    public Color getBackgroundColor() {
        return backgroundColor;
    }

    public void setForegroundColor(Color foregroundColor) {
        this.foregroundColor = foregroundColor;
        redraw();
    }

    public Color getForegroundColor() {
        return foregroundColor;
    }

    public void setTopLineIndex(int topLineIndex) {
        this.topLineIndex = topLineIndex;
        adjustScrollbars();
        redraw();
    }

    public int getTopLineIndex() {
        return topLineIndex;
    }

    public void setCaretPosition(int lineIndex, int column) {
        caretLineIndex = lineIndex;
        caretColumn = column;
    }

    public int getCaretLineIndex() {
        return caretLineIndex;
    }

    public int getCaretColumn() {
        return caretColumn;
    }

    public void setSelectionStart(int lineIndex, int column) {
        selectionStartLineIndex = lineIndex;
        selectionStartColumn = column;
        redraw();
    }

    public int getSelectionStartLineIndex() {
        return selectionStartLineIndex;
    }

    public int getSelectionStartColumn() {
        return selectionStartColumn;
    }

    public void setSelectionEnd(int lineIndex, int column) {
        selectionEndLineIndex = lineIndex;
        selectionEndColumn = column;
        redraw();
    }

    public int getSelectionEndLineIndex() {
        return selectionEndLineIndex;
    }

    public int getSelectionEndColumn() {
        return selectionEndColumn;
    }

    public int getLineHeight() {
        return lineHeight;
    }

    public int getNumVisibleLines() {
        return  (getClientArea().height + lineHeight - 1) / lineHeight;
    }

    public int getNumVisibleColumns() {
        return  (getClientArea().width + averageCharWidth - 1) / averageCharWidth;
    }

    public Point getLineColumnAt(int x, int y) {
        int lineIndex = topLineIndex + y / lineHeight;
        int column = (x - leftMargin + horizontalScrollOffset) / averageCharWidth; //XXX this only works for monospace fonts
        lineIndex = Math.max(0, Math.min(content.getLineCount()-1, lineIndex));
        column = Math.max(0, column);
        return new Point(column, lineIndex);
    }

    protected void adjustTopIndex() {
        // TODO Auto-generated method stub
    }

    protected void handlePaintEvent(PaintEvent e) {
        GC gc = e.gc;
        gc.setBackground(backgroundColor);
        gc.setForeground(foregroundColor);
        gc.setFont(font);
        
        Point size = getSize();
        gc.fillRectangle(0, 0, size.x, size.y);
        
        int lineIndex = topLineIndex;
        int numLines = content.getLineCount();
        int numVisibleLines = getNumVisibleLines();

        // adjust lineIndex
        if (lineIndex > numLines-numVisibleLines)
            lineIndex = numLines-numVisibleLines;
        if (lineIndex < 0)  
            lineIndex = 0;

        int x = leftMargin - horizontalScrollOffset;
        
        // draw the lines
        for (int y = 0; y < size.y && lineIndex < numLines; y += lineHeight) {
            drawLine(gc, lineIndex++, x, y);
        }
        
        configureScrollbars(); //FIXME surely not here! (also: this cuts performance in half!)
    }

    protected void drawLine(GC gc, int lineIndex, int x, int y) {
        String line = content.getLine(lineIndex);
        gc.drawString(line, x, y);
        
        if (lineIndex == caretLineIndex && caretState) {
            // draw caret
            String linePrefix = caretColumn >= line.length() ? line : line.substring(0, caretColumn);
            int caretX = x + gc.textExtent(linePrefix).x;
            gc.drawRectangle(caretX, y, 1, lineHeight-1);
        }
    }

    protected void configureScrollbars() {
        int numVisibleLines = getNumVisibleLines();
        ScrollBar vsb = getVerticalBar();
        vsb.setValues(vsb.getSelection(), 0, content.getLineCount(), numVisibleLines, 1, numVisibleLines);
        
        int maxWidth = 200*averageCharWidth; //FIXME
        int width = getClientArea().width;
        ScrollBar hsb = getHorizontalBar();
        hsb.setValues(hsb.getSelection(), 0, maxWidth, width, 4, width);
    }

    protected void handleVerticalScroll(Event event) {
        topLineIndex = getVerticalBar().getSelection();
        redraw();
    }

    protected void handleHorizontalScroll(Event event) {
        horizontalScrollOffset = getHorizontalBar().getSelection();
        redraw();
    }

    protected void adjustScrollbars() {
        getVerticalBar().setSelection(topLineIndex);
        getHorizontalBar().setSelection(horizontalScrollOffset);
    }

}

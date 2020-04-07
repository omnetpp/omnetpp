/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.largetable;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.operation.IRunnableContext;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.graphics.Transform;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Layout;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.ScrollBar;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.Debug;
import org.omnetpp.common.collections.IntRangeSet;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.util.CsvWriter;

/**
 * Custom-drawn table control that can display a virtually unlimited number of
 * elements without performance degradation. The number of elements is only
 * limited by the range of 'int'. Performance is O(1), i.e. independent of
 * the total number of rows.
 * <p>
 * Model data are not stored or cached inside the widget. Instead,
 * the table only knows how many rows it has, and a row renderer
 * does the actual rendering. A cell is primarily rendered as an
 * (optional) image plus a styled string, but completely owner-drawn
 * cells are also possible.
 * <p>
 * Although the implementation of this class has been inspired by VirtualTable,
 * this control intentionally implements a low-level SWT widget-like interface
 * instead of a content provider / label provider / ISelection style interface
 * that TableViewer, and to some degree, VirtualTable, provide.
 * <p>
 * The table header is that of an actual SWT Table, thus blends nicely with
 * other tables in the UI.
 * <p>
 * Motivation and Notes:
 * <p>
 * The SWT Table widget is much more scalable with the SWT.VIRTUAL flag than without it.
 * In the GTK3 version, performance starts to break down (at least in the ) around
 * 100,000 rows: setItemCount() starts to take several seconds when the item count
 * is either inflated or reduced by more than a few 10 thousand rows. The offenders
 * are the gtk_list_store_append() and gtk_list_store_remove() methods that are
 * called for each added/removed item in a loop. Other parts of Table seem to scale
 * well, but this behavior of setItemCount() prohibits Table's use for more than
 * a few thousand rows.
 * <p>
 * Similar table implementations that were considered but eventually not chosen:
 * - KTable (https://sourceforge.net/projects/ktable)
 * - NatTable (part of Nebula, https://www.eclipse.org/nattable)
 * - CTable (part of the SWT examples, see https://github.com/eclipse/eclipse.platform.swt)
 * <p>
 * KTable and NatTable have custom-drawn (non-native) table headers that stick out
 * on many platforms. They also have tons more features (like hierarchical and
 * column headers, merged cells etc) than what we actually need. Especially NatTable
 * is very heavy-weight and thus an overkill for our purposes.
 * <p>
 * CTable is a drop-in replacement of Table. However, it also has a custom-drawn
 * table headers, O(n) performance (although it still scales MUCH better than
 * an SWT Table), and some bugs (occasional NPE).
 *
 * @author Andras
 */
public class LargeTable extends Composite
{
    private static final Color LINE_COLOR = ColorFactory.GREY95;

    private final static boolean debug = false;

    /**
     * The number of rows in the table.
     */
    protected int itemCount;

    /**
     * This is an element close enough to the top of the visible area or null if there are no elements at all.
     */
    protected int topIndex;

    /**
     * List of selection listeners.
     *
     * @see #fireSelectionChanged
     */
    protected ListenerList<SelectionListener> selectionListeners = new ListenerList<>();

    /**
     * A list of selected elements;
     */
    protected IntRangeSet selectionIndices = new IntRangeSet();

    /**
     * The highlighted element. This is the highlight that can be moved with the up/down keys.
     */
    protected int focusIndex;

    /**
     * Starting point of a range selection.
     */
    protected int anchorIndex;

    /**
     * Responsible for drawing individual table items.
     */
    protected ILargeTableRowRenderer rowRenderer;

    /**
     * The height in pixels of all rows.
     */
    protected int rowHeight = 0; // uninitialized

    /**
     * Indicates whether the table will draw 1 pixels thick lines around cells.
     */
    protected boolean drawLines = true;

    /**
     * Container to support horizontal scrolling.
     */
    protected ScrolledComposite scrolledComposite;

    /**
     * Container for real and virtual table.
     */
    protected Composite composite;

    /**
     * Used to draw the actual content of the virtual table.
     */
    protected Canvas canvas;

    /**
     * Used to draw the header of the virtual table.
     */
    protected Table table;

    /**
     * Provides tooltips.
     */
    protected HoverSupport hoverSupport;

    /**
     * Context for running long operations (e.g. iterating selection elements) with a progress monitor.
     */
    protected IRunnableContext runnableContext;

    public LargeTable(Composite parent, int style) {
        super(parent, style | SWT.V_SCROLL);

        drawLines = true;
        setLayout(new FillLayout());
        setBackground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_BACKGROUND));

        createComposite(this);
        createCanvas(composite);
        createTable(composite);

        scrolledComposite.setContent(composite);

        getVerticalBar().addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                ScrollBar scrollBar = (ScrollBar)e.widget;
                double percentage = (double)scrollBar.getSelection() / (scrollBar.getMaximum() - scrollBar.getThumb());

                if (e.detail == SWT.ARROW_UP)
                    scroll(1);
                else if (e.detail == SWT.ARROW_DOWN)
                    scroll(-1);
                else if (e.detail == SWT.PAGE_UP)
                    scroll(getPageJumpCount());
                else if (e.detail == SWT.PAGE_DOWN)
                    scroll(-getPageJumpCount());
                else if (percentage == 0)
                    scrollToBegin();
                else if (percentage == 1)
                    scrollToEnd();
                else {
                    // avoid hysteresis (no change for a while in the position when turning to the opposite direction during scrolling)
                    percentage = (double)scrollBar.getSelection() / scrollBar.getMaximum();
                    topIndex = clamp((int)(itemCount * percentage));
                    redraw();
                }
            }
        });

        hoverSupport = new HoverSupport();
        hoverSupport.setHoverSizeConstaints(700, 200);
        hoverSupport.adapt(LargeTable.this, new IHoverInfoProvider() {
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                int rowIndex = getRowIndexAt(y);
                int columnIndex = getColumnIndexAt(x);
                if (rowIndex < 0 || columnIndex < 0)
                    return null;
                Assert.isTrue(rowIndex < itemCount && columnIndex < table.getColumnCount());
                return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(getRowRenderer().getTooltipText(rowIndex, columnIndex)));
            }
        });

        // recompute table size after the scrollable area size is known
        Display.getCurrent().asyncExec(new Runnable() {
            public void run() {
                recomputeTableSize();
            }
        });
    }

    /**
     * Sets the runnable context (e.g. progress dialog) for long running operations.
     */
    public void setRunnableContextForLongRunningOperations(IRunnableContext context) {
        runnableContext = context;
    }

    private void createComposite(Composite parent) {
        scrolledComposite = new ScrolledComposite(parent, SWT.H_SCROLL | SWT.V_SCROLL);
        scrolledComposite.getHorizontalBar().setIncrement(10);
        scrolledComposite.addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                composite.layout();
                composite.setSize(composite.computeSize(SWT.DEFAULT, SWT.DEFAULT));
                scrolledComposite.getVerticalBar().setVisible(false);
                scrolledComposite.getHorizontalBar().setPageIncrement(scrolledComposite.getSize().x);
            }
        });

        composite = new Composite(scrolledComposite, SWT.NONE);
        composite.setLayout(new ScalableTableCompositeLayout());
    }

    private void createCanvas(Composite parent) {
        canvas = new Canvas(parent, SWT.DOUBLE_BUFFERED);
        canvas.addFocusListener(new FocusListener() {
            public void focusGained(FocusEvent e) {
                redraw();
            }

            public void focusLost(FocusEvent e) {
                redraw();
            }
        });
        canvas.addPaintListener(new PaintListener() {
            public void paintControl(PaintEvent e) {
                if (topIndex >= itemCount)
                    scrollToBegin();

                paint(e.gc);
            }
        });

        canvas.addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                configureVerticalScrollBar();
                redraw();
            }
        });

        canvas.addKeyListener(new KeyListener() {
            public void keyPressed(KeyEvent e) {
                boolean withCtrl = (e.stateMask & SWT.MOD1) != 0;
                boolean withShift = (e.stateMask & SWT.MOD2) != 0;
                if (e.keyCode == SWT.F5)
                    refresh();
                else if (e.keyCode == SWT.ARROW_LEFT)
                    scrollHorizontal(-10);
                else if (e.keyCode == SWT.ARROW_RIGHT)
                    scrollHorizontal(10);
                else if (e.keyCode == SWT.ARROW_UP)
                    moveFocusBy(-1, withShift, withCtrl, false);
                else if (e.keyCode == SWT.ARROW_DOWN)
                    moveFocusBy(1, withShift, withCtrl, false);
                else if (e.keyCode == SWT.PAGE_UP)
                    moveFocusBy(-getPageJumpCount(), withShift, withCtrl, false);
                else if (e.keyCode == SWT.PAGE_DOWN)
                    moveFocusBy(getPageJumpCount(), withShift, withCtrl, false);
                else if (e.keyCode == SWT.HOME)
                    moveFocusTo(0, withShift, withCtrl, false);
                else if (e.keyCode == SWT.END)
                    moveFocusTo(itemCount-1, withShift, withCtrl, false);
                else if (e.keyCode == SWT.CR)
                    fireDefaultSelection();
                else if (e.keyCode == SWT.SPACE && withCtrl)
                    moveFocusBy(0, false, true, true);
                else if (e.keyCode == 'a' && withCtrl)
                    selectAll();
                else if (e.keyCode == 'c' && withCtrl)
                    copySelectionToClipboard();
            }

            public void keyReleased(KeyEvent e) {
            }
        });

        canvas.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDown(MouseEvent e) {
                int rowIndex = topIndex + e.y / getRowHeight();

                if (e.button == 1 && rowIndex < itemCount) {
                    boolean withCtrl = (e.stateMask & SWT.MOD1) != 0;
                    boolean withShift = (e.stateMask & SWT.MOD2) != 0;
                    moveFocusTo(rowIndex, withShift, withCtrl, true);
                }
            }
            @Override
            public void mouseDoubleClick(MouseEvent e) {
                fireDefaultSelection();
            }
        });

        canvas.addListener(SWT.MouseWheel, new Listener() {
            public void handleEvent(Event event) {
                scroll(-event.count);
            }
        });
    }

    private void createTable(Composite parent) {
        table = new Table(parent, SWT.NONE);
        table.setHeaderVisible(true);
        table.setLinesVisible(false);
        table.addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                composite.layout();
            }
        });

        table.setSize(table.computeSize(SWT.DEFAULT, SWT.DEFAULT));
    }

    public TableColumn createColumn(int style) {
        return createColumn(style, table.getColumnCount());
    }

    public TableColumn createColumn(int style, int index) {
        TableColumn tableColumn = new TableColumn(table, style, index);
        tableColumn.addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                recomputeTableSize();
            }
        });
        return tableColumn;
    }

    public Canvas getCanvas() {
        return canvas;
    }

    public boolean getLinesVisible() {
        return drawLines;
    }

    public void setLinesVisible(boolean linesVisible) {
        this.drawLines = linesVisible;
    }

    public int getGridLineWidth () {
        return 1;
    }

    public void setColumnOrder(int[] columnOrder) {
        table.setColumnOrder(columnOrder);
    }

    public TableColumn getSortColumn() {
        return table.getSortColumn();
    }

    public void setSortColumn(TableColumn column) {
        table.setSortColumn(column);
    }

    public int getSortDirection() {
        return table.getSortDirection();
    }

    public void setSortDirection(int direction) {
        table.setSortDirection(direction);
    }

    public ILargeTableRowRenderer getRowRenderer() {
        return rowRenderer;
    }

    public void setRowRenderer(ILargeTableRowRenderer rowRenderer) {
        this.rowRenderer = rowRenderer;
    }

    @Override
    public boolean setFocus() {
        if (canvas.isDisposed())
            return false;
        return canvas.setFocus();
    }

    @Override
    public void setMenu(Menu menu) {
        canvas.setMenu(menu);
    }

    public void addSelectionListener(SelectionListener listener) {
        selectionListeners.add(listener);
    }

    public void removeSelectionListener(SelectionListener listener) {
        selectionListeners.remove(listener);
    }

    protected Event newEvent() {
        Event event = new Event();
        event.display = getDisplay();
        event.widget = this;
        return event;
    }

    protected void fireSelectionChanged() {
        final SelectionEvent event = new SelectionEvent(newEvent());
        for (final SelectionListener listener : selectionListeners)
            SafeRunnable.run(() -> listener.widgetSelected(event));
    }

    protected void fireDefaultSelection() {
        final SelectionEvent event = new SelectionEvent(newEvent());
        for (final SelectionListener listener : selectionListeners)
            SafeRunnable.run(() -> listener.widgetDefaultSelected(event));
    }

    /**
     * Removes all selection elements.
     */
    public void clearSelection() {
        if (selectionIndices != null && !selectionIndices.isEmpty()) {
            selectionIndices.clear();
            fireSelectionChanged();
            redraw();
        }
    }

    public void setItemCount(int itemCount) {
        this.itemCount = itemCount;
        configureVerticalScrollBar();
        redraw();
    }

    public int getItemCount() {
        return itemCount;
    }

    /**
     * Returns the focused element. Note: there's no setFocusElement(), use gotoElement() instead!
     */
    public int getFocusIndex() {
        return focusIndex;
    }

    public int getColumnCount() {
        return table.getColumnCount();
    }

    public TableColumn[] getColumns() {
        return table.getColumns();
    }

    public TableColumn getColumn(int index) {
        return table.getColumn(index);
    }

    public int getSelectionCount () {
        return selectionIndices.size();
    }

    /**
     * Returns the current selection.
     */
    public IntRangeSet getSelectionIndices() {
        return new IntRangeSet(selectionIndices);
    }

    /**
     * Selects the given elements.
     */
    public void setSelectionIndices(IntRangeSet selectionIndices) {
        this.selectionIndices = new IntRangeSet(selectionIndices);

        fireSelectionChanged();
        redraw();
    }

    public void setSelectionIndices(int[] selectionIndices) {
        setSelectionIndices(new IntRangeSet(selectionIndices));
    }

    public void setSelectionIndex(int rowIndex) {
        setSelectionIndices(new IntRangeSet(rowIndex));
    }

    /**
     * Selects all rows of the table.
     */
    public void selectAll() {
        if (itemCount > 0)
            setSelectionIndices(new IntRangeSet(0, itemCount-1));
    }

    public void copySelectionToClipboard() {
        final CsvWriter writer = new CsvWriter('\t');

        // add header
        final int[] columnOrder = table.getColumnOrder();
        for (int i = 0; i < table.getColumnCount(); i++) {
            TableColumn column = table.getColumn(columnOrder[i]);
            writer.addField(column.getText());
        }
        writer.endRecord();

        // add selected lines
        boolean success = runLongOperation(new IRunnableWithProgress() {
            @Override
            public void run(IProgressMonitor monitor) throws InvocationTargetException, InterruptedException {
                int delta = 1;
                if (monitor != null) {
                    int size = selectionIndices.size();
                    int totalWork = size < 100 ? size : 100;
                    delta = size < 100 ? 1 : size / 100;
                    monitor.beginTask("A long running operation is in progress. Please wait or press Cancel to abort.", totalWork);
                }

                int count = 0;
                for (int rowIndex : selectionIndices.toArray()) {
                    if (monitor != null && monitor.isCanceled())
                        throw new InterruptedException();

                    toCSV(writer, rowIndex, columnOrder);
                    count++;

                    if (monitor != null && count % delta == 0)
                        monitor.worked(1);
                }
                if (monitor != null)
                    monitor.done();
            }
        });

        if (success) {
            Clipboard clipboard = new Clipboard(getDisplay());
            clipboard.setContents(new Object[] {writer.toString()}, new Transfer[] {TextTransfer.getInstance()});
            clipboard.dispose();
        }
    }

    protected void toCSV(CsvWriter writer, int rowIndex, int[] columnIndices) {
        for (int column : columnIndices)
            writer.addField(rowRenderer.getText(rowIndex, column));
        writer.endRecord();
    }

    protected boolean runLongOperation(IRunnableWithProgress runnable) {
        try {
            if (runnableContext != null)
                runnableContext.run(true, true, runnable);
            else
                runnable.run(null);
            return true;
        }
        catch (InterruptedException e) {
            return false;
        }
        catch (InvocationTargetException e) {
            Debug.println("VirtualTable.runLongOperation() failed: " + e.getTargetException().getMessage());
            return false;
        }
    }

    public void gotoIndex(int rowIndex) {
        doGotoElement(rowIndex);
        reveal(rowIndex);
    }

    public void gotoBegin() {
        doGotoElement(0);
        scrollToBegin();
    }

    public void gotoEnd() {
        doGotoElement(itemCount-1);
        scrollToEnd();
    }

    protected void doGotoElement(int rowIndex) {
        focusIndex = rowIndex;
        anchorIndex = rowIndex;

        selectionIndices.add(rowIndex);

        fireSelectionChanged();
        redraw();
    }

    /**
     * Updates the vertical bar state according to the current canvas and table content sizes.
     * Sets minimum, maximum, thumb, page increment, etc.
     */
    public void configureVerticalScrollBar() {
        ScrollBar verticalBar = getVerticalBar();
        verticalBar.setMinimum(0);

        verticalBar.setMaximum((int)Math.min(itemCount, 1E+6));
        verticalBar.setThumb((int)((double)verticalBar.getMaximum() * getVisibleElementCount() / itemCount));
        verticalBar.setIncrement(1);
        verticalBar.setPageIncrement(getPageJumpCount());
    }

    /**
     * Updates the vertical bar position to reflect the currently displayed elements.
     */
    protected void updateVerticalBarPosition() {
        int topVisibleIndex = getTopIndex();
        int bottomFullyVisibleIndex = getBottomIndex();
        double topPercentage = topVisibleIndex / (double)itemCount;
        double bottomPercentage = bottomFullyVisibleIndex / (double)itemCount;
        double topWeight = 1 / topPercentage;
        double bottomWeight = 1 / (1 - bottomPercentage);
        double percentage;

        if (Double.isInfinite(topWeight))
            percentage = topPercentage;
        else if (Double.isInfinite(bottomWeight))
            percentage = bottomPercentage;
        else
            percentage = (topPercentage * topWeight + bottomPercentage * bottomWeight) / (topWeight + bottomWeight);

        ScrollBar verticalBar = getVerticalBar();
        verticalBar.setSelection((int)((verticalBar.getMaximum() - verticalBar.getThumb()) * percentage));
    }

    public void moveFocus(int numberOfElements) {
        moveFocusBy(numberOfElements, false, false, false);
    }

    private int clamp(int rowIndex) {
        return rowIndex < 0 ? 0 : rowIndex < itemCount ? rowIndex : itemCount == 0 ? 0 : itemCount-1;
    }

    protected void moveFocusBy(int delta, boolean withShift, boolean withCtrl, boolean click) {
        int rowIndex = clamp(focusIndex + delta);
        moveFocusTo(rowIndex, withShift, withCtrl, click);
    }

    protected void moveFocusTo(int rowIndex, boolean withShift, boolean withCtrl, boolean click) {
        focusIndex = rowIndex;

        if (!withCtrl) {
            if (!withShift) {
                // no shift or ctrl: overwrite selection and anchor
                anchorIndex = rowIndex;
                selectionIndices = new IntRangeSet();
                selectionIndices.add(rowIndex);
            }
            else {
                // shift only: overwrite selection with this range selection
                selectionIndices = new IntRangeSet();
                if (anchorIndex <= rowIndex)
                    selectionIndices.addRange(anchorIndex, rowIndex);
                else
                    selectionIndices.addRange(rowIndex, anchorIndex);
            }
        }
        else {
            if (!withShift) {
                // ctrl only, by keyboard: move focus without affecting selection or anchor (so nothing to do here)
                // ctrl only, by mouse (or Space button): toggle selection of the given element, and move anchor
                if (click) {
                    anchorIndex = focusIndex;
                    if (selectionIndices.contains(rowIndex))
                        selectionIndices.remove(rowIndex);
                    else
                        selectionIndices.add(rowIndex);
                }
            }
            else {
                // ctrl+shift: add range to current selection
                if (anchorIndex <= rowIndex)
                    selectionIndices.addRange(anchorIndex, rowIndex);
                else
                    selectionIndices.addRange(rowIndex, anchorIndex);
            }
        }

        fireSelectionChanged();
        reveal(rowIndex);
    }

    /**
     * Scrolls the visible area horizontally with the given amount of pixels.
     */
    public void scrollHorizontal(int pixel) {
        Point p = scrolledComposite.getOrigin();
        scrolledComposite.setOrigin(p.x + pixel, p.y);
    }

    public void scroll(int delta) {
        topIndex = clamp(topIndex + delta);
        redraw();
    }

    public void reveal(int rowIndex) {
        int visibleItemCount = getFullyVisibleElementCount();
        if (rowIndex < topIndex)
            topIndex = clamp(rowIndex);
        else if (rowIndex >= topIndex + visibleItemCount)
            topIndex = clamp(rowIndex);
        if (topIndex >= itemCount - visibleItemCount)
            topIndex = clamp(itemCount - visibleItemCount);
        redraw();
    }

    public void revealFocus() {
        reveal(focusIndex);
    }

    public void scrollToBegin() {
        topIndex = 0;
        redraw();
    }

    public void scrollToEnd() {
        topIndex = clamp(itemCount - getFullyVisibleElementCount() + 1);
        redraw();
    }

    protected void recomputeTableSize() {
        int size = 0;
        for (int i = 0; i < table.getColumnCount(); i++)
            size += table.getColumn(i).getWidth();

        table.setSize(size, table.getSize().y);
        composite.setSize(composite.computeSize(SWT.DEFAULT, SWT.DEFAULT));
    }

    protected void paint(GC gc) {
        Rectangle clipping = gc.getClipping();
        Transform transform = new Transform(null);
        gc.getTransform(transform);
        gc.setBackground(getBackground());
        gc.fillRectangle(clipping);

        updateVerticalBarPosition();

        int visibleElementCount = getVisibleElementCount();
        for (int i = 0; i < visibleElementCount; i++) {
            int rowIndex = topIndex + i;
            if (rowIndex >= itemCount)
                break;
            Transform rowTransform = new Transform(null);
            int[] columnOrder = table.getColumnOrder();

            boolean isSelectedElement = selectionIndices != null && selectionIndices.contains(rowIndex);

            if (isSelectedElement) {
                gc.setBackground(Display.getCurrent().getSystemColor(canvas.isFocusControl() ? SWT.COLOR_LIST_SELECTION : SWT.COLOR_WIDGET_BACKGROUND));
                gc.fillRectangle(new Rectangle(0, i * getRowHeight(), clipping.x + clipping.width, getRowHeight()));
            }
            else
                gc.setBackground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_BACKGROUND));

            int y = i * getRowHeight();

            if (drawLines) {
                gc.setForeground(LINE_COLOR);
                gc.drawLine(0, y-1, clipping.x + clipping.width, y-1);
            }

            int x = 0;
            for (int j = 0; j < table.getColumnCount(); j++) {
                TableColumn column = table.getColumn(columnOrder[j]);
                // clipping has to be set explicitly without using the graphics transformation
                // to work correctly on Mac OS X
                gc.setClipping(new Rectangle(x, y, column.getWidth(), getRowHeight()).intersection(clipping));
                // do the transformation afterwards
                rowTransform.setElements(1, 0, 0, 1, 0, 0);
                rowTransform.translate(x, y);
                gc.setTransform(rowTransform);

                if (isSelectedElement && canvas.isFocusControl())
                    gc.setForeground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT));

                rowRenderer.drawCell(gc, rowIndex, columnOrder[j], column.getWidth(), column.getAlignment());

                gc.setTransform(transform);
                gc.setClipping(clipping);

                if (drawLines) {
                    gc.setForeground(LINE_COLOR);
                    gc.drawLine(x-1, y, x-1, y + getRowHeight());
                }

                x += column.getWidth();
            }

            if (rowIndex == focusIndex) {
                if (!isSelectedElement || selectionIndices.size() > 1) { // suppress focus border if it's the only selected element
                    gc.setForeground(Display.getCurrent().getSystemColor(isSelectedElement ? SWT.COLOR_LIST_SELECTION_TEXT : SWT.COLOR_LIST_FOREGROUND));
                    gc.setLineStyle(SWT.LINE_DOT);
                    gc.drawRectangle(1, i * getRowHeight(), getClientArea().width - 2, getRowHeight() - 2);
                    gc.setLineStyle(SWT.LINE_SOLID);
                }
            }
        }
    }

    public void refresh() {
        redraw();
    }

    @Override
    public void redraw() {
        canvas.redraw();
    }

    public Point getVisibleElementLocation(int rowIndex) {
        int visibleElementCount = getVisibleElementCount();
        if (rowIndex < topIndex || rowIndex > visibleElementCount) // note: if rowIndex==visibleElementCount, it might be partially visible
            throw new IllegalArgumentException("Row " + rowIndex + " is not visible");

        int rowHeight = getRowHeight();
        int x = getSize().x / 2;
        int y = getHeaderHeight() + rowHeight * rowIndex + rowHeight / 2;

        return new Point(x, y);
    }

    /**
     * Returns the top visible element.
     */
    public int getTopIndex() {
        return topIndex;
    }

    /**
     * Returns the bottom visible element even if it is not fully visible.
     */
    public int getBottomIndex() {
        return clamp(topIndex + getVisibleElementCount() - 1);
    }

    /**
     * Returns the number of visible elements including the one which is not fully visible.
     */
    public int getVisibleElementCount() {
        int rowHeight = getRowHeight();
        int height = canvas.getSize().y;

        return (int)Math.ceil((double)height / rowHeight);
    }

    /**
     * Returns the number of visible elements including the one which is not fully visible.
     */
    public int getFullyVisibleElementCount() {
        int rowHeight = getRowHeight();
        int height = canvas.getSize().y;

        return (int)Math.floor((double)height / rowHeight);
    }

    /**
     * Returns the number of elements that a page jump will skip.
     */
    public int getPageJumpCount() {
        return Math.max(1, getVisibleElementCount() - 1);
    }

    /**
     * Returns the height of rows in pixels.
     */
    public int getRowHeight() {
        if (rowHeight == 0) {
            if (rowRenderer == null) // occurs during construction, indirectly from table.setSize()
                return 16; // for now, just return some arbitrary but credible number without caching it
             GC gc = new GC(this);
             rowHeight = rowRenderer.getRowHeight(gc);
             gc.dispose();
        }

        return rowHeight;
    }

    /**
     * Returns the height of the table header in pixels.
     */
    public int getHeaderHeight() {
        return table.getHeaderHeight();
    }

    /**
     * Return the row index under the specified LargeTable-relative y coordinate,
     * or -1 if none.
     */
    public int getRowIndexAt(int y) {
        y =- getHeaderHeight();
        if (y < 0)
            return -1;
        int rowIndex = topIndex + y / getRowHeight();
        if (rowIndex >= itemCount)
            return -1;
        return rowIndex;
    }

    /**
     * Return the column index under the specified LargeTable-relative x coordinate,
     * or -1 if none.
     */
    public int getColumnIndexAt(int x) {
        if (x < 0)
            return -1;
        int columnCount = getColumnCount();
        for (int index = 0; index < columnCount; index++) {
            int columnWidth = table.getColumn(index).getWidth();
            if (x < columnWidth)
                return index;
            x -= columnWidth;
        }
        return -1;
    }
}

class ScalableTableCompositeLayout extends Layout {
    private final static boolean debug = false;

    @Override
    protected Point computeSize(Composite composite, int wHint, int hHint, boolean flushCache) {
        Table table = (Table)composite.getChildren()[1];
        Rectangle r = composite.getParent().getClientArea();

        return new Point(table.getSize().x, r.height);
    }

    @Override
    protected void layout(Composite composite, boolean flushCache) {
        if (composite.getChildren().length == 2) {
            Canvas canvas = (Canvas)composite.getChildren()[0];
            Table table = (Table)composite.getChildren()[1];

            Rectangle r = composite.getParent().getClientArea();
            int headerHeight = table.getHeaderHeight();

            if (debug)
                Debug.println("Relayouting virtual table: " + r.height);

            canvas.setBounds(0, headerHeight, table.getSize().x, r.height - headerHeight);

            int sumColumnWidth = 0;
            for (int i = 0; i < table.getColumnCount(); i++)
                sumColumnWidth += table.getColumn(i).getWidth();

            if (r.width > sumColumnWidth)
                table.setSize(r.width, table.getSize().y);
        }
    }
}

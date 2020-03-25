/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.virtualtable;

import java.lang.reflect.InvocationTargetException;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.operation.IRunnableContext;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
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
import org.omnetpp.common.collections.IEnumerator;
import org.omnetpp.common.collections.IRangeSet;
import org.omnetpp.common.collections.RangeSet;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.util.CsvWriter;

/**
 * VirtualTable is a widget that displays a list of (potentially huge
 * or unknown number of) elements in a multi-column format.
 *
 * Model data are not stored or cached inside the widget. Instead,
 * a content provider provides lists elements on demand (only the elements
 * that are necessary for display are retrieved), and a row renderer
 * does the actual rendering. A cell is primarily rendered as an
 * (optional) image plus a styled string, but completely owner-drawn
 * cells are also possible.
 *
 * Scrolling, and table positions in general, are expressed as a fix-point
 * element plus a delta. (Plain integers cannot be used, as the total
 * number of elements in the list may be unknown!) Content provider
 * is expected to tell the next/previous element related to an element,
 * and to provide an element at a percentage of the total elements
 * (this latter is used in scrollbar handling).
 *
 *
 * Size invariants kept:
 * - the width of the table is always greater or equal than the sum of its columns' width (so that the internal scroll bar never appears)
 * - the height of the table is constant (header plus one row)
 * - the width of the canvas is equal to the width of the table
 * - the height of the canvas is equal to the height of the client area of the scrollable composite minus the header's height
 */
public class VirtualTable<T>
    extends Composite
    implements IVirtualContentWidget<T>, ISelectionProvider
{
    private static final Color LINE_COLOR = ColorFactory.GREY95;

    private final static boolean debug = false;

    /**
     * This is an element close enough to the top of the visible area or null if there are no elements at all.
     */
    protected T fixPointElement;

    /**
     * The distance of the fix point element from the top of the visible area.
     * 0 means the fix point element is display as the top visible element, positive value means below it.
     */
    protected int fixPointDistance;

    /**
     * List of selection change listeners (element type: <code>ISelectionChangedListener</code>).
     *
     * @see #fireSelectionChanged
     */
    protected ListenerList<ISelectionChangedListener> selectionChangedListeners = new ListenerList<>();

    /**
     * True means a selection change operation is in progress and subsequent ones will be ignored.
     */
    protected boolean isSelectionChangeInProgress;

    /**
     * A list of selected elements;
     */
    protected IRangeSet<T> selectionElements;

    /**
     * The highlighted element. This is the highlight that can be moved with the up/down keys.
     */
    protected T focusElement;

    /**
     * Starting point of a range selection.
     */
    protected T anchorElement;

    /**
     * The input being show by this table.
     */
    protected Object input;

    /**
     * The content provider which will be queried for subsequent elements in the virtual table.
     */
    protected IVirtualTableContentProvider<T> contentProvider;

    /**
     * The row enumerator which will be used for range selections.
     */
    protected IEnumerator<T> rowEnumerator;

    /**
     * Responsible for drawing individual table items.
     */
    protected IVirtualTableRowRenderer<T> rowRenderer;

    /**
     * The height in pixels of all rows.
     */
    protected int rowHeight;

    /**
     * Indicates whether the table will draw 1 pixels thick lines around cells.
     */
    protected boolean drawLines;

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

    public VirtualTable(Composite parent, int style) {
        super(parent, style | SWT.V_SCROLL);

        drawLines = true;
        setLayout(new FillLayout());
        setBackground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_BACKGROUND));

        createComposite(this);
        createCanvas(composite);
        createTable(composite);

        scrolledComposite.setContent(composite);

        rowEnumerator = new IEnumerator<T>() {
            @Override
            public int compare(T element1, T element2) {
                return contentProvider.compare(element1, element2);
            }

            @Override
            public T getPrevious(T element) {
                return contentProvider.getNeighbourElement(element, -1);
            }

            @Override
            public T getNext(T element) {
                return contentProvider.getNeighbourElement(element, 1);
            }

            @Override
            public int getApproximateDelta(T element1, T element2) {
                return compare(element1, element2) <= 0 ?
                          (int)contentProvider.getDistanceToElement(element1, element2, Long.MAX_VALUE) :
                          (int)contentProvider.getDistanceToElement(element1, element2, Long.MIN_VALUE);
            }

            public boolean isExact() { return true; }
        };

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
                    relocateFixPoint(contentProvider.getApproximateElementAt(percentage), 0);
                    redraw();
                }
            }
        });

        hoverSupport = new HoverSupport();
        hoverSupport.setHoverSizeConstaints(700, 200);
        hoverSupport.adapt(canvas, new IHoverInfoProvider() {
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                T element = getElementAtDistanceFromFixPoint(y / getRowHeight() + getTopVisibleElementDistanceFromFixPoint());

                if (element == null)
                    return null;
                else
                    return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(getRowRenderer().getTooltipText(element)));
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
        composite.setLayout(new VirtualTableCompositeLayout());
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
                if (fixPointElement == null && input != null && contentProvider != null && contentProvider.getFirstElement() != null)
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
                    moveFocusTo(contentProvider.getFirstElement(), withShift, withCtrl, false);
                else if (e.keyCode == SWT.END)
                    moveFocusTo(contentProvider.getLastElement(), withShift, withCtrl, false);
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
                if (input != null && contentProvider != null) {
                    T element = getVisibleElementAt(e.y / getRowHeight());

                    if (e.button == 1 && element != null) {
                        boolean withCtrl = (e.stateMask & SWT.MOD1) != 0;
                        boolean withShift = (e.stateMask & SWT.MOD2) != 0;
                        moveFocusTo(element, withShift, withCtrl, true);
                    }
                }
            }
        });

        canvas.addListener(SWT.MouseWheel, new Listener() {
            public void handleEvent(Event event) {
                scroll(event.count);
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

    public TableColumn createColumn() {
        TableColumn tableColumn = new TableColumn(table, SWT.NONE);
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

    public boolean getDrawLines() {
        return drawLines;
    }

    public void setDrawLines(boolean drawLines) {
        this.drawLines = drawLines;
    }

    public void setColumnOrder(int[] columnOrder) {
        table.setColumnOrder(columnOrder);
    }

    public Object getInput() {
        return input;
    }

    public void setInput(Object input) {
        if (input != this.input) {
            // remember input
            this.input = input;
            getContentProvider().inputChanged(null, null, input);
            getRowRenderer().setInput(input);
            // clear state
            focusElement = null;
            relocateFixPoint(null, 0);
            clearSelection();
            configureVerticalScrollBar();
        }
    }

    public IEnumerator<T> getRowEnumerator() {
        return rowEnumerator;
    }

    public void setRowEnumerator(IEnumerator<T> rowEnumerator) {
        this.rowEnumerator = rowEnumerator;
    }

    public IVirtualTableContentProvider<T> getContentProvider() {
        return contentProvider;
    }

    public void setContentProvider(IVirtualTableContentProvider<T> contentProvider) {
        this.contentProvider = contentProvider;
        configureVerticalScrollBar();
    }

    public IVirtualTableRowRenderer<T> getRowRenderer() {
        return rowRenderer;
    }

    public void setRowRenderer(IVirtualTableRowRenderer<T> rowRenderer) {
        this.rowRenderer = rowRenderer;
    }

    @Override
    public boolean setFocus() {
        if (canvas.isDisposed()) // we can be called indirectly from Control.releaseWidget()
            return false;
        return canvas.setFocus();
    }

    @Override
    public void setMenu(Menu menu) {
        canvas.setMenu(menu);
    }

    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.add(listener);
    }

    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.remove(listener);
    }

    protected void fireSelectionChanged() {
        ISelection selection = getSelection();

        if (selection != null)
            fireSelectionChanged(new SelectionChangedEvent(this, selection));
    }

    /**
     * Notifies any selection changed listeners that the viewer's selection has changed.
     * Only listeners registered at the time this method is called are notified.
     *
     * @param event a selection changed event
     *
     * @see ISelectionChangedListener#selectionChanged
     */
    protected void fireSelectionChanged(final SelectionChangedEvent event) {
        for (ISelectionChangedListener listener : selectionChangedListeners) {
            SafeRunnable.run(new SafeRunnable() {
                public void run() {
                    listener.selectionChanged(event);
                }
            });
        }
    }

    public ISelection getSelection() {
        return new VirtualTableSelection<T>(getInput(), getSelectionElements());
    }

    @SuppressWarnings("unchecked")
    public void setSelection(ISelection selection) {
        if (selection instanceof IVirtualTableSelection) {
            IVirtualTableSelection<T> virtualTableSelection = (IVirtualTableSelection<T>)selection;
            Object selectionInput = virtualTableSelection.getInput();
            if (input != selectionInput)
                setInput(selectionInput);
            boolean isSelectionReallyChanged = !ObjectUtils.equals(selectionElements, virtualTableSelection.getElements());
            if (isSelectionReallyChanged && !isSelectionChangeInProgress) {
                try {
                    isSelectionChangeInProgress = true;
                    setSelectionElements(virtualTableSelection.getElements());
                    if (!selectionElements.isEmpty())
                        reveal(selectionElements.iterator().next());
                }
                finally {
                    isSelectionChangeInProgress = false;
                }
            }
        }
    }

    /**
     * Removes all selection elements.
     */
    public void clearSelection() {
        if (selectionElements != null && !selectionElements.isEmpty()) {
            selectionElements.clear();

            fireSelectionChanged();
            redraw();
        }
    }

    /**
     * Returns the focused element. Note: there's no setFocusElement(), use gotoElement() instead!
     */
    public T getFocusElement() {
        return focusElement;
    }

    /**
     * Returns the current selection.
     */
    public IRangeSet<T> getSelectionElements() {
        return selectionElements;
    }

    /**
     * Selects the given elements.
     */
    public void setSelectionElements(IRangeSet<T> elements) {
        selectionElements = elements;

        fireSelectionChanged();
        redraw();
    }

    /**
     * Selects all rows of the table.
     */
    public void selectAll() {
        if (contentProvider != null) {
            T first = contentProvider.getFirstElement();
            T last = contentProvider.getLastElement();
            if (first != null && last != null) {
                IRangeSet<T> elements = new RangeSet<T>(rowEnumerator);
                elements.addRange(first, last);
                setSelectionElements(elements);
            }
        }
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
                    int size = selectionElements.approximateSize();
                    int totalWork = size < 100 ? size : 100;
                    delta = size < 100 ? 1 : size / 100;
                    monitor.beginTask("A long running operation is in progress. Please wait or press Cancel to abort.", totalWork);
                }

                int count = 0;
                for (T element : selectionElements) {
                    if (monitor != null && monitor.isCanceled())
                        throw new InterruptedException();

                    toCSV(writer, element, columnOrder);
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

    protected void toCSV(CsvWriter writer, T element, int[] columns) {
        for (int i = 0; i < columns.length; i++) {
            String text = rowRenderer.getStyledText(element, columns[i], false).getString();
            writer.addField(text);
        }
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

    public void gotoElement(T element) {
        Assert.isTrue(element != null);

        doGotoElement(element);
        reveal(element);
    }

    public void gotoClosestElement(T element) {
        gotoElement(contentProvider.getClosestElement(element));
    }

    public void gotoBegin() {
        T firstElement = contentProvider.getFirstElement();

        if (firstElement != null)
            doGotoElement(firstElement);

        scrollToBegin();
    }

    public void gotoEnd() {
        T lastElement = contentProvider.getLastElement();

        if (lastElement != null)
            doGotoElement(lastElement);

        scrollToEnd();
    }

    protected void doGotoElement(T element) {
        Assert.isTrue(element != null);

        focusElement = element;
        anchorElement = element;

        selectionElements = new RangeSet<T>(rowEnumerator);
        selectionElements.add(element);

        fireSelectionChanged();
        redraw();
    }

    /**
     * Updates the invalid fix point to a valid one staying as close to the current position as it is possible.
     */
    protected void stayNear() {
        if (fixPointElement != null) {
            relocateFixPoint(contentProvider.getClosestElement(fixPointElement), fixPointDistance);
            redraw();
        }
    }

    /**
     * Updates the vertical bar state according to the current canvas and table content sizes.
     * Sets minimum, maximum, thumb, page increment, etc.
     */
    public void configureVerticalScrollBar() {
        ScrollBar verticalBar = getVerticalBar();
        verticalBar.setMinimum(0);

        if (contentProvider != null && rowRenderer != null) {
            long numberOfElements = contentProvider.getApproximateNumberOfElements();
            verticalBar.setMaximum((int)Math.min(numberOfElements, 1E+6));
            verticalBar.setThumb((int)((double)verticalBar.getMaximum() * getVisibleElementCount() / numberOfElements));
            verticalBar.setIncrement(1);
            verticalBar.setPageIncrement(getPageJumpCount());
        }
        else {
            verticalBar.setMaximum(0);
            verticalBar.setThumb(0);
            verticalBar.setIncrement(0);
            verticalBar.setPageIncrement(0);
        }
    }

    /**
     * Updates the vertical bar position to reflect the currently displayed elements.
     */
    protected void updateVerticalBarPosition() {
        T topVisibleElement = getTopVisibleElement();
        T bottomFullyVisibleElement = getBottomFullyVisibleElement();
        double topPercentage = topVisibleElement == null ? 0 : contentProvider.getApproximatePercentageForElement(topVisibleElement);
        double bottomPercentage = bottomFullyVisibleElement == null ? 1 : contentProvider.getApproximatePercentageForElement(bottomFullyVisibleElement);
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

    protected void moveFocusBy(int numberOfElements, boolean withShift, boolean withCtrl, boolean click) {
        T element = getFocusElement();

        if (element == null) {
            element = getTopVisibleElement();

            if (element == null)
                return;
        }

        element = contentProvider.getNeighbourElement(element, numberOfElements);

        if (element == null) {
            if (numberOfElements < 0)
                element = contentProvider.getFirstElement();
            else
                element = contentProvider.getLastElement();
        }

        moveFocusTo(element, withShift, withCtrl, click);
    }

    protected void moveFocusTo(T element, boolean withShift, boolean withCtrl, boolean click) {
        if (element == null)
            return;

        focusElement = element;

        if (!withCtrl) {
            if (!withShift) {
                // no shift or ctrl: overwrite selection and anchor
                anchorElement = element;
                selectionElements = new RangeSet<T>(rowEnumerator);
                selectionElements.add(element);
            }
            else {
                // shift only: overwrite selection with this range selection
                selectionElements = new RangeSet<T>(rowEnumerator);
                if (rowEnumerator.compare(anchorElement, element) <= 0)
                    selectionElements.addRange(anchorElement, element);
                else
                    selectionElements.addRange(element, anchorElement);
            }
        }
        else {
            if (!withShift) {
                // ctrl only, by keyboard: move focus without affecting selection or anchor (so nothing to do here)
                // ctrl only, by mouse (or Space button): toggle selection of the given element, and move anchor
                if (click) {
                    anchorElement = focusElement;
                    if (selectionElements.contains(element))
                        selectionElements.remove(element);
                    else
                        selectionElements.add(element);
                }
            }
            else {
                // ctrl+shift: add range to current selection
                if (rowEnumerator.compare(anchorElement, element) <= 0)
                    selectionElements.addRange(anchorElement, element);
                else
                    selectionElements.addRange(element, anchorElement);
            }
        }

        fireSelectionChanged();
        reveal(element);
    }

    /**
     * Scrolls the visible area horizontally with the given amount of pixels.
     */
    public void scrollHorizontal(int pixel) {
        Point p = scrolledComposite.getOrigin();
        scrolledComposite.setOrigin(p.x + pixel, p.y);
    }

    public void scroll(int numberOfElements) {
        relocateFixPoint(fixPointElement, fixPointDistance + numberOfElements);
        redraw();
    }

    public void reveal(T element) {
        Assert.isTrue(element != null);

        T topElement = getTopVisibleElement();

        int maxDistance = getFullyVisibleElementCount();
        long distance = topElement == null ? maxDistance : contentProvider.getDistanceToElement(topElement, element, maxDistance);

        if (distance == maxDistance) {
            T bottomElement = getBottomFullyVisibleElement();
            distance = bottomElement == null ? maxDistance : contentProvider.getDistanceToElement(bottomElement, element, maxDistance);

            if (distance == maxDistance)
                relocateFixPoint(element, 0);
            else
                relocateFixPoint(element, Math.max(0, getFullyVisibleElementCount() - 1));
        }

        redraw();
    }

    public void revealFocus() {
        T element = getFocusElement();

        if (element != null)
            reveal(element);
    }

    public void scrollToBegin() {
        relocateFixPoint(contentProvider.getFirstElement(), 0);
        redraw();
    }

    public void scrollToEnd() {
        relocateFixPoint(contentProvider.getLastElement(), Math.max(0, getFullyVisibleElementCount() - 1));
        redraw();
    }

    /**
     * Relocates fix point and ensures that the first visible row always displays an element.
     * If there are more elements than fully visible rows then the last fully visible row will
     * also always display an element.
     */
    protected void relocateFixPoint(T element, int distance) {
        fixPointElement = element;
        fixPointDistance = distance;

        if (element != null) {
            int maxDistance = getVisibleElementCount() * 2;
            long firstElementDistance = contentProvider.getDistanceToFirstElement(element, maxDistance);

            T topElement = getTopVisibleElement();
            T bottomElement = getBottomFullyVisibleElement();

            if ((topElement == null || bottomElement == null)) {
                if (firstElementDistance < maxDistance && topElement == null) {
                    fixPointElement = contentProvider.getFirstElement();
                    fixPointDistance = 0;
                }
                else if (isVisible() && bottomElement == null){
                    if (contentProvider.getApproximateNumberOfElements() >= getFullyVisibleElementCount()) {
                        fixPointElement = contentProvider.getLastElement();
                        fixPointDistance = Math.max(0, getFullyVisibleElementCount() - 1);
                    }
                    else {
                        fixPointElement = contentProvider.getFirstElement();
                        fixPointDistance = 0;
                    }
                }
            }
        }
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

        if (contentProvider != null && fixPointElement != null) {
            updateVerticalBarPosition();

            for (int i = 0; i < getVisibleElementCount(); i++) {
                Transform rowTransform = new Transform(null);
                T element = getElementAtDistanceFromFixPoint(i - fixPointDistance);
                IRangeSet<T> selectionElements = getSelectionElements();
                int[] columnOrder = table.getColumnOrder();

                if (element != null) {
                    boolean isSelectedElement = selectionElements != null && selectionElements.contains(element);

                    if (isSelectedElement) {
                        gc.setBackground(Display.getCurrent().getSystemColor(canvas.isFocusControl() ? SWT.COLOR_LIST_SELECTION : SWT.COLOR_WIDGET_BACKGROUND));
                        gc.fillRectangle(new Rectangle(0, i * getRowHeight(), clipping.x + clipping.width, getRowHeight()));
                    }
                    else
                        gc.setBackground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_BACKGROUND));

                    int y = i * getRowHeight();

                    if (drawLines) {
                        gc.setForeground(LINE_COLOR);
                        gc.drawLine(0, y - 1, clipping.x + clipping.width, y - 1);
                    }

                    int x = 0;
                    final int LINE_WIDTH = 1;
                    for (int j = 0; j < table.getColumnCount(); j++) {
                        TableColumn column = table.getColumn(columnOrder[j]);
                        // clipping has to be set explicitly without using the graphics transformation
                        // to work correctly on Mac OS X
                        gc.setClipping(new Rectangle(x+LINE_WIDTH, y, column.getWidth(), getRowHeight()));
                        // do the transformation afterwards
                        rowTransform.setElements(1, 0, 0, 1, 0, 0);
                        rowTransform.translate(x+LINE_WIDTH, y);
                        gc.setTransform(rowTransform);

                        if (isSelectedElement && canvas.isFocusControl())
                            gc.setForeground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT));

                        rowRenderer.drawCell(gc, element, columnOrder[j], isSelectedElement);

                        gc.setTransform(transform);
                        gc.setClipping((Rectangle)null);

                        if (drawLines) {
                            gc.setForeground(LINE_COLOR);
                            gc.drawLine(x, y, x, y + getRowHeight());
                        }

                        x += column.getWidth() + LINE_WIDTH;
                    }

                    if (element.equals(focusElement)) {
                        if (!isSelectedElement || selectionElements.approximateSize() > 1) { // suppress focus border if it's the only selected element
                            gc.setForeground(Display.getCurrent().getSystemColor(isSelectedElement ? SWT.COLOR_LIST_SELECTION_TEXT : SWT.COLOR_LIST_FOREGROUND));
                            gc.setLineStyle(SWT.LINE_DOT);
                            gc.drawRectangle(1, i * getRowHeight(), getClientArea().width - 2, getRowHeight() - 2);
                            gc.setLineStyle(SWT.LINE_SOLID);
                        }
                    }
                }
            }
        }
    }

    public void refresh() {
        redraw();
    }

    @Override
    public void redraw() {
        if (debug)
            Debug.println("Redrawing canvas");

        canvas.redraw();

        if (contentProvider != null && input != null && fixPointElement != null && isVisible()) {
            T topElement = getTopVisibleElement();

            if (topElement != null)
                relocateFixPoint(topElement, 0);
        }
    }

    protected String getTooltipText(T element) {
        return null;
    }

    public int getTopVisibleElementDistanceFromFixPoint() {
        return -fixPointDistance;
    }

    public int getBottomVisibleElementDistanceFromFixPoint() {
        return -fixPointDistance + Math.min((int)contentProvider.getApproximateNumberOfElements() - 1, getVisibleElementCount() - 1);
    }

    public int getBottomFullyVisibleElementDistanceFromFixPoint() {
        return -fixPointDistance + Math.min((int)contentProvider.getApproximateNumberOfElements() - 1, Math.max(0, getFullyVisibleElementCount() - 1));
    }

    public T getElementAtDistanceFromFixPoint(int distance) {
        if (fixPointElement == null)
            return null;
        else
            return contentProvider.getNeighbourElement(fixPointElement, distance);
    }

    public T getVisibleElementAt(int index) {
        return getElementAtDistanceFromFixPoint(getTopVisibleElementDistanceFromFixPoint() + index);
    }

    public Point getVisibleElementLocation(T visibleElement) {
        int visibleElementCount = getVisibleElementCount();
        int rowNumber = (int)contentProvider.getDistanceToElement(getTopVisibleElement(), visibleElement, visibleElementCount);

        if (rowNumber == visibleElementCount)
            throw new IllegalArgumentException("Element is not visible: " + visibleElement);

        int rowHeight = getRowHeight();
        int x = getSize().x / 2;
        int y = getHeaderHeight() + rowHeight * rowNumber + rowHeight / 2;

        return new Point(x, y);
    }

    /**
     * Returns the top visible element.
     */
    public T getTopVisibleElement() {
        if (contentProvider == null || fixPointElement == null)
            return null;
        else
            return getElementAtDistanceFromFixPoint(getTopVisibleElementDistanceFromFixPoint());
    }

    /**
     * Returns the bottom visible element even if it is not fully visible.
     */
    public T getBottomVisibleElement() {
        if (contentProvider == null || fixPointElement == null)
            return null;
        else
            return getElementAtDistanceFromFixPoint(getBottomVisibleElementDistanceFromFixPoint());
    }

    /**
     * Returns the bottom fully visible element.
     */
    public T getBottomFullyVisibleElement() {
        if (contentProvider == null || fixPointElement == null)
            return null;
        else
            return getElementAtDistanceFromFixPoint(getBottomFullyVisibleElementDistanceFromFixPoint());
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
        return Math.max(1, getFullyVisibleElementCount() - 1);
    }

    /**
     * Returns the height of rows in pixels.
     */
    public int getRowHeight() {
        if (rowHeight == 0) {
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
}

class VirtualTableCompositeLayout extends Layout {
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

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ContentViewer;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;

/**
 * Viewer that displays its contents like a file manager's "large icons" view.
 *
 * @author andras
 */
public class IconGridViewer extends ContentViewer {
    // configuration
    private int topMargin = 20, leftMargin = 10, rightMargin = 10;
    private int rowHeight = 120, columnWidth = 120;
    private int itemHeight = 100, itemWidth = 100;
    private Color dragRectangleOutlineColor = ColorFactory.LIGHT_BLUE4;
    private Color dragRectangleFillColor = ColorFactory.LIGHT_BLUE;
    private Color selectionFillColor = new Color(Display.getDefault(), 216, 235, 243); // very light blue
    private Color focusElementBorderColor = ColorFactory.LIGHT_BLUE;

    //  widgets
    private ScrolledComposite scrolledComposite;
    private Canvas canvas;
    private LightweightSystem lws;
    private IFigure containerFigure;
    private ListenerList<IDoubleClickListener> doubleClickListeners = new ListenerList<IDoubleClickListener>();

    // contents, selection
    private Object[] elements;
    private Map<Object,IFigure> elementsToFigures = new HashMap<>();
    private List<Object> selectedElements = new ArrayList<>();
    private Object focusElement;

    // mouse
    private int mouseButton;
    private Point dragStart;
    private RectangleFigure dragRectangleFigure;


    public IconGridViewer(Composite parent) {
        scrolledComposite = new ScrolledComposite(parent,  SWT.V_SCROLL | SWT.BORDER);
        canvas = new Canvas(scrolledComposite, SWT.NONE);
        canvas.setSize(500, 1000);
        scrolledComposite.setContent(canvas);
        lws = new LightweightSystem(canvas);
        containerFigure = lws.getRootFigure();
        containerFigure.setLayoutManager(new XYLayout());
        containerFigure.setRequestFocusEnabled(true);
        containerFigure.requestFocus();
        hookListeners();
    }

    public void setMargins(int topMargin, int leftMargin, int rightMargin) {
        this.topMargin = topMargin;
        this.leftMargin = leftMargin;
        this.rightMargin = rightMargin;
    }

    public void setSpacing(int rowHeight, int columnWidth) {
        this.rowHeight = rowHeight;
        this.columnWidth = columnWidth;
    }

    public void setItemSize(int itemHeight, int itemWidth) {
        this.itemHeight = itemHeight;
        this.itemWidth = itemWidth;
    }

    public void setBackground(Color color) {
        scrolledComposite.setBackground(color);
        canvas.setBackground(color);
    }

    private void hookListeners() {
        scrolledComposite.addControlListener(new ControlAdapter() {
            public void controlResized(ControlEvent e) {
                int width = scrolledComposite.getSize().x;
                arrange(width);
            }
        });

        // Note: don't use Draw2D's mouse listener because it doesn't capture
        // the mouse during drag, rendering it essentially useless.
        canvas.addMouseListener(new MouseListener() {
            @Override
            public void mouseDown(MouseEvent e) {
                canvas.setFocus();
                mouseButton = e.button;
                if (e.button != 1)
                    return;
                boolean shift = (e.stateMask & SWT.SHIFT) != 0;
                boolean ctrl = (e.stateMask & SWT.CTRL) != 0;
                dragStart = new Point(e.x, e.y);
                if (!ctrl)
                    selectedElements.clear();
                Object element = getElementAt(e.x, e.y);
                if (element != null) {
                    if (shift)
                        selectTo(element);
                    else
                        toggleSelection(element);
                }
            }

            @Override
            public void mouseUp(MouseEvent e) {
                mouseButton = 0;
                if (dragRectangleFigure != null) {
                    containerFigure.remove(dragRectangleFigure);
                    dragRectangleFigure = null;
                }
            }

            @Override
            public void mouseDoubleClick(MouseEvent e) {
                if (e.button != 1)
                    return;
                Object element = getElementAt(e.x, e.y);
                if (element != null)
                    fireDoubleClick(new DoubleClickEvent(IconGridViewer.this, getSelection()));
            }
        });

        canvas.addMouseMoveListener(new MouseMoveListener() {
            @Override
            public void mouseMove(MouseEvent e) {
                if (mouseButton != 1)
                    return; // not button1 drag
                if (dragRectangleFigure == null) {
                    dragRectangleFigure = new RectangleFigure();
                    dragRectangleFigure.setAlpha(80);
                    dragRectangleFigure.setFill(true);
                    dragRectangleFigure.setBackgroundColor(dragRectangleFillColor);
                    dragRectangleFigure.setForegroundColor(dragRectangleOutlineColor);
                    containerFigure.add(dragRectangleFigure);
                }
                boolean ctrl = (e.stateMask & SWT.CTRL) != 0;
                Rectangle bounds = new Rectangle(Math.min(e.x, dragStart.x), Math.min(e.y, dragStart.y), Math.abs(e.x - dragStart.x), Math.abs(e.y - dragStart.y));
                dragRectangleFigure.setBounds(bounds);
                selectByRectangle(bounds, ctrl);

                // scroll if mouse goes outside the canvas
                int yoffset = scrolledComposite.getOrigin().y;
                if (e.y - yoffset < 0)
                    scrolledComposite.setOrigin(0, yoffset - 2);
                else if (e.y - yoffset > scrolledComposite.getSize().y)
                    scrolledComposite.setOrigin(0, yoffset + 2);
            }
        });

        canvas.addFocusListener(new FocusListener() {
            @Override
            public void focusLost(FocusEvent arg0) {
                // emulate mouseUp
                mouseButton = 0;
                if (dragRectangleFigure != null) {
                    containerFigure.remove(dragRectangleFigure);
                    dragRectangleFigure = null;
                }
            }

            @Override
            public void focusGained(FocusEvent arg0) {
            }
        });

        canvas.addKeyListener(new KeyListener() {
            @Override
            public void keyPressed(KeyEvent ke) {
                boolean shift = (ke.stateMask & SWT.SHIFT) != 0;
                boolean ctrl = (ke.stateMask & SWT.CTRL) != 0;
                if (ke.keyCode == SWT.ARROW_LEFT)
                    moveLeft(ctrl || shift);
                else if (ke.keyCode == SWT.ARROW_RIGHT)
                    moveRight(ctrl || shift);
                else if (ke.keyCode == SWT.ARROW_UP)
                    moveUp(ctrl || shift);
                else if (ke.keyCode == SWT.ARROW_DOWN)
                    moveDown(ctrl || shift);
                else if (ke.keyCode == SWT.HOME)
                    moveHome(ctrl || shift);
                else if (ke.keyCode == SWT.END)
                    moveEnd(ctrl || shift);
                else if (ke.keyCode == SWT.PAGE_DOWN)
                    movePageDown(ctrl || shift);
                else if (ke.keyCode == SWT.PAGE_UP)
                    movePageUp(ctrl || shift);
                else if (ke.keyCode == '\r')
                    fireDoubleClick(new DoubleClickEvent(IconGridViewer.this, getSelection()));
                else if (ke.keyCode == ' ') {
                    ensureFocusElement();
                    if (focusElement != null)
                        toggleSelection(focusElement);
                }
            }

            @Override
            public void keyReleased(KeyEvent ke) {
            }
        });
    }

    public void setFocus() {
        canvas.setFocus();
    }

    public void addDoubleClickListener(IDoubleClickListener listener) {
        doubleClickListeners.add(listener);
    }

    public void removeDoubleClickListener(IDoubleClickListener listener) {
        doubleClickListeners.remove(listener);
    }

    protected void fireDoubleClick(final DoubleClickEvent event) {
        Object[] listeners = doubleClickListeners.getListeners();
        for (int i = 0; i < listeners.length; ++i) {
            final IDoubleClickListener l = (IDoubleClickListener) listeners[i];
            SafeRunnable.run(new SafeRunnable() {
                @Override
                public void run() {
                    l.doubleClick(event);
                }
            });
        }
    }

    public Object getElementAt(int x, int y) {
        IFigure figure = containerFigure.findFigureAt(x, y);
        if (figure == null || figure == containerFigure)
            return null;
        return getElementFromFigure(figure);
    }

    @Override
    public ScrolledComposite getControl() {
        return scrolledComposite;
    }

    public Canvas getCanvas() {
        return canvas;
    }

    @Override
    public ISelection getSelection() {
        return new StructuredSelection(selectedElements);
    }

    public void reveal(Object element) {
        int yoffset = scrolledComposite.getOrigin().y;
        int height = scrolledComposite.getSize().y;
        IFigure figure = elementsToFigures.get(element);
        if (figure == null)
            return; // not in elements[]
        Rectangle bounds = figure.getBounds();
        if (bounds.y < yoffset)
            scrolledComposite.setOrigin(0, bounds.y);
        else if (bounds.bottom() > yoffset + height)
            scrolledComposite.setOrigin(0, bounds.bottom()-height);
    }

    @Override
    @SuppressWarnings("unchecked")
    public void setSelection(ISelection selection, boolean reveal) {
        IStructuredSelection structuredSelection = (IStructuredSelection) selection;
        if (!structuredSelection.toList().equals(selectedElements)) {
            selectedElements.clear();
            selectedElements.addAll(structuredSelection.toList());
            setSelectionToWidget();
            fireSelectionChanged();
        }
    }

    protected void fireSelectionChanged() {
        SelectionChangedEvent event = new SelectionChangedEvent(this, getSelection());
        fireSelectionChanged(event);
    }

    private void setSelectionToWidget() {
        for (Object element : elements) {
            boolean selected = selectedElements.contains(element);
            IFigure figure = elementsToFigures.get(element);
            figure.setOpaque(selected);
            figure.setBorder(null);
        }
        if (focusElement != null) {
            IFigure figure = elementsToFigures.get(focusElement);
            figure.setBorder(new LineBorder(focusElementBorderColor, 1));
        }
    }

    @Override
    public void refresh() {
        IStructuredContentProvider contentProvider = (IStructuredContentProvider)getContentProvider();
        Object[] elements = contentProvider.getElements(getInput());

        boolean elementListChanged = false;
        if (!ArrayUtils.isEquals(elements, this.elements)) {
            // clear focus if element was deleted
            if (focusElement != null && !ArrayUtils.contains(elements, focusElement)) {
                int pos = ArrayUtils.indexOf(this.elements, focusElement);
                focusElement = elements.length == 0 ? null : pos < elements.length ? elements[pos] : elements[elements.length-1];
            }

            this.elements = elements;
            elementListChanged = true;

            // remove deleted elements
            ArrayList<Object> trash = new ArrayList<>();
            for (Object element : elementsToFigures.keySet())
                if (!ArrayUtils.contains(elements, element))
                    trash.add(element);
            for (Object element : trash)                 {
                IFigure figure = elementsToFigures.get(element);
                containerFigure.remove(figure);
                elementsToFigures.remove(element);
            }

            // add new elements
            for (Object element : elements) {
                if (!elementsToFigures.containsKey(element)) {
                    IFigure figure = createFigure();
                    containerFigure.add(figure, new Rectangle());
                    elementsToFigures.put(element, figure);
                }
            }

            // remove deleted elements from selection too
            for (Object element : selectedElements.toArray())
                if (!ArrayUtils.contains(elements, element))
                    selectedElements.remove(element);
        }

        // synchronize icons and labels
        ILabelProvider labelProvider = (ILabelProvider)getLabelProvider();
        for (Object element : elements) {
            IFigure figure = elementsToFigures.get(element);
            Label labelFigure = (Label)figure;
            String text = labelProvider.getText(element);
            Image image = labelProvider.getImage(element);
            labelFigure.setIcon(image);
            labelFigure.setText(text);
            figure.invalidateTree();
            figure.revalidate();
        }

        setSelectionToWidget();

        if (elementListChanged)
            arrange();
    }

    @Override
    protected void inputChanged(Object input, Object oldInput) {
        super.inputChanged(input, oldInput);
        refresh();
    }

    protected IFigure createFigure() {
        Label label = new Label();
        label.setTextPlacement(PositionConstants.SOUTH);
        label.setIconAlignment(PositionConstants.CENTER);
        label.setBackgroundColor(selectionFillColor);  // note: only takes effect when 'opaque' is set; we use this for selection
        return label;
    }

    protected void arrange() {
        arrange(canvas.getSize().x);
    }

    protected void arrange(int canvasWidth) {
        int row = 0, col = 0;
        int lastRow = 0;
        LayoutManager layoutManager = containerFigure.getLayoutManager();
        for (Object element : elements) {
            IFigure figure = elementsToFigures.get(element);
            Rectangle bounds = (Rectangle)layoutManager.getConstraint(figure);
            bounds.x = col * columnWidth + leftMargin;
            bounds.y = row * rowHeight + topMargin;
            bounds.width = itemWidth;
            bounds.height = itemHeight;
            figure.setBounds(bounds);
            lastRow = row;

            col++;
            if (bounds.right() + columnWidth + rightMargin >= canvasWidth) {
                row++;
                col = 0;
            }
        }
        layoutManager.layout(containerFigure);

        int height = (lastRow+1) * rowHeight + topMargin;
        canvas.setSize(canvasWidth, height);
    }

    protected Object getElementFromFigure(IFigure figure) {
        for (Map.Entry<Object,IFigure> entry : elementsToFigures.entrySet())
            if (entry.getValue() == figure)
                return entry.getKey();
        return null;
    }

    private void ensureFocusElement() {
        if (focusElement == null && elements.length > 0)
            focusElement = elements[0];
    }

    protected void moveBy(int amount, boolean extendSelection) {
        ensureFocusElement();
        if (focusElement == null)
            return;
        int pos = ArrayUtils.indexOf(elements, focusElement);
        int newPos = pos + amount;
        if (newPos >= 0 && newPos < elements.length)
            moveTo(elements[newPos], extendSelection);
    }

    protected void moveTo(Object element, boolean extendSelection) {
        focusElement = element;
        if (extendSelection)
            addToSelection(focusElement);
        else
            setSelection(new StructuredSelection(focusElement), true);
        reveal(focusElement);
    }

    protected void moveLeft(boolean extendSelection) {
        moveBy(-1, extendSelection);
    }

    protected void moveRight(boolean extendSelection) {
        moveBy(1, extendSelection);
    }

    protected int getItemsPerRow() {
        int width = canvas.getSize().x;
        return (width - leftMargin - rightMargin) / columnWidth;
    }

    protected int getNumVisibleRows() {
        int height = scrolledComposite.getSize().y;
        return height / rowHeight;
    }

    protected void moveUp(boolean extendSelection) {
        moveBy(-1*getItemsPerRow(), extendSelection);
    }

    protected void moveDown(boolean extendSelection) {
        moveBy(getItemsPerRow(), extendSelection);
    }

    protected void moveHome(boolean extendSelection) {
        if (elements.length > 0)
            moveTo(elements[0], extendSelection);
    }

    protected void moveEnd(boolean extendSelection) {
        if (elements.length > 0)
            moveTo(elements[elements.length-1], extendSelection);
    }

    protected void movePageUp(boolean extendSelection) {
        ensureFocusElement();
        if (focusElement == null)
            return;
        int pos = ArrayUtils.indexOf(elements, focusElement);
        int newPos = pos - getItemsPerRow() * getNumVisibleRows();
        moveTo(elements[Math.max(0, newPos)], extendSelection);
    }

    protected void movePageDown(boolean extendSelection) {
        ensureFocusElement();
        if (focusElement == null)
            return;
        int pos = ArrayUtils.indexOf(elements, focusElement);
        int newPos = pos + getItemsPerRow() * getNumVisibleRows();
        moveTo(elements[Math.min(elements.length-1, newPos)], extendSelection);
    }

    protected void selectByRectangle(Rectangle rectangle, boolean extendSelection) {
        if (!extendSelection)
            selectedElements.clear();
        for (Object element : elements) {
            if (!selectedElements.contains(element)) {
                IFigure figure = elementsToFigures.get(element);
                if (rectangle.contains(figure.getBounds().getCenter()))
                    selectedElements.add(element);
            }
        }
        if (!selectedElements.isEmpty())
            focusElement = selectedElements.get(selectedElements.size()-1); // focus on last element
        setSelectionToWidget();
        fireSelectionChanged();
    }

    protected void toggleSelection(Object element) {
        focusElement = element;
        if (!selectedElements.contains(element))
            selectedElements.add(element);
        else
            selectedElements.remove(element);
        setSelectionToWidget();
        fireSelectionChanged();
    }

    protected void addToSelection(Object element) {
        focusElement = element;
        if (!selectedElements.contains(element)) {
            selectedElements.add(element);
            setSelectionToWidget();
            fireSelectionChanged();
        }
        else {
            setSelectionToWidget(); // refraw focus mark
        }
    }

    protected void selectTo(Object element) {
        ensureFocusElement();
        int pos1 = ArrayUtils.indexOf(elements, focusElement);
        int pos2 = ArrayUtils.indexOf(elements, element);
        if (pos1 > pos2) { int tmp = pos1; pos1 = pos2; pos2 = tmp; } // swap
        for (int i = pos1; i <= pos2; i++)
            if (!selectedElements.contains(elements[i]))
                selectedElements.add(elements[i]);
        focusElement = element;
        setSelectionToWidget();
        fireSelectionChanged();
    }
}

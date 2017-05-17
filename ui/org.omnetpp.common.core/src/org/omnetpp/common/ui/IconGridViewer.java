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
import org.eclipse.draw2d.FlowLayout;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LayeredPane;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.util.LocalSelectionTransfer;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ContentViewer;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DragSource;
import org.eclipse.swt.dnd.DragSourceEvent;
import org.eclipse.swt.dnd.DragSourceListener;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.Debug;
import org.omnetpp.common.color.ColorFactory;


/**
 * Viewer that displays its contents like a file manager's "large icons" view.
 *
 * @author andras
 */
//TODO use SWT.MOD1 instead of SWT.CTRL? (Mac!)
//TODO keyboard: proper up/down, pngup/pgdn
public class IconGridViewer extends ContentViewer {
    // configuration
    private static final int DEFAULT_MARGIN = 20;
    private static final int DEFAULT_HORIZ_SPACING = 20, DEFAULT_VERT_SPACING = 20;
    private Color dragRectangleOutlineColor = ColorFactory.LIGHT_BLUE4;
    private Color dragRectangleFillColor = ColorFactory.LIGHT_BLUE;
    private Color selectionFillColor = new Color(Display.getDefault(), 216, 235, 243); // very light blue
    private Color focusElementBorderColor = ColorFactory.LIGHT_BLUE;

    //  widgets
    private ScrolledComposite scrolledComposite;
    private Canvas canvas;
    private LightweightSystem lws;
    private Layer feedbackLayer;
    private Layer contentLayer;
    private ListenerList<IDoubleClickListener> doubleClickListeners = new ListenerList<>();
    private ListenerList<IDropListener> dropListeners = new ListenerList<>();

    // contents, selection
    private Object[] elements;
    private Map<Object,IFigure> elementsToFigures = new HashMap<>();
    private List<Object> selectedElements = new ArrayList<>();
    private Object focusElement;

    private MouseHandler mouseHandler = new MouseHandler();
    private IRenameAdapter renameAdapter;

    public interface IDropListener {
        //TODO boolean canDrop(Object[] elements, Point p); or some other feedback API
        void drop(Object[] elements, org.eclipse.swt.graphics.Point p);
    }

    public interface IRenameAdapter {
        public boolean isRenameable(Object element);
        public String getName(Object element);
        public boolean isNameValid(Object element, String name);
        public void setName(Object element, String name);
    }

    // Note: don't use Draw2D's mouse listener because it doesn't capture
    // the mouse during drag, rendering it essentially useless.
    protected class MouseHandler implements MouseListener, MouseMoveListener, FocusListener, Runnable {
        // mouse
        private int mouseButton;
        private Point mouseDownPos;
        private RectangleFigure dragSelectionRectangle;
        private boolean wasDragDrop = false;
        private Object elementToRename;

        @Override
        public void mouseDown(MouseEvent e) {
            canvas.forceFocus(); // Not setFocus(), because it won't cause the Rename cell editor to close! See setFocus() impl.
            mouseButton = e.button;
            mouseDownPos = new Point(e.x, e.y);
            wasDragDrop = false;
            boolean withModifier = (e.stateMask & SWT.MODIFIER_MASK) != 0;
            boolean shift = (e.stateMask & SWT.SHIFT) != 0;
            boolean ctrl = (e.stateMask & SWT.CTRL) != 0;
            Object element = getElementAt(e.x, e.y);
            if (e.button == 1 && selectedElements.size() == 1 && selectedElements.get(0) == element && !withModifier) {
                // initiate direct rename, but only after a delay (otherwise it interferes with double-clicks)
                elementToRename = element;
                int delayMillis = Display.getCurrent().getDoubleClickTime() + 100;
                Display.getCurrent().timerExec(delayMillis, this); // ends up calling run()
                return;
            }

            if (mouseButton == 1 && !ctrl && !selectionContainsPoint(mouseDownPos)) // note: allow multi-selection to be drag'n'dropped
                clearSelection();
            if (element != null) {
                if (mouseButton == 1) {
                    if (shift)
                        selectTo(element);
                    else if (ctrl)
                        toggleSelection(element);
                    else // no modifier
                        addToSelection(element);
                }
                else {
                    addToSelection(element);
                }
            }
        }

        /*
         * DirectRename's timer
         */
        @Override
        public void run() {
            if (canvas.isDisposed() || Display.getCurrent().getFocusControl() != canvas)
                return;
            if (elementToRename != null && selectedElements.size() == 1 && selectedElements.get(0) == elementToRename) {
                startDirectRename(elementToRename);
                elementToRename = null;
            }
        }

        @Override
        public void mouseMove(MouseEvent e) {
            if (mouseButton != 1)
                return; // not a button1 drag

            // start drag selection
            if (dragSelectionRectangle == null && !selectionContainsPoint(mouseDownPos))
                startDrag();

            // update the drag selection rectangle
            if (dragSelectionRectangle != null) {
                boolean ctrl = (e.stateMask & SWT.CTRL) != 0;
                Rectangle bounds = new Rectangle(Math.min(e.x, mouseDownPos.x), Math.min(e.y, mouseDownPos.y), Math.abs(e.x - mouseDownPos.x), Math.abs(e.y - mouseDownPos.y));
                dragSelectionRectangle.setBounds(bounds);
                selectByRectangle(bounds, ctrl);
            }

            // scroll if mouse goes outside the canvas
            int yoffset = scrolledComposite.getOrigin().y;
            if (e.y - yoffset < 0)
                scrolledComposite.setOrigin(0, yoffset - 2);
            else if (e.y - yoffset > scrolledComposite.getSize().y)
                scrolledComposite.setOrigin(0, yoffset + 2);
        }

        @Override
        public void mouseUp(MouseEvent e) {
            boolean ctrl = (e.stateMask & SWT.CTRL) != 0;
            boolean shift = (e.stateMask & SWT.SHIFT) != 0;
            if (dragSelectionRectangle == null && !ctrl && !shift && !wasDragDrop) {
                Object element = getElementAt(mouseDownPos.x, mouseDownPos.y);
                    if (element == null)
                    clearSelection();
                else
                    select(element);
            }

            mouseButton = 0;
            if (dragSelectionRectangle != null)
                clearDrag();
        }

        @Override
        public void mouseDoubleClick(MouseEvent e) {
            if (e.button != 1)
                return;
            Object element = getElementAt(e.x, e.y);
            if (element != null)
                fireDoubleClick(new DoubleClickEvent(IconGridViewer.this, getSelection()));
        }

        public void dragInProgress() {
            wasDragDrop = true;
        }

        @Override
        public void focusLost(FocusEvent arg0) {
            // emulate mouseUp
            mouseButton = 0;
            if (dragSelectionRectangle != null)
                clearDrag();
        }

        @Override
        public void focusGained(FocusEvent arg0) {
        }

        protected void startDrag() {
            dragSelectionRectangle = new RectangleFigure();
            dragSelectionRectangle.setAlpha(80);
            dragSelectionRectangle.setFill(true);
            dragSelectionRectangle.setBackgroundColor(dragRectangleFillColor);
            dragSelectionRectangle.setForegroundColor(dragRectangleOutlineColor);
            feedbackLayer.add(dragSelectionRectangle);
        }

        protected void clearDrag() {
            if (dragSelectionRectangle != null) {
                feedbackLayer.remove(dragSelectionRectangle);
                dragSelectionRectangle = null;
            }
        }

    };

/*
 * TODO example code from: https://www.eclipse.org/forums/index.php/t/55474/
    FigureCanvas canvas = new FigureCanvas(shell);
    canvas.getViewport().setContentsTracksWidth(true);
    Figure panel = new Figure();
    panel.setLayoutManager(new ToolbarLayout());
    TextFlow content = new TextFlow(System.getProperties().toString());
    FlowPage fp = new FlowPage();
    fp.add(content);
    panel.add(fp);
    panel.add(new Label("A label"));
    canvas.setContents(panel);
 */

    public IconGridViewer(Composite parent) {
        scrolledComposite = new ScrolledComposite(parent,  SWT.V_SCROLL | SWT.BORDER);
        canvas = new Canvas(scrolledComposite, SWT.NONE);
        canvas.setSize(500, 1000);
        scrolledComposite.setContent(canvas);
        lws = new LightweightSystem(canvas);

        initializeFigures(parent);
        setupDragAndDrop();
        hookListeners();
    }

    protected void initializeFigures(Composite parent) {
        IFigure rootFigure = lws.getRootFigure();
        LayeredPane layeredPane = new LayeredPane();
        rootFigure.add(layeredPane);

        contentLayer = new Layer();
        contentLayer.setBorder(new MarginBorder(DEFAULT_MARGIN));
        FlowLayout flowlayout = new FlowLayout();
        flowlayout.setMajorSpacing(DEFAULT_VERT_SPACING);
        flowlayout.setMinorSpacing(DEFAULT_HORIZ_SPACING);
        contentLayer.setLayoutManager(flowlayout);
        contentLayer.setRequestFocusEnabled(true);
        layeredPane.add(contentLayer);

        feedbackLayer = new Layer();
        feedbackLayer.setLayoutManager(new XYLayout());
        layeredPane.add(feedbackLayer);

        contentLayer.requestFocus();
    }

    private void hookListeners() {
        scrolledComposite.addControlListener(new ControlAdapter() {
            public void controlResized(ControlEvent e) {
                adjustCanvasSize();
            }
        });

        canvas.addMouseListener(mouseHandler);
        canvas.addMouseMoveListener(mouseHandler);
        canvas.addFocusListener(mouseHandler);

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
                else if (ke.keyCode == SWT.F2)
                    startDirectRename(focusElement);
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

    protected void setupDragAndDrop() {
        int dndOperations = DND.DROP_COPY | DND.DROP_MOVE | DND.DROP_LINK;
        Transfer[] transferTypes = new Transfer[] { LocalSelectionTransfer.getTransfer() };
        final DragSource dragSource = new DragSource(getCanvas(), dndOperations);
        dragSource.setTransfer(transferTypes);

        dragSource.addDragListener(new DragSourceListener() {
            @Override
            public void dragStart(DragSourceEvent event) {
                Object element = getElementAt(event.x, event.y);
                if (element == null)
                    event.doit = false;
                else
                    mouseHandler.dragInProgress();

            }
            @Override
            public void dragFinished(DragSourceEvent event) {
            }

            @Override
            public void dragSetData(DragSourceEvent event) {
                event.data = getSelection();
            }
        });


        final DropTarget dropTarget = new DropTarget(getCanvas(), dndOperations);
        dropTarget.setTransfer(transferTypes);

        dropTarget.addDropListener(new DropTargetAdapter() {
            @Override
            public void dragOver(DropTargetEvent event) {
                org.eclipse.swt.graphics.Point p = getCanvas().toControl(event.x, event.y);
                //TODO allow our listener to report back whether drag-and-drop is possible
            }

            @Override
            public void drop(DropTargetEvent event) {
                org.eclipse.swt.graphics.Point p = getCanvas().toControl(event.x, event.y);
                Object[] elements = getSelection().toArray(); //((IStructuredSelection)event.data).toArray();
                fireDrop(elements, p);
            }
        });

    }

    public void setMargin(int margin) {
        contentLayer.setBorder(new MarginBorder(margin));
    }

    public int getMargin() {
        MarginBorder border = (MarginBorder)contentLayer.getBorder();
        return border.getInsets(contentLayer).left; // all four are the same
    }

    public void setSpacing(int horiz, int vert) {
        FlowLayout layout = (FlowLayout)contentLayer.getLayoutManager();
        layout.setMajorSpacing(vert);
        layout.setMinorSpacing(horiz);
    }

    public int getHorizontalSpacing() {
        FlowLayout layout = (FlowLayout)contentLayer.getLayoutManager();
        return layout.getMinorSpacing();
    }

    public int getVerticalSpacing() {
        FlowLayout layout = (FlowLayout)contentLayer.getLayoutManager();
        return layout.getMajorSpacing();
    }

    public void setBackground(Color color) {
        scrolledComposite.setBackground(color);
        canvas.setBackground(color);
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

    public void addDropListener(IDropListener listener) {
        dropListeners.add(listener);
    }

    public void removeDropListener(IDropListener listener) {
        dropListeners.remove(listener);
    }

    public void setRenameAdapter(IRenameAdapter renameAdapter) {
        this.renameAdapter = renameAdapter;
    }

    public IRenameAdapter getRenameAdapter() {
        return renameAdapter;
    }

    protected boolean selectionContainsPoint(Point p) {
        for (Object element : selectedElements)
            if (elementsToFigures.get(element).containsPoint(p))
                return true;
        return false;
    }

    protected void fireDoubleClick(final DoubleClickEvent event) {
        for (IDoubleClickListener l : doubleClickListeners) {
            SafeRunnable.run(new SafeRunnable() {
                @Override
                public void run() {
                    l.doubleClick(event);
                }
            });
        }
    }

    protected void fireDrop(Object[] elements, org.eclipse.swt.graphics.Point p) {
        for (IDropListener listener : dropListeners) {
            SafeRunnable.run(new SafeRunnable() {
                @Override
                public void run() {
                    listener.drop(elements, p);
                }
            });
        }
    }

    public Object getElementAt(int x, int y) {
        IFigure figure = contentLayer.findFigureAt(x, y);
        if (figure == null || figure == contentLayer)
            return null;
        return getElementFromFigure(figure);
    }

    public Object getElementAtOrAfter(int x, int y) {
        if (elements.length == 0)
            return null;
        Object element;
        if ((element = getElementAt(x, y)) != null) // exactly
            return element;

        int topMargin = getMargin();
        int hspacing = getHorizontalSpacing();

        if ((element = getElementAt(x + hspacing, y)) != null) // right in front of an element
            return element;
        if (y < topMargin) // in top margin area
            return elements[0];
        Object prevElement = getElementAt(x - hspacing, y); // right after an element (last one in the line); TODO find bbox of line!
        if (prevElement != null) {
            int index = ArrayUtils.indexOf(elements,  prevElement);
            if (index >= 0 && index != elements.length-1)
                return elements[index+1];
        }
        return null;
    }

    @Override
    public ScrolledComposite getControl() {
        return scrolledComposite;
    }

    public Canvas getCanvas() {
        return canvas;
    }

    @Override
    public IStructuredSelection getSelection() {
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
            for (Object element : structuredSelection.toList())
                if (ArrayUtils.contains(elements, element)) // add only those that are part of the viewer's content
                    selectedElements.add(element);
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
            figure.setBorder(new MarginBorder(1));
        }
        if (focusElement != null) {
            IFigure figure = elementsToFigures.get(focusElement);
            figure.setBorder(new LineBorder(focusElementBorderColor, 1));
        }
    }

    public void startDirectRename(Object element) {
        if (!ArrayUtils.contains(elements, element))
            throw new RuntimeException("element not found");

        IRenameAdapter renameAdapter = getRenameAdapter();
        if (renameAdapter == null || !renameAdapter.isRenameable(element))
            return;

        focusElement = null; // hide focus rectangle
        clearSelection();
        refresh(); // if selection was already empty, focus rect doesn't disappear otherwise

        CellEditor cellEditor = createCellEditor();
        configureCellEditor(cellEditor, element);

        cellEditor.setValue(renameAdapter.getName(element));
        cellEditor.getControl().setVisible(true);
        cellEditor.setFocus();
        cellEditor.activate();

        while (cellEditor.isActivated())
            Display.getCurrent().readAndDispatch();

        String value = (String)cellEditor.getValue(); // null if cancelled
        if (value != null && renameAdapter.isNameValid(element, value))
            renameAdapter.setName(element, value);

        cellEditor.dispose();

        select(element);
        refresh();
    }

    private CellEditor createCellEditor() {
        CellEditor cellEditor = new TextCellEditor(getCanvas(), SWT.BORDER | SWT.MULTI | SWT.WRAP) {
            @Override
            protected Control createControl(Composite parent) {
                Text text = (Text)super.createControl(parent);

                // Issue 1: The cell editor is normally committed via the widgetDefaultSelected()
                // method. However, with SWT.MULTI a plain Enter just inserts a newline and does not
                // fire a default selection (only Ctrl+Enter does).
                // Workaround: fake a default selection event from a keyboard listener.
                //
                // Issue 2: Escape key does not close the cell editor or clear (invalidate) the value
                // by default, only fires a cancelEditor() event.
                // Solution: manually invalidate the value and close the cell editor.
                //
                text.addKeyListener(new KeyAdapter() {
                    @Override
                    public void keyPressed(KeyEvent e) {
                        if (e.character == '\r') {
                            handleDefaultSelection(null); // khmm... (event arg is not used in the actual code)
                            e.doit = false; // prevents '\r' from being added to the text
                        }
                        else if (e.character == SWT.ESC) {
                            setValueValid(false);
                            fireCancelEditor();
                            deactivate();
                            e.doit = false;
                        }
                    }
                });
                return text;
            }
        };
        return cellEditor;
    }

    private void configureCellEditor(CellEditor cellEditor, Object element) {
        Control text = cellEditor.getControl();
        LabeledIcon f = (LabeledIcon)elementsToFigures.get(element);
        Rectangle b = f.getTextBounds();
        int sideMargin = 10, bottomMargin = 30; // make room for at least 1 extra line
        text.setBounds(b.x - sideMargin, b.y, b.width + 2*sideMargin, b.height + bottomMargin);
    }

    @Override
    public void refresh() {
        Debug.println("IconGridViewer.refresh()");

        IStructuredContentProvider contentProvider = (IStructuredContentProvider)getContentProvider();
        Object[] elements = contentProvider.getElements(getInput());

        boolean elementListChanged = false;
        if (!ArrayUtils.isEquals(elements, this.elements)) {
            Object[] oldElements = this.elements;
            this.elements = elements;
            elementListChanged = true;

            // remove deleted elements from selection
            for (Object element : selectedElements.toArray())
                if (!ArrayUtils.contains(elements, element))
                    selectedElements.remove(element);

            // clear focus if element no longer exists
            if (focusElement != null && !ArrayUtils.contains(elements, focusElement)) {
                int pos = ArrayUtils.indexOf(oldElements, focusElement);
                focusElement = elements.length == 0 ? null : pos < elements.length ? elements[pos] : elements[elements.length-1];
            }

            // remove all figures, then add them back in the elements' order
            contentLayer.removeAll();
            for (Object element : elements) {
                IFigure figure = elementsToFigures.get(element);
                if (figure == null) {
                    figure = createFigure();
                    elementsToFigures.put(element,  figure);
                }
                contentLayer.add(figure);
            }

            // remove deleted elements from elementsToFigures[]
            ArrayList<Object> trash = new ArrayList<>();
            for (Object element : elementsToFigures.keySet())
                if (!ArrayUtils.contains(elements, element))
                    trash.add(element);
            for (Object element : trash)
                elementsToFigures.remove(element); // its figure was already removed above
        }

        // synchronize icons and labels
        ILabelProvider labelProvider = (ILabelProvider)getLabelProvider();
        for (Object element : elements) {
            IFigure figure = elementsToFigures.get(element);
            LabeledIcon labelFigure = (LabeledIcon)figure;
            String text = labelProvider.getText(element);
            Image image = labelProvider.getImage(element);
            labelFigure.setIcon(image);
            labelFigure.setText(text);
        }

        setSelectionToWidget();

        if (elementListChanged)
            adjustCanvasSize();
    }

    @Override
    protected void inputChanged(Object input, Object oldInput) {
        super.inputChanged(input, oldInput);
        refresh();
    }

    protected IFigure createFigure() {
        LabeledIcon label = new LabeledIcon();
        label.setBackgroundColor(selectionFillColor);  // note: only takes effect when 'opaque' is set; we use this for selection
        label.setBorder(new MarginBorder(1));
        label.setPreferredSize(new Dimension(100, -1));
        return label;
    }

    protected void adjustCanvasSize() {
        int figuresMaxY = 0;
        if (elements != null)
            for (Object element : elements)
                figuresMaxY = Math.max(figuresMaxY, elementsToFigures.get(element).getBounds().bottom());

        org.eclipse.swt.graphics.Rectangle controlArea = scrolledComposite.getClientArea();
        canvas.setSize(controlArea.width, Math.max(figuresMaxY + getMargin(), controlArea.height));
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
        return (width - 2*DEFAULT_MARGIN) / 100; //FIXME
    }

    protected void moveUp(boolean extendSelection) {
        moveBy(-1*getItemsPerRow(), extendSelection);
    }

    protected void moveDown(boolean extendSelection) {
        moveBy(getItemsPerRow(), extendSelection);
    }

    protected void movePageUp(boolean extendSelection) {
        ensureFocusElement();
        if (focusElement == null)
            return;
        int pos = ArrayUtils.indexOf(elements, focusElement);
        int numRowsToJump = 5; //TODO
        int newPos = pos - getItemsPerRow() * numRowsToJump;
        moveTo(elements[Math.max(0, newPos)], extendSelection);
    }

    protected void movePageDown(boolean extendSelection) {
        ensureFocusElement();
        if (focusElement == null)
            return;
        int pos = ArrayUtils.indexOf(elements, focusElement);
        int numRowsToJump = 5; //TODO
        int newPos = pos + getItemsPerRow() * numRowsToJump;
        moveTo(elements[Math.min(elements.length-1, newPos)], extendSelection);
    }

    protected void moveHome(boolean extendSelection) {
        if (elements.length > 0)
            moveTo(elements[0], extendSelection);
    }

    protected void moveEnd(boolean extendSelection) {
        if (elements.length > 0)
            moveTo(elements[elements.length-1], extendSelection);
    }

//TODO
//    class Row {
//        int y;
//        List<IFigure> figures;
//    }
//
//    protected void findRows() {
//        int y = -1;
//        List<Integer> rows
//        for (IFigure f : )
//    }

    protected void selectByRectangle(Rectangle rectangle, boolean extendSelection) {
        if (!extendSelection)
            selectedElements.clear();
        for (Object element : elements) {
            if (!selectedElements.contains(element)) {
                IFigure figure = elementsToFigures.get(element);
                if (rectangle.intersects(figure.getBounds()))
                    selectedElements.add(element);
            }
        }
        if (!selectedElements.isEmpty())
            focusElement = selectedElements.get(selectedElements.size()-1); // focus on last element
        setSelectionToWidget();
        fireSelectionChanged();
    }

    protected void clearSelection() {
        if (!selectedElements.isEmpty()) {
            selectedElements.clear();
            setSelectionToWidget();
            fireSelectionChanged();
        }
    }

    protected void select(Object element) {
        if (selectedElements.size() != 1 || selectedElements.get(0) != element || focusElement != element) {
            focusElement = element;
            selectedElements.clear();
            selectedElements.add(element);
            setSelectionToWidget();
            fireSelectionChanged();
        }
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
            setSelectionToWidget(); // redraw focus mark
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
        // note: deliberately no focusElement = element;
        setSelectionToWidget();
        fireSelectionChanged();
    }
}

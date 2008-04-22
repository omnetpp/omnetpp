package org.omnetpp.common.virtualtable;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
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
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;

/**
 * The virtual table is a virtually infinite table.
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
    protected ListenerList selectionChangedListeners = new ListenerList();

	/**
	 * True means the table will jump to the selection and switch input automatically when it gets notified about a selection change
	 * even if the input is different from the current one.
	 */
	protected boolean followSelection = true;

	/**
	 * A list of selected elements;
	 */
	protected List<T> selectionElements;

	/**
	 * The input being show by this table.
	 */
	protected Object input;

	/**
	 * The content provider which will be queried for subsequent elements in the virtual table.
	 */
	protected IVirtualTableContentProvider<T> contentProvider;

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

	public VirtualTable(Composite parent, int style) {
		super(parent, style | SWT.V_SCROLL);

		drawLines = true;
		setLayout(new FillLayout());
		setBackground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_BACKGROUND));

		createComposite(this);
		createCanvas(composite);
		createTable(composite);

		scrolledComposite.setContent(composite); 
		
		getVerticalBar().addSelectionListener(new SelectionAdapter() {
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
				else
					scrollToElement(contentProvider.getApproximateElementAt(percentage));
			}
		});

    	hoverSupport = new HoverSupport();
    	hoverSupport.setHoverSizeConstaints(700, 200);
    	hoverSupport.adapt(canvas, new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
				T element = getElementAtDistanceFromFixPoint(y / getRowHeight() + getTopVisibleElementDistanceFromFixPoint());
				
				if (element == null)
					return null;
				else
					return HoverSupport.addHTMLStyleSheet(getRowRenderer().getTooltipText(element, outSizeConstraint));
			}
    	});

 		// recompute table size after the scrollable area size is known
        Display.getCurrent().asyncExec(new Runnable() {
			public void run() {
				recomputeTableSize();
			}
        });
	}

	private void createComposite(Composite parent) {
		scrolledComposite = new ScrolledComposite(parent, SWT.H_SCROLL | SWT.V_SCROLL);
		scrolledComposite.addControlListener(new ControlAdapter() {
			public void controlResized(ControlEvent e) {
				composite.layout();
		        composite.setSize(composite.computeSize(SWT.DEFAULT, SWT.DEFAULT));
				scrolledComposite.getVerticalBar().setVisible(false);
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
			public void controlResized(ControlEvent e) {
				configureVerticalScrollBar();
				redraw();
			}
		});

		canvas.addKeyListener(new KeyListener() {
			public void keyPressed(KeyEvent e) {
				if (e.keyCode == SWT.F5)
					refresh();
				else if (e.keyCode == SWT.ARROW_LEFT)
					scrollHorizontal(-10);
				else if (e.keyCode == SWT.ARROW_RIGHT)
					scrollHorizontal(10);
				else if (e.keyCode == SWT.ARROW_UP)
					moveSelection(-1);
				else if (e.keyCode == SWT.ARROW_DOWN)
					moveSelection(1);
				else if (e.keyCode == SWT.PAGE_UP)
					moveSelection(-getPageJumpCount());
				else if (e.keyCode == SWT.PAGE_DOWN)
					moveSelection(getPageJumpCount());
				else if (e.keyCode == SWT.HOME)
					gotoBegin();
				else if (e.keyCode == SWT.END)
					gotoEnd();
			}

			public void keyReleased(KeyEvent e) {
			}
		});
		
		canvas.addMouseListener(new MouseAdapter() {
			public void mouseDown(MouseEvent e) {
				if (input != null && contentProvider != null) {
					T element = getVisibleElementAt(e.y / getRowHeight());

					if (element != null && 
						(e.button == 1 || selectionElements == null || !selectionElements.contains(element))) {
						if ((e.stateMask & SWT.CONTROL) != 0) {
							if (selectionElements.contains(element))
								selectionElements.remove(element);
							else
								selectionElements.add(element);
							
							fireSelectionChanged();
						}
						else
							setSelectionElement(element);
					}
	
					redraw();
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
			public void controlResized(ControlEvent e) {
				composite.layout();
			}
		});

        table.setSize(table.computeSize(SWT.DEFAULT, SWT.DEFAULT));
	}

	public TableColumn createColumn() {
		TableColumn tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.addControlListener(new ControlAdapter() {
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
		// remember input
		this.input = input;
		getContentProvider().inputChanged(null, null, input);
		getRowRenderer().setInput(input);

		// clear state
		clearSelection();
		configureVerticalScrollBar();
		relocateFixPoint(null, 0);
	}

	public IVirtualTableContentProvider<T> getContentProvider() {
		return contentProvider ;
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
        Object[] listeners = selectionChangedListeners.getListeners();
        for (int i = 0; i < listeners.length; ++i) {
            final ISelectionChangedListener l = (ISelectionChangedListener) listeners[i];
            SafeRunnable.run(new SafeRunnable() {
                public void run() {
                    l.selectionChanged(event);
                }
            });
        }
    }

	public ISelection getSelection() {
		return new VirtualTableSelection<T>(getInput(), getSelectionElements());
	}

	@SuppressWarnings("unchecked")
	public void setSelection(ISelection selection) {
		if (debug)
			System.out.println("VirtualTable got selection: " + selection);

		IVirtualTableSelection<T> virtualTableSelection = (IVirtualTableSelection<T>)selection;
		Object selectionInput = virtualTableSelection.getInput();

		if (input != selectionInput) {
			if (followSelection)
				setInput(selectionInput);
			else
				throw new RuntimeException("Invalid selection");
		}

		if (!elementListEquals(selectionElements, virtualTableSelection.getElements())) {
			setSelectionElements(virtualTableSelection.getElements());
			scrollToSelectionElement();
	
			fireSelectionChanged();
		}
	}

	/**
	 * Removes all selection elements.
	 */
	public void clearSelection() {
		if (selectionElements != null && selectionElements.size() != 0) {
			selectionElements.clear();
	
			fireSelectionChanged();
		}
	}

	/**
	 * Returns the current selection.
	 */
	public T getSelectionElement() {
		if (selectionElements != null && selectionElements.size() != 0)
			return selectionElements.get(0);
		else
			return null;
	}

	public void setSelectionElement(T element) {
		Assert.isTrue(element != null);

		selectionElements = new ArrayList<T>();
		selectionElements.add(element);
		
		fireSelectionChanged();
	}

	/**
	 * Returns the current selection.
	 */
	public List<T> getSelectionElements() {
		return selectionElements;
	}

	/**
	 * Selects the given elements, and goes to the first one.
	 */
	public void setSelectionElements(List<T> elements) {
		selectionElements = elements;

		fireSelectionChanged();
	}

	/**
	 * See setFollowSelection().
	 */
	public boolean getFollowSelection() {
		return followSelection;
	}

	/**
	 * Sets whether this widget should always switch to the element which comes in
	 * the selection (=true), or stick to the input set with setInput() (=false).
	 * The proper setting typically depends on whether the widget is used in an
	 * editor (false) or in a view (true).
	 *
	 * Default is true.
	 */
	public void setFollowSelection(boolean followSelection) {
		this.followSelection = followSelection;
	}

	/**
	 * Position the selection to the given element and make it visible.
	 */
	public void gotoElement(T element) {
		Assert.isTrue(element != null);

		setSelectionElement(element);		
		scrollToElement(element);
	}

	/**
	 * Position the selection to the closest shown element and make it visible.
	 */
	public void gotoClosestElement(T element) {
		gotoElement(contentProvider.getClosestElement(element));
	}

	/**
	 * Position the selection to the very beginning of the table.
	 */
	public void gotoBegin() {
		T firstElement = contentProvider.getFirstElement();
		
		if (firstElement != null)
			setSelectionElement(firstElement);

		scrollToBegin();
	}

	/**
	 * Position the selection to the very end of the table.
	 */
	public void gotoEnd() {
		T lastElement = contentProvider.getLastElement();
		
		if (lastElement != null)
			setSelectionElement(lastElement);

		scrollToEnd();
	}

	/**
	 * Updates the invalid fix point to a valid one staying as close to the current position as it is possible.
	 */
	public void stayNear() {
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
			verticalBar.setMaximum((int)Math.max(numberOfElements, 1E+6));
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

	/**
	 * Moves the selection with the given number of elements up or down and scrolls if necessary.
	 */
	public void moveSelection(int numberOfElements) {
		T element = getSelectionElement();

		if (element == null)
			element = getTopVisibleElement();

		element = contentProvider.getNeighbourElement(element, numberOfElements);
		
		if (element == null) {
			if (numberOfElements < 0)
				element = contentProvider.getFirstElement();
			else
				element = contentProvider.getLastElement();
		}

		if (element != null)
			setSelectionElement(element);

		scrollToElement(element);
	}

	/**
	 * Scrolls the visible area horizontally with the given amount of pixels.
	 */
	public void scrollHorizontal(int pixel) {
		Point p = scrolledComposite.getOrigin();
		scrolledComposite.setOrigin(p.x + pixel, p.y);
	}
	
	/**
	 * Scrolls with the given number of elements up or down.
	 */
	public void scroll(int numberOfElements) {
		relocateFixPoint(fixPointElement, fixPointDistance + numberOfElements);
		redraw();
	}

	/**
	 * Scroll to the given element making it visible with as little scrolling as it is possible.
	 */
	public void scrollToElement(T element) {
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
				relocateFixPoint(element, getFullyVisibleElementCount() - 1);
		}

		redraw();
	}
	
	/**
	 * Scroll to the selection element making it visible.
	 */
	public void scrollToSelectionElement() {
		T element = getSelectionElement();

		if (element != null)
			scrollToElement(element);
	}

	/**
	 * Scroll to the very beginning making it visible.
	 */
	public void scrollToBegin() {
		relocateFixPoint(contentProvider.getFirstElement(), 0);
		redraw();
	}

	/**
	 * Scroll to the very end making it visible.
	 */
	public void scrollToEnd() {
		relocateFixPoint(contentProvider.getLastElement(), getFullyVisibleElementCount() - 1);
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
						fixPointDistance = getFullyVisibleElementCount() - 1;
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
				int x = 0;
				Transform rowTransform = new Transform(null);
				T element = getElementAtDistanceFromFixPoint(i - fixPointDistance);
				List<T> selectionElements = getSelectionElements();
				int[] columnOrder = table.getColumnOrder();

				if (element != null) {
					boolean isSelectedElement = selectionElements != null && selectionElements.contains(element);
					
					if (isSelectedElement) {
						gc.setBackground(Display.getCurrent().getSystemColor(canvas.isFocusControl() ? SWT.COLOR_LIST_SELECTION : SWT.COLOR_WIDGET_BACKGROUND));
						gc.fillRectangle(new Rectangle(0, i * getRowHeight(), clipping.x + clipping.width, getRowHeight()));
					}
					else
						gc.setBackground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_BACKGROUND));

					if (drawLines) {
						gc.setForeground(LINE_COLOR);
						gc.drawLine(0, i * getRowHeight() - 1, clipping.x + clipping.width, i * getRowHeight() - 1);
					}
	
					for (int j = 0; j < table.getColumnCount(); j++) {
						TableColumn column = table.getColumn(columnOrder[j]);
						rowTransform.setElements(1, 0, 0, 1, 0, 0);
						rowTransform.translate(x, i * getRowHeight());
						gc.setTransform(rowTransform);
						gc.setClipping(new Rectangle(0, 0, column.getWidth(), getRowHeight()));

						if (isSelectedElement && canvas.isFocusControl())
							gc.setForeground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT));

						rowRenderer.drawCell(gc, element, columnOrder[j]);
						x += column.getWidth();
						
						if (drawLines) {
							gc.setForeground(LINE_COLOR);
							gc.drawLine(0, 0, 0, getRowHeight());
						}
	
						gc.setTransform(transform);
						gc.setClipping((Rectangle)null);
					}
				}
			}
		}
	}
	
	public void refresh() {
		redraw();
	}

	public void redraw() {
		if (debug)
			System.out.println("Redrawing canvas");

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
		return -fixPointDistance + Math.min((int)contentProvider.getApproximateNumberOfElements() - 1, getFullyVisibleElementCount() - 1);
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
            throw new IllegalArgumentException("Element is not visisble: " + visibleElement);

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

	private boolean elementListEquals(List<T> a, List<T> b) {
		if (a == b)
			return true;
		
		if (a == null || b == null || a.size() != b.size())
			return false;

		for (int i = 0; i < a.size(); i++) {
			T ae = a.get(i);
			T be = b.get(i);

			if (!(ae == null ? be == null : ae.equals(be)))
				return false;
		}
		
		return true;
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
				System.out.println("Relayouting virtual table: " + r.height);

			canvas.setBounds(0, headerHeight, table.getSize().x, r.height - headerHeight);

			int sumColumnWidth = 0;
			for (int i = 0; i < table.getColumnCount(); i++)
				sumColumnWidth += table.getColumn(i).getWidth();

			if (r.width > sumColumnWidth)
				table.setSize(r.width, table.getSize().y);
		}
	}
}

package org.omnetpp.simulation.canvas;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import org.apache.commons.collections.CollectionUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.DelegatingLayout;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.DecoratingStyledCellLabelProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Pattern;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.ui.ArrayTreeContentProvider;
import org.omnetpp.common.ui.CheckedTreeSelectionDialog2;
import org.omnetpp.common.ui.HoverInfo;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.simulation.InspectorRegistry;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.ISimulationStateListener;
import org.omnetpp.simulation.editors.SimulationEditor;
import org.omnetpp.simulation.figures.IInspectorFigure;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.inspectors.InspectorDescriptor;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.ObjectFieldsTreeLabelProvider;

/**
 *
 * @author Andras
 */
//TODO turn off callouts while the simulation is running!!! also add a global action to turn it on/off!
//FIXME how to evaluate "$PARNAME" references in display strings???
public class SimulationCanvas extends FigureCanvas implements IInspectorContainer {
    private final Image BACKGROUND_IMAGE = SimulationPlugin.getCachedImage("icons/misc/paper.png"); //XXX bg.png

    // the simulation editor that contains this simulation canvas
    private SimulationEditor simulationEditor;

    // layers
    private Figure inspectorsLayer;
    private Layer controlsLayer;

    // inspectors, selection handling
    protected List<IInspectorPart> inspectors = new ArrayList<IInspectorPart>();
    protected ISimulationStateListener simulationListener;
    protected ListenerList selectionChangedListeners = new ListenerList(); // list of selection change listeners (type ISelectionChangedListener)
    protected IStructuredSelection currentSelection = new StructuredSelection();

    // support for moving/resizing inspector by the border (or by arbitrary area of any child figure)
    protected MoveResizeSupport moveResizeSupport;

    // support for a floating toolbar for inspectors
    protected FloatingToolbarSupport floatingToolbarSupport;

    protected CalloutSupport calloutSupport;

    /**
     * Inner class, used for the background with the paper pattern
     */
    class ImagePatternFigure extends Figure {
        private Pattern pattern;

        public ImagePatternFigure(Image image) {
            pattern = new Pattern(Display.getCurrent(), image);
        }

        protected void paintFigure(Graphics graphics) {
            graphics.setBackgroundPattern(pattern);
            graphics.fillRectangle(getBounds());
            graphics.setBackgroundPattern(null);
        }
    }

    public SimulationCanvas(Composite parent, int style, SimulationEditor simulationEditor) {
        super(parent, style);

        this.simulationEditor = simulationEditor;

        Layer layeredPane = new Layer() {
            @Override
            public void paint(Graphics graphics) {
                graphics.setAntialias(SWT.ON);
                super.paint(graphics);
            }
        };
        layeredPane.setLayoutManager(new StackLayout());

        inspectorsLayer = new ImagePatternFigure(BACKGROUND_IMAGE);
        inspectorsLayer.setLayoutManager(new XYLayout());
        layeredPane.add(inspectorsLayer);


        controlsLayer = new Layer();
        controlsLayer.setLayoutManager(new DelegatingLayout());
        layeredPane.add(controlsLayer);

        setContents(layeredPane);

        // create context menu (note: addMenuDetectListener() is not good because it's too early:
        // setRemoveAllWhenShown() takes effect after that, clearing our newly added actions;
        // and calling removeAll() in the listener would remove contributed actions too)
        final MenuManager menuManager = new MenuManager("#popup");
        menuManager.setRemoveAllWhenShown(true);
        setMenu(menuManager.createContextMenu(this));
        menuManager.addMenuListener(new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager manager) {
                org.eclipse.swt.graphics.Point loc = toControl(Display.getCurrent().getCursorLocation());
                Point p = translateCanvasToAbsoluteFigureCoordinates(loc.x, loc.y);
                IInspectorPart inspectorPart = findInspectorAt(p.x, p.y);
                if (inspectorPart != null)
                    inspectorPart.populateContextMenu(menuManager, p.x, p.y);
            }
        });

        // add advanced tooltip support for inspectors
        FigureUtils.addTooltipSupport(this, this.getContents());

        // add move/resize support inspectors
        moveResizeSupport = new MoveResizeSupport(this);

        // add support for a floating toolbar for inspectors
        floatingToolbarSupport = new FloatingToolbarSupport(this);

        // when an inspector is selected, show its nearest parent
        calloutSupport = new CalloutSupport(this);
        addSelectionChangedListener(new ISelectionChangedListener() {
            @Override
            public void selectionChanged(SelectionChangedEvent event) {
                setCalloutsToSelection(getSelection(), false); //XXX might also try "true", but it's not to my taste...
            }
        });

    }

    @Override
    public SimulationEditor getEditor() {
        return simulationEditor;
    }

    @Override
    public void dispose() {
        for (IInspectorPart inspectorPart : inspectors.toArray(new IInspectorPart[inspectors.size()]))
            removeInspectorPart(inspectorPart);
        super.dispose();
    }

    public void addMoveResizeSupport(IFigure figure) {
        if (moveResizeSupport != null)
            moveResizeSupport.adopt(figure);
    }

    public void removeMoveResizeSupport(IFigure figure) {
        if (moveResizeSupport != null)
            moveResizeSupport.forget(figure);
    }

    public void addMoveResizeSupport(Control control) {
        if (moveResizeSupport != null)
            moveResizeSupport.adopt(control);
    }

    public void removeMoveResizeSupport(Control control) {
        if (moveResizeSupport != null)
            moveResizeSupport.forget(control);
    }

    public IInspectorPart findInspectorAt(int x, int y) {
        IFigure figure = getInspectorsLayer().findFigureAt(x, y);
        return findInspectorFor(figure);
    }

    public IInspectorPart findInspectorFor(IFigure figure) {
        // inspectors' root figure must be IInspectorFigure, and only that
        while (figure != null && !(figure instanceof IInspectorFigure))
            figure = figure.getParent();
        if (figure == null)
            return null;
        for (IInspectorPart inspector : getInspectors())
            if (inspector.getFigure() == figure)
                return inspector;
        return null;
    }

    public IInspectorPart findInspectorFor(Control control) {
        while (control != null && control != this) {
            for (IInspectorPart inspector : getInspectors())
                if (inspector.getSWTControl() == control)
                    return inspector;
            control = control.getParent();
        }
        return null;
    }

    @Override
    public FigureCanvas getControl() {
        return this;
    }

    public Figure getInspectorsLayer() {
        return inspectorsLayer;
    }

    public Layer getControlsLayer() {
        return controlsLayer;
    }

    @Override
    public org.eclipse.swt.graphics.Point translateAbsoluteFigureCoordinatesToCanvas(int x, int y) {
        int xoffset = getHorizontalBar().getSelection();
        int yoffset = getVerticalBar().getSelection();
        return new org.eclipse.swt.graphics.Point(x - xoffset, y - yoffset);
    }

    @Override
    public Point translateCanvasToAbsoluteFigureCoordinates(int x, int y) {
        int xoffset = getHorizontalBar().getSelection();
        int yoffset = getVerticalBar().getSelection();
        return new Point(x + xoffset, y + yoffset);
    }

    public void addInspectorPart(IInspectorPart inspector) {
        addInspectorPart(inspector, -1, -1);
    }

    public void addInspectorPart(IInspectorPart inspector, int x, int y) {
        // register the inspector, and add it to the canvas
        inspectors.add(inspector);
        IFigure inspectorFigure = inspector.getFigure();
        getInspectorsLayer().add(inspectorFigure); // must precede location computation, otherwise getPreferredSize() might fail

        // initial refresh
        try {
            inspector.refresh();  // side effect: fill figure with content, so getPreferredSize() returns a more realistic size during location computation
        } catch (Exception e) {
            SimulationPlugin.logError("Error refreshing inspector " + inspector.toString(), e);
        }

        // placement on canvas
        if (x == -1 && y == -1) {
            Point loc = getLocationForInspector(inspector);
            x = loc.x; y = loc.y;
        }
        getInspectorsLayer().setConstraint(inspectorFigure, new Rectangle(x, y, -1, -1));

        // if inspector has SWT parts, add listeners to show floating toolbar when mouse is over them
        Control control = inspector.getSWTControl();
        if (control != null && floatingToolbarSupport != null)
            floatingToolbarSupport.addFloatingToolbarSupportTo(control, inspector);

    }

    protected Point getLocationForInspector(IInspectorPart inspector) {
        // choose best location (close to the top-left corner of the currently viewed area of the canvas)
        Point targetPoint = getViewport().getViewLocation();

        // collect potential locations
        int spacing = 10;
        IFigure figure = inspector.getFigure();
        Dimension size = figure.getPreferredSize();
        ArrayList<Point> candidates = new ArrayList<Point>();
        candidates.add(targetPoint);
        for (IInspectorPart otherInspector : getInspectors()) {
            if (otherInspector != inspector) {
                IFigure otherFigure = otherInspector.getFigure();
                Rectangle otherRectangle = otherFigure.getBounds(); // assume bounds is already set (layouting has run)
                candidates.add(otherRectangle.getTopLeft().translate(-spacing - size.width, 0)); //left
                candidates.add(otherRectangle.getTopLeft().translate(0, -spacing - size.height)); //above
                candidates.add(otherRectangle.getTopRight().translate(spacing, 0)); //right
                candidates.add(otherRectangle.getBottomLeft().translate(0, spacing)); //below
                candidates.add(new Point(targetPoint.x, otherRectangle.y)); //left-aligned 1
                candidates.add(new Point(targetPoint.x, otherRectangle.bottom() + spacing)); //left-aligned 2
            }
        }

        // choose best (smallest metric)
        double bestMetric = Double.POSITIVE_INFINITY;
        Point bestPoint = null;
        int sheetWidth = getInspectorsLayer().getSize().width;
        outer: for (Point point : candidates) {
            if (point.x >= 0 && point.y >= 0) {
                Rectangle rectangle = new Rectangle(point, size);

                // if overlaps with any other rectangle, discard
                for (IInspectorPart otherInspector : getInspectors()) {
                    if (otherInspector != inspector) {
                        IFigure otherFigure = otherInspector.getFigure();
                        Rectangle otherRectangle = otherFigure.getBounds(); // assume bounds is already set (layouting has run)
                        if (otherRectangle.intersects(rectangle))
                            continue outer; // overlaps
                    }
                }

                // compute metric
                final double f = 100;
                double metric = Math.sqrt(sqr(point.x - targetPoint.x) + f * sqr(point.y - targetPoint.y)); // f penalizes large delta y, to reduce the resulting "quarter-disc" effect
                if (point.x < targetPoint.x)
                    metric += 10 * (targetPoint.x - point.x); // penalty for hanging out on the left
                if (point.y < targetPoint.y)
                    metric += 10 * (targetPoint.y - point.y); // penalty for hanging out on the top
                if (rectangle.right() > sheetWidth)
                    metric += 10000 * (rectangle.right() - sheetWidth); // penalty for not fitting into sheetWidth

                if (metric < bestMetric) {
                    bestPoint = point;
                    bestMetric = metric;
                }
            }
        }
        figure.setBounds(new Rectangle(bestPoint, size)); // needed when many inspectors are opened in one go (no layouting between them)
        return bestPoint;
    }

    private static double sqr(double x) {
        return x*x;
    }
    public void removeInspectorPart(IInspectorPart inspectorPart) {
        System.out.println("removeInspectorPart: " + inspectorPart);
        Assert.isTrue(inspectors.contains(inspectorPart));

        if (floatingToolbarSupport != null)
            floatingToolbarSupport.closeFloatingToolbar();
        inspectors.remove(inspectorPart);
        inspectorPart.dispose();

        // note: this is done last (we don't want selection listeners see the nearly dead inspector)
        if (SelectionUtils.contains(currentSelection, inspectorPart))
            deselect(inspectorPart);
        //FIXME: if a submodules are selected within that inspector, remove them too!!!
    }

    public void refreshInspectors() {
        for (IInspectorPart inspectorPart : inspectors.toArray(new IInspectorPart[inspectors.size()])) {
            try {
                inspectorPart.refresh();
            } catch (Exception e) {
                SimulationPlugin.logError("Error refreshing inspector " + inspectorPart.toString(), e);
            }
        }
    }

    public void close(IInspectorPart inspectorPart) {
        removeInspectorPart(inspectorPart);
    }

    @Override
    public List<IInspectorPart> getInspectors() {
        return inspectors;
    }

    //FIXME there can be more than one inspector for this object!!! rename this to "getInspectorsFor()" or "getFirstInspectorFor()" ??
    public IInspectorPart findInspectorFor(cObject object) {
        for (IInspectorPart inspector : inspectors)
            if (inspector.getObject() == object)
                return inspector;
        return null;
    }

    public List<IInspectorPart> inspect(List<cObject> objects, boolean interactive) {
        List<IInspectorPart> inspectors = new ArrayList<IInspectorPart>();

        // count how many new inspectors we'll need to open
        List<cObject> uninspected = new ArrayList<cObject>();
        for (cObject object : objects) {
            IInspectorPart inspector = findInspectorFor(object);
            if (inspector == null)
                uninspected.add(object);
            else
                inspectors.add(inspector);
        }

        // if there are many inspectors to open, offer a checkbox list dialog for selection
        if (uninspected.size() >= 5 && interactive)
            uninspected = chooseObjectsToInspect(Display.getCurrent().getActiveShell(), "Select new inspectors to open:", uninspected);

        // open, select, reveal
        if (uninspected != null) {
            for (cObject object : uninspected) {
                IInspectorPart inspector = createInspectorFor(object);
                addInspectorPart(inspector);
                inspectors.add(inspector);
            }

            if (!inspectors.isEmpty()) {
                asyncReveal(inspectors.get(0));
                select(inspectors.toArray(), true);
            }
        }

        if (!inspectors.isEmpty())
            inspectors.get(0).setFocus();

        return inspectors;
    }

    public IInspectorPart inspect(cObject object) {
        Assert.isNotNull(object);

        IInspectorPart inspector = findInspectorFor(object);
        if (inspector != null) {
            reveal(inspector);
        }
        else {
            inspector = createInspectorFor(object);
            addInspectorPart(inspector);
            asyncReveal(inspector);
        }
        select(inspector, true);
        inspector.setFocus();

        return inspector;
    }

    protected IInspectorPart createInspectorFor(cObject object) {
        InspectorRegistry inspectorRegistry = SimulationPlugin.getDefault().getInspectorRegistry();
        InspectorDescriptor bestInspectorType = inspectorRegistry.getBestInspectorType(object);
        if (bestInspectorType == null)
            throw new RuntimeException("No suitable inspector for object"); // cannot happen, as we have catch-all inspector types
        return bestInspectorType.create(this, object);
    }

    public void reveal(IInspectorPart inspector) {
        // note: not good for newly created inspectors; use asyncReveal for them
        inspector.raiseToTop();
        Rectangle bounds = inspector.getFigure().getBounds();
        Rectangle viewport = getViewport().getBounds().getCopy().translate(getViewport().getViewLocation());
        if (!viewport.contains(bounds)) {
            // must scroll
            int leftEdge = viewport.x;
            int topEdge = viewport.y;

            // scroll minimum amount so that the inspector (or at least its top-left corner) fits into the viewport
            if (bounds.right() > leftEdge+viewport.width) leftEdge = bounds.right() - viewport.width;
            if (bounds.bottom() > topEdge+viewport.height) topEdge = bounds.bottom() - viewport.height;
            if (bounds.x < leftEdge) leftEdge = bounds.x;
            if (bounds.y < topEdge) topEdge = bounds.y;

            scrollSmoothTo(leftEdge, topEdge);
        }
    }

    protected void asyncReveal(final IInspectorPart inspector) {
        // Why needed: for new inspectors the following layout() call doesn't work to reveal the inspector
        // (looks like it doesn't cause the scrollbar or getContents().getBounds() to be updated); as a
        // workaround, we call reveal() in an asyncExec().
        //
        //getContents().getLayoutManager().layout(getContents());
        //reveal(finalInspector);

        Display.getDefault().asyncExec(new Runnable() {
            @Override
            public void run() {
                reveal(inspector);
            }
        });
    }

    /**
     * Utility function: pops up a dialog and lets the user choose one or more of the offered objects.
     * Null as return value means cancel.
     */
    @SuppressWarnings({ "unchecked", "rawtypes" })
    protected List<cObject> chooseObjectsToInspect(Shell parent, String message, List<cObject> objects) {
        CheckedTreeSelectionDialog2 dialog = new CheckedTreeSelectionDialog2(
                parent,
                new DecoratingStyledCellLabelProvider(new ObjectFieldsTreeLabelProvider(), null, null),
                new ArrayTreeContentProvider());
        dialog.setTitle("Inspect Objects");
        dialog.setMessage(message);
        dialog.setHelpAvailable(false);
        dialog.setInput(objects);
        dialog.setStatusLineAboveButtons(false);
        dialog.setInitialSelections(objects.toArray());

        if (dialog.open() == Window.OK)
            return (List)Arrays.asList(dialog.getResult());
        return null;
    }

    public void openFloatingToolbarFor(IInspectorPart inspector) {
        if (floatingToolbarSupport != null) {
            floatingToolbarSupport.closeFloatingToolbar();  // close existing
            floatingToolbarSupport.openFloatingToolbarFor(inspector);
        }
    }

    public void closeFloatingToolbar() {
        if (floatingToolbarSupport != null)
            floatingToolbarSupport.closeFloatingToolbar();
    }

    /**
     * Returns null if the floating toolbar is not displayed.
     */
    public Control getFloatingToolbar() {
        return floatingToolbarSupport == null ? null : floatingToolbarSupport.getFloatingToolbar();
    }

    public IInspectorPart getFloatingToolbarOwner() {
        return floatingToolbarSupport == null ? null : floatingToolbarSupport.getFloatingToolbarOwner();
    }

    @Override
    public void updateFloatingToolbarActions() {
        if (floatingToolbarSupport != null)
            floatingToolbarSupport.updateFloatingToolbarActions();
    }

    public HoverInfo getHoverFor(int canvasX, int canvasY) {
        Point p = translateCanvasToAbsoluteFigureCoordinates(canvasX, canvasY);
        IInspectorPart inspector = findInspectorAt(p.x, p.y);
        return inspector == null ? null : inspector.getHoverFor(p.x, p.y);
    }

    @SuppressWarnings({ "rawtypes", "unchecked" })
    public void select(Object object, boolean removeOthers) {
        if (removeOthers) {
            setSelection(new StructuredSelection(object));
        }
        else {
            IStructuredSelection selection = getSelection();
            if (!selection.toList().contains(object)) {
                List list = new ArrayList(selection.toList());
                list.add(object);
                setSelection(new StructuredSelection(list));
            }
        }
    }

    @SuppressWarnings("rawtypes")
    public void select(Object[] objects, boolean removeOthers) {
        if (removeOthers) {
            setSelection(new StructuredSelection(objects));
        }
        else {
            IStructuredSelection selection = getSelection();
            Collection list = CollectionUtils.union(selection.toList(), Arrays.asList(objects));
            setSelection(new StructuredSelection(list.toArray()));
        }
   }

    public void toggleSelection(Object object) {
        if (getSelection().toList().contains(object))
            deselect(object);
        else
            select(object, false);
    }

    @SuppressWarnings({ "unchecked", "rawtypes" })
    public void deselect(Object object) {
        if (getSelection().toList().contains(object)) {
            List list = new ArrayList(getSelection().toList());
            list.remove(object);
            setSelection(new StructuredSelection(list));
        }
    }

    public void deselectAll() {
        setSelection(new StructuredSelection());
    }

    public IStructuredSelection getSelection() {
        return currentSelection;
    }

    public void setSelection(ISelection selection) {
        if (!selection.equals(currentSelection)) {
            Assert.isTrue(selection instanceof StructuredSelection);
            this.currentSelection = (StructuredSelection)selection;
            System.out.println("Selection changed to: " + currentSelection.toList());

            //XXX debug: assert on its contents
            for (Object o : currentSelection.toList())
                if (!(o instanceof IInspectorPart) && !(o instanceof SelectionItem))
                    System.out.println("**** SimulationCanvas: UNEXPECTED OBJECT IN SELECTION!!! SHOULD BE AN INSPECTOR OR AN ITEM WITHIN AN INSPECTOR ****");

            fireSelectionChange();
        }
    }

    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.add(listener);
    }

    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.remove(listener);
    }

    protected void fireSelectionChange() {
        for (IInspectorPart inspector : inspectors)
            inspector.selectionChanged(currentSelection);

        Object[] listeners = selectionChangedListeners.getListeners();
        final SelectionChangedEvent event = new SelectionChangedEvent(this, currentSelection);
        for (int i = 0; i < listeners.length; ++i) {
            final ISelectionChangedListener l = (ISelectionChangedListener) listeners[i];
            SafeRunnable.run(new SafeRunnable() {
                public void run() {
                    l.selectionChanged(event);
                }
            });
        }
    }

    @Override
    public void populateContextMenu(MenuManager menu, ISelection selection) {
        getEditor().populateContextMenu(menu, selection);
    }

    /**
     * Add callouts for the selected inspectors (and remove all other callouts)
     */
    protected void setCalloutsToSelection(ISelection selection, boolean allTheWayUp) {
        calloutSupport.removeAllCallouts();
        List<IInspectorPart> inspectors = SelectionUtils.getObjects(selection, IInspectorPart.class);
        for (IInspectorPart inspector : inspectors) {
            try {
                calloutSupport.addCalloutFor(inspector, allTheWayUp);
            }
            catch (CommunicationException e) {
                // skip it
            }
        }
    }
}

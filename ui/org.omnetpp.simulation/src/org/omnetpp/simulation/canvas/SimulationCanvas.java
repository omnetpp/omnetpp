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
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.ui.ArrayTreeContentProvider;
import org.omnetpp.common.ui.CheckedTreeSelectionDialog2;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.ISimulationStateListener;
import org.omnetpp.simulation.editors.SimulationEditor;
import org.omnetpp.simulation.figures.IInspectorFigure;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.inspectors.InfoTextInspectorPart;
import org.omnetpp.simulation.inspectors.ObjectFieldsInspectorPart;
import org.omnetpp.simulation.inspectors.QueueInspectorPart;
import org.omnetpp.simulation.model.cMessageHeap;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPar;
import org.omnetpp.simulation.model.cQueue;
import org.omnetpp.simulation.model.cSimpleModule;
import org.omnetpp.simulation.model.cWatchBase;
import org.omnetpp.simulation.views.ObjectTreeView;

/**
 *
 * @author Andras
 */
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
                Point p = toControl(Display.getCurrent().getCursorLocation());
                IInspectorPart inspectorPart = findInspectorAt(p.x, p.y);
                if (inspectorPart != null)
                    inspectorPart.populateContextMenu(menuManager, p);
            }
        });

        // add advanced tooltip support for inspectors
        FigureUtils.addTooltipSupport(this, this.getContents());

        // add move/resize support inspectors
        moveResizeSupport = new MoveResizeSupport(this);

        // add support for a floating toolbar for inspectors
        floatingToolbarSupport = new FloatingToolbarSupport(this);
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
    public Point translateAbsoluteFigureCoordinatesToCanvas(int x, int y) {
        int xoffset = getHorizontalBar().getSelection();
        int yoffset = getVerticalBar().getSelection();
        return new Point(x - xoffset, y - yoffset);
    }

    @Override
    public org.eclipse.draw2d.geometry.Point translateCanvasToAbsoluteFigureCoordinates(int x, int y) {
        int xoffset = getHorizontalBar().getSelection();
        int yoffset = getVerticalBar().getSelection();
        return new org.eclipse.draw2d.geometry.Point(x + xoffset, y + yoffset);
    }

    public void addInspectorPart(IInspectorPart inspectorPart) {
        int lastY = getInspectorsLayer().getPreferredSize().height;
        addInspectorPart(inspectorPart, 0, lastY+5);
    }

    public void addInspectorPart(final IInspectorPart inspector, int x, int y) {
        final IFigure inspectorFigure = inspector.getFigure();
        getInspectorsLayer().add(inspectorFigure);
        getInspectorsLayer().setConstraint(inspectorFigure, new Rectangle(x, y, -1, -1));

        // if inspector has SWT parts, add listeners to show floating toolbar when mouse is over them
        Control control = inspector.getSWTControl();
        if (control != null && floatingToolbarSupport != null)
            floatingToolbarSupport.addFloatingToolbarSupportTo(control, inspector);

        // register the inspector
        inspectors.add(inspector);
        inspector.refresh();
    }

    public void removeInspectorPart(IInspectorPart inspectorPart) {
        System.out.println("removeInspectorPart: " + inspectorPart);
        Assert.isTrue(inspectors.contains(inspectorPart));

        if (floatingToolbarSupport != null)
            floatingToolbarSupport.closeFloatingToolbar();
        inspectors.remove(inspectorPart);
        inspectorPart.dispose();
    }

    public void refreshInspectors() {
        for (IInspectorPart inspectorPart : inspectors.toArray(new IInspectorPart[inspectors.size()]))
            inspectorPart.refresh();
    }

    public void close(IInspectorPart inspectorPart) {
        removeInspectorPart(inspectorPart);
    }

    @Override
    public List<IInspectorPart> getInspectors() {
        return inspectors;
    }

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

        return inspector;
    }

    protected IInspectorPart createInspectorFor(cObject object) {
        //TODO more dynamic inspector type selection
        //TODO move inspector creation out of SimulationCanvas!!!
        IInspectorPart inspector = null;
        if (object instanceof cModule && !(object instanceof cSimpleModule))
            inspector = new GraphicalModuleInspectorPart(this, (cModule)object);
        else if (object instanceof cQueue)
            inspector = new QueueInspectorPart(this, object);
        else if (object instanceof cMessageHeap)  // khmm...
            inspector = new QueueInspectorPart(this, object);
        else if (object instanceof cPar || object instanceof cWatchBase)
            inspector = new InfoTextInspectorPart(this, object);
        else // fallback
            inspector = new ObjectFieldsInspectorPart(this, object);
        return inspector;
    }

    public void reveal(IInspectorPart inspector) {
        // note: not good for newly created inspectors; use asyncReveal for them
        inspector.raiseToTop();
        Rectangle bounds = inspector.getFigure().getBounds();
        scrollSmoothTo(bounds.x, bounds.y);  // scrolls so that inspector is at the top of the screen (behavior could be improved)
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
                new DecoratingStyledCellLabelProvider(new ObjectTreeView.ViewLabelProvider(), null, null), //TODO make that inner class top-level...
                new ArrayTreeContentProvider());
        dialog.setTitle("Inspect Objects");
        dialog.setMessage(message);
        dialog.setHelpAvailable(false);
        dialog.setInput(objects);
        dialog.setStatusLineAboveButtons(false);
        dialog.setInitialSelections(objects.toArray());

        if (dialog.open() == Window.OK)
            return (List<cObject>)(List)Arrays.asList(dialog.getResult());
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


}

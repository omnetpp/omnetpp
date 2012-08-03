package org.omnetpp.simulation.inspectors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.CoordinateListener;
import org.eclipse.draw2d.DelegatingLayout;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.FigureListener;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MenuDetectEvent;
import org.eclipse.swt.events.MenuDetectListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Pattern;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.ToolBar;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.ISimulationStateListener;
import org.omnetpp.simulation.inspectors.actions.CloseAction;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cQueue;
import org.omnetpp.simulation.model.cSimpleModule;

/**
 *
 * @author Andras
 */
//FIXME how to evaluate "$PARNAME" references in display strings???
public class SimulationCanvas extends FigureCanvas implements IInspectorContainer, ISelectionProvider {
    private final Image BACKGROUND_IMAGE = SimulationPlugin.getCachedImage("icons/misc/paper.png"); //XXX bg.png

    // layers
    private Figure inspectorsLayer;
    private Layer controlsLayer;

    // inspectors, selection handling
    protected List<IInspectorPart> inspectors = new ArrayList<IInspectorPart>();
    protected ISimulationStateListener simulationListener;
    protected ListenerList selectionChangedListeners = new ListenerList(); // list of selection change listeners (type ISelectionChangedListener)
    protected IStructuredSelection currentSelection = new StructuredSelection();

    // support for moving/resizing inspector by the border (or by arbitrary area of any child figure)
    MoveResizeSupport moveResizeSupport = new MoveResizeSupport(this);
    
    // floating toolbar
    private IInspectorPart floatingToolbarOwner = null;
    private Control floatingToolbar = null;
    private Point floatingToolbarDisplacement;  // relative to its normal location (= right-aligned just above the inspector)
    private boolean isFloatingToolbarStickyTimerActive = false;

    private Runnable floatingToolbarStickyTimer = new Runnable() {
        @Override
        public void run() {
            isFloatingToolbarStickyTimerActive = false;

            // floating toolbar sticky period is over: let another inspector steal the toolbar, or just remove it
            if (floatingToolbar != null && !floatingToolbar.isDisposed() && !containsMouse(floatingToolbar)) {
                Point e = Display.getCurrent().map(null, SimulationCanvas.this, Display.getCurrent().getCursorLocation());
                org.eclipse.draw2d.geometry.Point mouse = translateCanvasToAbsoluteFigureCoordinates(e.x, e.y);
                IInspectorPart inspectorUnderMouse = findInspectorAt(mouse.x, mouse.y);

                System.out.println("sticky timer expired, under mouse: " + inspectorUnderMouse);
                if (inspectorUnderMouse == null) {
                    closeFloatingToolbar();
                }
                else {
                    closeFloatingToolbar();
                    openFloatingToolbarFor(inspectorUnderMouse);
                }
            }
        }
    };

    private FigureListener floatingToolbarRelocator = new FigureListener() {
        @Override
        public void figureMoved(IFigure source) {
            relocateFloatingToolbar();
        }
    };

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

    public SimulationCanvas(Composite parent, int style) {
        super(parent, style);

        //setBackground(new Color(null, 235, 235, 235));

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

        // create context menu
        final MenuManager contextMenuManager = new MenuManager("#popup");
        setMenu(contextMenuManager.createContextMenu(this));
        addMenuDetectListener(new MenuDetectListener() {
            //@Override
            public void menuDetected(MenuDetectEvent e) {
                contextMenuManager.removeAll();
                Point p = toControl(e.x, e.y);
                IInspectorPart inspectorPart = findInspectorAt(p.x, p.y);
                if (inspectorPart != null)
                    inspectorPart.populateContextMenu(contextMenuManager, p);
            }
        });

        FigureUtils.addTooltipSupport(this, this.getContents());  //TODO make use of this!!!

        // add floating toolbar support
        addMouseMoveListener(new MouseMoveListener() {
            @Override
            public void mouseMove(MouseEvent e) {
                org.eclipse.draw2d.geometry.Point mouse = translateCanvasToAbsoluteFigureCoordinates(e.x, e.y);

                IInspectorPart inspectorUnderMouse = findInspectorAt(mouse.x, mouse.y);

                // open floating toolbar when mouse moves over any inspector
                if (floatingToolbar == null && inspectorUnderMouse != null && (e.stateMask&SWT.BUTTON_MASK)==0) {
                    openFloatingToolbarFor(inspectorUnderMouse);
                    return;
                }

                // floating toolbar is "sticky" for one second duration after you move the mouse out of its owner
                // inspector. When this one second expires, we let another inspector steal the floating toolbar
                // or just remove it. (This is similar to what e.g. Windows 7 with its desktop gadgets).
                if (floatingToolbar != null) {
                    if (inspectorUnderMouse != floatingToolbarOwner) {
                        // mouse is "away from home" (over empty area or another inspector): start sticky timer
                        startFloatingToolbarStickyTimerIfNotRunning();
                    }
                    else {
                        // mouse is back home (over the same inspector that has the floating toolbar): cancel sticky timer
                        cancelFloatingToolbarStickyTimer();
                    }
                }
            }
        });

        // need to move floating toolbar when FigureCanvas is scrolled
        getViewport().addCoordinateListener(new CoordinateListener() {
            @Override
            public void coordinateSystemChanged(IFigure source) {
                if (floatingToolbar != null)
                    relocateFloatingToolbar();
            }
        });
    }


    public void addMoveResizeSupport(IFigure figure) {
        moveResizeSupport.adopt(figure);
    }

    public void removeMoveResizeSupport(IFigure figure) {
        moveResizeSupport.forget(figure);
    }

    public void addMoveResizeSupport(Control control) {
        moveResizeSupport.adopt(control);
    }

    public void removeMoveResizeSupport(Control control) {
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

    @Override
    public void dispose() {
        for (IInspectorPart inspectorPart : inspectors.toArray(new IInspectorPart[inspectors.size()]))
            removeInspectorPart(inspectorPart);
        super.dispose();
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
        if (control != null)
            addFloatingToolbarSupportTo(control, inspector);

        // register the inspector
        inspectors.add(inspector);
        inspector.refresh();
    }

    protected void addFloatingToolbarSupportTo(Control control, final IInspectorPart inspector) {
        MouseTrackAdapter listener = new MouseTrackAdapter() {
            @Override
            public void mouseEnter(MouseEvent e) {
                //XXX this is more or less a copypasta of the listener code in the ctor!
                IInspectorPart inspectorUnderMouse = inspector; // for clarity

                // open floating toolbar when mouse moves over any inspector
                if (floatingToolbar == null && (e.stateMask&SWT.BUTTON_MASK)==0) {
                    openFloatingToolbarFor(inspectorUnderMouse);
                }

                // floating toolbar is "sticky" for one second duration after you move the mouse out of its owner
                // inspector. When this one second expires, we let another inspector steal the floating toolbar
                // or just remove it. (This is similar to what e.g. Windows 7 with its desktop gadgets).
                if (floatingToolbar != null) {
                    if (inspectorUnderMouse != floatingToolbarOwner) {
                        startFloatingToolbarStickyTimerIfNotRunning();
                    }
                    else {
                        cancelFloatingToolbarStickyTimer();
                    }
                }
                
            }
        };

        addMouseTrackListenerRec(control, listener);
    }

    protected static void addMouseTrackListenerRec(Control control, MouseTrackListener listener) {
        control.addMouseTrackListener(listener);
        if (control instanceof Composite)
            for (Control child : ((Composite) control).getChildren())
                addMouseTrackListenerRec(child, listener);
    }

    protected void startFloatingToolbarStickyTimerIfNotRunning() {
        if (!isFloatingToolbarStickyTimerActive) {
            Display.getCurrent().timerExec(1000, floatingToolbarStickyTimer);
            isFloatingToolbarStickyTimerActive = true;
        }
    }

    protected void cancelFloatingToolbarStickyTimer() {
        if (isFloatingToolbarStickyTimerActive) {
            Display.getCurrent().timerExec(-1, floatingToolbarStickyTimer);
            isFloatingToolbarStickyTimerActive = false;
        }
    }

    public void removeInspectorPart(IInspectorPart inspectorPart) {
        System.out.println("removeInspectorPart: " + inspectorPart);
        Assert.isTrue(inspectors.contains(inspectorPart));

        if (floatingToolbar != null && floatingToolbarOwner == inspectorPart)
            closeFloatingToolbar();
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

    public IInspectorPart inspect(cObject object) {
        Assert.isNotNull(object);

        IInspectorPart inspector = findInspectorFor(object);
        if (inspector != null) {
            reveal(inspector);
        }
        else {
            inspector = createInspectorFor(object);
            addInspectorPart(inspector);

            // Note: the following layout() call doesn't work to reveal the inspector (looks like
            // it doesn't cause the scrollbar or getContents().getBounds() to be updated); as a
            // workaround, we call reveal() in an asyncExec().
            //getContents().getLayoutManager().layout(getContents());
            //reveal(finalInspector);

            final IInspectorPart finalInspector = inspector;
            Display.getDefault().asyncExec(new Runnable() {
                @Override
                public void run() {
                    reveal(finalInspector);
                }
            });
        }
        return inspector;
    }

    protected IInspectorPart createInspectorFor(cObject object) {
        //TODO more dynamic inspector type selection
        //TODO move inspector creation out of SimulationCanvas!!!
        IInspectorPart inspector = null;
        if (object instanceof cModule && !(object instanceof cSimpleModule))
            inspector = new GraphicalModuleInspectorPart(this, (cModule)object);
//        else if (object instanceof cMessage)
//            inspectorPart = new MessageInspectorPart(this, (cMessage)object);
        else if (object instanceof cQueue)
            inspector = new QueueInspectorPart(this, (cQueue)object);
        else // fallback
            inspector = new ObjectFieldsInspectorPart(this, object);
//            inspector = new InfoTextInspectorPart(this, object);
        return inspector;
    }

    public void reveal(IInspectorPart inspector) {
        Rectangle bounds = inspector.getFigure().getBounds();
        scrollSmoothTo(bounds.x, bounds.y);  // scrolls so that inspector is at the top of the screen (behavior could be improved)
    }

    @Override
    public cObject[] getObjectsFromSelection(ISelection selection) {
        if (selection.isEmpty() || !(selection instanceof IStructuredSelection))
            return new cObject[0];

        List<cObject> result = new ArrayList<cObject>();
        for (Object element : ((IStructuredSelection)selection).toList()) {
            if (element instanceof cObject)
                result.add((cObject)element);
            else if (element instanceof IAdaptable) {
                cObject object = (cObject) ((IAdaptable)element).getAdapter(cObject.class);
                if (object != null)
                    result.add(object);
            }
        }
        return result.toArray(new cObject[0]);
    }

    @SuppressWarnings({ "rawtypes", "unchecked" })
    public void select(cObject object, boolean removeOthers) {
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

    public void toggleSelection(cObject object) {
        if (getSelection().toList().contains(object))
            deselect(object);
        else
            select(object, false);
    }

    @SuppressWarnings({ "unchecked", "rawtypes" })
    public void deselect(cObject object) {
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

    protected void openFloatingToolbarFor(IInspectorPart inspector) {
        Assert.isTrue(floatingToolbar == null);

        Composite panel = new Composite(this, SWT.BORDER);
        panel.setLayout(new FillLayout());

        // add icons to the toolbar
        ToolBar toolbar = new ToolBar(panel, SWT.HORIZONTAL);
        ToolBarManager manager = new ToolBarManager(toolbar);

        inspector.populateFloatingToolbar(manager);

        manager.add(new DragHandle(inspector));

        CloseAction closeAction = new CloseAction();
        closeAction.setInspectorPart(inspector);
        manager.add(closeAction);
        manager.update(true);

        panel.setCursor(new Cursor(Display.getCurrent(), SWT.CURSOR_ARROW));

        // set the size of the toolbar
        panel.layout();
        panel.setSize(panel.computeSize(SWT.DEFAULT, SWT.DEFAULT));

        // raise to the top
        panel.moveAbove(null);

        // we may need to displace it by some vector compared to its normal location,
        // to ensure it is fully visible (i.e. appears within canvas bounds)
        Point naturalLoc = computeFloatingToolbarNaturalLocation(inspector.getFigure(), panel);
        Point adjustedLoc = new Point(naturalLoc.x, naturalLoc.y);
        if (adjustedLoc.x + panel.getSize().x > getClientArea().width)
            adjustedLoc.x = getClientArea().width - panel.getSize().x;
        if (adjustedLoc.x < 0)
            adjustedLoc.x = 0;
        if (adjustedLoc.y + panel.getSize().y > getClientArea().height)
            adjustedLoc.y = getClientArea().height - panel.getSize().y;
        if (adjustedLoc.y < 0)
            adjustedLoc.y = 0;
        floatingToolbarDisplacement = new Point(adjustedLoc.x - naturalLoc.x, adjustedLoc.y - naturalLoc.y);

        // created!
        floatingToolbar = panel;
        floatingToolbarOwner = inspector;

        // move it to the correct location
        relocateFloatingToolbar();

        // when figure is moved/resized, make the toolbar follow it 
        inspector.getFigure().addFigureListener(floatingToolbarRelocator);

        // This listener solves the following problem: Zoom buttons on module inspectors' 
        // floating toolbar could not be clicked repeatedly, because toolbar jumped away
        // after the first click (due to the inspector being resized). Workaround is to
        // decouple the toolbar from the inspector while the mouse is over the toolbar.
        // Luckily, according to the listener the DragHandle is NOT part of the toolbar, 
        // so dragging by the handle DOES move the toolbar too, as it should.
        toolbar.addMouseTrackListener(new MouseTrackAdapter() {
            @Override
            public void mouseEnter(MouseEvent e) {
                detachFloatingToolbarFromInspector();
            }
            @Override
            public void mouseExit(MouseEvent e) {
                reattachFloatingToolbarToInspector();
            }
        });
    }

    protected void reattachFloatingToolbarToInspector() {
        Assert.isTrue(floatingToolbar != null);
        
        // recompute displacement
        Point naturalLoc = computeFloatingToolbarNaturalLocation(floatingToolbarOwner.getFigure(), floatingToolbar);
        Point actualLoc = floatingToolbar.getLocation();
        floatingToolbarDisplacement = new Point(actualLoc.x - naturalLoc.x, actualLoc.y - naturalLoc.y);
        
        floatingToolbarOwner.getFigure().addFigureListener(floatingToolbarRelocator);
    }

    protected void detachFloatingToolbarFromInspector() {
        Assert.isTrue(floatingToolbar != null);
        floatingToolbarOwner.getFigure().removeFigureListener(floatingToolbarRelocator);
    }

    protected void relocateFloatingToolbar() {
        Point naturalLoc = computeFloatingToolbarNaturalLocation(floatingToolbarOwner.getFigure(), floatingToolbar);
        floatingToolbar.setLocation(naturalLoc.x + floatingToolbarDisplacement.x, naturalLoc.y + floatingToolbarDisplacement.y);
    }

    protected Point computeFloatingToolbarNaturalLocation(IInspectorFigure inspectorFigure, Control floatingToolbar) {
        Rectangle figureBounds = inspectorFigure.getBounds().getCopy();
        inspectorFigure.translateToAbsolute(figureBounds);
        Point toolbarSize = floatingToolbar.getSize();
        return new Point(figureBounds.right() - toolbarSize.x, figureBounds.y - toolbarSize.y - 3);
    }

    protected static boolean containsMouse(Control control) {
        Point mouse = Display.getCurrent().getCursorLocation();
        return Display.getCurrent().map(control.getParent(), null, control.getBounds()).contains(mouse);
    }

    public void closeFloatingToolbar() {
        if (floatingToolbar != null) {
            floatingToolbar.dispose();
            floatingToolbar = null;
            floatingToolbarOwner.getFigure().removeFigureListener(floatingToolbarRelocator);
            floatingToolbarOwner = null;
        }
    }

    /**
     * Returns null if not displayed.
     */
    public Control getFloatingToolbar() {
        return floatingToolbar;
    }

    public IInspectorPart getFloatingToolbarOwner() {
        return floatingToolbarOwner;
    }


}

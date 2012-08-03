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
//XXX snap to grid for the move/resize?
//FIXME how to evaluate "$PARNAME" references in display strings???
//NOTE: see ModelCanvas in the old topic/guienv2 branch for scrollable version (using ScrolledComposite)
public class SimulationCanvas extends FigureCanvas implements IInspectorContainer, ISelectionProvider {
    private final Image BACKGROUND_IMAGE = SimulationPlugin.getCachedImage("icons/misc/paper.png"); //XXX bg.png

    private Figure inspectorsLayer;
    private Layer controlsLayer;

    protected List<IInspectorPart> inspectors = new ArrayList<IInspectorPart>();
    protected ISimulationStateListener simulationListener;
    protected ListenerList selectionChangedListeners = new ListenerList(); // list of selection change listeners (type ISelectionChangedListener)
    protected IStructuredSelection currentSelection = new StructuredSelection();

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
                IInspectorPart inspectorPart = findInspectorPartAt(p.x, p.y);
                if (inspectorPart != null)
                    inspectorPart.populateContextMenu(contextMenuManager, p);
            }
        });

        FigureUtils.addTooltipSupport(this, this.getContents());  //TODO make use of this!!!
   }

    protected IInspectorPart findInspectorPartAt(int x, int y) {
        IFigure target = getInspectorsLayer().findFigureAt(x, y);
        while (target != null && !(target instanceof IInspectorFigure))
            target = target.getParent();
        if (target == null)
            return null;
        for (IInspectorPart inspector : getInspectors())
            if (inspector.getFigure() == target)
                return inspector;
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

    public void addInspectorPart(IInspectorPart inspectorPart, final int x, final int y) {
        final IFigure inspectorFigure = inspectorPart.getFigure();
        getInspectorsLayer().add(inspectorFigure);
        getInspectorsLayer().setConstraint(inspectorFigure, new Rectangle(x, y, -1, -1));

        // register the inspector
        inspectors.add(inspectorPart);
        addFloatingControlsSupport(inspectorPart);  //TODO also to its SWT parts!!!
        inspectorPart.refresh();
    }

    public void removeInspectorPart(IInspectorPart inspectorPart) {
        System.out.println("removeInspectorPart: " + inspectorPart);
        Assert.isTrue(inspectors.contains(inspectorPart));
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

    private IInspectorPart floatingControlsOwner = null;
    private Control floatingControls = null;
    private Point floatingControlsDisplacement;  // relative to its normal location (= right-aligned just above the inspector)
    private boolean isRemoveFloatingControlsTimerActive = false;
    
    private Runnable removeFloatingControlsTimer = new Runnable() {
        @Override
        public void run() {
            System.out.println("removeFloatingControlsTimer expired");
            isRemoveFloatingControlsTimerActive = false;
            if (floatingControls != null && !floatingControls.isDisposed() && !containsMouse(floatingControls))
                closeFloatingControls();
        }
    };

    protected static boolean containsMouse(Control control) {
        Point mouse = Display.getCurrent().getCursorLocation();
        return Display.getCurrent().map(control.getParent(), null, control.getBounds()).contains(mouse);
    }
    
    protected void addFloatingControlsSupportTo(Control control, final IInspectorPart inspector) {
        MouseTrackAdapter listener = new MouseTrackAdapter() { //TODO also: cancel 1s timer!
            @Override
            public void mouseEnter(MouseEvent e) {
                if (floatingControls != null && floatingControlsOwner.isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                if (floatingControls == null) {
                    openFloatingControls(inspector);
                }
                if (floatingControls != null) {
                    if (isRemoveFloatingControlsTimerActive) {
                        Display.getCurrent().timerExec(-1, removeFloatingControlsTimer); // cancel timer
                        isRemoveFloatingControlsTimerActive = false;
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

    protected void addFloatingControlsSupport(final IInspectorPart inspector) {
        final IInspectorFigure figure = inspector.getFigure();
        
        addMouseMoveListener(new MouseMoveListener() {
            @Override
            public void mouseMove(MouseEvent e) {
                if (floatingControls != null && floatingControlsOwner.isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                
                // Removing floating controls. Windows 7 does the following with its desktop gadgets:
                // when user moves at least 10 pixels away from the inspector's bounding box,
                // plus one second elapses without moving back within inspector bounds.
                // We do the same, except ignore the 10 pixels distance.
                org.eclipse.draw2d.geometry.Point mouse = translateCanvasToAbsoluteFigureCoordinates(e.x, e.y);
                
                if (floatingControls == null && figure.containsPoint(mouse) && (e.stateMask&SWT.BUTTON_MASK)==0) {
                    //TODO if others have floating controls open, we should only open ours (and close others') with a delay -- otherwise it's a pain to work with overlapping inspectors!!!
                    openFloatingControls(inspector);
                }
                if (floatingControls != null) {
                    //NOTE: ONCE THE MOUSE ENTERS FLOATINGCONTROLS, WE STOP RECEIVING NOTIFICATIONS SO CANNOT CANCEL THE TIMER!
                    if (figure.containsPoint(mouse)) {
                        if (isRemoveFloatingControlsTimerActive) {
                            Display.getCurrent().timerExec(-1, removeFloatingControlsTimer); // cancel timer
                            isRemoveFloatingControlsTimerActive = false;
                        }
                    }
                    else {
                        if (!isRemoveFloatingControlsTimerActive) {
                            Display.getCurrent().timerExec(1000, removeFloatingControlsTimer); // start timer
                            isRemoveFloatingControlsTimerActive = true;
                        }
                    }
                }
            }
        });

        // need to adjust control's bounds when figure is moved or resized
        figure.addFigureListener(new FigureListener() {
            @Override
            public void figureMoved(IFigure source) {
                if (floatingControls != null && floatingControlsOwner.isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                if (floatingControls != null)
                    relocateFloatingControls();
            }
        });

        // also also when FigureCanvas is scrolled
        getViewport().addCoordinateListener(new CoordinateListener() {
            @Override
            public void coordinateSystemChanged(IFigure source) {
                if (floatingControls != null && floatingControlsOwner.isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                if (floatingControls != null)
                    relocateFloatingControls();
            }
        });

    }

    protected void openFloatingControls(IInspectorPart inspector) {
        Assert.isTrue(floatingControls == null);

        // close floating controls of all other inspectors
        if (floatingControls != null)
            closeFloatingControls();

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
        Point naturalLoc = computeFloatingControlsNaturalLocation(inspector.getFigure(), panel);
        Point adjustedLoc = new Point(naturalLoc.x, naturalLoc.y);
        if (adjustedLoc.x + panel.getSize().x > getSize().x)
            adjustedLoc.x = getSize().x - panel.getSize().x;
        if (adjustedLoc.x < 0)
            adjustedLoc.x = 0;
        if (adjustedLoc.y + panel.getSize().y > getSize().y)
            adjustedLoc.y = getSize().y - panel.getSize().y;
        if (adjustedLoc.y < 0)
            adjustedLoc.y = 0;
        floatingControlsDisplacement = new Point(adjustedLoc.x - naturalLoc.x, adjustedLoc.y - naturalLoc.y);

        // created!
        floatingControls = panel;
        floatingControlsOwner = inspector;
        
        // move it to the correct location
        relocateFloatingControls();
    }

    protected void relocateFloatingControls() {
        Point naturalLoc = computeFloatingControlsNaturalLocation(floatingControlsOwner.getFigure(), floatingControls);
        floatingControls.setLocation(naturalLoc.x + floatingControlsDisplacement.x, naturalLoc.y + floatingControlsDisplacement.y);
    }

    protected Point computeFloatingControlsNaturalLocation(IInspectorFigure inspectorFigure, Control floatingControls) {
        Rectangle targetBounds = inspectorFigure.getBounds().getCopy();
        inspectorFigure.translateToAbsolute(targetBounds);
        Point targetTopRightCorner = new Point(targetBounds.right(), targetBounds.y);
        Point controlsSize = floatingControls.getSize();
        return new Point(targetTopRightCorner.x - controlsSize.x, targetTopRightCorner.y - controlsSize.y - 3); // canvas coordinates
    }

    /**
     * Returns null if not displayed.
     */
    public Control getFloatingControls() {
        return floatingControls;
    }

    public IInspectorPart getFloatingControlsOwner() {
        return floatingControlsOwner;
    }
    
    public void closeFloatingControls() {
        floatingControls.dispose();
        floatingControls = null;
        floatingControlsOwner = null;
    }

}

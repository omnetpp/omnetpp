package org.omnetpp.simulation.inspectors;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.draw2d.CoordinateListener;
import org.eclipse.draw2d.FigureListener;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.action.ControlContribution;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ToolBar;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.inspectors.actions.CloseAction;
import org.omnetpp.simulation.inspectors.actions.IInspectorAction;
import org.omnetpp.simulation.model.cObject;

/**
 * Default implementation for IInspectorPart, base class for inspector classes
 */
//TODO normal resize for SWT inspectors, module inspectors, etc
public abstract class AbstractInspectorPart implements IInspectorPart, IAdaptable {
    protected cObject object;
    protected IInspectorFigure figure;
    protected IInspectorContainer inspectorContainer;
    protected boolean isSelected;

    public AbstractInspectorPart(IInspectorContainer parent, cObject object) {
        this.object = object;
        this.inspectorContainer = parent;

        figure = createFigure();
        addMoveResizeSupport();
        addFloatingControlsSupport();
    }

    protected abstract IInspectorFigure createFigure();

    protected void addMoveResizeSupport() {
        // add move/resize/selection support
        new InspectorMouseListener(this); //XXX revise this listener!
    }

    @Override
    @SuppressWarnings("rawtypes")
    public Object getAdapter(Class adapter) {
        // being able to adapt to cObject helps working with the selection
        if (adapter.isInstance(object))
            return object;
        if (adapter.isInstance(this))
            return this;
        return null;
    }
    
//XXX this is the Figure-based implementation of floating controls. Drawback: it can never appear over SWT widgets like the object fields tree
//    class FloatingControlsLocator implements Locator {
//        public void relocate(IFigure target) {
//            // see RelativeLocator.relocate()
//            IFigure reference = AbstractInspectorPart.this.figure;
//            Rectangle targetBounds = new Rectangle(reference.getBounds());
//            reference.translateToAbsolute(targetBounds);
//            target.translateToRelative(targetBounds);
//
//            Dimension targetSize = target.getPreferredSize();
//
//            targetBounds.x += targetBounds.width - targetSize.width;
//            targetBounds.y -= targetSize.height;
//            targetBounds.setSize(targetSize);
//            target.setBounds(targetBounds);
//        }
//    }
//
////    private long controlsDisplayedTime;
//    protected void addFloatingControlsSupport() {
//        //XXX experimental; TODO fade-in/fade-out effect! (layer to be turned into AlphaLayer)
//        final IFigure floatingControls = createFloatingControls();
//        figure.addMouseMotionListener(new MouseMotionListener.Stub() {
//            @Override
//            public void mouseHover(MouseEvent me) {
//                getContainer().getControlsLayer().add(floatingControls);
//                getContainer().getControlsLayer().getLayoutManager().setConstraint(floatingControls, new FloatingControlsLocator());
////                controlsDisplayedTime = System.currentTimeMillis();
//            }
//
//            @Override
//            public void mouseMoved(MouseEvent me) {
//                if (floatingControls.getParent() != null) {
////                    long delay = System.currentTimeMillis() - controlsDisplayedTime;
////                    if (delay >= 1000) // leave it displayed at least for a few seconds
//                        getContainer().getControlsLayer().remove(floatingControls);
//                }
//            }
//        });
//
//        // force the locator to be invoked whenever the inspector figure moves; without this, the floating
//        // controls follow the inspector figure with a quite noticeable lag (apparently the layouter is only
//        // invoked when the mouse rests)
//        figure.addFigureListener(new FigureListener() {
//            @Override
//            public void figureMoved(IFigure source) {
//                getContainer().getControlsLayer().invalidate();
//            }
//        });
//    }
//
//    class ToolButton extends Button {
//        public ToolButton(Image image) {
//            super(image);
//            setRolloverEnabled(true);
//        }
//    }
//    class ToolbarSpace extends Figure {
//        public ToolbarSpace() {
//            setPreferredSize(8, 8);
//        }
//    }
//
//    protected IFigure createFloatingControls() {
//        IFigure toolbar = new Figure();
//        toolbar.setLayoutManager(new ToolbarLayout(true));
//        toolbar.setBorder(new LineBorder(ColorFactory.GREY50, 1));
//
//        //XXX dummy buttons
//        toolbar.add(new ToolButton(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_ETOOL_HOME_NAV)));
//        toolbar.add(new ToolButton(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_ETOOL_SAVE_EDIT)));
//        toolbar.add(new ToolButton(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_ETOOL_PRINT_EDIT)));
//        toolbar.add(new ToolbarSpace());
//        ToolButton closeButton = new ToolButton(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_TOOL_DELETE));
//        toolbar.add(closeButton);
//
//        closeButton.addActionListener(new ActionListener() {
//            @Override
//            public void actionPerformed(ActionEvent event) {
//                getContainer().close(AbstractInspectorPart.this);
//            }
//        });
//        return toolbar;
//    }

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

    //TODO: only 1 floating controls in the canvas
    protected void addFloatingControlsSupportTo(Control control) {
        MouseTrackAdapter listener = new MouseTrackAdapter() { //TODO also: cancel 1s timer!
            @Override
            public void mouseEnter(MouseEvent e) {
                if (isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                if (floatingControls == null) {
                    openFloatingControls();
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

    protected void addFloatingControlsSupport() {
        Composite canvas = getContainer().getControl();
        canvas.addMouseMoveListener(new MouseMoveListener() {
            @Override
            public void mouseMove(MouseEvent e) {
                if (isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                // Removing floating controls. Windows 7 does the following with its desktop gadgets:
                // when user moves at least 10 pixels away from the inspector's bounding box,
                // plus one second elapses without moving back within inspector bounds.
                // We do the same, except ignore the 10 pixels distance.
                org.eclipse.draw2d.geometry.Point mouse = getContainer().translateCanvasToAbsoluteFigureCoordinates(e.x, e.y);
                
                if (floatingControls == null && figure.containsPoint(mouse) && (e.stateMask&SWT.BUTTON_MASK)==0) {
                    //TODO if others have floating controls open, we should only open ours (and close others') with a delay -- otherwise it's a pain to work with overlapping inspectors!!!
                    openFloatingControls();
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
                if (isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                if (floatingControls != null)
                    relocateFloatingControls();
            }
        });

        // also also when FigureCanvas is scrolled
        getContainer().getControl().getViewport().addCoordinateListener(new CoordinateListener() {
            @Override
            public void coordinateSystemChanged(IFigure source) {
                if (isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                if (floatingControls != null)
                    relocateFloatingControls();
            }
        });

    }

    private boolean dragInProgress = false;
    private int moveOffsetX, moveOffsetY;
    private static Cursor dragCursor = null;

    protected IAction my(IInspectorAction action) {
        action.setInspectorPart(this);
        return action;
    }
    
    protected void openFloatingControls() {
        Assert.isTrue(floatingControls == null);

        // close floating controls of all other inspectors
        for (IInspectorPart inspector : getContainer().getInspectors())
            if (inspector.getFloatingControls() != null)
                inspector.closeFloatingControls();

        Composite panel = new Composite(getContainer().getControl(), SWT.BORDER);
        panel.setLayout(new FillLayout());

        // add icons to the toolbar
        ToolBar toolbar = new ToolBar(panel, SWT.HORIZONTAL);
        ToolBarManager manager = new ToolBarManager(toolbar);

        addIconsToFloatingToolbar(manager);
        
        final Label[] dragHandleRef = new Label[1];
        manager.add(new ControlContribution("") {
            @Override
            protected Control createControl(Composite parent) {
                Label dragHandle = new Label(parent, SWT.NONE);
                dragHandle.setImage(SimulationPlugin.getCachedImage("icons/etool16/draghandle.png"));
                dragHandle.setToolTipText("Drag handle");
                dragHandleRef[0] = dragHandle;
                return dragHandle;
            }
        });

        manager.add(my(new CloseAction()));
        manager.update(true);
        final Label dragHandle = dragHandleRef[0];
        
        panel.setCursor(new Cursor(Display.getCurrent(), SWT.CURSOR_ARROW));

        // set the size of the toolbar
        panel.layout();
        panel.setSize(panel.computeSize(SWT.DEFAULT, SWT.DEFAULT));

        // raise to the top
        panel.moveAbove(null);

        // created!
        floatingControls = panel;

        // we may need to displace it by some vector compared to its normal location,
        // to ensure it is fully visible (i.e. appears within canvas bounds)
        Point naturalLoc = computeFloatingControlsNaturalLocation();
        Point adjustedLoc = new Point(naturalLoc.x, naturalLoc.y);
        if (adjustedLoc.x + panel.getSize().x > getContainer().getControl().getBounds().width)
            adjustedLoc.x = getContainer().getControl().getBounds().width - panel.getSize().x;
        if (adjustedLoc.x < 0)
            adjustedLoc.x = 0;
        if (adjustedLoc.y + panel.getSize().y > getContainer().getControl().getBounds().width)
            adjustedLoc.y = getContainer().getControl().getBounds().height - panel.getSize().y;
        if (adjustedLoc.y < 0)
            adjustedLoc.y = 0;
        floatingControlsDisplacement = new Point(adjustedLoc.x - naturalLoc.x, adjustedLoc.y - naturalLoc.y);

        // move it to the correct location
        relocateFloatingControls();

        if (dragCursor == null)
            dragCursor = new Cursor(Display.getCurrent(), SWT.CURSOR_HAND);

        // allow panel to be moved around with the mouse
        dragHandle.addMouseListener(new MouseAdapter() {
            public void mouseDown(MouseEvent e) {
                dragInProgress = true;
                Point p = Display.getCurrent().getCursorLocation();
                org.eclipse.draw2d.geometry.Point pp = getContainer().translateCanvasToAbsoluteFigureCoordinates(p.x, p.y); // well, p is screen coord not canvas, but here it only affects moveOffset
                Rectangle figureBounds = figure.getBounds();
                moveOffsetX = figureBounds.x - pp.x;
                moveOffsetY = figureBounds.y - pp.y;
                dragHandle.setCursor(dragCursor);
                raiseToTop();
                
                // By default draw2d updates canvas bounds continually while dragging, which
                // results in a weird effect: when dragging up an inspector from near the bottom 
                // of the canvas and the canvas shrinks as a result, the mouse overtakes the 
                // drag handle and other interesting things happen. The workaround is to disable
                // updating the canvas bounds while dragging; this can be achieved by setting
                // preferredSize.
                IFigure contents = getContainer().getControl().getContents();
                contents.setPreferredSize(contents.getPreferredSize());
            }
            public void mouseUp(MouseEvent e) {
                dragInProgress = false;
                dragHandle.setCursor(null);

                // restore no preferredSize
                IFigure contents = getContainer().getControl().getContents();
                contents.setPreferredSize(null);
            }
        });
        dragHandle.addMouseMoveListener(new MouseMoveListener() {
            public void mouseMove(MouseEvent e) {
                if (dragInProgress && (e.stateMask&SWT.BUTTON_MASK) != SWT.BUTTON1)
                    dragInProgress = false;
                if (dragInProgress) {
                    Point p = Display.getCurrent().getCursorLocation();
                    org.eclipse.draw2d.geometry.Point pp = getContainer().translateCanvasToAbsoluteFigureCoordinates(p.x, p.y); // well, p is screen coord not canvas, but here it only affects moveOffset  
                    Rectangle r = figure.getBounds().getCopy();
                    r.setLocation(pp.x + moveOffsetX, pp.y + moveOffsetY);
                    
                    // don't allow inspectors to stick out on the left or top, because it can be difficult or impossible to bring them back
                    if (r.x < 0) r.x = 0;
                    if (r.y < 0) r.y = 0;
                    figure.getParent().setConstraint(figure, r);
                }
            }
        });
    }

    // expected to be overridden in subclasses
    protected void addIconsToFloatingToolbar(ToolBarManager manager) {
    }

    protected void relocateFloatingControls() {
        Point naturalLoc = computeFloatingControlsNaturalLocation();
        floatingControls.setLocation(naturalLoc.x + floatingControlsDisplacement.x, naturalLoc.y + floatingControlsDisplacement.y);
    }

    protected Point computeFloatingControlsNaturalLocation() {
        IFigure reference = AbstractInspectorPart.this.figure;
        Rectangle targetBounds = reference.getBounds().getCopy();
        reference.translateToAbsolute(targetBounds);
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

    public void closeFloatingControls() {
        floatingControls.dispose();
        floatingControls = null;
    }

    @SuppressWarnings({ "rawtypes", "unchecked" })
    public void raiseToTop() {
        // there's no public API in draw2d for changing the order (Z-order) of children, but
        // Randy Hudson himself suggests just to change the list returned by getChildren()...
        // http://dev.eclipse.org/mhonarc/lists/gef-dev/msg00914.html
        List siblings = figure.getParent().getChildren();
        siblings.remove(figure);
        siblings.add(figure);
        figure.getParent().invalidate();
    }

    public void dispose() {
        System.out.println("disposing inspector: " + object);

        if (floatingControls != null)
            closeFloatingControls();

        if (figure != null) {
            if (figure.getParent() != null)
                figure.getParent().remove(figure);
            figure = null;
        }

        object = null;
    }

    public boolean isDisposed() {
        return object == null;
    }

    @Override
    public cObject getObject() {
        return object;
    }

    @Override
    public IInspectorFigure getFigure() {
        return figure;
    }

    @Override
    public boolean isSelected() {  //XXX needed? canvas holds the selection anyway, this only allows inconsistency!!!
        return isSelected;
    }

    @Override
    public void refresh() {
        Assert.isTrue(figure.getParent()!=null && inspectorContainer!=null, "inspector not yet installed");
        Assert.isTrue(object != null, "inspector already disposed");

        // automatically close the inspector when the underlying object gets deleted
        if (object.isDisposed()) {
            System.out.println("object disposed - auto-closing inspector: ");
            getContainer().close(this);
        }
    }

    @Override
    public IInspectorContainer getContainer() {
        return inspectorContainer;
    }

    @Override
    public void selectionChanged(IStructuredSelection selection) {
        boolean oldSelectedState = isSelected;
        isSelected = selection.toList().contains(object); //XXX or the inspectorPart???
        if (oldSelectedState != isSelected)
            figure.setSelectionBorder(isSelected);
    }

    @Override
    public String toString() {
        if (object.isDisposed())
            return getClass().getSimpleName() + ":<disposed>";
        else
            return getClass().getSimpleName() + ":(" + object.getClassName() + ")" + object.getFullPath();
    }

    protected static boolean containsMouse(Control control) {
        Point mouse = Display.getCurrent().getCursorLocation();
        return Display.getCurrent().map(control.getParent(), null, control.getBounds()).contains(mouse);
    }

}
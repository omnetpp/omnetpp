package org.omnetpp.simulation.inspectors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.CoordinateListener;
import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.FigureListener;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.model.cObject;

/**
 * Default implementation for IInspectorPart, base class for inspector classes
 */
//TODO normal resize for SWT inspectors, module inspectors, etc
//TODO floating controls misplaced if canvas is scrolled
public abstract class AbstractInspectorPart implements IInspectorPart {
	protected cObject object;
	protected IInspectorFigure figure;
	protected IInspectorContainer inspectorContainer;
	protected boolean isSelected;

	public AbstractInspectorPart(IInspectorContainer parent, cObject object) {
		this.object = object;
		this.inspectorContainer = parent;
		
		figure = createFigure();
		figure.setInspectorPart(this);
		addMoveResizeSupport();
		addFloatingControlsSupport();
	}

    protected abstract IInspectorFigure createFigure();

    protected void addMoveResizeSupport() {
        // add move/resize/selection support
        new InspectorMouseListener(this); //XXX revise this listener! 
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
    private boolean isRemoveFloatingControlsTimerActive = false;
    private Runnable removeFloatingControlsTimer = new Runnable() {
        @Override
        public void run() {
            System.out.println("removeFloatingControlsTimer expired");
            isRemoveFloatingControlsTimerActive = false;
            if (floatingControls != null && !floatingControls.isDisposed() && !containsMouse(floatingControls)) {
                floatingControls.dispose();
                floatingControls = null;
            }
        }
    };

    //TODO: only 1 floating controls in the canvas
    protected void addFloatingControlsSupportTo(Control control) {
        MouseTrackAdapter listener = new MouseTrackAdapter() { //TODO also: cancel 1s timer!
            @Override
            public void mouseEnter(MouseEvent e) {
                if (isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                if (floatingControls == null) {
                    floatingControls = createFloatingControls();
                    relocateFloatingControls();
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
                if (floatingControls == null && figure.containsPoint(e.x, e.y)) {
                    floatingControls = createFloatingControls();
                    relocateFloatingControls();
                }
                if (floatingControls != null) {
                    //NOTE: ONCE THE MOUSE ENTERS FLOATINGCONTROLS, WE STOP RECEIVING NOTIFICATIONS SO CANNOT CANCEL THE TIMER!
                    if (figure.containsPoint(e.x, e.y)) {
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

    private boolean mouse1Down = false;
    private int moveOffsetX, moveOffsetY;
    private static Cursor dragCursor = null;
    
    protected Control createFloatingControls() {
        Composite panel = new Composite(getContainer().getControl(), SWT.BORDER);
        panel.setLayout(new FillLayout());
        
        ToolBar toolbar = new ToolBar(panel, SWT.VERTICAL);

        // add icons to the toolbar
        ToolItem closeButton = new ToolItem(toolbar, SWT.PUSH);
        closeButton.setImage(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_TOOL_DELETE));
        closeButton.setToolTipText("Close");

        final Label dragHandle = new Label(toolbar, SWT.NONE);
        dragHandle.setAlignment(SWT.CENTER);
        dragHandle.setImage(SimulationPlugin.getCachedImage("icons/etool16/draghandle.png"));
        dragHandle.setToolTipText("Drag handle");
        ToolItem dragHandleWrapperItem = new ToolItem(toolbar, SWT.SEPARATOR);
        dragHandleWrapperItem.setControl(dragHandle);

        new ToolItem(toolbar, SWT.SEPARATOR);
        
        ToolItem startButton = new ToolItem(toolbar, SWT.PUSH);
        startButton.setImage(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_TOOL_COPY));
        startButton.setToolTipText("Start Foo");
        
        ToolItem stopButton = new ToolItem(toolbar, SWT.PUSH);
        stopButton.setImage(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_TOOL_PASTE));
        stopButton.setToolTipText("Stop Foo");

        // set the size of the toolbar
        panel.layout();
        panel.setSize(panel.computeSize(SWT.DEFAULT, SWT.DEFAULT));
        
        // add action to buttons
        closeButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                getContainer().close(AbstractInspectorPart.this);
            }
        });

        if (dragCursor == null) 
            dragCursor = new Cursor(Display.getCurrent(), SWT.CURSOR_HAND);

        // allow panel to be moved around with the mouse
        dragHandle.addMouseListener(new MouseAdapter() {
            public void mouseDown(MouseEvent e) {
                mouse1Down = true;
                Point p = Display.getCurrent().getCursorLocation();
                Rectangle figureBounds = figure.getBounds();
                moveOffsetX = figureBounds.x - p.x;
                moveOffsetY = figureBounds.y - p.y;
                dragHandle.setCursor(dragCursor);
            }
            public void mouseUp(MouseEvent e) {
                mouse1Down = false;
                dragHandle.setCursor(null);
            }
        });
        dragHandle.addMouseMoveListener(new MouseMoveListener() {
            public void mouseMove(MouseEvent e) {
                if (mouse1Down) {
                    Point p = Display.getCurrent().getCursorLocation();
                    Rectangle r = figure.getBounds().getCopy();
                    r.setLocation(p.x + moveOffsetX, p.y + moveOffsetY);
                    figure.getParent().setConstraint(figure, r);
                }
            }
        });
        
        return panel;
    }
    
    protected void relocateFloatingControls() {
        IFigure reference = AbstractInspectorPart.this.figure;
        Rectangle targetBounds = new Rectangle(reference.getBounds());
        reference.translateToAbsolute(targetBounds);
        Point targetTopRightCorner = new Point(targetBounds.right(), targetBounds.y); 
        floatingControls.setLocation(targetTopRightCorner.x + 3, targetTopRightCorner.y);        
    }
    
    public void dispose() {
		System.out.println("disposing inspector: " + object);

		if (floatingControls != null) {
            floatingControls.dispose();
            floatingControls = null;        
        }

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

    public static IInspectorPart findInspectorPartAt(FigureCanvas canvas, int x, int y) {
        IFigure target = canvas.getContents().findFigureAt(x, y);
        while (target != null && !(target instanceof IInspectorFigure))
            target = target.getParent();
        return target==null ? null : ((IInspectorFigure)target).getInspectorPart();
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
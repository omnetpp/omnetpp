package org.omnetpp.simulation.canvas;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.CoordinateListener;
import org.eclipse.draw2d.FigureListener;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.swt.SWT;
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
import org.eclipse.swt.widgets.ToolBar;
import org.omnetpp.simulation.figures.IInspectorFigure;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.inspectors.actions.CloseAction;
import org.omnetpp.simulation.inspectors.actions.IInspectorAction;

/**
 *
 * @author Andras
 */
//FIXME SWT inspector cannot steal floating toolbar if it's over (and inside the bounds of) the toolbar's current owner (non-SWT inspector)
public class FloatingToolbarSupport {
    private SimulationCanvas simulationCanvas;

    private IInspectorPart floatingToolbarOwner = null;
    private Control floatingToolbar = null;
    private Point floatingToolbarDisplacement;  // relative to its normal location (= right-aligned just above the inspector)
    private boolean isFloatingToolbarStickyTimerActive = false;
    private List<IInspectorAction> toolbarActions = null;  // so inspectors can call updateActions()

    private Runnable floatingToolbarStickyTimer = new Runnable() {
        @Override
        public void run() {
            isFloatingToolbarStickyTimerActive = false;

            // floating toolbar sticky period is over: let another inspector steal the toolbar, or just remove it
            if (floatingToolbar != null && !floatingToolbar.isDisposed() && !containsMouse(floatingToolbar)) {
                Point e = Display.getCurrent().map(null, simulationCanvas, Display.getCurrent().getCursorLocation());
                org.eclipse.draw2d.geometry.Point mouse = simulationCanvas.translateCanvasToAbsoluteFigureCoordinates(e.x, e.y);
                IInspectorPart inspectorUnderMouse = simulationCanvas.findInspectorAt(mouse.x, mouse.y);

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
     * Constructor
     */
    public FloatingToolbarSupport(SimulationCanvas canvas) {
        this.simulationCanvas = canvas;

        // basic listener for managing the floating toolbar
        simulationCanvas.addMouseMoveListener(new MouseMoveListener() {
            @Override
            public void mouseMove(MouseEvent e) {
                org.eclipse.draw2d.geometry.Point mouse = simulationCanvas.translateCanvasToAbsoluteFigureCoordinates(e.x, e.y);

                IInspectorPart inspectorUnderMouse = simulationCanvas.findInspectorAt(mouse.x, mouse.y);

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
        simulationCanvas.getViewport().addCoordinateListener(new CoordinateListener() {
            @Override
            public void coordinateSystemChanged(IFigure source) {
                if (floatingToolbar != null)
                    relocateFloatingToolbar();
            }
        });
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

    public void addFloatingToolbarSupportTo(Control control, final IInspectorPart inspector) {
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

    public void openFloatingToolbarFor(IInspectorPart inspector) {
        Assert.isTrue(floatingToolbar == null);

        Composite panel = new Composite(simulationCanvas, SWT.BORDER);
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

        // collect IInspectorActions into toolbarActions, for updateFloatingToolbarActions()
        toolbarActions = new ArrayList<IInspectorAction>();
        for (IContributionItem item : manager.getItems()) {
            if (item instanceof ActionContributionItem) {
                IAction action = ((ActionContributionItem)item).getAction();
                if (action instanceof IInspectorAction)
                    toolbarActions.add((IInspectorAction)action);
            }
        }

        // set the size of the toolbar
        panel.layout();
        panel.setSize(panel.computeSize(SWT.DEFAULT, SWT.DEFAULT));

        // raise to the top
        panel.moveAbove(null);

        // we may need to displace it by some vector compared to its normal location,
        // to ensure it is fully visible (i.e. appears within canvas bounds)
        Point naturalLoc = computeFloatingToolbarNaturalLocation(inspector.getFigure(), panel);
        Point adjustedLoc = new Point(naturalLoc.x, naturalLoc.y);
        org.eclipse.swt.graphics.Rectangle canvasClientArea = simulationCanvas.getClientArea();
        if (adjustedLoc.x + panel.getSize().x > canvasClientArea.width)
            adjustedLoc.x = canvasClientArea.width - panel.getSize().x;
        if (adjustedLoc.x < 0)
            adjustedLoc.x = 0;
        if (adjustedLoc.y + panel.getSize().y > canvasClientArea.height)
            adjustedLoc.y = canvasClientArea.height - panel.getSize().y;
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

    public void closeFloatingToolbar() {
        if (floatingToolbar != null) {
            floatingToolbar.dispose();
            floatingToolbar = null;
            floatingToolbarOwner.getFigure().removeFigureListener(floatingToolbarRelocator);
            floatingToolbarOwner = null;
            toolbarActions = null;
        }
    }

    public void updateFloatingToolbarActions() {
        if (toolbarActions != null)
            for (IInspectorAction action : toolbarActions)
                action.update();
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

}

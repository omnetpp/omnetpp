package org.omnetpp.runtimeenv.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutListener;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.MenuDetectEvent;
import org.eclipse.swt.events.MenuDetectListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.figures.SubmoduleFigureEx;
import org.omnetpp.runtimeenv.util.InspectorMouseListener;
import org.omnetpp.runtimeenv.widgets.FigureCanvas;

/**
 * 
 * @author Andras
 */
//TODO canvas selection mechanism
public class ModelCanvas extends EditorPart {
    public static final String EDITOR_ID = "org.omnetpp.runtimeenv.editors.ModelCanvas";
    protected ScrolledComposite sc;
    protected FigureCanvas canvas;
    protected List<IInspectorPart> inspectors = new ArrayList<IInspectorPart>();
    protected ISelectionChangedListener inspectorSelectionListener;

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        if (!(input instanceof ModuleIDEditorInput))
            throw new PartInitException("Invalid input: it must be a module in the simulation");
        
        setSite(site);
        setInput(input);
        setPartName(input.getName());
        
        site.setSelectionProvider(new SelectionProvider());
        
        inspectorSelectionListener = new ISelectionChangedListener() {
            @Override
            public void selectionChanged(SelectionChangedEvent event) {
                inspectorSelectionChanged(event);
            }
        };
    }

    @Override
    public void createPartControl(Composite parent) {
        sc = new ScrolledComposite(parent, SWT.V_SCROLL | SWT.H_SCROLL);
        
        // create canvas
        canvas = new FigureCanvas(sc, SWT.DOUBLE_BUFFERED);
        sc.setContent(canvas);
        canvas.setBackground(new Color(null, 235, 235, 235));
        canvas.getRootFigure().setLayoutManager(new XYLayout());

        // recalculate canvas size when figures change or editor area gets resized
        canvas.getRootFigure().addLayoutListener(new LayoutListener.Stub() {
            @Override
            public void postLayout(IFigure container) {
                recalculateCanvasSize();
            }
        });
        sc.addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                recalculateCanvasSize();
            }
        });
        
        // create context menu
        final MenuManager contextMenuManager = new MenuManager("#popup");
        canvas.setMenu(contextMenuManager.createContextMenu(canvas));
        canvas.addMenuDetectListener(new MenuDetectListener() {
            @Override
            public void menuDetected(MenuDetectEvent e) {
                contextMenuManager.removeAll();
                populateContextMenu(contextMenuManager, e);
            }
        });

        int moduleID = ((ModuleIDEditorInput)getEditorInput()).getModuleID();
        createModulePart(moduleID);
    }

    protected void recalculateCanvasSize() {
        Dimension size = canvas.getRootFigure().getPreferredSize();
        org.eclipse.swt.graphics.Rectangle clientArea = sc.getClientArea();
        canvas.setSize(Math.max(size.width, clientArea.width), Math.max(size.height, clientArea.height));
    } 

    protected void populateContextMenu(final MenuManager contextMenuManager, MenuDetectEvent e) {
        Point p = canvas.toControl(e.x, e.y);
        GraphicalModulePart modulePart = GraphicalModulePart.findModulePartAt(canvas, p.x, p.y);
        SubmoduleFigureEx submoduleFigure = modulePart==null? null : modulePart.findSubmoduleAt(p.x, p.y);
        if (submoduleFigure != null) {
            final int submoduleID = submoduleFigure.getModuleID();

            //XXX factor out actions
            contextMenuManager.add(new Action("Open in New Canvas") {
                @Override
                public void run() {
                    System.out.println("opening editor for " + submoduleID);
                    IWorkbenchPage page = Activator.getActiveWorkbenchPage();
                    if (page != null) {
                        Activator.openEditor(new ModuleIDEditorInput(submoduleID), EDITOR_ID);
                    }
                }
            });

            contextMenuManager.add(new Action("Open on This Canvas") {
                @Override
                public void run() {
                    createModulePart(submoduleID);
                }
            });
        }
    }

    protected GraphicalModulePart createModulePart(int moduleID) {
        GraphicalModulePart modulePart = new GraphicalModulePart(moduleID);
        addInspectorPart(modulePart);
        return modulePart;
    }

    protected void addInspectorPart(IInspectorPart inspectorPart) {
        int lastY = canvas.getRootFigure().getPreferredSize().height;
        addInspectorPart(inspectorPart, 0, lastY+5);
    }
    
    protected void addInspectorPart(IInspectorPart inspectorPart, final int x, final int y) {
        final IFigure moduleFigure = inspectorPart.getFigure();
        canvas.getRootFigure().add(moduleFigure);
        canvas.getRootFigure().setConstraint(moduleFigure, new Rectangle(x, y, -1, -1));
        
        // reveal new inspector on canvas
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                if (!sc.isDisposed())
                    reveal(moduleFigure); //XXX also select it
            }});
        
        // add move/resize/selection support
        inspectors.add(inspectorPart);
        new InspectorMouseListener(inspectorPart);
        
        // listen on selection changes
        inspectorPart.addSelectionChangedListener(inspectorSelectionListener);        
    }

    public void removeInspectorPart(IInspectorPart inspectorPart) {
        Assert.isTrue(inspectors.contains(inspectorPart));
        inspectors.remove(inspectorPart);
        canvas.getRootFigure().remove(inspectorPart.getFigure());
        inspectorPart.removeSelectionChangedListener(inspectorSelectionListener);        
        //XXX what else?
    }
    
    public void reveal(IFigure figure) {
        Rectangle bounds = figure.getBounds(); //XXX maybe not good if coords are parent-relative
        sc.setOrigin(bounds.x, bounds.y);
    }
    
    @Override
    public void setFocus() {
        canvas.setFocus();
    }

    protected void inspectorSelectionChanged(SelectionChangedEvent event) {
        // for now, simply just take over that selection
        getSite().getSelectionProvider().setSelection(event.getSelection());
    }

    @Override
    public void doSave(IProgressMonitor monitor) {
        // Nothing
    }

    @Override
    public void doSaveAs() {
        // Nothing
    }

    @Override
    public boolean isDirty() {
        return false;
    }

    @Override
    public boolean isSaveAsAllowed() {
        return false;
    }

}

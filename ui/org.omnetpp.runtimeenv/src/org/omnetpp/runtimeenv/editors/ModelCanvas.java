package org.omnetpp.runtimeenv.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MenuDetectEvent;
import org.eclipse.swt.events.MenuDetectListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.figures.CompoundModuleFigureEx;
import org.omnetpp.runtimeenv.figures.SubmoduleFigureEx;
import org.omnetpp.runtimeenv.util.CompoundModuleFigureMouseListener;
import org.omnetpp.runtimeenv.widgets.FigureCanvas;

/**
 * 
 * @author Andras
 */
//TODO canvas selection mechanism
public class ModelCanvas extends EditorPart {
    public static final String EDITOR_ID = "org.omnetpp.runtimeenv.editors.ModelCanvas";
    protected FigureCanvas canvas;

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        if (!(input instanceof ModuleIDEditorInput))
            throw new PartInitException("Invalid input: it must be a module in the simulation");
        
        setSite(site);
        setInput(input);
        setPartName(input.getName());
        site.setSelectionProvider(new SelectionProvider());
    }

    @Override
    public void createPartControl(Composite parent) {
        // create canvas
        canvas = new FigureCanvas(parent, SWT.BORDER | SWT.DOUBLE_BUFFERED);  //FIXME wrap canvas into a ScrolledComposite!!
        canvas.setBackground(new Color(null, 228, 228, 228));
        canvas.getRootFigure().setLayoutManager(new XYLayout());
        
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

    private GraphicalModulePart createModulePart(int moduleID) {
        final GraphicalModulePart modulePart = new GraphicalModulePart(canvas.getRootFigure(), moduleID);
        CompoundModuleFigureEx moduleFigure = modulePart.getModuleFigure();
        canvas.getRootFigure().setConstraint(moduleFigure, new Rectangle(20,20,-1,-1)); //TODO proper positioning

        // add selection support
        moduleFigure.addMouseListener(new MouseListener.Stub() {
            @Override
            public void mousePressed(MouseEvent me) {
                if (!modulePart.isSelected()) {
                    modulePart.setSelected(true);
                    me.consume();
                }
            }
        });
        
        // add move/resize support
        new CompoundModuleFigureMouseListener(moduleFigure);
        return modulePart;
    }

    @Override
    public void setFocus() {
        canvas.setFocus();
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

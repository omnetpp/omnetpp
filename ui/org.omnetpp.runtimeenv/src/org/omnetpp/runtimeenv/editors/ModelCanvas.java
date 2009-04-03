package org.omnetpp.runtimeenv.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MenuDetectEvent;
import org.eclipse.swt.events.MenuDetectListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.animation.widgets.AnimationCanvas;
import org.omnetpp.runtimeenv.figures.SubmoduleFigureEx;

/**
 * 
 * @author Andras
 */
//TODO canvas selection mechanism
//TODO submodule context menu: "Open (go into)", "Add to this canvas", "Open in new network view" 
public class ModelCanvas extends EditorPart {
    public static final String EDITOR_ID = "org.omnetpp.runtimeenv.editors.ModelCanvas";
    protected AnimationCanvas canvas;
    protected GraphicalModulePart modulePart;

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        if (!(input instanceof ModuleIDEditorInput))
            throw new PartInitException("Invalid input: it must be a module in the simulation");
        
        setSite(site);
        setInput(input);
        setPartName(input.getName());
    }

    @Override
    public void createPartControl(Composite parent) {
        //parent.setLayout(new FormLayout());
        //parent.setBackground(new Color(null, 228, 228, 228));
        
        canvas = new AnimationCanvas(parent, SWT.DOUBLE_BUFFERED);

        int moduleID = ((ModuleIDEditorInput)getEditorInput()).getModuleID();
        modulePart = new GraphicalModulePart(canvas.getRootFigure(), moduleID);

        final MenuManager contextMenuManager = new MenuManager("#popup");
        //contextMenuManager.setRemoveAllWhenShown(true);
        
//        contextMenuManager.addMenuListener(new IMenuListener() {
//            @Override
//            public void menuAboutToShow(IMenuManager manager) {
//                manager.add(new Action("Sample Action " + Math.random()) {}); //XXX
//            }
//        });
//        canvas.setMenu(new Menu(canvas));

        canvas.setMenu(contextMenuManager.createContextMenu(canvas));
        
        canvas.addMenuDetectListener(new MenuDetectListener() {
            @Override
            public void menuDetected(MenuDetectEvent e) {
                System.out.println("MenuDetectEvent");
                Point p = canvas.toControl(e.x, e.y);
                GraphicalModulePart part = findModulePartAt(p.x, p.y);
                SubmoduleFigureEx submoduleFigure = part.findSubmoduleAt(p.x, p.y);
                if (submoduleFigure != null) {
                    final int submoduleID = submoduleFigure.getModuleID();
                    contextMenuManager.removeAll();

                    contextMenuManager.add(new Action("Open in New Canvas") {
                        @Override
                        public void run() {
                            System.out.println("opening editor for " + submoduleID);
                            IWorkbenchPage page = Activator.getActiveWorkbenchPage();
                            if (page != null) {
                                IEditorPart editr = Activator.openEditor(new ModuleIDEditorInput(submoduleID), EDITOR_ID);
                            }
                        }
                    }); //XXX

                    contextMenuManager.add(new Action("Open on This Canvas") {
                        @Override
                        public void run() {
                            new GraphicalModulePart(canvas.getRootFigure(), submoduleID);
                        }
                    }); //XXX
                    
                    
                }
            }
        });
    }

    protected GraphicalModulePart findModulePartAt(int x, int y) {
        return modulePart; //XXX for now...
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

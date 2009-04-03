package org.omnetpp.runtimeenv.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MenuDetectEvent;
import org.eclipse.swt.events.MenuDetectListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.figures.SubmoduleFigureEx;
import org.omnetpp.runtimeenv.util.FormLayoutMouseListener;
import org.omnetpp.runtimeenv.util.FormLayoutMouseListenerTest;
import org.omnetpp.runtimeenv.widgets.FigureCanvas;

/**
 * 
 * @author Andras
 */
//TODO canvas selection mechanism
//TODO submodule context menu: "Open (go into)", "Add to this canvas", "Open in new network view"
//TODO may need to refactor everything so that a module canvas is a figure
public class ModelCanvas extends EditorPart {
    public static final String EDITOR_ID = "org.omnetpp.runtimeenv.editors.ModelCanvas";
    protected Composite composite;

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
        composite = new Composite(parent, SWT.BORDER);
        composite.setBackground(new Color(null, 228, 228, 228));
        
        composite.setLayout(new FormLayout());
        //composite.setLayout(new FillLayout(SWT.VERTICAL));

        int moduleID = ((ModuleIDEditorInput)getEditorInput()).getModuleID();
        createModuleCanvas(moduleID);

        FormLayoutMouseListenerTest.run(parent.getShell());
}

    private void createModuleCanvas(int moduleID) {
        final FigureCanvas canvas = new FigureCanvas(composite, SWT.BORDER | SWT.DOUBLE_BUFFERED);

        FormData formData = new FormData();
        //formData.left = new FormAttachment(0, 0);
        //formData.top = new FormAttachment(0, 0);  //XXX (coolBar.getParent(), 0);
        //formData.right = new FormAttachment(100, 0);
        //formData.bottom = new FormAttachment(100, 0);
        formData.width = 500;
        formData.height = 500;
        canvas.setLayoutData(formData);
        FormLayoutMouseListener listener = new FormLayoutMouseListener(composite, true, true);
        canvas.addMouseListener(listener);
        canvas.addMouseMoveListener(listener);
        canvas.addMouseTrackListener(listener);
        
        GraphicalModulePart modulePart = new GraphicalModulePart(canvas.getRootFigure(), moduleID);

        addContextMenu(canvas, modulePart); //FIXME 1 arg enough
    }

    private void addContextMenu(final FigureCanvas canvas, final GraphicalModulePart modulePart) {
        final MenuManager contextMenuManager = new MenuManager("#popup");
        canvas.setMenu(contextMenuManager.createContextMenu(canvas));
        
        canvas.addMenuDetectListener(new MenuDetectListener() {
            @Override
            public void menuDetected(MenuDetectEvent e) {
                System.out.println("MenuDetectEvent");
                Point p = canvas.toControl(e.x, e.y);
                //part = findModulePartAt(p.x, p.y);
                SubmoduleFigureEx submoduleFigure = modulePart.findSubmoduleAt(p.x, p.y);
                if (submoduleFigure != null) {
                    final int submoduleID = submoduleFigure.getModuleID();
                    contextMenuManager.removeAll();

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
                    }); //XXX

                    contextMenuManager.add(new Action("Open on This Canvas") {
                        @Override
                        public void run() {
                            createModuleCanvas(submoduleID);
                            //new GraphicalModulePart(canvas.getRootFigure(), submoduleID);
                        }
                    });
                }
            }
        });
    }

    @Override
    public void setFocus() {
        //canvas.setFocus();
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

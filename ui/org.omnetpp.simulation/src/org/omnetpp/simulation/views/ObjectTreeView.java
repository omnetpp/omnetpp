package org.omnetpp.simulation.views;

import java.io.IOException;
import java.util.Arrays;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.DecoratingStyledCellLabelProvider;
import org.eclipse.jface.viewers.DelegatingStyledCellLabelProvider.IStyledLabelProvider;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.jface.viewers.StyledString.Styler;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextStyle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchPartSite;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.SelectionProvider;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.ISimulationStateListener;
import org.omnetpp.simulation.controller.Simulation;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.editors.SimulationEditor;
import org.omnetpp.simulation.model.cObject;

/**
 *
 * @author Andras
 */
//FIXME display message if no active simulation
//FIXME better loading of objects!! (from bg, etc)
public class ObjectTreeView extends ViewWithMessagePart {
    // note: view ID is defined in IConstants.java
    protected TreeViewer viewer;
    protected MenuManager contextMenuManager = new MenuManager("#PopupMenu");

    private SimulationEditor associatedSimulationEditor;
    private IPartListener partListener;

    private ISimulationStateListener simulationListener;

    public static class ViewContentProvider implements ITreeContentProvider {
        public Object[] getChildren(Object element) {
            if (element instanceof cObject) {
                cObject object = (cObject)element;
                if (!object.isFilledIn())
                    object.safeLoad(); //FIXME return if failed!
                cObject[] childObjects = object.getChildObjects();
                try {
                    object.getSimulation().loadUnfilledObjects(Arrays.asList(childObjects)); //FIXME return if failed??
                }
                catch (IOException e) {
                    SimulationPlugin.logError("Could not retrieve objects from simulation process", e);
                }
                return childObjects;
            }
            return new Object[0];
        }

        public Object[] getElements(Object inputElement) {
            return getChildren(inputElement);
        }

        public Object getParent(Object element) {
            if (element instanceof cObject) {
                cObject object = (cObject)element;
                if (!object.isFilledIn())
                    object.safeLoad(); //FIXME return if failed!
                if (!object.isDisposed())
                    return object.getOwner();
            }
            return null;  // we don't know
        }

        public boolean hasChildren(Object element) {
            if (element instanceof cObject) {
                cObject object = (cObject)element;
                if (!object.isFilledIn())
                    object.safeLoad();  //FIXME return if failed!
                return object.getChildObjects().length > 0;
            }
            return false;
        }

        public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
            // Do nothing
        }

        public void dispose() {
            // Do nothing
        }
    }

    public static class ViewLabelProvider implements IStyledLabelProvider {
        private class ColorStyler extends Styler {
            Color color;
            public ColorStyler(Color color) { this.color = color; }
            @Override public void applyStyles(TextStyle textStyle) { textStyle.foreground = color; }
        };

        private Styler greyStyle = new ColorStyler(ColorFactory.GREY60);
        private Styler brownStyle = new ColorStyler(ColorFactory.BURLYWOOD4);

        public StyledString getStyledText(Object element) {
            if (element instanceof cObject) {
                cObject object = (cObject)element;
                if (!object.isFilledIn())
                    object.safeLoad(); //FIXME return if failed!
                if (object.isDisposed())
                    return new StyledString("<n/a>"); // deleted or error
                StyledString styledString = new StyledString(object.getFullName());
                styledString.append(" (" + object.getClassName() + ")", greyStyle); //TODO use Simkernel.getObjectShortTypeName(obj);
                styledString.append("  " + object.getInfo(), brownStyle);
                return styledString;
            }
            return new StyledString(element.toString());
        }

        public Image getImage(Object element) {
            if (element instanceof cObject) {
                cObject object = (cObject)element;
                if (!object.isFilledIn())
                    object.safeLoad();
                String icon = object.getIcon(); // note: may be empty
                return SimulationPlugin.getCachedImage(SimulationUIConstants.IMG_OBJ_DIR + icon + ".png", SimulationUIConstants.IMG_OBJ_COGWHEEL);
            }
            return PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_ERROR_TSK);
        }

        public boolean isLabelProperty(Object element, String property) {
            return true;
        }

        public void dispose() {
            // nothing
        }

        public void addListener(ILabelProviderListener listener) {
            // nothing
        }

        public void removeListener(ILabelProviderListener listener) {
            // nothing
        }
    }

    /**
     * This is a callback that will allow us to create the viewer and initialize it.
     */
    public Control createViewControl(Composite parent) {
        viewer = new TreeViewer(parent, SWT.DOUBLE_BUFFERED | SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL);
        viewer.setContentProvider(new ViewContentProvider());
        viewer.setLabelProvider(new DecoratingStyledCellLabelProvider(new ViewLabelProvider(), null, null));

        // create context menu
        getViewSite().registerContextMenu(contextMenuManager, viewer);
        viewer.getTree().setMenu(contextMenuManager.createContextMenu(viewer.getTree()));
        //TODO dynamic menu based on which object is selected

        // double-click opens an inspector
        viewer.addDoubleClickListener(new IDoubleClickListener() {
            public void doubleClick(DoubleClickEvent event) {
                Object element = ((IStructuredSelection)event.getSelection()).getFirstElement();
                if (element instanceof cObject)
                    associatedSimulationEditor.openInspector((cObject)element);
            }
        });

        // export our selection to the workbench
        getViewSite().setSelectionProvider(new SelectionProvider());
        viewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                getViewSite().getSelectionProvider().setSelection(event.getSelection());
            }
        });

        // listen on editor changes
        partListener = new IPartListener() {
            public void partActivated(IWorkbenchPart part) {
                if (part instanceof IEditorPart)
                    editorActivated((IEditorPart) part);
            }

            public void partBroughtToTop(IWorkbenchPart part) {
            }

            public void partClosed(IWorkbenchPart part) {
                if (part instanceof IEditorPart)
                    editorClosed((IEditorPart) part);
            }

            public void partDeactivated(IWorkbenchPart part) {
            }

            public void partOpened(IWorkbenchPart part) {
            }
        };
        getSite().getPage().addPartListener(partListener);  //TODO unhookListeners() etc -- see PinnableView!

        simulationListener = new ISimulationStateListener() {
            @Override
            public void simulationStateChanged(final SimulationController controller) {
                DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
                    @Override
                    public void run() {
                        ObjectTreeView.this.simulationStateChanged(controller);
                    }
                });
            }
        };

        // associate ourselves with the current simulation editor
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                SimulationEditor editor = getActiveSimulationEditor();
                if (editor != null)
                    associateWithEditor(editor);
                else
                    showMessage("No associated simulation.");
            }
        });

        return viewer.getTree();
    }

    @Override
    @SuppressWarnings("rawtypes")
    public Object getAdapter(Class adapter) {
        // ensure this view has a custom page in Properties View; see PropertySheet documentation
        if (adapter.equals(IPropertySheetPage.class))
            return new SimulationObjectPropertySheetPage();
        return super.getAdapter(adapter);
    }

    protected void simulationStateChanged(SimulationController controller) {
        if (controller.getUIState() == SimState.DISCONNECTED)
            showMessage("No simulation process.");
        else {
            hideMessage();

            if (controller.getSimulation().hasRootObjects()) {
                cObject input = controller.getSimulation().getRootObject(Simulation.ROOTOBJ_SIMULATION);
                if (!input.equals(viewer.getInput()))
                    viewer.setInput(input);
            }

            viewer.refresh();
        }
    }

    protected void editorActivated(IEditorPart editor) {
        if (editor != associatedSimulationEditor && editor instanceof SimulationEditor)
            associateWithEditor((SimulationEditor)editor);
    }

    protected void editorClosed(IEditorPart editor) {
        if (editor == associatedSimulationEditor)
            disassociateFromEditor();
    }

    protected void associateWithEditor(SimulationEditor editor) {
        associatedSimulationEditor = editor;

        SimulationController controller = ((SimulationEditor)editor).getSimulationController();
        controller.addSimulationStateListener(simulationListener);

        if (controller.getUIState() == SimState.DISCONNECTED || !controller.getSimulation().hasRootObjects()) {
            showMessage("No simulation process.");
        }
        else {
            hideMessage();
            cObject input = controller.getSimulation().getRootObject(Simulation.ROOTOBJ_SIMULATION);
            viewer.setInput(input);
            viewer.refresh();
        }
    }

    protected void disassociateFromEditor() {
        associatedSimulationEditor.getSimulationController().removeSimulationStateListener(simulationListener);
        associatedSimulationEditor = null;
        viewer.setInput(null);
        viewer.refresh();

        showMessage("No associated simulation.");
    }

    protected SimulationEditor getActiveSimulationEditor() {
        IWorkbenchPartSite site = getSite();
        IWorkbenchPage page = site==null ? null : site.getWorkbenchWindow().getActivePage();
        if (page != null) {
            if (page.getActiveEditor() instanceof SimulationEditor)
                return (SimulationEditor)page.getActiveEditor();
            // no active simulation editor; just return the first one we find
            for (IEditorReference ref : page.getEditorReferences())
                if (ref.getEditor(false) instanceof SimulationEditor)
                    return (SimulationEditor)ref.getEditor(false);
        }
        return null;
    }

    /**
     * Passing the focus request to the viewer's control.
     */
    public void setFocus() {
        viewer.getControl().setFocus();
    }

    @Override
    public void dispose() {
        if (associatedSimulationEditor != null)
            associatedSimulationEditor.getSimulationController().removeSimulationStateListener(simulationListener);
        super.dispose();
    }
}

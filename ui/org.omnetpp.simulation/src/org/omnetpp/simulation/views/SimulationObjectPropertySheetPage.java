package org.omnetpp.simulation.views;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.omnetpp.common.ui.IUpdateableAction;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.canvas.SelectionUtils;
import org.omnetpp.simulation.canvas.SimulationCanvas;
import org.omnetpp.simulation.editors.SimulationEditor;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.ObjectFieldsViewer;
import org.omnetpp.simulation.ui.SetObjectViewerModeAction;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Mode;

/**
 *
 * @author Andras
 */
public class SimulationObjectPropertySheetPage implements IPropertySheetPage {
    public static final ImageDescriptor IMG_MODE_AUTO = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_auto.png");

    private Composite composite;
    private Label label;
    private ObjectFieldsViewer viewer;
    private boolean autoMode = true; //TODO
    private List<IUpdateableAction> actions = new ArrayList<IUpdateableAction>();

    @Override
    public void createControl(Composite parent) {
        composite = new Composite(parent, SWT.BORDER);
        composite.setSize(300, 200);
        composite.setLayout(new GridLayout(1, false));

        label = new Label(composite, SWT.NONE);
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        viewer = new ObjectFieldsViewer(composite, SWT.BORDER | SWT.V_SCROLL | SWT.MULTI);
        viewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // create context menu
        final MenuManager contextMenuManager = new MenuManager("#PopupMenu");
        // getViewSite().registerContextMenu(contextMenuManager, viewer); --XXX how?
        viewer.getTree().setMenu(contextMenuManager.createContextMenu(viewer.getTree()));
        contextMenuManager.setRemoveAllWhenShown(true);
        contextMenuManager.addMenuListener(new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager manager) {
                IEditorPart activeEditor = getActiveEditor();
                if (activeEditor instanceof SimulationEditor)
                    ((SimulationEditor)activeEditor).populateContextMenu(contextMenuManager, viewer.getSelection());
            }
        });

        viewer.getTree().addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetDefaultSelected(SelectionEvent e) {
                // inspect the selected object(s)
                IEditorPart activeEditor = getActiveEditor();
                if (activeEditor instanceof SimulationEditor) {
                    ISelection selection = viewer.getSelection();
                    List<cObject> objects = SelectionUtils.getObjects(selection, cObject.class);
                    SimulationCanvas simulationCanvas = ((SimulationEditor)activeEditor).getSimulationCanvas();
                    for (cObject object : objects)
                        simulationCanvas.inspect(object);
                }
            }
        });
    }

    @Override
    public Control getControl() {
        return composite;
    }

    @Override
    public void setFocus() {
        viewer.getTree().setFocus();
    }

    @Override
    public void dispose() {
    }

    protected class SetAutoModeAction extends Action implements IUpdateableAction {
        public SetAutoModeAction(String tooltip, ImageDescriptor image) {
            super(tooltip, AS_CHECK_BOX);
            setToolTipText(tooltip);
            setImageDescriptor(image);
        }

        @Override
        public void run() {
            autoMode = !autoMode;
            chooseAutoMode();
            updateActions();
        }

        @Override
        public void update() {
            setChecked(autoMode);
        }
    }

    protected class SetModeAction extends SetObjectViewerModeAction {
        public SetModeAction(ObjectFieldsViewer viewer, Mode mode) {
            super(viewer, mode);
        }

        @Override
        public void run() {
            super.run();
            updateActions();
        }
    }

    @Override
    public void setActionBars(IActionBars actionBars) {
        IToolBarManager manager = actionBars.getToolBarManager();
        //TODO manager.add(my(new SortAction()));
        manager.add(new Separator());
        manager.add(my(new SetAutoModeAction("Auto mode", IMG_MODE_AUTO)));
        manager.add(my(new SetModeAction(viewer, Mode.PACKET)));
        manager.add(my(new SetModeAction(viewer, Mode.CHILDREN)));
        manager.add(my(new SetModeAction(viewer, Mode.GROUPED)));
        manager.add(my(new SetModeAction(viewer, Mode.INHERITANCE)));
        manager.add(my(new SetModeAction(viewer, Mode.FLAT)));
        updateActions();
    }

    protected IUpdateableAction my(IUpdateableAction action) {
        actions.add(action);
        return action;
    }

    protected void updateActions() {
        for (IUpdateableAction action : actions)
            action.update();
    }

    @Override
    public void selectionChanged(IWorkbenchPart part, ISelection selection) {
        if (selection instanceof IStructuredSelection) {
            List<cObject> objects = SelectionUtils.getObjects(selection, cObject.class);
            if (!objects.isEmpty()) {
                // update label and set input to the tree
                String text;
                if (objects.size() == 1) {
                    cObject object = objects.get(0);
                    text = "(" + object.getShortTypeName() + ") " + object.getFullPath() + " - " + object.getInfo();
                    viewer.setInput(object);
                }
                else {
                    text = objects.size() + " objects";
                    viewer.setInput(objects);
                }
                label.setText(text);
                label.setToolTipText(text); // because label text is usually not fully visible

                // refresh tree mode (if automatic)
                if (autoMode)
                    chooseAutoMode();
            }
        }
    }

    protected void chooseAutoMode() {
        Object input = viewer.getTreeViewer().getInput();
        Mode mode = ObjectFieldsViewer.getPreferredMode(input);
        viewer.setMode(mode);
        updateActions();
    }

    protected IEditorPart getActiveEditor() {
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IWorkbenchPage workbenchPage = workbenchWindow==null ? null : workbenchWindow.getActivePage();
        return workbenchPage==null ? null : workbenchPage.getActiveEditor();
    }
}

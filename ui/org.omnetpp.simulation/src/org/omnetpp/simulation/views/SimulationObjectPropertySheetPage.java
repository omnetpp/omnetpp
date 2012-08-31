package org.omnetpp.simulation.views;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IToolBarManager;
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
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.editors.SimulationEditor;
import org.omnetpp.simulation.inspectors.ObjectFieldsViewer;
import org.omnetpp.simulation.inspectors.SelectionUtils;
import org.omnetpp.simulation.inspectors.ObjectFieldsViewer.Mode;
import org.omnetpp.simulation.inspectors.SimulationCanvas;
import org.omnetpp.simulation.model.cArray;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPacket;
import org.omnetpp.simulation.model.cQueue;

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
    private List<UpdateableAction> actions = new ArrayList<UpdateableAction>();

    @Override
    public void createControl(Composite parent) {
        composite = new Composite(parent, SWT.BORDER);
        composite.setSize(300, 200);
        composite.setLayout(new GridLayout(1, false));

        label = new Label(composite, SWT.NONE);
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        viewer = new ObjectFieldsViewer(composite, SWT.BORDER | SWT.V_SCROLL | SWT.MULTI);
        viewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        viewer.getTree().addSelectionListener(new SelectionAdapter() {
            @Override
            @SuppressWarnings("unchecked")
            public void widgetDefaultSelected(SelectionEvent e) {
                // inspect the selected object(s)
                IEditorPart activeEditor = getActiveEditor();
                if (activeEditor instanceof SimulationEditor) {
                    ISelection selection = viewer.getTreeViewer().getSelection();
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

    protected abstract class UpdateableAction extends Action {
        public UpdateableAction(String text, int style) {
            super(text, style);
        }

        abstract void update();
    }

    public class SetAutoModeAction extends UpdateableAction {
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

    public class SetModeAction extends UpdateableAction {
        private Mode mode;

        public SetModeAction(Mode mode, String tooltip, ImageDescriptor image) {
            super(tooltip, AS_CHECK_BOX);
            setToolTipText(tooltip);
            setImageDescriptor(image);
            this.mode = mode;
        }

        @Override
        public void run() {
            viewer.setMode(mode);
            autoMode = false;
            updateActions();
        }

        @Override
        public void update() {
            setChecked(viewer.getMode() == mode);
        }
    }

    @Override
    public void setActionBars(IActionBars actionBars) {
        IToolBarManager manager = actionBars.getToolBarManager();
        //TODO manager.add(my(new SortAction()));
        manager.add(new Separator());
        manager.add(my(new SetAutoModeAction("Auto mode", IMG_MODE_AUTO)));
        manager.add(my(new SetModeAction(Mode.PACKET, "Packet mode", ObjectFieldsViewer.IMG_MODE_PACKET)));
        manager.add(my(new SetModeAction(Mode.CHILDREN, "Children mode", ObjectFieldsViewer.IMG_MODE_CHILDREN)));
        manager.add(my(new SetModeAction(Mode.GROUPED, "Grouped mode", ObjectFieldsViewer.IMG_MODE_GROUPED)));
        manager.add(my(new SetModeAction(Mode.INHERITANCE, "Inheritance mode", ObjectFieldsViewer.IMG_MODE_INHERITANCE)));
        manager.add(my(new SetModeAction(Mode.FLAT, "Flat mode", ObjectFieldsViewer.IMG_MODE_FLAT)));
        updateActions();
    }

    protected UpdateableAction my(UpdateableAction action) {
        actions.add(action);
        return action;
    }

    protected void updateActions() {
        for (UpdateableAction action : actions)
            action.update();
    }

    @Override
    public void selectionChanged(IWorkbenchPart part, ISelection selection) {
        if (selection instanceof IStructuredSelection) {
            Object element = ((IStructuredSelection)selection).getFirstElement();
            cObject object = null;
            if (element instanceof cObject)
                object = (cObject)element;
            else if (element instanceof IAdaptable)
                object = (cObject) ((IAdaptable)element).getAdapter(cObject.class);

            if (object != null) {
                // update label
                String text = "(" + object.getShortTypeName() + ") " + object.getFullPath() + " - " + object.getInfo();
                label.setText(text);
                label.setToolTipText(text); // because label text is usually not fully visible

                // set input to the tree
                viewer.setInput(object);

                // refresh tree mode (if automatic)
                if (autoMode)
                    chooseAutoMode();
            }
        }
    }

    protected void chooseAutoMode() {
        cObject object = (cObject) viewer.getTreeViewer().getInput();
        boolean isSubclassedFromcPacket = (object instanceof cPacket) && !object.getClassName().equals("cPacket");
        boolean isContainer = (object instanceof cModule) || (object instanceof cQueue) || (object instanceof cArray);
        Mode mode = isSubclassedFromcPacket ? ObjectFieldsViewer.Mode.PACKET : 
            isContainer ? ObjectFieldsViewer.Mode.CHILDREN : ObjectFieldsViewer.Mode.GROUPED;
        viewer.setMode(mode);
        updateActions();
    }

    protected IEditorPart getActiveEditor() {
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IWorkbenchPage workbenchPage = workbenchWindow==null ? null : workbenchWindow.getActivePage();
        return workbenchPage==null ? null : workbenchPage.getActiveEditor();
    }
}

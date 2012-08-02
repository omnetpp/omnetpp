package org.omnetpp.simulation.inspectors;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.inspectors.ObjectFieldsViewer.Mode;
import org.omnetpp.simulation.inspectors.ObjectFieldsViewer.Ordering;
import org.omnetpp.simulation.inspectors.actions.SetModeAction;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPacket;

/**
 *
 * @author Andras
 */
//TODO icon buttons to switch mode, ordering, etc.
//TODO resize by border
//TODO drag by label
//TODO adaptive label: display the most useful info that fits in the available space
public class ObjectFieldsInspectorPart extends AbstractSWTInspectorPart {
    private static final ImageDescriptor IMG_MODE_PACKET = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_packet.png");
    private static final ImageDescriptor IMG_MODE_CHILDREN = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_children.png");
    private static final ImageDescriptor IMG_MODE_FLAT = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_flat.png");
    private static final ImageDescriptor IMG_MODE_GROUPED = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_grouped.png");
    private static final ImageDescriptor IMG_MODE_INHERITANCE = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_inher.png");

    private Composite frame;
    private Label label;
    private ObjectFieldsViewer viewer;

    private IAction packetModeTool;  //TODO null these when floating toolbar disappears
    private IAction childrenModeTool;
    private IAction groupedModeTool;
    private IAction inheritaceModeTool;
    private IAction flatModeTool;


    public ObjectFieldsInspectorPart(IInspectorContainer parent, cObject object) {
        super(parent, object);
    }

    @Override
    protected Control createControl(Composite parent) {
        if (!object.isFilledIn())
            object.safeLoad(); // for getClassName() in next line
        boolean isSubclassedFromcPacket = (object instanceof cPacket) && !object.getClassName().equals("cPacket");

        frame = new Composite(parent, SWT.BORDER);
        frame.setSize(300, 200);
        frame.setLayout(new GridLayout(1, false));

        label = new Label(frame, SWT.NONE);
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        viewer = new ObjectFieldsViewer(frame, SWT.BORDER | SWT.V_SCROLL | SWT.MULTI);
        viewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        viewer.setMode(isSubclassedFromcPacket ? ObjectFieldsViewer.Mode.PACKET : ObjectFieldsViewer.Mode.GROUPED);
        viewer.setInput(object);

        viewer.getTree().addSelectionListener(new SelectionListener() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                inspectorContainer.select(object, true);  //XXX ???
            }

            @Override
            public void widgetDefaultSelected(SelectionEvent e) {
                // inspect the selected object(s)
                ISelection selection = viewer.getTreeViewer().getSelection();
                cObject[] objects = getContainer().getObjectsFromSelection(selection);
                for (cObject object : objects)
                    getContainer().inspect(object);
            }
        });

        frame.layout();

        return frame;
    }

    @Override
    public boolean isMaximizable() {
        return false;
    }

    public Mode getMode() {
        return viewer.getMode();
    }

    public void setMode(Mode mode) {
        viewer.setMode(mode);
        updateModeSwitcherToolsState();
    }

    public Ordering getOrdering() {
        return viewer.getOrdering();
    }

    public void setOrdering(Ordering ordering) {
        viewer.setOrdering(ordering);
    }

    public boolean isReverseOrder() {
        return viewer.isReverseOrder();
    }

    public void setReverseOrder(boolean reverseOrder) {
        viewer.setReverseOrder(reverseOrder);
    }

    @Override
    public void populateContextMenu(MenuManager contextMenuManager, Point p) {
    }

    @Override
    public void populateFloatingToolbar(ToolBarManager manager) {
        manager.add(packetModeTool = my(new SetModeAction(Mode.PACKET, "Packet mode", IMG_MODE_PACKET)));
        manager.add(childrenModeTool = my(new SetModeAction(Mode.CHILDREN, "Children mode", IMG_MODE_CHILDREN)));
        manager.add(groupedModeTool = my(new SetModeAction(Mode.GROUPED, "Grouped mode", IMG_MODE_GROUPED)));
        manager.add(inheritaceModeTool = my(new SetModeAction(Mode.INHERITANCE, "Inheritance mode", IMG_MODE_INHERITANCE)));
        manager.add(flatModeTool = my(new SetModeAction(Mode.FLAT, "Flat mode", IMG_MODE_FLAT)));
        updateModeSwitcherToolsState();
    }

    public void updateModeSwitcherToolsState() {
        Mode mode = viewer.getMode();
        if (packetModeTool != null) {
            packetModeTool.setChecked(mode == Mode.PACKET);
            childrenModeTool.setChecked(mode == Mode.CHILDREN);
            groupedModeTool.setChecked(mode == Mode.GROUPED);
            inheritaceModeTool.setChecked(mode == Mode.INHERITANCE);
            flatModeTool.setChecked(mode == Mode.FLAT);
        }
    }

    @Override
    public void refresh() {
        super.refresh();

        if (!isDisposed()) {
            String text = "(" + object.getClassName() + ") " + object.getFullPath() + " - " + object.getInfo();
            label.setText(text);
            label.setToolTipText(text); // because label text is usually not fully visible

            viewer.refresh();
        }
    }
}

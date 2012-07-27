package org.omnetpp.simulation.inspectors;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.inspectors.ObjectFieldsViewer.Mode;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPacket;

/**
 * 
 * @author Andras
 */
//TODO cObjects should be double-clickable
//TODO icon buttons to switch mode, ordering, etc.
//TODO resize by border
//TODO drag by label
//TODO adaptive label: display the most useful info that fits in the available space
public class ObjectFieldsInspectorPart extends AbstractSWTInspectorPart {
    private static final Image IMG_MODE_PACKET = SimulationPlugin.getCachedImage("icons/etool16/treemode_packet.png");
    private static final Image IMG_MODE_CHILDREN = SimulationPlugin.getCachedImage("icons/etool16/treemode_children.png");
    private static final Image IMG_MODE_FLAT = SimulationPlugin.getCachedImage("icons/etool16/treemode_flat.png");
    private static final Image IMG_MODE_GROUPED = SimulationPlugin.getCachedImage("icons/etool16/treemode_grouped.png");
    private static final Image IMG_MODE_INHERITANCE = SimulationPlugin.getCachedImage("icons/etool16/treemode_inher.png");
    
    private Composite frame;
    private Label label;
    private ObjectFieldsViewer viewer;

    private ToolItem packetModeTool;  //TODO null these when floating toolbar disappears
    private ToolItem childrenModeTool;
    private ToolItem groupedModeTool;
    private ToolItem inheritaceModeTool;
    private ToolItem flatModeTool;

    
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
	    
        viewer = new ObjectFieldsViewer(frame, SWT.BORDER | SWT.V_SCROLL);
        viewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        viewer.setMode(isSubclassedFromcPacket ? ObjectFieldsViewer.Mode.PACKET : ObjectFieldsViewer.Mode.GROUPED);
        viewer.setInput(object);
	 	
	 	viewer.getTree().addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                inspectorContainer.select(object, true);
            }
        });
	 	
	 	frame.layout();
	 	
		return frame;
	}

	@Override
	public boolean isMaximizable() {
		return false;
	}

	@Override
	public void populateContextMenu(MenuManager contextMenuManager, Point p) {
	}

	@Override
	protected void addIconsToFloatingToolbar(ToolBar toolbar) {
	    packetModeTool = addModeSwitcherTool(toolbar, Mode.PACKET, "Packet mode", IMG_MODE_PACKET);
	    childrenModeTool = addModeSwitcherTool(toolbar, Mode.CHILDREN, "Children mode", IMG_MODE_CHILDREN);
	    groupedModeTool = addModeSwitcherTool(toolbar, Mode.GROUPED, "Grouped mode", IMG_MODE_GROUPED);
	    inheritaceModeTool = addModeSwitcherTool(toolbar, Mode.INHERITANCE, "Inheritance mode", IMG_MODE_INHERITANCE);
	    flatModeTool = addModeSwitcherTool(toolbar, Mode.FLAT, "Flat mode", IMG_MODE_FLAT);
        updateModeSwitcherToolsState();
	}
	
	protected ToolItem addModeSwitcherTool(ToolBar toolbar, final Mode mode, String tooltip, Image img) {
	    ToolItem item = new ToolItem(toolbar, SWT.CHECK);
	    item.setImage(img);
	    item.setToolTipText(tooltip);
	    item.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                viewer.setMode(mode);
                updateModeSwitcherToolsState();
            }
        });
	    return item;
    }

    protected void updateModeSwitcherToolsState() {
        Mode mode = viewer.getMode();
        packetModeTool.setSelection(mode == Mode.PACKET);
        childrenModeTool.setSelection(mode == Mode.CHILDREN);
        groupedModeTool.setSelection(mode == Mode.GROUPED);
        inheritaceModeTool.setSelection(mode == Mode.INHERITANCE);
        flatModeTool.setSelection(mode == Mode.FLAT);
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

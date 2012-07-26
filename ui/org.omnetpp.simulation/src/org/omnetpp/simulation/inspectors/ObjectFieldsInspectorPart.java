package org.omnetpp.simulation.inspectors;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPacket;

/**
 * 
 * @author Andras
 */
//TODO icon buttons to switch mode, ordering, etc.
public class ObjectFieldsInspectorPart extends AbstractSWTInspectorPart {
    private ObjectFieldsViewer viewer;
    
	public ObjectFieldsInspectorPart(IInspectorContainer parent, cObject object) {
		super(parent, object);
	}

	@Override
	protected Control createControl(Composite parent) {
	    if (!object.isFilledIn())
	        object.safeLoad(); // for getClassName() in next line
	    boolean isSubclassedFromcPacket = (object instanceof cPacket) && !object.getClassName().equals("cPacket");
        viewer = new ObjectFieldsViewer(parent, SWT.BORDER | SWT.V_SCROLL);
        viewer.setMode(isSubclassedFromcPacket ? ObjectFieldsViewer.Mode.PACKET : ObjectFieldsViewer.Mode.GROUPED);
        viewer.setInput(object);

	 	viewer.getTree().setSize(300, 200);
	 	
	 	viewer.getTree().addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                inspectorContainer.select(object, true);
            }
        });
		return viewer.getTree();
	}

	@Override
	public boolean isMaximizable() {
		return false;
	}

	@Override
	public void populateContextMenu(MenuManager contextMenuManager, Point p) {
	}

	@Override
	public void refresh() {
		super.refresh();
		viewer.refresh();
	}
}

package org.omnetpp.simulation.inspectors;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPacket;

/**
 * 
 * @author Andras
 */
//TODO cObjects should be double-clickable
//TODO icon buttons to switch mode, ordering, etc.
//TODO add title, resize border, etc.
//TODO adaptive label: display the most useful info that fits in the available space
public class ObjectFieldsInspectorPart extends AbstractSWTInspectorPart {
    private Composite frame;
    private Label label;
    private ObjectFieldsViewer viewer;
    
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

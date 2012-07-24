package org.omnetpp.simulation.inspectors;

import org.apache.commons.lang.StringUtils;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.List;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cObject.Field;

public class ExampleSWTInspectorPart extends AbstractSWTInspectorPart {

	public ExampleSWTInspectorPart(cObject object) {
		super(object);
	}

	@Override
	protected Control createContents(Composite parent) {
		List listbox = new List(parent, SWT.BORDER | SWT.DOUBLE_BUFFERED | SWT.V_SCROLL);
		listbox.setItems("no content yet".split(" ")); //XXX

		Point p = listbox.computeSize(SWT.DEFAULT, SWT.DEFAULT);
	 	figure.setPreferredSize(new Dimension(100, 120)); //XXX
	 	
	 	listbox.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                inspectorContainer.select(object, true); //XXX
            }
        });
		return listbox;
	}

	//@Override
	public boolean isMaximizable() {
		return false;
	}

	//@Override
	public void populateContextMenu(MenuManager contextMenuManager, Point p) {
	}

	@Override
	public void refresh() {
		super.refresh();
		if (!isDisposed()) {
            if (!object.isFilledIn())
                object.safeLoad();
		    if (!object.isFieldsFilledIn())
		        object.safeLoadFields();
		    
			List listbox = (List)getControl();
			listbox.setItems(new String[0]);  // clear listbox

			listbox.add("\"" + object.getFullName() + "\" (" + object.getClassName() + ")");
			
			for (Field field : object.getFields()) {
			    String txt = field.declaredOn + "::" + field.name + " = ";
			    if (field.values == null)
			        txt += field.value;  // prints "null" if value==null
			    else
			        txt += "[ " + StringUtils.join(field.values, ", ") + " ]";
			    txt += " (" + field.type + ")";
			    
                listbox.add(txt);
			}
		}
	}

}

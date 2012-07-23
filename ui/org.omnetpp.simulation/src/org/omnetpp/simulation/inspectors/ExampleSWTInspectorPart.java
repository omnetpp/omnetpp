package org.omnetpp.simulation.inspectors;

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

public class ExampleSWTInspectorPart extends AbstractSWTInspectorPart {

	public ExampleSWTInspectorPart(cObject object) {
		super(object);
	}

	@Override
	protected Control createContents(Composite parent) {
		List listbox = new List(parent, SWT.BORDER | SWT.DOUBLE_BUFFERED);
		listbox.setItems("one two three four five six seven eight nine ten eleven twelve".split(" "));

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
			((List)getControl()).setItem(0, object.getFullPath());
			((List)getControl()).setItem(1, object.getClassName());
			((List)getControl()).setItem(2, object.getInfo());
		}
	}

}

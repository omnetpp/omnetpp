package org.omnetpp.runtimeenv.editors;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.List;
import org.omnetpp.runtime.nativelibs.simkernel.cObject;

public class ExampleSWTInspectorPart extends AbstractSWTInspectorPart {

	public ExampleSWTInspectorPart(cObject object) {
		super(object);
	}

	@Override
	protected Control createContents(Composite parent) {
		List listbox = new List(parent, SWT.BORDER | SWT.DOUBLE_BUFFERED);
		listbox.setItems("one two three four".split(" "));

		Point p = listbox.computeSize(SWT.DEFAULT, SWT.DEFAULT);
	 	figure.setPreferredSize(new Dimension(p.x,p.y)); //XXX
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
			((Button)getControl()).setText(object.toString());
		}
	}

}

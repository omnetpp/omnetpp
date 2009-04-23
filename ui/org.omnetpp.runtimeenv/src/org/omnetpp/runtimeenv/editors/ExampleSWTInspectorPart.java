package org.omnetpp.runtimeenv.editors;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.experimental.simkernel.swig.cObject;

public class ExampleSWTInspectorPart extends SWTInspectorPart {

	public ExampleSWTInspectorPart(cObject object) {
		super(object);
	}

	@Override
	protected Control createContents(Composite parent) {
		parent.setLayout(new FillLayout()); //XXX not here!
		Button button = new Button(parent, SWT.BORDER);
		button.setText(object.toString());
		return button;
	}

	@Override
	public boolean isMaximizable() {
		return false;
	}

	@Override
	public void populateContextMenu(MenuManager contextMenuManager, Point p) {
	}

	@Override
	protected void update() {
		super.update();
		if (!isDisposed()) {
			((Button)getControl()).setText(object.toString());
		}
	}

}

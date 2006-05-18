package org.omnetpp.scave2.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.common.color.ColorFactory;

public class ChartSheetPage extends ScrolledForm {

	public ChartSheetPage(Composite parent, int style) {
		super(parent, style | SWT.V_SCROLL);
		initialize();
	}
	
	private void initialize() {
		setExpandHorizontal(true);
		setBackground(ColorFactory.asColor("white"));
		// TODO: create chart sheet composite
	}
}

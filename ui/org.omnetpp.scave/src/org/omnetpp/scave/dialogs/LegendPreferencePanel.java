package org.omnetpp.scave.dialogs;

import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;

public class LegendPreferencePanel extends Composite {

	public LegendPreferencePanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}

	private void initialize() {
		setSize(new Point(300, 200));
		setLayout(new GridLayout());
	}

}

package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Text;

public class TextAccess extends ControlAccess<Text>
{
	public TextAccess(Text text) {
		super(text);
	}
	
	public Text getText() {
		return widget;
	}

	public void typeIn(String text) {
		// TODO: post events
		getText().setText(text);
	}
}

package org.omnetpp.test.gui.access;

import org.eclipse.swt.custom.StyledText;

public class StyledTextAccess extends CompositeAccess<StyledText>
{
	public StyledTextAccess(StyledText styledText) {
		super(styledText);
	}
	
	public StyledText getStyledText() {
		return widget;
	}

	public String getText() {
		return widget.getText();
	}
}

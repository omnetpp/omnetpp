package org.omnetpp.test.gui.access;

import org.eclipse.swt.custom.StyledText;
import org.omnetpp.test.gui.core.InUIThread;

public class StyledTextAccess extends CompositeAccess<StyledText>
{
	public StyledTextAccess(StyledText styledText) {
		super(styledText);
	}

	public StyledText getStyledText() {
		return widget;
	}

	@InUIThread
	public String getText() {
		return widget.getText();
	}

}

package org.omnetpp.academic;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;

public class GenerateNeddocHandler extends AbstractHandler {

	public Object execute(ExecutionEvent event) throws ExecutionException {
		MessageDialog.openInformation(Display.getCurrent().getActiveShell(), "Generate NED Documentation", 
				"Generating NED documentation is only available in the commercial version of OMNeT++. However, " +
				"we are happy to generate documentation for any open source simulation model. " +
				"Please contact us directly or on the mailing list.\n\n" +
				"The OMNeT++ Team");
		return null;
	}
}

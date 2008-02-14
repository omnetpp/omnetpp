package org.omnetpp.launch;

import org.eclipse.debug.internal.ui.DebugUIPlugin;
import org.eclipse.debug.ui.DebugUITools;
import org.eclipse.debug.ui.ILaunchShortcut;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorPart;

// currently not used (see LaunchShorcut  and LaunchGroup extension point)
public class SimulationShortcut implements ILaunchShortcut {

	public void launch(ISelection selection, String mode) {
		DebugUITools.openLaunchConfigurationDialogOnGroup(DebugUIPlugin.getShell(), (IStructuredSelection)selection, "org.omnetpp.launch.runGroup");
	}

	public void launch(IEditorPart editor, String mode) {
	}
}

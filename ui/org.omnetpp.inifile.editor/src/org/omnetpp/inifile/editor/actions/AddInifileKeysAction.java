package org.omnetpp.inifile.editor.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.inifile.editor.InifileEditorPlugin;

public class AddInifileKeysAction extends Action {
	public AddInifileKeysAction() {
		setText("Add Missing Keys");
		setToolTipText("Add Missing Keys");
		setImageDescriptor(InifileEditorPlugin.getImageDescriptor("icons/launchsim.gif"));
	}

	public void run() {
		IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		IEditorPart iniEditor = page.getActiveEditor();
		IEditorInput iniInput = iniEditor.getEditorInput();
		IFileEditorInput iniFileInput = (IFileEditorInput)iniInput;

		AddInifileKeysDialog dialog = new AddInifileKeysDialog(null, null, null);
		dialog.open();
	}

}

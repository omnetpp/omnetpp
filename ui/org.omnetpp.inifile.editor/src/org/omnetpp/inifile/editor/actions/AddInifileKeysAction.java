package org.omnetpp.inifile.editor.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.editors.InifileEditorData;

public class AddInifileKeysAction extends Action {
	public AddInifileKeysAction() {
		setText("Add Missing Keys");
		setToolTipText("Add Missing Keys");
		setImageDescriptor(InifileEditorPlugin.getImageDescriptor("icons/launchsim.gif"));
	}

	public void run() {
		IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
		IWorkbenchPage page = workbenchWindow.getActivePage();
		IEditorPart editor = page.getActiveEditor();
		if (editor instanceof InifileEditor) {
			InifileEditor e = (InifileEditor) editor;
			InifileEditorData editorData = e.getEditorData();
			AddInifileKeysDialog dialog = new AddInifileKeysDialog(workbenchWindow.getShell(), null, editorData.getInifileAnalyzer());
			dialog.open();
		}
	}

}

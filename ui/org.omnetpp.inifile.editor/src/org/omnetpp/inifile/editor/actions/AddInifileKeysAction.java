package org.omnetpp.inifile.editor.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;

public class AddInifileKeysAction extends Action {
	public AddInifileKeysAction() {
		setText("Add Missing Keys");
		setToolTipText("Add Missing Keys");
		setImageDescriptor(InifileEditorPlugin.getImageDescriptor("icons/genkeys.png"));
	}

	public void run() {
		IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
		IWorkbenchPage page = workbenchWindow.getActivePage();
		IEditorPart editor = page.getActiveEditor();
		if (editor instanceof InifileEditor) {
			InifileEditorData editorData = ((InifileEditor) editor).getEditorData();
			AddInifileKeysDialog dialog = new AddInifileKeysDialog(workbenchWindow.getShell(), editorData.getInifileAnalyzer());
			if (dialog.open()==Dialog.OK) {
				String[] keys = dialog.getKeys();
				IInifileDocument doc = editorData.getInifileDocument();
				String section = dialog.getSection();
				for (String key : keys)
					if (!doc.containsKey(section, key))
						InifileUtils.addEntry(doc, section, key, "", null);
			}
		}
	}

}

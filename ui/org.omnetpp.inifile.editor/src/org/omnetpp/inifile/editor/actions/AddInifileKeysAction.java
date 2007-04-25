package org.omnetpp.inifile.editor.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.editors.InifileSelectionItem;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Add keys for unassigned parameters in the ini file. Brings up a dialog.
 * @author Andras
 */
//XXX add to text editor context menu
public class AddInifileKeysAction extends Action {
	public AddInifileKeysAction() {
		setText("Add Missing Keys");
		setToolTipText("Add Missing Keys");
		setImageDescriptor(InifileEditorPlugin.getImageDescriptor("icons/genkeys.png"));
	}

	public void run() {
		// get active editor
		IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
		IWorkbenchPage page = workbenchWindow.getActivePage();
		IEditorPart editor = page.getActiveEditor();
		if (editor instanceof InifileEditor) {
			// dig out currently selected section in the editor...
			InifileEditor inifileEditor = ((InifileEditor) editor);
			IStructuredSelection editorSelection = (IStructuredSelection) inifileEditor.getSite().getSelectionProvider().getSelection();
			InifileSelectionItem selectionItem = (InifileSelectionItem) editorSelection.getFirstElement();
			String initialSection = selectionItem==null ? null : selectionItem.getSection();
			
			// ...and the inifile analyzer
			InifileEditorData editorData = inifileEditor.getEditorData();
			InifileAnalyzer analyzer = editorData.getInifileAnalyzer();
			
			// open the dialog
			AddInifileKeysDialog dialog = new AddInifileKeysDialog(workbenchWindow.getShell(), analyzer, initialSection);
			if (dialog.open()==Dialog.OK) {
				// add user-selected keys to the document, and also **.apply-default if chosen by the user
				IInifileDocument doc = editorData.getInifileDocument();
				String[] keys = dialog.getKeys();
				String section = dialog.getSection();
				for (String key : keys)
					if (!doc.containsKey(section, key))
						InifileUtils.addEntry(doc, section, key, "", null);
				if (dialog.getAddApplyDefault())
					InifileUtils.addEntry(doc, section, "**.apply-default", "true", null);
			}
		}
	}

}

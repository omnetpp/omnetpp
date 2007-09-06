package org.omnetpp.inifile.editor.actions;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.texteditor.ResourceAction;

import org.omnetpp.inifile.editor.IGotoInifile;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.editors.InifileSelectionItem;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.text.InifileEditorMessages;
import org.omnetpp.inifile.editor.text.actions.InifileTextEditorAction;

/**
 * Add keys for unassigned parameters in the ini file. Brings up a dialog.
 * @author Andras
 */
public class AddInifileKeysAction extends ResourceAction {
    public final static String ID = "AddMissingKeys";

	public AddInifileKeysAction() {
        super(InifileEditorMessages.getResourceBundle(), ID+".");
        setId(ID);
        setActionDefinitionId(InifileTextEditorAction.ACTION_DEFINITION_PREFIX+ID);

	}

	@Override
	public void run() {
		// get active editor
		IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
		IWorkbenchPage page = workbenchWindow.getActivePage();
		IEditorPart editor = page.getActiveEditor();
		if (editor instanceof InifileEditor) {
			// dig out the inifile analyzer
			InifileEditor inifileEditor = ((InifileEditor) editor);
			InifileEditorData editorData = inifileEditor.getEditorData();
			InifileAnalyzer analyzer = editorData.getInifileAnalyzer();
			IInifileDocument doc = editorData.getInifileDocument();

			// does the inifile have sections at all?
			if (doc.getSectionNames().length==0) {
				MessageDialog.openConfirm(workbenchWindow.getShell(), "Empty Ini File",
					"Ini file contains no sections or settings, please choose a NED network first.");
				doc.addSection(GENERAL, null);
				doc.addEntry(GENERAL, CFGID_NETWORK.getKey(), "", "", null);
				inifileEditor.gotoEntry(GENERAL, CFGID_NETWORK.getKey(), IGotoInifile.Mode.AUTO);
				return;
			}

			//XXX what if network name is invalid? check it inside the dialog??

			// determine currently selected section in the editor...
			IStructuredSelection editorSelection = (IStructuredSelection) inifileEditor.getSite().getSelectionProvider().getSelection();
			InifileSelectionItem selectionItem = (InifileSelectionItem) editorSelection.getFirstElement();
			String initialSection = selectionItem==null ? null : selectionItem.getSection();

			// open the dialog
			AddInifileKeysDialog dialog = new AddInifileKeysDialog(workbenchWindow.getShell(), analyzer, initialSection);
			if (dialog.open()==Dialog.OK) {
				// add user-selected keys to the document, and also **.apply-default if chosen by the user
				String[] keys = dialog.getKeys();
				String section = dialog.getSection();
				doc.addEntries(section, keys, null, null, null);
				if (dialog.getAddApplyDefault())
					InifileUtils.addEntry(doc, section, "**.apply-default", "true", "");
			}
		}
	}

}

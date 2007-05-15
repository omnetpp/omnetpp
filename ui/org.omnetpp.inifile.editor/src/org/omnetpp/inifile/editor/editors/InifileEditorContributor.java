package org.omnetpp.inifile.editor.editors;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.ide.IDEActionFactory;
import org.eclipse.ui.part.MultiPageEditorActionBarContributor;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.ITextEditorActionConstants;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.editor.ShowViewAction;
import org.omnetpp.inifile.editor.actions.AddInifileKeysAction;

/**
 * Manages the installation/deinstallation of global actions for multi-page editors.
 * Responsible for the redirection of global actions to the active editor.
 * Multi-page contributor replaces the contributors for the individual editors in the multi-page editor.
 */
public class InifileEditorContributor extends MultiPageEditorActionBarContributor {
	private IEditorPart activeEditorPart;
	private IAction addInifileKeysAction = new AddInifileKeysAction();  
	private IAction showModuleParametersView = new ShowViewAction(IConstants.MODULEPARAMETERS_VIEW_ID); 
	private IAction showModuleHierarchyView = new ShowViewAction(IConstants.MODULEHIERARCHY_VIEW_ID); 

	
	/**
	 * Creates a multi-page contributor.
	 */
	public InifileEditorContributor() {
		super();
	}
	
	/* (non-JavaDoc)
	 * Method declared in AbstractMultiPageEditorActionBarContributor.
	 */
	public void setActivePage(IEditorPart part) {
		if (activeEditorPart == part)
			return;

		activeEditorPart = part;

		IActionBars actionBars = getActionBars();
		ITextEditor textEditor = (part instanceof ITextEditor) ? (ITextEditor) part : null;
		if (actionBars != null && textEditor != null) {
			actionBars.setGlobalActionHandler(
				ActionFactory.DELETE.getId(),
				textEditor.getAction(ITextEditorActionConstants.DELETE));
			actionBars.setGlobalActionHandler(
				ActionFactory.UNDO.getId(),
				textEditor.getAction(ITextEditorActionConstants.UNDO));
			actionBars.setGlobalActionHandler(
				ActionFactory.REDO.getId(),
				textEditor.getAction(ITextEditorActionConstants.REDO));
			actionBars.setGlobalActionHandler(
				ActionFactory.CUT.getId(),
				textEditor.getAction(ITextEditorActionConstants.CUT));
			actionBars.setGlobalActionHandler(
				ActionFactory.COPY.getId(),
				textEditor.getAction(ITextEditorActionConstants.COPY));
			actionBars.setGlobalActionHandler(
				ActionFactory.PASTE.getId(),
				textEditor.getAction(ITextEditorActionConstants.PASTE));
			actionBars.setGlobalActionHandler(
				ActionFactory.SELECT_ALL.getId(),
				textEditor.getAction(ITextEditorActionConstants.SELECT_ALL));
			actionBars.setGlobalActionHandler(
				ActionFactory.FIND.getId(),
				textEditor.getAction(ITextEditorActionConstants.FIND));
			actionBars.setGlobalActionHandler(
				IDEActionFactory.BOOKMARK.getId(),
				textEditor.getAction(IDEActionFactory.BOOKMARK.getId()));
			actionBars.updateActionBars();
		}
	}

	public void contributeToMenu(IMenuManager manager) { //XXX refine...
		IMenuManager menu = new MenuManager("Editor &Menu");
		manager.prependToGroup(IWorkbenchActionConstants.MB_ADDITIONS, menu);
		menu.add(addInifileKeysAction);
		menu.add(showModuleParametersView);
		menu.add(showModuleHierarchyView);
	}

	public void contributeToToolBar(IToolBarManager manager) {
		manager.add(new Separator());
		manager.add(addInifileKeysAction);
		manager.add(showModuleParametersView);
		manager.add(showModuleHierarchyView);
	}
}

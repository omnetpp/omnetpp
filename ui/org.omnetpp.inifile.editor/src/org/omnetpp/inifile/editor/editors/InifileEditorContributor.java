package org.omnetpp.inifile.editor.editors;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.RetargetAction;
import org.eclipse.ui.ide.IDEActionFactory;
import org.eclipse.ui.part.MultiPageEditorActionBarContributor;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.ITextEditorActionConstants;
import org.omnetpp.inifile.editor.actions.AddInifileKeysAction;

/**
 * Manages the installation/deinstallation of global actions for multi-page editors.
 * Responsible for the redirection of global actions to the active editor.
 * Multi-page contributor replaces the contributors for the individual editors in the multi-page editor.
 */
public class InifileEditorContributor extends MultiPageEditorActionBarContributor {
	private IEditorPart activeEditorPart;
	private IAction addInifileKeysAction = new AddInifileKeysAction();
	private RetargetAction undoAction;
	private RetargetAction redoAction;
	//private IAction showModuleParametersView = new ShowViewAction(IConstants.MODULEPARAMETERS_VIEW_ID); 
	//private IAction showModuleHierarchyView = new ShowViewAction(IConstants.MODULEHIERARCHY_VIEW_ID);

	
	/**
	 * Creates a multi-page contributor.
	 */
	public InifileEditorContributor() {
		super();
	}
	
	/* (non-JavaDoc)
	 * Method declared in AbstractMultiPageEditorActionBarContributor.
	 */
	@Override
	public void setActivePage(IEditorPart part) {
		if (activeEditorPart == part)
			return;

		activeEditorPart = part;

		IActionBars actionBars = getActionBars();
		if (actionBars==null)
			return; 

		ITextEditor textEditor = (part instanceof ITextEditor) ? (ITextEditor) part : null;
		if (textEditor != null) {
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
		}
		else {
			// form page selected: activate text editor's undo/redo support here as well
			InifileEditor multipageEditor = (InifileEditor) getPage().getActiveEditor();
			actionBars.setGlobalActionHandler(
					ActionFactory.UNDO.getId(),
					multipageEditor.getTextEditor().getAction(ITextEditorActionConstants.UNDO));
			actionBars.setGlobalActionHandler(
					ActionFactory.REDO.getId(),
					multipageEditor.getTextEditor().getAction(ITextEditorActionConstants.REDO));

			// deactivate the others
			actionBars.setGlobalActionHandler(ActionFactory.DELETE.getId(), null);
			actionBars.setGlobalActionHandler(ActionFactory.CUT.getId(), null);
			actionBars.setGlobalActionHandler(ActionFactory.COPY.getId(), null);
			actionBars.setGlobalActionHandler(ActionFactory.PASTE.getId(), null);
			actionBars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), null);
			actionBars.setGlobalActionHandler(ActionFactory.FIND.getId(), null);
			actionBars.setGlobalActionHandler(IDEActionFactory.BOOKMARK.getId(), null);
		}
		actionBars.updateActionBars();
	}

	@Override
	public void contributeToMenu(IMenuManager manager) {
		IMenuManager editMenu = manager.findMenuUsingPath(IWorkbenchActionConstants.M_EDIT);
		editMenu.appendToGroup(IWorkbenchActionConstants.MB_ADDITIONS, addInifileKeysAction);
	}

	@Override
	public void contributeToToolBar(IToolBarManager manager) {
		manager.add(new Separator());

		undoAction = new RetargetAction(ActionFactory.UNDO.getId(), "Undo");
		redoAction = new RetargetAction(ActionFactory.REDO.getId(), "Redo");

		ISharedImages sharedImages = PlatformUI.getWorkbench().getSharedImages();
    	undoAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_UNDO));
    	undoAction.setDisabledImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_UNDO_DISABLED));
    	redoAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_REDO));
    	redoAction.setDisabledImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_REDO_DISABLED));
    	
    	manager.add(undoAction);
    	manager.add(redoAction);

    	getPage().addPartListener(undoAction);
    	getPage().addPartListener(redoAction);

		manager.add(addInifileKeysAction);
    	
	}
	
    @Override
    public void dispose() {
        getPage().removePartListener(undoAction);
        getPage().removePartListener(redoAction);
        undoAction.dispose();
        redoAction.dispose();
    }
	
}

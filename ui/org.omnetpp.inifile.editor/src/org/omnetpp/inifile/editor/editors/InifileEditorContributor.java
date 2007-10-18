package org.omnetpp.inifile.editor.editors;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.RetargetAction;
import org.eclipse.ui.editors.text.TextEditorActionContributor;
import org.eclipse.ui.part.MultiPageEditorActionBarContributor;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.eclipse.ui.texteditor.RetargetTextEditorAction;

import org.omnetpp.inifile.editor.actions.AddInifileKeysAction;
import org.omnetpp.inifile.editor.text.InifileEditorMessages;
import org.omnetpp.inifile.editor.text.actions.InifileTextEditorAction;
import org.omnetpp.inifile.editor.text.actions.ToggleCommentAction;

/**
 * Manages the installation/deinstallation of global actions for multi-page editors.
 * Responsible for the redirection of global actions to the active editor.
 * Multi-page contributor replaces the contributors for the individual editors in the multi-page editor.
 */
public class InifileEditorContributor extends MultiPageEditorActionBarContributor {
    TextEditorActionContributor textEdContributor = new TextEditorActionContributor();
	private IAction addInifileKeysAction;
	private RetargetAction undoAction;
	private RetargetAction redoAction;
    private RetargetTextEditorAction fContentAssistProposal;
    private RetargetTextEditorAction fToggleCommentAction;
	//private IAction showModuleParametersView = new ShowViewAction(IConstants.MODULEPARAMETERS_VIEW_ID);
	//private IAction showModuleHierarchyView = new ShowViewAction(IConstants.MODULEHIERARCHY_VIEW_ID);

	/**
	 * Creates a multi-page contributor.
	 */
	public InifileEditorContributor() {
		super();
        fContentAssistProposal= new RetargetTextEditorAction(InifileEditorMessages.getResourceBundle(), "ContentAssistProposal."); //$NON-NLS-1$
        fContentAssistProposal.setActionDefinitionId(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);
        fToggleCommentAction = createRetargetAction(ToggleCommentAction.ID);
        addInifileKeysAction =  new AddInifileKeysAction();
	}

	@Override
	public void init(IActionBars bars, IWorkbenchPage page) {
	    super.init(bars, page);
	    textEdContributor.init(bars, page);
	    // start with disabled actions
	    textEdContributor.setActiveEditor(null);
	}

    /* (non-JavaDoc)
	 * Method declared in AbstractMultiPageEditorActionBarContributor.
	 */
	@Override
	public void setActivePage(IEditorPart part) {

		// the rest of actions are enabled only if the active editor is a text editor
		ITextEditor textEditor = (part instanceof ITextEditor) ? (ITextEditor) part : null;
		textEdContributor.setActiveEditor(part);

		// the UNDO/REDO is always redirected to the text editor
		IEditorPart multipageEditor = getPage().getActiveEditor();
		if (multipageEditor != null && multipageEditor instanceof InifileEditor) {
		    setTextEditorGlobalActionHandler(ActionFactory.UNDO.getId(), ((InifileEditor)multipageEditor).getTextEditor());
		    setTextEditorGlobalActionHandler(ActionFactory.REDO.getId(), ((InifileEditor)multipageEditor).getTextEditor());
		}

        fContentAssistProposal.setAction(getAction(textEditor, ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS));
        fToggleCommentAction.setAction(getAction(textEditor, ToggleCommentAction.ID));
		getActionBars().updateActionBars();
	}


	@Override
	public void contributeToMenu(IMenuManager menuManager) {
	    super.contributeToMenu(menuManager);
	    MenuManager sourceMenu = new MenuManager("Source");
	    sourceMenu.add(fToggleCommentAction);
	    sourceMenu.add(new Separator());
	    sourceMenu.add(fContentAssistProposal);
	    sourceMenu.add(new Separator());
	    sourceMenu.add(addInifileKeysAction);
	    menuManager.insertAfter(IWorkbenchActionConstants.M_EDIT, sourceMenu);
	}

	@Override
	public void contributeToToolBar(IToolBarManager manager) {
		manager.add(new Separator());

		// FIXME maybe we could use the default Undo/redo
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

    /**
     * Creates a retargetable action based on the id (name and icon is taken from the resource bundle)
     */
    private static RetargetTextEditorAction createRetargetAction(String id) {
        RetargetTextEditorAction action = new RetargetTextEditorAction(InifileEditorMessages.getResourceBundle(), id+".");
        action.setActionDefinitionId(InifileTextEditorAction.ACTION_DEFINITION_PREFIX + id);
        return action;
    }

    /**
     * Returns the action registered with the given text editor.
     *
     * @param editor the editor, or <code>null</code>
     * @param actionId the action id
     * @return the action, or <code>null</code> if none
     */
    protected final IAction getAction(ITextEditor editor, String actionId) {
        return (editor == null || actionId == null ? null : editor.getAction(actionId));
    }

    /**
     * Sets the globalActionHandler for the given action taken from the provided editor.
     * If the editor is NULL the action handler will be removed
     */
    private void setTextEditorGlobalActionHandler(String id, ITextEditor editor) {
        getActionBars().setGlobalActionHandler(id, getAction(editor, id));
    }

}

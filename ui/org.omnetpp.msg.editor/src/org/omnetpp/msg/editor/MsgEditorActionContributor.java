package org.omnetpp.msg.editor;


import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.editors.text.TextEditorActionContributor;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.RetargetTextEditorAction;

import org.omnetpp.msg.editor.actions.CorrectIndentationAction;
import org.omnetpp.msg.editor.actions.MsgTextEditorAction;
import org.omnetpp.msg.editor.actions.ToggleCommentAction;


/**
 * Contributes MSG editing actions to the desktop's Source menu and the toolbar.
 *
 * @author rhornig
 */
public class MsgEditorActionContributor extends TextEditorActionContributor {
    private RetargetTextEditorAction fToggleCommentAction;
    private RetargetTextEditorAction fCorrectIndentationAction;

	/**
	 * Default constructor.
	 */
	public MsgEditorActionContributor() {
		super();
		fToggleCommentAction = createRetargetAction(ToggleCommentAction.ID);
		fCorrectIndentationAction = createRetargetAction(CorrectIndentationAction.ID);
	}

	private static RetargetTextEditorAction createRetargetAction(String id) {
	    RetargetTextEditorAction action = new RetargetTextEditorAction(MsgEditorMessages.getResourceBundle(), id+".");
	    action.setActionDefinitionId(MsgTextEditorAction.ACTION_DEFINITION_PREFIX + id);
	    return action;
	}

	/*
	 * @see IEditorActionBarContributor#setActiveEditor(IEditorPart)
	 */
	@Override
	public void setActiveEditor(IEditorPart part) {
		super.setActiveEditor(part);

		ITextEditor editor= null;
        if (part instanceof ITextEditor)
            editor= (ITextEditor) part;

        // retarget the actions to the current editor
        fToggleCommentAction.setAction(getAction(editor, ToggleCommentAction.ID));
        fCorrectIndentationAction.setAction(getAction(editor, CorrectIndentationAction.ID));
	}

	@Override
	public void contributeToMenu(IMenuManager menuManager) {
	    super.contributeToMenu(menuManager);
        MenuManager sourceMenu = new MenuManager("Source");
        sourceMenu.add(fToggleCommentAction);
        sourceMenu.add(fCorrectIndentationAction);
        menuManager.insertAfter(IWorkbenchActionConstants.M_EDIT, sourceMenu);
	}

}

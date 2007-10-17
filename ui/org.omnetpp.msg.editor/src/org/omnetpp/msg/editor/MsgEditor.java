package org.omnetpp.msg.editor;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.texteditor.ITextEditorActionConstants;

import org.omnetpp.msg.editor.actions.CorrectIndentationAction;
import org.omnetpp.msg.editor.actions.ToggleCommentAction;



public class MsgEditor extends TextEditor {

    public static final String[] KEY_BINDING_SCOPES = { "org.omnetpp.context.msgEditor" };

    public MsgEditor() {
		super();
        setSourceViewerConfiguration(new MsgSourceViewerConfiguration());
	}
    
    @Override
    protected void createActions() {
        super.createActions();
        
        IAction a = new ToggleCommentAction(this);
        setAction(a.getId(), a);
        markAsSelectionDependentAction(a.getId(), true);

        a = new CorrectIndentationAction(this);
        setAction(a.getId(), a);
        markAsSelectionDependentAction(a.getId(), true);
    }

	@Override
    protected void editorContextMenuAboutToShow(IMenuManager menu) {
        super.editorContextMenuAboutToShow(menu);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, ToggleCommentAction.ID);
        addAction(menu, ITextEditorActionConstants.GROUP_EDIT, CorrectIndentationAction.ID);
    }
	
	@Override
	protected void initializeKeyBindingScopes() {
	    setKeyBindingScopes(KEY_BINDING_SCOPES);
	}
}

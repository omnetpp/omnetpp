package org.omnetpp.msg.editor.actions;

import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.TextEditorAction;

import org.omnetpp.msg.editor.MsgEditorMessages;

public class MsgTextEditorAction extends TextEditorAction {
    public final static String ACTION_DEFINITION_PREFIX = "org.omnetpp.msg.editor.";

    /**
     * Special action that uses the provided id to set the actionDefinitionId, the Id and
     * the label, icon,tooltip, description fields (from the resource bundle) the id is used as a prefix
     * in the property file
     */
    public MsgTextEditorAction(String id, ITextEditor editor) {
        super(MsgEditorMessages.getResourceBundle(), id+".", editor);
        setId(id);
        setActionDefinitionId(ACTION_DEFINITION_PREFIX+id);
    }
}

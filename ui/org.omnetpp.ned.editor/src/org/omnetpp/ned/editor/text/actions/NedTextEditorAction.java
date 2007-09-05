package org.omnetpp.ned.editor.text.actions;

import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.TextEditorAction;

import org.omnetpp.ned.editor.text.NedEditorMessages;

public class NedTextEditorAction extends TextEditorAction {
    public final static String ACTION_DEFINITION_PREFIX = "org.omnetpp.ned.editor.text.";

    /**
     * Special action that uses the provided id to set the actionDefinitionId, the Id and
     * the label, icon,tooltip, description fields (from the resource boundle) the id is used as a prefix
     * in the property file
     */
    public NedTextEditorAction(String id, ITextEditor editor) {
        super(NedEditorMessages.getResourceBundle(), id+".", editor);
        setId(id);
        setActionDefinitionId(ACTION_DEFINITION_PREFIX+id);
    }
}

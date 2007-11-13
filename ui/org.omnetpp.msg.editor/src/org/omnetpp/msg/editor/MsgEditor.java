package org.omnetpp.msg.editor;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.text.IDocument;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.texteditor.ITextEditorActionConstants;
import org.omnetpp.msg.editor.actions.CorrectIndentationAction;
import org.omnetpp.msg.editor.actions.ToggleCommentAction;


/**
 * Editor for message definition files.
 * 
 * @author rhornig
 */
//FIXME update syntax highlighting to the new syntax
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

        // if the file is in the old format, offer upgrading it
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                convertOldMsgfile();
            }
        });

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

    /**
     * Detect when the file is in the old format, and offer converting it.
     */
    protected void convertOldMsgfile() {
        IDocument doc = getDocumentProvider().getDocument(getEditorInput());
        if (MsgTreeUtil.needsConversion(doc.get())) {
            String fileName = getEditorInput().getName();
            if (MessageDialog.openQuestion(getSite().getShell(), "Old Msg File Syntax", 
                    "File \""+fileName+"\" is in the old (3.x) format, and needs to be converted. " +
                    "This includes converting properties to the \"@\" syntax, and removing " +
                    "the fields and properties keywords. " +
            "Do you want to convert the editor contents now?")) {
                String newText = MsgTreeUtil.convertToNewSyntax(doc.get());
                doc.set(newText);
            }
        }
    }

}

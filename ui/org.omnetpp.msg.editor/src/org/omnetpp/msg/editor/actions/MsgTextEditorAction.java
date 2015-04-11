/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.msg.editor.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.TextEditorAction;
import org.omnetpp.msg.editor.MsgEditorMessages;
import org.omnetpp.msg.editor.MsgEditorPlugin;

public abstract class MsgTextEditorAction extends TextEditorAction {
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

    @Override
    public void run() {
        try {
            doRun();
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            MsgEditorPlugin.logError(e);
        }
    }

    /**
     * Gets invoked from run() if the editor is ScaveEditor and selection is
     * an IStructuredSelection -- redefine it to do the real work.
     */
    protected abstract void doRun();
}

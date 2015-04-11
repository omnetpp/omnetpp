/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.TextEditorAction;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.text.InifileEditorMessages;

public abstract class InifileTextEditorAction extends TextEditorAction {
    public final static String ACTION_DEFINITION_PREFIX = "org.omnetpp.inifile.editor.text.";

    /**
     * Special action that uses the provided id to set the actionDefinitionId, the Id and
     * the label, icon,tooltip, description fields (from the resource bundle) the id is used as a prefix
     * in the property file
     */
    public InifileTextEditorAction(String id, ITextEditor editor) {
        super(InifileEditorMessages.getResourceBundle(), id+".", editor);
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
            InifileEditorPlugin.logError(e);
        }
    }

    protected abstract void doRun();
}

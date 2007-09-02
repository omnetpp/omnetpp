package org.omnetpp.ned.editor.text.actions;

import java.util.ResourceBundle;

import org.eclipse.core.resources.IFile;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.texteditor.TextEditorAction;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.ex.NedFileElementEx;

/**
 * Organize "import" lines in a NED file.
 *
 * @author andras
 */
public class OrganizeImportsAction extends TextEditorAction {
    public OrganizeImportsAction(ResourceBundle resourceBundle, String prefix, TextualNedEditor editor) {
        super(resourceBundle, prefix, editor);
    }

    @Override
    public void run() {
        IFile file = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        NedFileElementEx nedFileElement = NEDResourcesPlugin.getNEDResources().getNedFileElement(file);
        //TODO add code here

        ((TextualNedEditor)getTextEditor()).setText(nedFileElement.getNEDSource());
    }

}

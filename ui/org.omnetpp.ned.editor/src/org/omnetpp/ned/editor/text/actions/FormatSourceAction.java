package org.omnetpp.ned.editor.text.actions;

import java.util.ResourceBundle;

import org.eclipse.core.resources.IFile;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.texteditor.TextEditorAction;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;

/**
 * Reformat the NED source code.
 *
 * @author andras
 */
public class FormatSourceAction extends TextEditorAction {
    public FormatSourceAction(ResourceBundle resourceBundle, String prefix, TextualNedEditor editor) {
        super(resourceBundle, prefix, editor);
    }

    @Override
    public void run() {
        IFile ifile = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        INEDElement model = NEDResourcesPlugin.getNEDResources().getNedFileElement(ifile);
        NEDTreeUtil.cleanupPojoTree(model);
        ((TextualNedEditor)getTextEditor()).setText(model.getNEDSource());
    }

}

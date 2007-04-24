package org.omnetpp.ned.editor.text.actions;

import java.util.ResourceBundle;

import org.eclipse.core.resources.IFile;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.texteditor.TextEditorAction;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.NedFileNodeEx;

/**
 * Converts the editors content to new NED version
 * 
 * @author rhornig
 */
public class ConvertToNewFormatAction extends TextEditorAction {
    public ConvertToNewFormatAction(ResourceBundle resourceBundle, String prefix, TextualNedEditor editor) {
        super(resourceBundle, prefix, editor);
    }
    
    @Override
    public void update() {
        IFile ifile = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        NedFileNodeEx nedFileNode = (NedFileNodeEx)NEDResourcesPlugin.getNEDResources().getNEDFileModel(ifile);
        // enable only if the model does not have a syntax error and in V1 format
        setEnabled(nedFileNode != null 
                   && !NEDResourcesPlugin.getNEDResources().containsNEDErrors(ifile) 
                   && !"2".equals(nedFileNode.getVersion()));
    }
    
    @Override
    public void run() {
        IFile ifile = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        NEDElement model = NEDResourcesPlugin.getNEDResources().getNEDFileModel(ifile);
        ((TextualNedEditor)getTextEditor()).setText(NEDTreeUtil.generateNedSource(model, false));
    }

}

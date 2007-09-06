package org.omnetpp.ned.editor.text.actions;

import org.eclipse.core.resources.IFile;
import org.eclipse.ui.part.FileEditorInput;

import org.omnetpp.common.editor.text.TextEditorUtil;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.NedFileElementEx;

/**
 * Converts the editors content to new NED version
 *
 * @author rhornig
 */
public class ConvertToNewFormatAction extends NedTextEditorAction {
    public static final String ID = "ConvertToNewFormat";

    public ConvertToNewFormatAction(TextualNedEditor editor) {
        super(ID, editor);
    }


    @Override
    public void update() {
        IFile ifile = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        NedFileElementEx nedFileNode = NEDResourcesPlugin.getNEDResources().getNedFileElement(ifile);
        // enable only if the model does not have a syntax error and in V1 format
        setEnabled(nedFileNode != null
                   && !nedFileNode.hasSyntaxError()
                   && !"2".equals(nedFileNode.getVersion()));
    }

    @Override
    public void run() {
        IFile ifile = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        INEDElement model = NEDResourcesPlugin.getNEDResources().getNedFileElement(ifile);
        NEDTreeUtil.cleanupPojoTree(model);
        TextEditorUtil.getDocument(getTextEditor()).set(NEDTreeUtil.generateNedSource(model, false));
        TextEditorUtil.resetMarkerAnnotations(getTextEditor());
    }

}

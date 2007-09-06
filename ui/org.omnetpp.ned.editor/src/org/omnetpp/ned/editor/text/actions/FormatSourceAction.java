package org.omnetpp.ned.editor.text.actions;

import org.eclipse.core.resources.IFile;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.NedFileElementEx;

/**
 * Reformat the NED source code.
 *
 * @author andras
 */
public class FormatSourceAction extends NedTextEditorAction {
    public static final String ID = "FormatSource";

    public FormatSourceAction(TextualNedEditor editor) {
        super(ID, editor);
    }

    @Override
	public void update() {
    	setEnabled(getTextEditor() != null && !getNedFileElement().hasSyntaxError());
	}

    @Override
    public void run() {
        IFile ifile = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        INEDElement model = NEDResourcesPlugin.getNEDResources().getNedFileElement(ifile);
        NEDTreeUtil.cleanupPojoTree(model);
        ((TextualNedEditor)getTextEditor()).pullChangesFromNEDResources();
    }

    protected NedFileElementEx getNedFileElement() {
		IFile file = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        return NEDResourcesPlugin.getNEDResources().getNedFileElement(file);
	}

}

package org.omnetpp.ned.editor.text.actions;

import org.eclipse.core.resources.IFile;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.common.editor.text.TextEditorAction;
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
public class FormatSourceAction extends TextEditorAction {
    public static final String ID = "org.omnetpp.ned.editor.text.FormatSource";

    public FormatSourceAction(TextualNedEditor editor) {
        super(editor);
        setId(ID);
        setActionDefinitionId(ID);
        setText("Format Source");
        setDescription("Format the NED source file");
        setToolTipText(getDescription());
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
        ((TextualNedEditor)getTextEditor()).setText(model.getNEDSource());
    }

    protected NedFileElementEx getNedFileElement() {
		IFile file = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        return NEDResourcesPlugin.getNEDResources().getNedFileElement(file);
	}

}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.actions;

import org.eclipse.core.resources.IFile;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.common.editor.text.TextEditorUtil;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.core.refactoring.RefactoringTools;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedTreeUtil;
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
        NedFileElementEx nedFileNode = NedResourcesPlugin.getNedResources().getNedFileElement(ifile);
        // enable only if the model does not have a syntax error and in V1 format
        setEnabled(nedFileNode != null
                   && !nedFileNode.hasSyntaxError()
                   && !"2".equals(nedFileNode.getVersion()));
    }

    @Override
    protected void doRun() {
        IFile ifile = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        INedElement model = NedResourcesPlugin.getNedResources().getNedFileElement(ifile);
        RefactoringTools.cleanupTree(model);
        TextEditorUtil.getDocument(getTextEditor()).set(NedTreeUtil.generateNedSource(model, false));
        TextEditorUtil.resetMarkerAnnotations(getTextEditor());
    }

}

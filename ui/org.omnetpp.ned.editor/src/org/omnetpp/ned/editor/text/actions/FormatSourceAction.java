/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.actions;

import org.eclipse.core.resources.IFile;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.core.refactoring.RefactoringTools;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.INedElement;
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
    protected void doRun() {
        IFile ifile = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        INedElement model = NedResourcesPlugin.getNedResources().getNedFileElement(ifile);
        RefactoringTools.cleanupTree(model);
        ((TextualNedEditor)getTextEditor()).pullChangesFromNedResources();
    }

    protected NedFileElementEx getNedFileElement() {
        IFile file = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        return NedResourcesPlugin.getNedResources().getNedFileElement(file);
    }

}

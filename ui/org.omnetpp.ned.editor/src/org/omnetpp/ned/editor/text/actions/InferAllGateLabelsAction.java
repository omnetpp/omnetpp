/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.actions;

import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.ned.core.refactoring.RefactoringTools;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * @author levy
 */
public class InferAllGateLabelsAction extends NedTextEditorAction {
    public static final String ID = "InferAllGateLabels";

    public InferAllGateLabelsAction(TextualNedEditor editor) {
        super(ID, editor);
    }

    @Override
    protected void doRun() {
        TextualNedEditor textEditor = (TextualNedEditor)getTextEditor();
        ISelection selection = textEditor.getSelectionProvider().getSelection();

        if (selection instanceof ITextSelection) {
            for (INedTypeElement typeElement : textEditor.getModel().getTopLevelTypeNodes())
                for (RefactoringTools.AddGateLabels runnable : RefactoringTools.inferAllGateLabels(typeElement, false))
                    runnable.run();

            textEditor.pullChangesFromNedResources();
        }
    }
}

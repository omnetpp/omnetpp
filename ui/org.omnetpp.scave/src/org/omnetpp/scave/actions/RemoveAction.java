/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.Iterator;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.commands.CompoundCommand;
import org.omnetpp.scave.model.commands.ICommand;
import org.omnetpp.scave.model.commands.RemoveChartCommand;
import org.omnetpp.scave.model.commands.RemoveInputFileCommand;

/**
 * Removes selected elements.
 */
public class RemoveAction extends AbstractScaveAction {
    public RemoveAction() {
        setText("Remove");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ISharedImages.IMG_ELCL_REMOVE));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection structuredSelection) {
        ICommand command = createCommand(structuredSelection);
        scaveEditor.getActiveCommandStack().execute(command);
    }

    @SuppressWarnings("unchecked")
    @Override
    public boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        if (selection.isEmpty() || selection instanceof IDListSelection)
            return false;
        Iterator<Object> elements = selection.iterator();
        while (elements.hasNext()) {
            Object element = elements.next();
            if (!(element instanceof ModelObject))  //TODO || editor.isTemporaryObject((AnalysisObject)element)
                return false;
        }
        return true; // only non-temporary EObjects selected
    }

    /**
     * Creates a command that removes the selected objects.
     *
     * Simple DeleteCommand.create(ed, selection.toList()) does not work,
     * because it would delete the referenced node when the references are
     * displayed under their parent (Edit.Children=true).
     *
     * The solution is to wrap the reference values
     * (overriding isWrappingNeeded() in the ItemProvider) and execute
     * RemoveCommand on them (instead of DeleteCommand).
     *
     * TODO: fix EditingDomainActionBarContributor.deleteAction too.
     */
    private ICommand createCommand(IStructuredSelection selection) {
        CompoundCommand command = new CompoundCommand("Remove");

        for (Iterator<?> i = selection.iterator(); i.hasNext();) {
            Object object = i.next();
            if (object instanceof AnalysisItem)
                command.append(new RemoveChartCommand((AnalysisItem)object));
            else if (object instanceof InputFile)
                command.append(new RemoveInputFileCommand((InputFile)object));
        }
        return command;
    }
}

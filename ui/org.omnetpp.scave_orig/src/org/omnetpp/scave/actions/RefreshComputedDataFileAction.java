/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.concurrent.Callable;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model2.ComputedResultFileUpdater;

/**
 * TODO
 *
 * @author Tomi
 */
public class RefreshComputedDataFileAction extends AbstractScaveAction {

    public RefreshComputedDataFileAction() {
        setText("Refresh Computed Data");
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        if (selection != null && selection.getFirstElement() instanceof ProcessingOp)
        {
            final ProcessingOp selected = (ProcessingOp)selection.getFirstElement();
            selected.setComputationHash(0);
            final ResultFileManager manager = scaveEditor.getResultFileManager();
            ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                public Object call() {
                    ComputedResultFileUpdater.instance().ensureComputedFile(selected, manager, null);
                    return null;
                }
            });
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return  selection.getFirstElement() instanceof ProcessingOp;
    }
}


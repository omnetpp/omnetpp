/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Clears internal caches that speed up result queries.
 */
public class ClearCachesAction extends AbstractScaveAction {

    public ClearCachesAction() {
        setText("Clear Caches (development-only)");
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        scaveEditor.getFilterCache().clear();
        scaveEditor.getMemoizationCache().clear();
    }

    @Override
    public boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Refreshes the result files (only reloads already loaded files if
 * they have changed on the disk).
 *
 * @author andras
 */
public class RefreshResultFilesAction extends AbstractScaveAction {

    public RefreshResultFilesAction() {
        setText("Refresh");
        setDescription("Refresh results files");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_REFRESHFILES));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        scaveEditor.refreshResultFiles();
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}

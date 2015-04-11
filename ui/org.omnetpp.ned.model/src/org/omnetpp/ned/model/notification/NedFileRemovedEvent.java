/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.notification;

import org.eclipse.core.resources.IFile;

/**
 * Generated when a NED file gets removed from NEDResources. The cause might
 * be any of the following:
 *  - project closed
 *  - ".nedfolders" file changed, and file is now outside all NED Source Folders
 *  - project is no longer an OMNeT++ project
 *  - file deleted or moved
 *
 * Editors that edit the given file are expected to close.
 *
 * This notification exists because not all of the above cases generate a
 * resource change notification about the file itself.
 *
 * @author andras
 */
public class NedFileRemovedEvent extends NedModelChangeEvent {
    protected IFile file;

    public NedFileRemovedEvent(IFile file) {
        super(null);
        this.file = file;
    }

    /**
     * Returns the file that was removed from NEDResources.
     */
    public IFile getFile() {
        return file;
    }

    @Override
    public String toString() {
        return super.toString()+" FILE REMOVED: "+file.getFullPath();
    }
}

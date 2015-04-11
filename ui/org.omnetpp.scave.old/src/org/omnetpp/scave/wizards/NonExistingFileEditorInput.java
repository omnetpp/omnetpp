/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.wizards;

import org.eclipse.core.resources.IFile;
import org.eclipse.ui.IPersistableElement;
import org.eclipse.ui.part.FileEditorInput;

/**
 * Represents newly created editor inputs that haven't been saved yet
 * ("Untitled 3.sdataset").
 */
// FIXME use NullEditorInput instead
public class NonExistingFileEditorInput extends FileEditorInput {
    public NonExistingFileEditorInput(IFile file) {
        super(file);
    }

    /**
     * We don't want to be persisted. (After restart, it'd pop up as normal
     * FileEditorInput, resulting in a "file does not exist" error.
     */
    public IPersistableElement getPersistable() {
        return null;
    }
}

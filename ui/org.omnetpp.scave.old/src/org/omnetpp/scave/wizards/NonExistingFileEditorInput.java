/*******************************************************************************
 * Copyright (c) 2000, 2005 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
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

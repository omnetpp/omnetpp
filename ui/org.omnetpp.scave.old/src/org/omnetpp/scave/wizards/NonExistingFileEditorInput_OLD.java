/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.wizards;

import java.io.File;

import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;

import org.eclipse.jface.resource.ImageDescriptor;

import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;
import org.eclipse.ui.editors.text.ILocationProvider;

/**
 * @since 3.1
 */
// FIXME delete me
public class NonExistingFileEditorInput_OLD implements IEditorInput, ILocationProvider {

    private static int fgNonExisting= 0;

    private File fFile;
    private String fName;

    public NonExistingFileEditorInput_OLD(File file, String namePrefix) {
        super();
        fFile= file;
        ++fgNonExisting;
        fName= namePrefix + " " + fgNonExisting; //$NON-NLS-1$
    }

    /*
     * @see org.eclipse.ui.IEditorInput#exists()
     */
    public boolean exists() {
        return false;
    }

    /*
     * @see org.eclipse.ui.IEditorInput#getImageDescriptor()
     */
    public ImageDescriptor getImageDescriptor() {
        return null;
    }

    /*
     * @see org.eclipse.ui.IEditorInput#getName()
     */
    public String getName() {
        return fName;
    }

    /*
     * @see org.eclipse.ui.IEditorInput#getPersistable()
     */
    public IPersistableElement getPersistable() {
        return null;
    }

    /*
     * @see org.eclipse.ui.IEditorInput#getToolTipText()
     */
    public String getToolTipText() {
        return fName;
    }

    /*
     * @see org.eclipse.core.runtime.IAdaptable#getAdapter(java.lang.Class)
     */
    public Object getAdapter(Class adapter) {
        if (ILocationProvider.class.equals(adapter))
            return this;
        return Platform.getAdapterManager().getAdapter(this, adapter);
    }

    /*
     * @see org.eclipse.ui.editors.text.ILocationProvider#getPath(java.lang.Object)
     */
    public IPath getPath(Object element) {
        if (element instanceof NonExistingFileEditorInput_OLD) {
            NonExistingFileEditorInput_OLD input= (NonExistingFileEditorInput_OLD) element;
            return Path.fromOSString(input.fFile.getAbsolutePath());
        }
        return null;
    }

    /*
     * @see java.lang.Object#equals(java.lang.Object)
     */
    public boolean equals(Object o) {
        if (o == this)
            return true;

        if (o instanceof NonExistingFileEditorInput_OLD) {
            NonExistingFileEditorInput_OLD input = (NonExistingFileEditorInput_OLD) o;
            return fFile.equals(input.fFile);
        }

        return false;
    }

    /*
     * @see java.lang.Object#hashCode()
     */
    public int hashCode() {
        return fFile.hashCode();
    }
}

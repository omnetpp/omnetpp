/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.wizards;

import java.io.File;

import org.eclipse.core.runtime.IPath;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.IEditorDescriptor;
import org.eclipse.ui.IEditorRegistry;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.editors.text.EditorsUI;

import org.omnetpp.scave.plugin.ScavePlugin;

/**
 * Wizard with no page that creates the untitled text file
 * and opens the text editor.
 *
 * FIXME this doesn't work -- remove
 */
public class UntitledTextFileWizard extends Wizard implements INewWizard {

    private IWorkbenchWindow fWindow;

    public UntitledTextFileWizard() {
    }

    /*
     * @see org.eclipse.ui.IWorkbenchWindowActionDelegate#dispose()
     */
    public void dispose() {
        fWindow= null;
    }

    private File queryFile() {
        IPath stateLocation= ScavePlugin.getDefault().getStateLocation();
        IPath path= stateLocation.append("/_" + new Object().hashCode()+".sdataset"); //$NON-NLS-1$ //$NON-NLS-2$
        return new File(path.toOSString());
    }

    private String getEditorId(File file) {
        IWorkbench workbench= fWindow.getWorkbench();
        IEditorRegistry editorRegistry= workbench.getEditorRegistry();
        IEditorDescriptor descriptor= editorRegistry.getDefaultEditor(file.getName());
        if (descriptor != null)
            return descriptor.getId();
        return EditorsUI.DEFAULT_TEXT_EDITOR_ID;
    }

//  private IEditorInput createEditorInput(File file) {
//      return new NonExistingFileEditorInput(file, "UnTiTlEd");
//  }

    /*
     * @see org.eclipse.jface.wizard.Wizard#performFinish()
     */
    public boolean performFinish() {
//      File file= queryFile();
//      System.out.println("File: "+file.getAbsolutePath());
//      IEditorInput input = createEditorInput(file);
//      System.out.println("Input: "+input.getName()+ ", "+input.getToolTipText());
//      String editorId = getEditorId(file);
//      IWorkbenchPage page = fWindow.getActivePage();
//      try {
//          page.openEditor(input, editorId);
//      } catch (PartInitException e) {
//          e.printStackTrace();  // TODO like this: EditorsPlugin.log(e);
//          return false;
//      }
        return true;
    }

    /*
     * @see org.eclipse.ui.IWorkbenchWizard#init(org.eclipse.ui.IWorkbench, org.eclipse.jface.viewers.IStructuredSelection)
     */
    public void init(IWorkbench workbench, IStructuredSelection selection) {
        // FIXME ***HERE*** is the selection!!!!
        System.out.println("SELECTION IS: ("+
                selection.getFirstElement().getClass().getCanonicalName()+") "
                +selection.getFirstElement().toString());
        // --> IFile (should be!)
        fWindow= workbench.getActiveWorkbenchWindow();
    }
}

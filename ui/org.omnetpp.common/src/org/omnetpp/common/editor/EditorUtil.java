package org.omnetpp.common.editor;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IEditorDescriptor;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.FileEditorInput;

/**
 * Utility methods for managing editors
 * 
 * @author rhornig
 */
public class EditorUtil {
    /**
     * Convenience function for opening editors in the workbench.
     * If there is no editor registered for this file type, displays
     * an error dialog and returns null.
     * 
     * See also IWorkbenchPage.
     * 
     * @return the editor, or null
     */
    public static IEditorPart openEditor(IFile file, boolean activate) throws PartInitException {
        IEditorDescriptor edesc = PlatformUI.getWorkbench().getEditorRegistry().getDefaultEditor(file.getName());
        if (edesc==null) {
        	MessageDialog.openError(PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell(), "Error", 
        			"Cannot open "+file.toString()+": no editor registered for this file type.");
        	return null;
        }

    	IWorkbenchPage activePage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		return activePage.openEditor(new FileEditorInput(file), edesc.getId(), activate);
    }
}

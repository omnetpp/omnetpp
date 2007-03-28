package org.omnetpp.common.editor;

import org.eclipse.core.resources.IFile;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.FileEditorInput;

/**
 * Utility methods for managing editors
 * @author rhornig
 */
public class EditorUtil {

    /**
     * Convenience function for opening editors in the workbench. See also IWorkbenchPage.
     */
    public static IEditorPart openEditor(IFile file, String editorID) throws PartInitException {
    	IWorkbenchPage activePage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		return activePage.openEditor(new FileEditorInput(file), editorID);
    }

    /**
     * Convenience function for opening editors in the workbench. See also IWorkbenchPage.
     */
    public static IEditorPart openEditor(IFile file, String editorID, boolean activate) throws PartInitException {
    	IWorkbenchPage activePage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		return activePage.openEditor(new FileEditorInput(file), editorID, activate);
    }

    /**
     * Convenience function for opening editors in the workbench. See also IWorkbenchPage.
     */
    public static IEditorPart openEditor(IFile file, String editorID, boolean activate, int matchFlags) throws PartInitException {
    	IWorkbenchPage activePage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		return activePage.openEditor(new FileEditorInput(file), editorID, activate, matchFlags);
    }
}

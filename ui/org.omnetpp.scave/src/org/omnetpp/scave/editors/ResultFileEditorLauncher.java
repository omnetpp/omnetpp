package org.omnetpp.scave.editors;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.ui.IEditorLauncher;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.omnetpp.scave.Activator;
import org.omnetpp.scave.wizard.ScaveModelWizard;

/**
 * This launcher associated with scalar and vector files.
 * It actually opens the analysis file associated with the result file.
 * If the analysis file not yet exists, it opens the "New Analysis" wizard
 * to create it.
 * 
 * @author tomi
 */
public class ResultFileEditorLauncher implements IEditorLauncher {

	/**
	 * Opens the analysis file belongs to <code>resultFile</code>.
	 * The result file is specified by its OS path.
	 */
	public void open(IPath resultFile) {
		IWorkbench workbench = PlatformUI.getWorkbench();
		IWorkbenchWindow workbenchWindow = workbench.getActiveWorkbenchWindow();
		IWorkbenchPage page = workbenchWindow.getActivePage();
		try {
			IFile file = getAnalysisFileForResultFile(resultFile);
			if (file != null)
				IDE.openEditor(page, file);
		} catch (CoreException e) {
			Activator.logError(e);
		}
	}
	
	/**
	 * Returns the analysis file belongs to <code>resultFile</code>.
	 * If the analysis file does not exists, a "New Analysis" wizard
	 * is opened. It returns <code>null</code>, when the user canceled
	 * the dialog. 
	 */
	private IFile getAnalysisFileForResultFile(IPath path) throws CoreException {
		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		IFile[] resultFiles = root.findFilesForLocation(path);

		if (resultFiles.length > 0)
		{
			IFile resultFile = resultFiles[0];
			IFile analysisFile = root.getFile(resultFile.getFullPath().addFileExtension("scave"));
			if (!analysisFile.getLocation().toFile().exists()) {
				if(openNewAnalysisWizard(resultFile))
					return analysisFile;
				else
					return null;
			}
			return analysisFile;
		}
		else
			return null;
	}
	
	/**
	 * Opens the "New Analysis" wizard. The new analysis file will
	 * contain the specified <code>resultFile</code>.
	 * 
	 * @param resultFile the initial content of the analysis file
	 * @return true iff the file is created
	 */
	private boolean openNewAnalysisWizard(IFile resultFile) {
		IWorkbench workbench = PlatformUI.getWorkbench();
		IWorkbenchWindow workbenchWindow = workbench.getActiveWorkbenchWindow();
		IStructuredSelection selection = new StructuredSelection(resultFile); 
		ScaveModelWizard wizard = new ScaveModelWizard();
		wizard.init(workbench, selection);
		wizard.setInitialInputFile(resultFile);
		
		WizardDialog dialog = new WizardDialog(workbenchWindow.getShell(), wizard);
		return dialog.open() == Window.OK;
	}
}

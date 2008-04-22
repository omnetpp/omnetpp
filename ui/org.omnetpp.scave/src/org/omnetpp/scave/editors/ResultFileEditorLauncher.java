package org.omnetpp.scave.editors;

import java.util.regex.Pattern;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.MessageDialog;
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
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.wizard.ScaveModelWizard;

/**
 * This launcher associated with scalar and vector files.
 * It actually opens the analysis file associated with the result file.
 * If the analysis file not yet exists, it opens the "New Analysis" wizard
 * to create it.
 * 
 * This launcher currently also opens for vci files.
 * 
 * @author tomi
 */
public class ResultFileEditorLauncher implements IEditorLauncher {

	/**
	 * Opens the analysis file that belongs to <code>resultFile</code>.
	 * The result file is specified by its OS path.
	 */
	public void open(IPath resultFile) {
		IWorkbench workbench = PlatformUI.getWorkbench();
		IWorkbenchWindow workbenchWindow = workbench.getActiveWorkbenchWindow();
		IWorkbenchPage page = workbenchWindow.getActivePage();
		try {
			if (resultFile.getFileExtension().equals("vci")) {
				MessageDialog.openInformation(workbenchWindow.getShell(), "Vector Index File", 
						"This file ("+resultFile.toString()+") is an index file for an output vector file (.vec), "+
						"and cannot be opened by itself. Please open the corresponding .vec file.");
				return;
			}
			else {
				IFile file = getAnalysisFileForResultFile(resultFile);
				if (file != null)
					IDE.openEditor(page, file);
			}
		} catch (CoreException e) {
			ScavePlugin.logError(e);
		}
	}
	
	/**
	 * Returns the analysis file belongs to <code>resultFile</code>.
	 * If the analysis file does not exists, a "New Analysis" wizard
	 * is opened. It returns <code>null</code>, when the user canceled
	 * the dialog. 
	 */
	private IFile getAnalysisFileForResultFile(IPath path) throws CoreException {
		// ignore files which are not "vec" or "sca"
		if (!"vec".equals(path.getFileExtension()) && !"sca".equals(path.getFileExtension()))
			return null; 
		
		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		IFile[] resultFiles = root.findFilesForLocation(path);

		if (resultFiles.length > 0)
		{
			IFile resultFile = resultFiles[0];
			IPath baseName = resultFile.getFullPath().removeFileExtension();

			IContainer container = resultFile.getParent() instanceof IProject ? resultFile.getParent() : resultFile.getParent().getParent(); 
			IPath analysisFileName = new Path(suffixPattern.matcher(baseName.lastSegment()).replaceFirst("")).addFileExtension("anf"); 
			IFile analysisFile = container.getFile(analysisFileName);
			
			if (analysisFile.getLocation().toFile().exists()) {
				return analysisFile; // return existing
			}
			else {
				String fileName = suffixPattern.matcher(baseName.toString()).replaceFirst("-*");  // convert numeric suffices to *
				openNewAnalysisWizard(analysisFile, new String[] {fileName+".vec", fileName+".sca"}); 
				return null; // wizard opens the editor, too, so we don't need to
			}
		}
		else
			return null;
	}
	
	private static final Pattern suffixPattern = Pattern.compile("-[0-9]+$");
	
	/**
	 * Opens the "New Analysis" wizard. The new analysis file will
	 * contain the specified <code>resultFile</code>.
	 * 
	 * @param resultFile the initial content of the analysis file
	 * @return true iff the file is created
	 */
	private boolean openNewAnalysisWizard(IFile analysisFile, String[] initialInputFiles) {
		IWorkbench workbench = PlatformUI.getWorkbench();
		IWorkbenchWindow workbenchWindow = workbench.getActiveWorkbenchWindow();
		IStructuredSelection selection = new StructuredSelection(analysisFile); 
		ScaveModelWizard wizard = new ScaveModelWizard();
		wizard.init(workbench, selection);
		wizard.setDefaultAnalysisFileName(analysisFile.getName());
		wizard.setInitialInputFiles(initialInputFiles);
		
		WizardDialog dialog = new WizardDialog(workbenchWindow.getShell(), wizard);
		return dialog.open() == Window.OK;
	}
}

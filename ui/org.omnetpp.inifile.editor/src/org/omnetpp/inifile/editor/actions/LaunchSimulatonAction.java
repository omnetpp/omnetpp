package org.omnetpp.inifile.editor.actions;

import java.io.ByteArrayInputStream;
import java.io.InputStream;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.inifile.editor.InifileEditorPlugin;

public class LaunchSimulatonAction extends Action {
	private static final String LAUNCH_EDITOR_ID = "org.omnetpp.experimental.animation.editors.AnimationEditor";

	public LaunchSimulatonAction() {
		setText("Launch simulation");
		setToolTipText("Launch simulation"); //XXX refine
		setImageDescriptor(InifileEditorPlugin.getImageDescriptor("icons/launchsim.gif"));
	}

	public void run() {
		IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		IEditorPart iniEditor = page.getActiveEditor();
		IEditorInput iniInput = iniEditor.getEditorInput();
		IFileEditorInput iniFileInput = (IFileEditorInput)iniInput;

		IFile iniFile = iniFileInput.getFile();
		IPath location = iniFile.getLocation().removeFileExtension().addFileExtension("launch");
		IFile launchFile = ResourcesPlugin.getWorkspace().getRoot().getFileForLocation(location);

		try {
			if (!launchFile.exists()) {
				String contents = "inifile "+iniFile.getName()+"\n";
				InputStream bis = new ByteArrayInputStream(contents.getBytes());
				launchFile.create(bis, false, null);
			}

			IEditorInput launchInput = new FileEditorInput(launchFile);
			page.openEditor(launchInput, LAUNCH_EDITOR_ID);

		} catch (PartInitException e) {
			MessageDialog.openError(null, "Ini-file Editor", "Cannot launch simulation: "+e.getMessage());
			e.printStackTrace(); //XXX
		} catch (CoreException e) {
			MessageDialog.openError(null, "Ini-file Editor", "Cannot launch simulation: "+e.getMessage());
			e.printStackTrace(); //XXX
		} 

	}

}

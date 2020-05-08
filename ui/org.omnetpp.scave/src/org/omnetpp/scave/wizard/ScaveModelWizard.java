/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.wizard;


import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.actions.WorkspaceModifyOperation;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.part.ISetSelectionTarget;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model.Analysis;


/**
 * This is a simple wizard for creating a new analysis file.
 * Code is based on the EMF-created wizard.
 *
 * The new analysis file may be empty or may contain
 * a specified input file.
 */
//XXX uses property file from the model package
public class ScaveModelWizard extends Wizard implements INewWizard {
    private static final String FILENAME_EXTENSION = "anf";

    /**
     * This is the file creation page.
     */
    protected NewFileCreationPage newFileCreationPage;

    /**
     * Remember the selection during initialization for populating the default container.
     */
    protected IStructuredSelection selection;

    /**
     * Remember the workbench during initialization.
     */
    protected IWorkbench workbench;

    /**
     * The analysis file name offered by default.
     */
    protected String defaultAnalysisFileName;

    /**
     * Result files that the new analysis file should contain.
     */
    protected String[] initialInputFiles;

    /**
     * This just records the information.
     */
    public void init(IWorkbench workbench, IStructuredSelection selection) {
        this.workbench = workbench;
        this.selection = selection;
        setWindowTitle("New Analysis File");
        setDefaultPageImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_WIZBAN_NEWSCAVEMODEL));
    }

    public void setInitialInputFiles(String[] initialInputFiles) {
        this.initialInputFiles = initialInputFiles;
    }

    /**
     * The framework calls this to create the contents of the wizard.
     */
    @Override
    public void addPages() {
        // Create a page, set the title, and the initial model file name.
        //
        newFileCreationPage = new NewFileCreationPage("Whatever", selection);
        newFileCreationPage.setTitle("New Analysis File");
        newFileCreationPage.setDescription("Create New Analysis File");
        newFileCreationPage.setFileName(defaultAnalysisFileName==null ? "new."+FILENAME_EXTENSION : defaultAnalysisFileName);
        newFileCreationPage.setFileExtension(FILENAME_EXTENSION);
        addPage(newFileCreationPage);

        // Try and get the resource selection to determine a current directory for the file dialog.
        if (selection != null && !selection.isEmpty()) {
            // Get the resource...
            Object selectedElement = selection.iterator().next();
            if (selectedElement instanceof IResource) {
                // Get the resource parent, if it's a file.
                IResource selectedResource = (IResource)selectedElement;
                if (selectedResource.getType() == IResource.FILE)
                    selectedResource = selectedResource.getParent();

                // This gives us a directory...
                if (selectedResource instanceof IFolder || selectedResource instanceof IProject) {
                    // Set this for the container.
                    newFileCreationPage.setContainerFullPath(selectedResource.getFullPath());

                    // Make up a unique new name here.
                    String defaultModelBaseFilename = defaultAnalysisFileName!=null ? defaultAnalysisFileName.replaceFirst("\\."+FILENAME_EXTENSION+"$", "") : "new";
                    String defaultModelFilenameExtension = FILENAME_EXTENSION;
                    String modelFilename = defaultModelBaseFilename + "." + defaultModelFilenameExtension;
                    for (int i = 1; ((IContainer)selectedResource).findMember(modelFilename) != null; ++i) {
                        modelFilename = defaultModelBaseFilename + i + "." + defaultModelFilenameExtension;
                    }
                    newFileCreationPage.setFileName(modelFilename);
                }
            }
        }
    }

    public void setDefaultAnalysisFileName(String defaultAnalysisFileName) {
        this.defaultAnalysisFileName = defaultAnalysisFileName;
    }

    /**
     * Get the file from the page.
     */
    public IFile getAnalysisFile() {
        return newFileCreationPage.getAnalysisFile();
    }

    /**
     * Create a new analysis file.
     */
    protected Analysis createAnalysisNode() {
        return ScaveWizardUtil.createAnalysisNode(initialInputFiles);
    }

    /**
     * Do the work after everything is specified.
     */
    @Override
    public boolean performFinish() {
        try {
            // Remember the file.
            final IFile modelFile = getAnalysisFile();

            // Do the work within an operation.
            WorkspaceModifyOperation operation =
                new WorkspaceModifyOperation() {
                    @Override
                    protected void execute(IProgressMonitor progressMonitor) {
                        try {
                            Analysis rootObject = createAnalysisNode();
                            ScaveWizardUtil.saveAnfFile(modelFile, rootObject);
                        }
                        catch (Exception exception) {
                            ScavePlugin.logError(exception);
                        }
                        finally {
                            progressMonitor.done();
                        }
                    }
                };

            getContainer().run(false, false, operation);

            // Select the new file resource in the current view.
            IWorkbenchWindow workbenchWindow = workbench.getActiveWorkbenchWindow();
            IWorkbenchPage page = workbenchWindow.getActivePage();
            final IWorkbenchPart activePart = page.getActivePart();
            if (activePart instanceof ISetSelectionTarget) {
                final ISelection targetSelection = new StructuredSelection(modelFile);
                getShell().getDisplay().asyncExec(() -> {
                    ((ISetSelectionTarget)activePart).selectReveal(targetSelection);
                });
            }

            // Open an editor on the new file.
            try {
                IDE.openEditor(page, modelFile);
            }
            catch (PartInitException exception) {
                MessageDialog.openError(workbenchWindow.getShell(), "Cannot open Analysis editor", exception.getMessage());
                return false;
            }

            return true;
        }
        catch (Exception exception) {
            ScavePlugin.logError(exception);
            return false;
        }
    }

    /**
     * This is the one page of the wizard.
     */
    public static class NewFileCreationPage extends WizardNewFileCreationPage {
        /**
         * Pass in the selection.
         */
        public NewFileCreationPage(String pageId, IStructuredSelection selection) {
            super(pageId, selection);
        }

        /**
         * The framework calls this to see if the file is correct.
         */
        @Override
        protected boolean validatePage() {
            if (super.validatePage()) {
                // Make sure the file ends in ".anf".
                String requiredExt = FILENAME_EXTENSION;
                String enteredExt = new Path(getFileName()).getFileExtension();
                if (enteredExt == null || !enteredExt.equals(requiredExt)) {
                    setErrorMessage("File must have the '" + requiredExt + "' extension");
                    return false;
                } else
                    return true;
            } else
                return false;
        }

        /**
         *
         */
        public IFile getAnalysisFile() {
            return ResourcesPlugin.getWorkspace().getRoot().getFile(getContainerFullPath().append(getFileName()));
        }
    }
}

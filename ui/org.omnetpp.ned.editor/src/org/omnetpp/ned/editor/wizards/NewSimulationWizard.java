package org.omnetpp.ned.editor.wizards;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.dialogs.ListSelectionDialog;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateBasedWizard;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;

/**
 * "New Simulation" wizard.
 *
 * @author Andras
 */
public class NewSimulationWizard extends TemplateBasedWizard  implements INewWizard {

    private IWorkbench workbench;
    private IStructuredSelection selection;
    private WizardNewFileCreationPage firstPage;

    public NewSimulationWizard() {
        setWizardType("simulation");
    }

    public void init(IWorkbench workbench, IStructuredSelection currentSelection) {
        this.workbench = workbench;
        this.selection = currentSelection;
    }

    public WizardNewFileCreationPage getFirstPage() {
        return firstPage;
    }

    @Override
    public void addPages() {
        setWindowTitle(isImporting() ? "Import Simulation" : "New Simulation");
        firstPage = new WizardNewFileCreationPage("folder selection page", selection) {
            @Override
            protected String getNewFileLabel() {
                return "Name for new f&older:";
            }
        };
        firstPage.setAllowExistingResources(false);
        firstPage.setTitle(isImporting() ? "Import Simulation" : "New Simulation");
        firstPage.setDescription("Choose folder for simulation files");
        firstPage.setFileExtension("");
        firstPage.setFileName("");
        addPage(firstPage);
        super.addPages();
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);

        String simulationFolderName = folder.getName();
        context.getVariables().put("simulationFolderName", simulationFolderName);
        
        String simulationName = StringUtils.capitalize(StringUtils.makeValidIdentifier(simulationFolderName));
        context.getVariables().put("simulationName", simulationName);

        // variables to help support project, simulation and file wizards with the same template code. 
        // Intention: targetTypeName and targetMainFile are the NED type and file of "the network" in this simulation.
        context.setVariableIfMissing("targetTypeName", "${simulationName}"); // may be customized on further pages
        context.setVariableIfMissing("targetMainFile", "${targetTypeName}.ned"); // let targetTypeName be edited on pages 

        // NED-related stuff
        IFile someNedFile = folder.getFile(new Path("whatever.ned"));
        String packageName = NEDResourcesPlugin.getNEDResources().getExpectedPackageFor(someNedFile);
        context.getVariables().put("nedPackageName", StringUtils.defaultString(packageName,""));

        // namespace
        String namespaceName = NEDResourcesPlugin.getNEDResources().getSimplePropertyFor(folder, INEDTypeResolver.NAMESPACE_PROPERTY);
        context.getVariables().put("namespaceName", StringUtils.defaultString(namespaceName,""));

        return context;
    }

    @Override
    public boolean performFinish() {
        // create folder before invoking the template
        IFolder simulationFolder = getFolder();
        try {
            simulationFolder.create(false, true, null);
        }
        catch (CoreException e1) {
            NedEditorPlugin.logError(e1);
            MessageDialog.openError(getShell(), "Problem", "Cannot create simulation folder " + simulationFolder.getFullPath().toString() + ": " + e1.getMessage());
            return false;
        }

        // do it
        boolean ok = super.performFinish();
        if (!ok)
            return false;

        // offer user to open the files
        List<IFile> files = new ArrayList<IFile>();
        List<IFile> nedFiles = new ArrayList<IFile>();
        try {
            for (IResource resource : simulationFolder.members()) {
                if (resource instanceof IFile) {
                    files.add((IFile)resource);
                    if ("ned".equals(resource.getFileExtension()))
                        nedFiles.add((IFile)resource);
                }
            }
        }
        catch (CoreException e) {
            CommonPlugin.logError(e);
        }
        ListSelectionDialog dialog = new ListSelectionDialog(getShell(), files,
                new ArrayContentProvider(), new WorkbenchLabelProvider(),
                "Select files to open:");
        dialog.setInitialElementSelections(nedFiles);
        if (dialog.open() == Window.OK) {
            for (Object o : dialog.getResult())
                openFile((IFile)o);
        }
        return true;
    }

    protected void openFile(IFile file) {
        try {
            IWorkbenchWindow dwindow = workbench.getActiveWorkbenchWindow();
            IWorkbenchPage page = dwindow.getActivePage();
            if (page != null)
                IDE.openEditor(page, file, true);
        }
        catch (org.eclipse.ui.PartInitException e) {
            CommonPlugin.logError(e);
        }
    }

    @Override
    protected IWizardPage getFirstExtraPage() {
        return null;
    }

    @Override
    protected IFolder getFolder() {
        // return the handle of the simulation folder to be created
        String simulationFolderName = firstPage.getFileName();
        IPath path = firstPage.getContainerFullPath().append(simulationFolderName);
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        return root.getFolder(path);
    }

}

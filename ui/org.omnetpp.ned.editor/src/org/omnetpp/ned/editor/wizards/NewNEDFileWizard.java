package org.omnetpp.ned.editor.wizards;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.eclipse.ui.ide.IDE;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.BuiltinProjectTemplate;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.ICustomWizardPage;
import org.omnetpp.common.wizard.TemplateBasedWizard;
import org.omnetpp.common.wizard.XSWTWizardPage;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.NedEditorPlugin;

/**
 * "New NED file" wizard
 * 
 * @author Andras
 */
public class NewNEDFileWizard extends TemplateBasedWizard {
    public static final String WIZARDTYPE = "nedfile";

    private IWorkbench workbench;
    private IStructuredSelection selection;
    private WizardNewFileCreationPage firstPage;

    public NewNEDFileWizard() {
    }
    
    public void init(IWorkbench workbench, IStructuredSelection currentSelection) {
        this.workbench = workbench;
        this.selection = currentSelection;
        setWindowTitle("New NED File");
    }

    @Override
    public void addPages() {
        firstPage = new WizardNewFileCreationPage("file selection page", selection);
        firstPage.setAllowExistingResources(false);
        firstPage.setTitle("New NED File");
        firstPage.setDescription("Choose NED file");
        firstPage.setFileExtension("ned");
        firstPage.setFileName("untitled.ned");
        addPage(firstPage);
        super.addPages();
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);

        context.getVariables().put("wizardType", WIZARDTYPE);
        context.getVariables().put("nedFileName", firstPage.getFileName());

        IPath filePath = firstPage.getContainerFullPath().append(firstPage.getFileName());
        IFile newFile = ResourcesPlugin.getWorkspace().getRoot().getFile(filePath);
        String packageName = NEDResourcesPlugin.getNEDResources().getExpectedPackageFor(newFile);
        context.getVariables().put("nedPackageName", StringUtils.defaultString(packageName,""));

        String nedTypeName = StringUtils.capitalize(StringUtils.makeValidIdentifier(filePath.removeFileExtension().lastSegment()));
        context.getVariables().put("nedTypeName", nedTypeName);
        
        return context;
    }
    
    @Override
    public boolean performFinish() {
        boolean ok = super.performFinish();
        if (!ok)
            return false;
        
        // open the file for editing 
        IPath filePath = firstPage.getContainerFullPath().append(firstPage.getFileName());
        IFile newFile = ResourcesPlugin.getWorkspace().getRoot().getFile(filePath);
        if (!newFile.exists())
            return false;
        
        try {
            IWorkbenchWindow dwindow = workbench.getActiveWorkbenchWindow();
            IWorkbenchPage page = dwindow.getActivePage();
            if (page != null)
                IDE.openEditor(page, newFile, true);
        } 
        catch (org.eclipse.ui.PartInitException e) {
            NedEditorPlugin.logError(e);
            return false;
        }
        return true;
    }

    @Override
    protected IWizardPage getFirstExtraPage() {
        return null;
    }

    @Override
    protected IContainer getFolder() {
        IPath path = firstPage.getContainerFullPath();
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        return path.segmentCount()==1 ? root.getProject(path.toString()) : root.getFolder(path);
    }

    @Override
    protected List<IContentTemplate> getTemplates() {
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        
        result.add(new BuiltinProjectTemplate("Empty file", "XXX TODO", null) { //FIXME use null category!!!
            @Override
            public CreationContext createContext(IContainer folder) {
                CreationContext context = super.createContext(folder);
                context.getVariables().put("isEmpty", true);
                context.getVariables().put("isSimple", false);
                context.getVariables().put("isModule", false);
                context.getVariables().put("isNetwork", false);
                context.getVariables().put("isChannel", false);
                context.getVariables().put("isModuleinterface", false);
                context.getVariables().put("isChannelinterface", false);
                return context;
            }
            @Override
            public ICustomWizardPage[] createCustomPages() throws CoreException {
                try {
                    ICustomWizardPage page = new XSWTWizardPage(this, "select type", getClass().getResourceAsStream("/template/nedtype.xswt"), "template/nedtype.xswt", null);
                    page.setTitle("Choose NED Type");
                    page.setDescription("Select options below");
                    return new ICustomWizardPage[] { page }; 
                }
                catch (IOException e) {
                    throw new CoreException(new Status(IStatus.ERROR, CommonPlugin.PLUGIN_ID, "Error loading template file"));
                }
            }
            
            public void performFinish(CreationContext context) throws CoreException {
                substituteNestedVariables(context);
                createFileFromPluginResource("dummy", "untitled.ned.ftl", context); 
            }
        });

        result.addAll(loadTemplatesFromWorkspace(WIZARDTYPE));
        return result;
    }

}

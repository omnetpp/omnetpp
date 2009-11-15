package org.omnetpp.ned.editor.wizards;

import java.net.URL;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateBasedFileWizard;
import org.omnetpp.common.wizard.TemplateSelectionPage.ITemplateAddedListener;
import org.omnetpp.ned.core.NEDResourcesPlugin;

/**
 * "New NED file" wizard
 * 
 * @author Andras
 */
public abstract class AbstractNedFileWizard extends TemplateBasedFileWizard {
    @Override
    public void addPages() {
        super.addPages();
        setWindowTitle("New NED File");
        
        WizardNewFileCreationPage firstPage = getFirstPage();
        firstPage.setTitle("New NED File");
        firstPage.setDescription("Choose NED file");
        firstPage.setFileExtension("ned");
        firstPage.setFileName("untitled.ned");
        
        getTemplateSelectionPage().setTemplateAddedListener(new ITemplateAddedListener() {
            public void addTemplateFrom(URL url) throws CoreException {
                //TODO...
            }
        });
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);

        IPath filePath = getFirstPage().getContainerFullPath().append(getFirstPage().getFileName());
        IFile newFile = ResourcesPlugin.getWorkspace().getRoot().getFile(filePath);
        String packageName = NEDResourcesPlugin.getNEDResources().getExpectedPackageFor(newFile);
        context.getVariables().put("nedPackageName", StringUtils.defaultString(packageName,""));

        String nedTypeName = StringUtils.capitalize(StringUtils.makeValidIdentifier(filePath.removeFileExtension().lastSegment()));
        context.getVariables().put("nedTypeName", nedTypeName);
        
        return context;
    }

}

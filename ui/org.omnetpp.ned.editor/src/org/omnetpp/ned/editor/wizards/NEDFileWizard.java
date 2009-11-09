package org.omnetpp.ned.editor.wizards;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.wizard.BuiltinProjectTemplate;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.ICustomWizardPage;
import org.omnetpp.common.wizard.XSWTWizardPage;

/**
 * "New NED file" wizard
 * 
 * @author Andras
 */
public class NEDFileWizard extends AbstractNedFileWizard {

    @Override
    protected List<IContentTemplate> getTemplates() {
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();

        result.add(new BuiltinProjectTemplate("Empty NED file", null, null) {
            public void performFinish(CreationContext context) throws CoreException {
                substituteNestedVariables(context);
                context.getVariables().put("isEmpty", true);
                createFileFromPluginResource("dummy", "untitled.ned.ftl", context); 
            }
        });
        
        result.add(new BuiltinProjectTemplate("NED file with one item", null, null) {
            @Override
            public CreationContext createContext(IContainer folder) {
                CreationContext context = super.createContext(folder);
                context.getVariables().put("isEmpty", false);
                context.getVariables().put("isSimple", false);
                context.getVariables().put("isModule", false);
                context.getVariables().put("isNetwork", true);
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

        result.addAll(loadTemplatesFromWorkspace(getWizardType()));
        return result;
    }

}

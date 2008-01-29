package org.omnetpp.neddoc;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.ui.IActionDelegate;

public class GenerateNeddocAction implements IActionDelegate {
    private ISelection selection;

    public GenerateNeddocAction() {
    }

    public void run(IAction action) {
        try {
            GeneratorConfiguration configuration = NeddocPlugin.getDefault().getGeneratorConfiguration();
            configuration.projects = getSelectedOpenProjects().toArray(new IProject[0]);
            GeneratorConfigurationDialog dialog = new GeneratorConfigurationDialog(null, configuration);
            dialog.open();
            
            if (dialog.getReturnCode() == Window.OK)
                for (DocumentationGenerator generator : dialog.getConfiguredDocumentationGenerators())
                        generator.generate();
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public void selectionChanged(IAction action, ISelection selection) {
        this.selection = selection;
        List<IProject> projects = getSelectedOpenProjects();
        
        if (action.isEnabled() == projects.isEmpty())
            action.setEnabled(!projects.isEmpty());
    }

    @SuppressWarnings("unchecked")
    private List<IProject> getSelectedOpenProjects() {
        List<IProject> projects = new ArrayList<IProject>();
        if (selection instanceof IStructuredSelection) {
            for (Iterator it = ((IStructuredSelection) selection).iterator(); it.hasNext();) {
                Object element = it.next();
                IProject project = null;
                if (element instanceof IProject)
                    project = (IProject) element;
                else if (element instanceof IAdaptable)
                    project = (IProject) ((IAdaptable) element).getAdapter(IProject.class);
                if (project != null && project.isOpen())
                    projects.add(project);
            }
        }
        return projects;
    }
}

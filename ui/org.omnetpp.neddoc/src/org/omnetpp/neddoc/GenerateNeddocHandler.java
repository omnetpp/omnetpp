/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.neddoc;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.expressions.IEvaluationContext;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.window.Window;

/**
 * This controls the documentation generation for one or multiple OMNeT++ projects.
 *
 * @see org.eclipse.core.commands.IHandler
 * @see org.eclipse.core.commands.AbstractHandler
 */
public class GenerateNeddocHandler extends AbstractHandler {
    public GenerateNeddocHandler() {
    }

    /**
     * the command has been executed, so extract extract the needed information
     * from the application context.
     */
    public Object execute(ExecutionEvent event) throws ExecutionException {
        try {
            GeneratorConfiguration configuration = NeddocPlugin.getDefault().getGeneratorConfiguration();
            configuration.projects = getSelectedOpenProjects(event).toArray(new IProject[0]);
            GeneratorConfigurationDialog dialog = new GeneratorConfigurationDialog(null, configuration);
            dialog.open();

            if (dialog.getReturnCode() == Window.OK)
                for (DocumentationGenerator generator : dialog.getConfiguredDocumentationGenerators())
                    scheduleGenerateJob(generator);

            return null;
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public Job scheduleGenerateJob(DocumentationGenerator generator) throws Exception {
        WorkspaceJob job = new WorkspaceJob("Generating NED Documentation...") {
            @Override
            public IStatus runInWorkspace(IProgressMonitor monitor) {
                return generator.generate(monitor);
            }
        };
        job.setUser(true);
        job.schedule();
        return job;
    }
    
    @SuppressWarnings("rawtypes")
    private List<IProject> getSelectedOpenProjects(ExecutionEvent event) {
        List<IProject> projects = new ArrayList<IProject>();

        IEvaluationContext context = (IEvaluationContext)event.getApplicationContext();
        if (context == null || !(context.getDefaultVariable() instanceof List))
            return projects;  // empty list

        List selection = (List)context.getDefaultVariable();

        // collect the selected projects that are open
        for (Object element : selection) {
            IProject project = null;
            IResource resource = null;
            if (element instanceof IResource)
                resource = (IResource) element;
            else if (element instanceof IAdaptable)
                resource = (IResource) ((IAdaptable) element).getAdapter(IResource.class);
            if (resource != null)
                project = resource.getProject();
            if (project != null && project.isOpen())
                projects.add(project);
        }

        return projects;
    }
}

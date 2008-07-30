package org.omnetpp.neddoc;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.expressions.EvaluationContext;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.window.Window;

// TODO take into account the current selection too if it is a project
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
                    generator.generate();

    	    return null;
	    }
	    catch (Exception e) {
	        throw new RuntimeException(e);
	    }
	}

	@SuppressWarnings("unchecked")
    private List<IProject> getSelectedOpenProjects(ExecutionEvent event) {
		List<IProject> projects = new ArrayList<IProject>();
		
		EvaluationContext context = (EvaluationContext)event.getApplicationContext();
		if (context == null)
			return projects;
		List selection = (List)context.getDefaultVariable();
		if (!(selection instanceof List))
			return projects;
		
		// collect the projects that are open
		for (Object element : selection) {
			IProject project = null;
			if (element instanceof IProject)
				project = (IProject) element;
			else if (element instanceof IAdaptable)
				project = (IProject) ((IAdaptable) element).getAdapter(IProject.class);
			if (project != null && project.isOpen())
				projects.add(project);
		}
		
        return projects;
    }
}

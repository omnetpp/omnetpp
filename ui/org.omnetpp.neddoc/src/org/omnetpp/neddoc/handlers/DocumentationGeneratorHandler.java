package org.omnetpp.neddoc.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.neddoc.DocumentationGenerator;
import org.omnetpp.neddoc.GeneratorConfiguration;
import org.omnetpp.neddoc.GeneratorConfigurationDialog;

/**
 * @see org.eclipse.core.commands.IHandler
 * @see org.eclipse.core.commands.AbstractHandler
 */
public class DocumentationGeneratorHandler extends AbstractHandler {
    private GeneratorConfiguration configuration = new GeneratorConfiguration();

    /**
	 * The constructor.
	 */
	public DocumentationGeneratorHandler() {
	}

	/**
	 * the command has been executed, so extract extract the needed information
	 * from the application context.
	 */
	public Object execute(ExecutionEvent event) throws ExecutionException {
	    try {
    	    ISelection selection = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getSelectionService().getSelection();
    	    
    	    if (selection instanceof IStructuredSelection) {
    	        IStructuredSelection structuredSelection = (IStructuredSelection)selection;
    	        Object element = structuredSelection.getFirstElement();

                IProject project = null;
                
                if (element instanceof IProject)
                    project = (IProject)element;
                else if (element instanceof IResource)
                    project = ((IResource)element).getProject();

                GeneratorConfigurationDialog dialog = new GeneratorConfigurationDialog(null, project, configuration);
                dialog.open();
                
                if (dialog.getReturnCode() == Window.OK)
                    for (DocumentationGenerator generator : dialog.getConfiguredDocumentationGenerators())
                        generator.generate();
    	    }
    
    	    return null;
	    }
	    catch (Exception e) {
	        throw new RuntimeException(e);
	    }
	}
}

package org.omnetpp.neddoc.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.IProject;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.neddoc.Neddoc;

/**
 * Our sample handler extends AbstractHandler, an IHandler base class.
 * @see org.eclipse.core.commands.IHandler
 * @see org.eclipse.core.commands.AbstractHandler
 */
public class SampleHandler extends AbstractHandler {
	/**
	 * The constructor.
	 */
	public SampleHandler() {
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
    	        
    	        if (element instanceof IProject)
    	            new Neddoc((IProject)element).generate();
    	    }
    
    	    return null;
	    }
	    catch (Exception e) {
	        // TODO:
	        throw new RuntimeException(e);
	    }
	}
}

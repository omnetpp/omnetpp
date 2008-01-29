package org.omnetpp.neddoc;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.window.Window;

/**
 * This controls the documentation generation for one or multiple OMNeT++/OMNEST projects.
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
            GeneratorConfigurationDialog dialog = new GeneratorConfigurationDialog(null, NeddocPlugin.getDefault().getGeneratorConfiguration());
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
}

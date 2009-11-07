/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;


import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.ICustomWizardPage;

/**
 * Interface for project content templates
 * 
 * @author Andras
 */
public interface IProjectTemplate {
	/**
	 * Name of the template. This is the name that appears in the wizard's 
	 * template selection page.
	 */
    String getName();

    /**
	 * Description of the template. This may appear as a tooltip 
	 * or in a description pane in the wizard.
	 */
    String getDescription();
    
    /**
     * Template category is used for organizing the templates into a tree 
     * in the wizard's template selection page.
     */
    String getCategory();

    /**
	 * The icon that appears with the name in the wizard's 
	 * template selection page.
	 */
    Image getImage();

    /**
     * Creates an initial context with the project and some initial variables set.
     */
    CreationContext createContext(IProject project);
    
    /**
     * Create custom wizard pages. Should never return null (may return an 
     * empty array though).    
     */
    ICustomWizardPage[] createCustomPages() throws CoreException;

    /**
     * Configure the project (add files, etc) using the variables, both in the context.
     * This method is usually called from a background thread, so it can no longer
     * access the GUI (e.g. custom wizard pages).
     */
    void configureProject(CreationContext context) throws CoreException;
}

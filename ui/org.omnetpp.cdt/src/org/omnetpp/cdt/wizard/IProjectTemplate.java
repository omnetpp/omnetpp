/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;

import java.util.Map;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.swt.graphics.Image;

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
     * Create custom wizard pages. Should never return null (may return an 
     * empty array though).    
     */
    IWizardPage[] createCustomPages();

    /**
     * Extract variables from the wizard pages.
     */
    Map<String,Object> extractVariablesFromPages(IWizardPage[] customPages);

    /**
     * Configure the project (add files, etc) using the variables.
     * The variables will come from the wizard pages.
     */
    void configure(IProject project, Map<String, Object> vars, IProgressMonitor monitor) throws CoreException;
}

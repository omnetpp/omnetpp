/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.wizard;


import java.util.Set;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.wizard.IWizard;
import org.eclipse.swt.graphics.Image;

/**
 * Interface for content templates
 * 
 * @author Andras
 */
public interface IContentTemplate {
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
     * in the wizard's template selection page. An empty string will place
     * the template to the top level of the tree.
     */
    String getCategory();

    /**
	 * The icon that appears with the name in the wizard's 
	 * template selection page.
	 */
    Image getImage();

    /**
     * Returns the types of wizards where this content template can appear.
     * Possible values are "project", "nedfile", "msgfile", "nedcomponent",
     * "simplemodule", "compoundmodule", "network", "inifile", "wizard" etc.
     */
    Set<String> getSupportedWizardTypes();

    /**
     * Creates an initial context with the given resource, and some initial variables set.
     * The resource is the workspace resource the wizard should create, or in which 
     * it should create content.
     */
    CreationContext createContext(IContainer folder, IWizard wizard);
    
    /**
     * Create custom wizard pages. Should never return null (may return an 
     * empty array though).    
     */
    ICustomWizardPage[] createCustomPages() throws CoreException;

    /**
     * Performs the task of the wizard. Creates the resource specified in the context,
     * or content inside that resource (i.e. if it is a project or folder). If the resource
     * is a project, this method may also configure it (set C++ source folders, etc).
     * It may use the template variables passed in via the context as input.
     * This method is usually called from a background thread, so it can no longer
     * access the GUI (e.g. custom wizard pages).
     */
    void performFinish(CreationContext context) throws CoreException;
}

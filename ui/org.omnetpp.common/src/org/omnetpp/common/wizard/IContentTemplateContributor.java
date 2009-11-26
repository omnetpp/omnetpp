/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.wizard;


import org.eclipse.core.runtime.CoreException;

/**
 * Extension interface for content templates.
 * 
 * @author Andras
 */
public interface IContentTemplateContributor {

    /**
     * Return extra template code, which will be included before each template.
     * May define functions, macros etc.
     */
    String getAdditionalTemplateCode() throws CoreException;
    
    /**
     * May add variables to the context.
     */
    void contributeToContext(CreationContext context);

}

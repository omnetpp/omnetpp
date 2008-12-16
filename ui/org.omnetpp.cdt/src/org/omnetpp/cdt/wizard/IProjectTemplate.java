/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.graphics.Image;

/**
 * Interface for project content templates
 * 
 * @author Andras
 */
public interface IProjectTemplate {
    String getName();
    Image getImage();
    String getDescription();
    String getCategory();
    void configure(IProject project, IProgressMonitor monitor) throws CoreException;
}

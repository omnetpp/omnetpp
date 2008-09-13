package org.omnetpp.cdt.wizard;

import org.eclipse.core.resources.IProject;
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
    void configure(IProject project);
}

package org.omnetpp.ned.core;

import java.io.IOException;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.preference.IPreferencePageContainer;
import org.omnetpp.common.project.NedSourceFoldersConfiguration;
import org.omnetpp.common.project.ProjectUtils;

/**
 * When more than one property pages edit the ".nedfolders" file, they can
 * communicate via NedSourceFoldersEditingManager, i.e. can edit the same
 * working copy.
 * 
 * @author Andras
 */
public class NedSourceFoldersEditingManager {
    private long dialogHashCode;
    private IProject project;
    private NedSourceFoldersConfiguration config;

    /**
     * Returns a working copy that is valid for the present Properties dialog session.
     * The same working copy is given to all property pages in the same dialog.
     */
    public NedSourceFoldersConfiguration getConfig(IPreferencePageContainer propertyDialog, IProject project) throws IOException, CoreException {
        if (!project.equals(this.project) || propertyDialog.hashCode() != dialogHashCode) {
            // seems to be different dialog session than last time -- reload file
            config = ProjectUtils.readNedFoldersFile(project);
            dialogHashCode = propertyDialog.hashCode();
            this.project = project;
        }
        return config;
    }
}

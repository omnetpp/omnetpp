package org.omnetpp.ide.installer;

import java.io.File;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;

/**
 * This class provides the default project installer behavior as follows:
 *  - download distribution file
 *  - extract distribution under workspace
 *  - import project into workspace
 *  - open project
 *  - build project
 *  - optionally show welcome page
 *
 * @author levy
 */
public class DefaultProjectInstaller extends AbstractProjectInstaller {
    public DefaultProjectInstaller(ProjectDescription projectDescription, ProjectInstallationOptions projectInstallationOptions) {
        super(projectDescription, projectInstallationOptions);
    }

    @Override
    public void run(IProgressMonitor progressMonitor) throws CoreException {
        progressMonitor.beginTask("Installing " + projectDescription.getTitle() + " into the workspace", 5);
        File projectDistributionFile = downloadProjectDistribution(progressMonitor, projectDescription.getDistributionURL());
        File projectDirectory = extractProjectDistribution(progressMonitor, projectDistributionFile);
        IProject project = importProjectIntoWorkspace(progressMonitor, projectDirectory);
        openProject(progressMonitor, project);
        String welcomePage = projectDescription.getWelcomePage();
        if (welcomePage != null)
            openEditor(project.getFile(welcomePage));
        expandProject(project);
        initializeProjectFeaturesState(project);
        buildProject(progressMonitor, project);
        progressMonitor.done();
    }
}
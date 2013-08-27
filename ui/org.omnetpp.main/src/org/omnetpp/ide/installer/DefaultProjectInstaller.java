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
        progressMonitor.beginTask("Installing project " + projectDescription.getName() + " into workspace", 5);
        File projectDistributionFile = downloadProjectDistribution(progressMonitor, projectDescription.getDistributionURL());
        if (progressMonitor.isCanceled()) return;
        File projectDirectory = extractProjectDistribution(progressMonitor, projectDistributionFile);
        if (progressMonitor.isCanceled()) return;
        IProject project = importProjectIntoWorkspace(progressMonitor, projectDirectory);
        if (progressMonitor.isCanceled()) return;
        openProject(progressMonitor, project);
        if (progressMonitor.isCanceled()) return;
        buildProject(progressMonitor, project);
        if (progressMonitor.isCanceled()) return;
        String welcomePage = projectDescription.getWelcomePage();
        if (welcomePage != null)
            openEditor(project.getFile(welcomePage));
        expandProject(project);
        progressMonitor.done();
    }
}
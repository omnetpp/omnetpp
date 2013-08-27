package org.omnetpp.ide.installer;

import java.net.URL;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.common.CommonPlugin;

/**
 * This class wraps the project installation task into a job.
 *
 * @author levy
 */
public class InstallProjectJob extends Job {
    protected URL projectDescriptionURL;
    protected ProjectInstallationOptions projectInstallationOptions;

    public InstallProjectJob(URL projectDescriptionURL, ProjectInstallationOptions projectInstallationOptions) {
        super("Install Project");
        this.projectDescriptionURL = projectDescriptionURL;
        this.projectInstallationOptions = projectInstallationOptions;
    }

    @Override
    protected IStatus run(IProgressMonitor progressMonitor) {
        try {
            InstallProjectTask installProjectTask = new InstallProjectTask(projectDescriptionURL, projectInstallationOptions);
            installProjectTask.run(progressMonitor);
            return new Status(Status.OK, CommonPlugin.PLUGIN_ID, "Ok");
        }
        catch (OperationCanceledException e) {
            return new Status(Status.OK, CommonPlugin.PLUGIN_ID, "Ok");
        }
        catch (CoreException e) {
            return e.getStatus();
        }
        catch (Exception e) {
            return new Status(Status.ERROR, CommonPlugin.PLUGIN_ID, e.getMessage(), e.getCause());
        }
    }
}
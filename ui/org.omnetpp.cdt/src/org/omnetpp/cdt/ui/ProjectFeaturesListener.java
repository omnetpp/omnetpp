package org.omnetpp.cdt.ui;

import java.util.Arrays;
import java.util.List;

import org.eclipse.cdt.core.settings.model.CProjectDescriptionEvent;
import org.eclipse.cdt.core.settings.model.ICProjectDescriptionListener;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.build.ProjectFeaturesManager;
import org.omnetpp.common.Debug;
import org.omnetpp.common.project.ProjectUtils;

/**
 * Responsible for keeping a project's configuration (NED excluded packages, CDT
 * source folders and exclusions, preprocessor symbols etc.) reasonably in sync
 * with the enabled project features. This is achieved by listening on various
 * resource change events.
 *
 * In principle, it should do the following:
 * <ul>
 * <li>when a project is created (e.g. via import), it should silently (no
 * dialog!) initialize the "enabled" state of project features to the default
 * state (see the "initiallyEnabled" attribute in ".oppfeatures"), and modify
 * the project's configuration (CDT and NED) accordingly.
 * <li>when "something" changes in the project, check that the project state
 * (CDT and NED) still corresponds to the list of enabled features, and if not,
 * offer fixing it to the user. However, these checks (and corresponding pop-up
 * dialogs) should not be too aggressive in order not to annoy the user, and
 * especially it should NOT take place when the Project Properties dialog is
 * open (the Properties dialog edits its own working copy of the config, so
 * fixing the original copy would only create confusion.)
 * </ul>
 *
 * Currently it does the following:
 * <ul>
 * <li>when an ".oppfeatures" file is created, and it updates CDT and NED
 * setting according to the default feature selection (without asking)
 * <li>when an ".oppfeatures" is modified, it checks the project state (CDT and
 * NED), and offers the user to fix it if it was not consistent with the feature
 * selection.
 * <li>at project build, we also perform the checks and offer the user to fix
 * any inconsistency (this is done in MakefileBuilder)
 * </ul>
 *
 * We could also listen on CDT config changes, folder creation notifications
 * etc., but this is not done (it was too aggressive and annoying).
 *
 * TODO update docu
 *
 * @author Andras
 */
public class ProjectFeaturesListener implements IResourceChangeListener, ICProjectDescriptionListener {

    public void hookListeners() {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this, IResourceChangeEvent.POST_CHANGE);
        //CoreModel.getDefault().addCProjectDescriptionListener(this, CProjectDescriptionEvent.APPLIED);
    }

    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
        //CoreModel.getDefault().removeCProjectDescriptionListener(this);
    }

    public void handleEvent(CProjectDescriptionEvent event) {
        //Debug.println(" +++++++++++++++++++++++ CProjectDescriptionEvent: " + event);
        //adjustProjectConfiguration(event.getProject());
    }

    public static final List<String> WATCHED_CONFIG_FILES = Arrays.asList(
            ProjectUtils.PROJECTFEATURES_FILENAME,
            ProjectUtils.PROJECTFEATURESTATE_FILENAME,
            ProjectUtils.NEDFOLDERS_FILENAME
            //ProjectUtils.NEDEXCLUSIONS_FILENAME -- do not watch!
    );

    public void resourceChanged(IResourceChangeEvent event) {
        Assert.isTrue(event.getType() == IResourceChangeEvent.POST_CHANGE);
        try {
            event.getDelta().accept(new IResourceDeltaVisitor() {
                public boolean visit(IResourceDelta delta) throws CoreException {
                    IResource resource = delta.getResource();
                    int kind = delta.getKind();
                    if ((kind == IResourceDelta.ADDED || kind == IResourceDelta.CHANGED) && isDotFileInOmnetppProject(resource)) {
                        if (delta.getKind() == IResourceDelta.ADDED && resource.getName().equals(ProjectUtils.PROJECTFEATURES_FILENAME))
                            omnetppProjectWithFeaturesCreated(resource.getProject());
                        else if (delta.getKind() == IResourceDelta.CHANGED && WATCHED_CONFIG_FILES.contains(resource.getName()))
                            adjustProjectConfiguration(resource.getProject());
                        return false;
                    }
                    return true;
                }

                protected boolean isDotFileInOmnetppProject(IResource resource) throws CoreException {
                    return resource instanceof IFile && resource.getName().charAt(0) == '.' &&
                            resource.getParent() instanceof IProject && ProjectUtils.isOpenOmnetppProject(resource.getProject());
                }
            });
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
    }

    protected void omnetppProjectWithFeaturesCreated(final IProject project) {
        Job job = new Job("Configuring project according to Project Features") {
            @Override
            protected IStatus run(IProgressMonitor monitor) {
                try {
                    // we need to run it in a workspace job because the workspace is locked during a resource change
                    // notification so we would not be able to save the updated CDT configuration
                    ProjectFeaturesManager features = new ProjectFeaturesManager(project);
                    if (features.loadFeaturesFile()) {
                        Debug.println("Project Features: activating default feature selection for new project " + project.getName());
                        features.initializeProjectState();
                    }
                }
                catch (CoreException e) {
                    Activator.logError("Error activating default feature selection for new project", e);
                }
                return Status.OK_STATUS;
            }
        };
        job.schedule();
    }

    protected void adjustProjectConfiguration(IProject project) {
        if (project.getFile(ProjectUtils.PROJECTFEATURES_FILENAME).exists())
            adjustProjectState(project);
    }

    protected void adjustProjectState(final IProject project) {
        // Note: must use workspace job, because the workspace is locked during resource change notification
        Job job = new Job("Configuring project according to Project Features") {
            @Override
            protected IStatus run(IProgressMonitor monitor) {
                try {
                    ProjectFeaturesManager features = new ProjectFeaturesManager(project);
                    if (features.loadFeaturesFile()) {
                        Debug.println("Configuring project according to Project Features for new project " + project.getName());
                        features.adjustProjectState();
                    }
                } catch (CoreException e) {
                    Activator.logError("Error adjusting project state according to Project Features", e);
                }
                return Status.OK_STATUS;
            }
        };
        job.schedule();
    }

}

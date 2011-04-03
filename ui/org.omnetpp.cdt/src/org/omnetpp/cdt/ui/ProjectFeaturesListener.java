package org.omnetpp.cdt.ui;

import java.util.ArrayList;
import java.util.List;

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
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.build.ProjectFeaturesManager;
import org.omnetpp.cdt.build.ProjectFeaturesManager.Problem;
import org.omnetpp.common.Debug;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.ProblemsMessageDialog;
import org.omnetpp.common.util.UIUtils;

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
 * @author Andras
 */
public class ProjectFeaturesListener implements IResourceChangeListener /*,ICProjectDescriptionListener*/ {

    public void hookListeners() {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this, IResourceChangeEvent.POST_CHANGE);
        //CoreModel.getDefault().addCProjectDescriptionListener(this, CProjectDescriptionEvent.APPLIED); 
    }
    
    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
        //CoreModel.getDefault().removeCProjectDescriptionListener(this);
    }

    //public void handleEvent(CProjectDescriptionEvent event) {
    //    verifyProjectConfiguration(event.getProject());
    //}

    public void resourceChanged(IResourceChangeEvent event) {
        Assert.isTrue(event.getType() == IResourceChangeEvent.POST_CHANGE);
        try {
            event.getDelta().accept(new IResourceDeltaVisitor() {
                public boolean visit(IResourceDelta delta) throws CoreException {
                    IResource resource = delta.getResource();
                    if (isFeaturesDescriptionFile(resource)) {
                        if (delta.getKind() == IResourceDelta.ADDED)
                            omnetppProjectWithFeaturesCreated(resource.getProject());
                        else if (delta.getKind() == IResourceDelta.CHANGED)
                            verifyProjectConfiguration(resource.getProject());
                        return false;
                    }
                    return true;
                }

                protected boolean isFeaturesDescriptionFile(IResource resource) throws CoreException {
                    return (resource.getParent() instanceof IProject &&   // note: least costly/more selective checks first
                            resource instanceof IFile &&
                            resource.getName().equals(ProjectFeaturesManager.PROJECTFEATURES_FILENAME) && 
                            ProjectUtils.isOpenOmnetppProject(resource.getProject()));
                }

            });
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
    }

    protected void omnetppProjectWithFeaturesCreated(final IProject project) {
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                try {
                    // we need to run it in asyncExec() because the workspace is locked during a resource change 
                    // notification so we would not be able to save the updated CDT configuration
                    ProjectFeaturesManager features = new ProjectFeaturesManager(project);
                    if (features.loadFeaturesFile()) {
                        Debug.println("Project Features: activating default feature selection for new project " + project.getName());
                        features.initializeProjectState();
                    }
                }
                catch (CoreException e) {
                    Activator.logError(e);
                }
            }
        });
    }

    protected void verifyProjectConfiguration(IProject project) {
        try {
            if (ProjectUtils.isOpenOmnetppProject(project)) {
                final ProjectFeaturesManager features = new ProjectFeaturesManager(project);
                if (features.loadFeaturesFile()) {
                    Debug.println("Project Features: checking project " + project.getName());
                    final List<Problem> problems = features.validateProjectState();
                    if (!problems.isEmpty()) {
                        Display.getDefault().asyncExec(new Runnable() {
                            public void run() {
                                offerFixingProblems(features, problems);
                            }
                        });
                    }

                }
            }
        } 
        catch (CoreException e) {
            Activator.logError("Error checking project state wrt. project features", e);
        }
    }

    protected void offerFixingProblems(ProjectFeaturesManager features, List<Problem> problems) {
        if (isOkToFixConfigProblems(features.getProject(), problems)) {
            try {
                features.fixupProjectState();
            }
            catch (CoreException e) {
                Activator.logError(e);
                Shell parentShell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
                ErrorDialog.openError(parentShell, "Error", "Error fixing project state", e.getStatus());
            }
        }
    }
    
    protected boolean isOkToFixConfigProblems(IProject project, List<Problem> problems) {
        List<String> problemTexts = new ArrayList<String>();
        for (Problem p : problems)
            problemTexts.add(p.toString());
        Shell parentShell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
        return ProblemsMessageDialog.openConfirm(parentShell, "Project Setup Inconsistency", 
                "Some configuration settings in project \"" + project.getName() + "\" do not correspond " +
                "to the enabled project features. Do you want to fix the project state?", 
                problemTexts, UIUtils.ICON_ERROR);
    }
    
}

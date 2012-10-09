package org.omnetpp.ide;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.FileUtils;

/**
 * Work around the problem that sometimes when a project is opened, it does not
 * correctly pick up natures (OMNeT++ Nature and/or C++ nature). This usually
 * occurs on Windows, and when the IDE has just been started, and projects that
 * have not been opened before produce it with a greater chance. When the user
 * closes and re-opens the project, it usually opens correctly.
 *
 * Until we can determine the root cause of the program and fix it properly,
 * this class is used to automate the workaround: i.e. if the project does not
 * have the natures but from the files it appears it should have, this class
 * closes and re-opens the project.
 *
 * @author Andras
 */
public class OmnetppProjectOpenListener implements IResourceChangeListener {
    protected static final String CDT_SCANNERCONFIG_NATURE = "org.eclipse.cdt.managedbuilder.core.ScannerConfigNature";
    protected static final String CDT_MANAGEDBUILD_NATURE = "org.eclipse.cdt.managedbuilder.core.managedBuildNature";
    protected static final String CDT_C_NATURE = "org.eclipse.cdt.core.cnature";
    protected static final String CDT_CC_NATURE = "org.eclipse.cdt.core.ccnature";

    protected static final int MAX_RETRY_COUNT = 5;

    protected Map<IProject,Integer> retryCounts = new HashMap<IProject, Integer>();

    public OmnetppProjectOpenListener() {
    }

    public void resourceChanged(IResourceChangeEvent event) {
        if (event.getType() == IResourceChangeEvent.POST_CHANGE) {
            IResourceDelta delta = event.getDelta();
            IResourceDelta[] projectDeltas = delta.getAffectedChildren();

            for (int i = 0; i < projectDeltas.length; i++) {
                IResourceDelta resourceDelta = projectDeltas[i];
                if ((resourceDelta.getFlags() & IResourceDelta.OPEN) != 0) {  // note: OPEN is sent on both project close and open
                    IProject project = (IProject) resourceDelta.getResource();
                    if (project.isOpen())
                        projectOpened(project);
                }
            }
        }
    }

    protected void projectOpened(final IProject project) {
        // check if the project has picked up the natures it was supposed to pick up
        boolean areNaturesOK;
        try {
            areNaturesOK = areNaturesOK(project);
        }
        catch (CoreException e) {
            OmnetppMainPlugin.logError(e);
            return;  // leave it alone
        }

        if (areNaturesOK) {
            // project opened OK, remove retry counter
            Integer count = retryCounts.remove(project);
            if (count != null) {
                OmnetppMainPlugin.getDefault().getLog().log(new Status(IStatus.INFO, OmnetppMainPlugin.PLUGIN_ID,
                        "Project " + project.getName() + " opened correctly after " + count + " close/reopen cycle(s)."));
            }
        }
        else {
            // increment count
            Integer count = retryCounts.get(project);
            count = count==null ? 1 : count + 1;
            retryCounts.put(project, count);

            // try reopening max n times
            if (count < MAX_RETRY_COUNT) {
                // schedule reopen (cannot directly close/open project, as the resource tree is locked during notifications)
                Display.getDefault().asyncExec(new Runnable() {
                    public void run() {
                        try {
                            project.close(null);
                            project.open(null);
                        }
                        catch (CoreException e) {
                            OmnetppMainPlugin.logError(e);
                        }
                    }});
            }
            else {
                retryCounts.remove(project);
                DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
                    public void run() {
                        OmnetppMainPlugin.getDefault().getLog().log(new Status(IStatus.WARNING, OmnetppMainPlugin.PLUGIN_ID,
                                "Project " + project.getName() + " may have opened incorrectly: " +
                                "it looks like an OMNeT++ and/or CDT C++ project, but was not recognized as such. " +
                                "Have tried to close and re-open " + MAX_RETRY_COUNT + " times, giving up."
                                ));
                        Shell parentShell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
                        MessageDialog.openWarning(parentShell, "Warning",
                                "Project " + project.getName() + " may have opened incorrectly: " +
                                "it looks like an OMNeT++ and/or CDT C++ project, but was not recognized as such. " +
                                "Try closing and re-opening the project."
                        );
                    }
                });
            }
        }
    }

    protected boolean areNaturesOK(IProject project) throws CoreException {
        // check CDT natures
        if (!project.isNatureEnabled(CDT_CC_NATURE)) {
            // if a .cproject file exists then probably this is a C++ project
            File cprojectFile = project.getLocation().append(".cproject").toFile();
            if (cprojectFile.isFile() && !cprojectFile.canRead()) {
                // check nature in .project too
                if (projectFileContainsNature(project, CDT_CC_NATURE))
                    return false;
            }
        }

        // check OMNeT++ nature
        if (!project.isNatureEnabled(IConstants.OMNETPP_NATURE_ID)) {
            // if .nedfolders or .oppbuildspec exists then probably this is an OMNeT++ project
            File nedfoldersFile = project.getLocation().append(".nedfolders").toFile();
            File oppbuildspecFile = project.getLocation().append(".oppbuildspec").toFile();
            if ((nedfoldersFile.isFile() && !nedfoldersFile.canRead()) || (oppbuildspecFile.isFile() && oppbuildspecFile.canRead())) {
                // check nature in .project too
                if (projectFileContainsNature(project, IConstants.OMNETPP_NATURE_ID))
                    return false;
            }
        }

        return true; // looks OK
    }

    protected boolean projectFileContainsNature(IProject project, String natureId) {
        try {
            File projectFile = project.getLocation().append(".project").toFile();
            String contents = FileUtils.readTextFile(projectFile, null);
            return contents.contains("<nature>" + natureId + "</nature>"); // keep it simple...
        }
        catch (IOException e) {
            return false;
        }
    }


}

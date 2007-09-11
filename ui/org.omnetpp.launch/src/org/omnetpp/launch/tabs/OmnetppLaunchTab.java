package org.omnetpp.launch.tabs;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.ui.AbstractLaunchConfigurationTab;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;

import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPlugin;

/**
 * Base class for omnetpp launch tabs
 *
 * @author rhornig
 */
public abstract class OmnetppLaunchTab extends AbstractLaunchConfigurationTab implements
    SelectionListener, ModifyListener {

    protected OmnetppLaunchTab embeddingTab = null;
    /**
     * Config being modified
     */
    private ILaunchConfiguration fLaunchConfig;


    public OmnetppLaunchTab() {
        this(null);
    }


    public OmnetppLaunchTab(OmnetppLaunchTab embeddingTab) {
        super();
        this.embeddingTab = embeddingTab;
    }

    /**
     * Returns the launch configuration this tab was initialized from.
     *
     * @return launch configuration this tab was initialized from
     */
    protected ILaunchConfiguration getCurrentLaunchConfiguration() {
        return fLaunchConfig;
    }

    /**
     * Sets the launch configuration this tab was initialized from
     *
     * @param config launch configuration this tab was initialized from
     */
    protected void setCurrentLaunchConfiguration(ILaunchConfiguration config) {
        fLaunchConfig = config;
    }

    public void initializeFrom(ILaunchConfiguration config) {
        setCurrentLaunchConfiguration(config);
    }

    public void widgetDefaultSelected(SelectionEvent e) {
        updateLaunchConfigurationDialog();
        // notify the embedding tab
        if (embeddingTab != null)
            embeddingTab.widgetDefaultSelected(e);
    }

    public void widgetSelected(SelectionEvent e) {
        updateLaunchConfigurationDialog();
        // notify the embedding tab
        if (embeddingTab != null)
            embeddingTab.widgetSelected(e);
    }

    public void modifyText(ModifyEvent e) {
        updateLaunchConfigurationDialog();
        // notify the embedding tab
        if (embeddingTab != null)
            embeddingTab.modifyText(e);
    }

    /**
     * Expands and returns the working directory attribute of the given launch
     * configuration. Returns <code>null</code> if a working directory is not
     * specified. If specified, the working is verified to point to an existing
     * directory in the local file system.
     *
     * @return an absolute path to a directory in the local file system, or
     * <code>null</code> if unspecified
     * @throws CoreException if unable to retrieve the associated launch
     * configuration attribute, if unable to resolve any variables, or if the
     * resolved location does not point to an existing directory in the local
     * file system
     */
    public IPath getWorkingDirectoryPath(){
        return LaunchPlugin.getWorkingDirectoryPath(getCurrentLaunchConfiguration());
    }

    /**
     * Returns the currently selected EXE file (or null if none selected)
     */
    protected IFile getExeFile() {
        IFile exefile = null;
        String name;
        try {
            name = getCurrentLaunchConfiguration().getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
            exefile = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(name));
        } catch (CoreException e) {
            LaunchPlugin.logError("Error getting program name from configuration", e);
        }
        return exefile;
    }

    /**
     * Returns the project in which the currently selected EXE file is located
     */
    protected IProject getProject() {
        IFile exeFile = getExeFile();
        return exeFile != null ? exeFile.getProject() : null;
    }

    /**
     * Utility function: constructs a path that is relative to the relativeTo.
     */
    protected static IPath makeRelativePathTo(IPath path, IPath relativeTo) {

        int goUpLevels = relativeTo.segmentCount() - path.matchingFirstSegments(relativeTo);
        // walk up until the first common segment
        IPath resultPath = new Path("");
        for (int i=0; i<goUpLevels; ++i)
            resultPath = resultPath.append("../");
        // add the rest of the path (non common part)
        return resultPath.append(path.removeFirstSegments(path.matchingFirstSegments(relativeTo))).makeRelative();
    }

    @Override
    protected void setErrorMessage(String errorMessage) {
        super.setErrorMessage(errorMessage);
        if (embeddingTab != null)
            embeddingTab.setErrorMessage(errorMessage);
    }

}

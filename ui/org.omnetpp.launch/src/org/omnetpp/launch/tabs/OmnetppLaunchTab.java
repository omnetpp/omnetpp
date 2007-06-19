package org.omnetpp.launch.tabs;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.variables.VariablesPlugin;
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
 * @author rhornig
 */
public abstract class OmnetppLaunchTab extends AbstractLaunchConfigurationTab implements
    SelectionListener, ModifyListener {

    protected static final String EMPTY_STRING = ""; //$NON-NLS-1$

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
        if(embeddingTab != null)
            embeddingTab.widgetDefaultSelected(e);
    }

    public void widgetSelected(SelectionEvent e) {
        updateLaunchConfigurationDialog();
        // notify the embedding tab
        if(embeddingTab != null)
            embeddingTab.widgetSelected(e);
    }

    public void modifyText(ModifyEvent e) {
        updateLaunchConfigurationDialog();
        // notify the embedding tab
        if(embeddingTab != null)
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
    protected IPath getWorkingDirectoryPath(){
        String location;
        try {
            location = getCurrentLaunchConfiguration().getAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY,
                    "${workspace_loc:"+getCurrentLaunchConfiguration().getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME,"")+"}");
            if (location != null) {
                String expandedLocation = VariablesPlugin.getDefault().getStringVariableManager().performStringSubstitution(location);
                if (expandedLocation.length() > 0) {
                    IPath newPath = new Path(expandedLocation);
                    return newPath.makeAbsolute();
                }
            }
        } catch (CoreException e) {
            LaunchPlugin.logError("Error getting working directory from configuration", e);
        }
        return new Path("${workspace_loc}");
    }

    /**
     * @param path
     * @param relativeTo
     * @return The provided path in relative form to the relativeTo path
     */
    protected static IPath makeRelativePathTo(IPath path, IPath relativeTo) {

        int goUpLeveles = relativeTo.segmentCount() - path.matchingFirstSegments(relativeTo);
        // walk up until the first common segment
        IPath resultPath = new Path("");
        for(int i=0; i<goUpLeveles; ++i)
            resultPath = resultPath.append("../");
        // add the rest of the path (non common part)
        return resultPath.append(path.removeFirstSegments(path.matchingFirstSegments(relativeTo))).makeRelative();
    }
}

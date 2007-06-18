package org.omnetpp.launch.tabs;

import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.ui.AbstractLaunchConfigurationTab;

/**
 * Base class for omnetpp launch tabs
 * @author rhornig
 */
public abstract class OmnetppLaunchTab extends AbstractLaunchConfigurationTab {

    protected static final String EMPTY_STRING = ""; //$NON-NLS-1$

    /**
     * Config being modified
     */
    private ILaunchConfiguration fLaunchConfig;


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
    private void setCurrentLaunchConfiguration(ILaunchConfiguration config) {
        fLaunchConfig = config;
    }

    /* (non-Javadoc)
     * @see org.eclipse.debug.ui.ILaunchConfigurationTab#initializeFrom(org.eclipse.debug.core.ILaunchConfiguration)
     *
     * Subclasses may override this method and should call super.initializeFrom(...).
     */
    public void initializeFrom(ILaunchConfiguration config) {
        setCurrentLaunchConfiguration(config);
    }

}

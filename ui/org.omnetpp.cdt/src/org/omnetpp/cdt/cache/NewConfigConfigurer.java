/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.cache;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.collections.ListUtils;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.CProjectDescriptionEvent;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescriptionListener;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.WriteAccessException;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.build.ProjectFeaturesManager;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;

/**
 * Detects when a new configuration (ICConfigurationDescription) has been added
 * to a project, and configures it accordingly. Configuring currently means
 * copying the source entries from some other configuration of the same project.
 *
 * @author Andras
 */
public class NewConfigConfigurer {
    private Map<IProject,List<String>> existingConfigIdsMap = new HashMap<IProject,List<String>>();

    private ICProjectDescriptionListener listener = new ICProjectDescriptionListener() {
        public void handleEvent(CProjectDescriptionEvent e) {
            projectChanged(e);
        }
    };

    public NewConfigConfigurer() {
    }

    public void hookListeners() {
        CoreModel.getDefault().addCProjectDescriptionListener(listener, CProjectDescriptionEvent.LOADED | CProjectDescriptionEvent.APPLIED);
    }

    public void unhookListeners() {
        CoreModel.getDefault().removeCProjectDescriptionListener(listener);
    }

    @SuppressWarnings("unchecked")
    protected void projectChanged(CProjectDescriptionEvent e) {
        IProject project = e.getProject();
        int eventType = e.getEventType();
        ICProjectDescription prjDesc = e.getNewCProjectDescription();
        List<String> newConfigIds = getConfigIDs(prjDesc);

        if (eventType == CProjectDescriptionEvent.LOADED) {
            // initialize entry
            existingConfigIdsMap.put(project, newConfigIds);
        }
        else {
            // see if a new config has popped up
            List<String> oldConfigIds = existingConfigIdsMap.get(project);
            if (oldConfigIds != null && !oldConfigIds.equals(newConfigIds)) {
                // note: CollectionUtils.removeAll() is bogus!!!
                List<String> addedConfigIds = ListUtils.removeAll(newConfigIds, oldConfigIds);
                if (!addedConfigIds.isEmpty())
                    configurationsAdded(prjDesc, addedConfigIds);
            }

            // update entry
            existingConfigIdsMap.put(project, newConfigIds);
        }
    }

    protected List<String> getConfigIDs(ICProjectDescription prjDesc) {
        List<String> result = new ArrayList<String>();
        for (ICConfigurationDescription cfg : prjDesc.getConfigurations())
            result.add(cfg.getId());
        return result;
    }

    protected void configurationsAdded(final ICProjectDescription prjDesc, final List<String> configIds) {
        Debug.println("NewConfigConfigurer: new build configuration(s) detected: " + StringUtils.join(configIds, " "));

        // cannot configure from within the change listener, do it a little later
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                try {
                    configure(prjDesc.getProject(), configIds); // pass IProject because prjDesc may have changed by the time asyncExec() runs
                }
                catch (Exception e) {
                    Activator.logError("Failed to update new build configuration(s) " + StringUtils.join(configIds, ", "), e);
                }
            }
        });
    }

    protected void configure(IProject project, List<String> configIds) throws WriteAccessException, CoreException {
        ICProjectDescription prjDesc = CoreModel.getDefault().getProjectDescription(project);
        if (prjDesc == null)
            return; // project closed or something

        // pick an existing config to copy the source entries from
        ICConfigurationDescription refCfg = null;
        for (ICConfigurationDescription cfg : prjDesc.getConfigurations())
            if (!configIds.contains(cfg.getId()))
            {refCfg = cfg; break;}
        if (refCfg == null)
            return; // nothing to copy from

        boolean changed = false;

        // copy source entries
        ICSourceEntry[] defaultSetting = CDataUtil.adjustEntries((ICSourceEntry[])null, true, project);
        for (String id : configIds) {
            ICConfigurationDescription cfg = prjDesc.getConfigurationById(id);
            if (cfg != null) {
                // config still exists; so if its source entries haven't been changed yet by some other means, update it
                ICSourceEntry[] sourceEntries = cfg.getSourceEntries();
                if (Arrays.equals(sourceEntries, defaultSetting) && !Arrays.equals(sourceEntries, refCfg.getSourceEntries())) {
                    cfg.setSourceEntries(refCfg.getSourceEntries());
                    changed = true;
                }
            }
        }

        // apply project feature settings (provided the project has features defined)
        ProjectFeaturesManager features = new ProjectFeaturesManager(project);
        if (features.loadFeaturesFile()) {
            // convert config IDs to actual configs
            List<ICConfigurationDescription> cfgs = new ArrayList<ICConfigurationDescription>();
            for (String id : configIds) {
                ICConfigurationDescription cfg = prjDesc.getConfigurationById(id);
                if (cfg != null)
                    cfgs.add(cfg);
            }

            // fix them up
            if (!cfgs.isEmpty()) {
                features.fixupConfigurations(cfgs.toArray(new ICConfigurationDescription[]{}), features.getEnabledFeatures());
                changed = true;
            }
        }

        // save
        if (changed)
            CoreModel.getDefault().setProjectDescription(project, prjDesc);
    }
}

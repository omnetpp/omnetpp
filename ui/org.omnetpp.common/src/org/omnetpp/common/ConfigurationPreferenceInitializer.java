/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common;

import java.io.File;

import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;
import org.omnetpp.common.util.LicenseUtils;
import org.omnetpp.common.util.ProcessUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Class used to initialize default preference values.
 */
public class ConfigurationPreferenceInitializer extends AbstractPreferenceInitializer {
    public void initializeDefaultPreferences() {
        IPreferenceStore store = CommonPlugin.getConfigurationPreferenceStore();
        store.setDefault(IConstants.PREF_OMNETPP_ROOT, getOmnetppRootDefault().toOSString());
        IPath omnetppRoot = new Path(store.getString(IConstants.PREF_OMNETPP_ROOT));
        if (omnetppRoot.segmentCount() > 0)
            store.setDefault(IConstants.PREF_OMNETPP_IMAGE_PATH, omnetppRoot.append("images").toOSString());
        store.setDefault(IConstants.PREF_GRAPHVIZ_DOT_EXECUTABLE, getGraphvizDotExecutableDefault());
        store.setDefault(IConstants.PREF_DOXYGEN_EXECUTABLE, getDoxygenExecutableDefault());
        store.setDefault(IConstants.PREF_COPYRIGHT_LINE, "");
        store.setDefault(IConstants.PREF_DEFAULT_LICENSE, IConstants.IS_COMMERCIAL ? LicenseUtils.CUSTOM : LicenseUtils.LGPL);
        store.setDefault(IConstants.PREF_CUSTOM_LICENSE_HEADER, LicenseUtils.DEFAULT_CUSTOM_LICENSE_HEADER);
    }

    private IPath getOmnetppRootDefault() {
        String omnetppRootEnv = System.getenv("OMNETPP_ROOT");
        if (StringUtils.isNotEmpty(omnetppRootEnv)) {
            IPath omnetppRootEnvPath =  new Path(omnetppRootEnv);
            if (containsConfigFiles(omnetppRootEnvPath))
                return omnetppRootEnvPath;
        }

        // look for the Makefile.inc or configuser.vc in parent directories
        IPath path = new Path(Platform.getInstallLocation().getURL().getFile());
        while (!path.isEmpty() && !path.isRoot()) {
            path = path.removeLastSegments(1);
            if (containsConfigFiles(path))
                return path;
        }

        // determine the omnetpp_root during development
        String bundleLocationStr = CommonPlugin.getDefault().getBundle().getLocation(); // during development, this will be "reference:file:/C:/.../omnetpp/ui/org.omnetpp.common"
        Path bundleLocation = cleanupBundleLocation(bundleLocationStr);
        if (bundleLocation.removeLastSegments(1).lastSegment().equals("ui"))
            return bundleLocation.removeLastSegments(2);

        return new Path("");
    }

    private Path cleanupBundleLocation(String location) {
        location = StringUtils.removeStart(location, "reference:file:");
        if (location.matches("^/.+:.*"))  // treat "/C:/..." mishap, strip leading slash
            location = location.substring(1);
        return new Path(location);
    }

    private boolean containsConfigFiles(IPath path) {
        return new File(path.append("configuser.vc").toString()).exists() || new File(path.append("Makefile.inc").toString()).exists();
    }

    private String getGraphvizDotExecutableDefault() {
        return ProcessUtils.lookupExecutable("dot") != null ? "dot" : "";
    }

    private String getDoxygenExecutableDefault() {
        return ProcessUtils.lookupExecutable("doxygen") != null ? "doxygen" : "";
    }
}

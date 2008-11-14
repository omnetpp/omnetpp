package org.omnetpp.common;

import java.io.File;

import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;
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
	}
	
	private IPath getOmnetppRootDefault() {
	    String omnetppRootEnv = System.getenv("OMNETPP_ROOT");
	    if (StringUtils.isNotEmpty(omnetppRootEnv)) {
	        IPath omnetppRootEnvPath =  new Path(omnetppRootEnv);
	        if (containsConfigFiles(omnetppRootEnvPath))
	            return omnetppRootEnvPath;
	    }
	        
        Path platformPath = new Path(Platform.getInstallLocation().getURL().getFile());
        if (containsConfigFiles(platformPath.removeLastSegments(1)))
            return platformPath.removeLastSegments(1);

        // TODO look for the Makefile.inc or configuser.vc in parent directories etc.
        return new Path("");
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

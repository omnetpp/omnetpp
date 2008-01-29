package org.omnetpp.ide.preferences;

import java.io.File;

import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;
import org.omnetpp.common.util.ProcessUtils;
import org.omnetpp.ide.OmnetppMainPlugin;

/**
 * Class used to initialize default preference values.
 */
public class OmnetppPreferenceInitializer extends AbstractPreferenceInitializer {
	public void initializeDefaultPreferences() {
		IPreferenceStore store = OmnetppMainPlugin.getDefault().getPreferenceStore();
		store.setDefault(OmnetppPreferencePage.OMNETPP_ROOT, getOmnetppRootDefault());
		store.setDefault(OmnetppPreferencePage.GRAPHVIZ_DOT_EXECUTABLE, getGraphvizDotExecutableDefault());
        store.setDefault(OmnetppPreferencePage.DOXYGEN_EXECUTABLE, getDoxygenExecutableDefault());
	}
	
	private String getOmnetppRootDefault() {
	    String omnetppRootEnv = System.getenv("OMNETPP_ROOT");
	    if (omnetppRootEnv != null && containsConfigFiles(omnetppRootEnv))
	        return omnetppRootEnv;
	        
        Path platformPath = new Path(Platform.getInstallLocation().getURL().getFile());
        if (containsConfigFiles(platformPath.append("..").toString()))
            return platformPath.removeLastSegments(1).toString();

        // TODO look for the Makefile.inc or configuser.vc in parent directories etc.
        return "";
	}
	
	private boolean containsConfigFiles(String path) {
	    return new File(path+"/configuser.vc").exists() || new File(path+"/Makefile.inc").exists();  
	}
	
	private String getGraphvizDotExecutableDefault() {
        return ProcessUtils.lookupExecutable("dot") != null ? "dot" : "";
	}
	
	private String getDoxygenExecutableDefault() {
	    return ProcessUtils.lookupExecutable("doxygen") != null ? "doxygen" : "";
	}
}

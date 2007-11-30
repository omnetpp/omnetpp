package org.omnetpp.cdt.preferences;

import java.io.File;

import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

import org.omnetpp.cdt.Activator;

/**
 * Class used to initialize default preference values.
 */
public class PreferenceInitializer extends AbstractPreferenceInitializer {

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer#initializeDefaultPreferences()
	 */
	public void initializeDefaultPreferences() {
		IPreferenceStore store = Activator.getDefault().getPreferenceStore();
		store.setDefault(OmnetppCdtPreferencePage.OMNETPP_ROOT, getOmnetppRootDefault());
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

}

package org.eclipse.cdt.msw.build.ui;

import org.eclipse.cdt.msw.build.Activator;
import org.eclipse.cdt.utils.WindowsRegistry;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;
import org.omnetpp.common.util.StringUtils;

public class MSVCPreferenceInitializer extends AbstractPreferenceInitializer {
    public final static String[] VC_VERSIONS = {
        "9.0","8.0","7.1","7.0",
    };
    public final static String[] SDK_VERSIONS = {
        "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.1",
        "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.0A",
        "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.0",
    };
    public final static String[] SDK_VC_VERSIONS = {
        "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.1\\WinSDKCompiler",
        "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.0A\\WinSDKCompiler",
        "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.0\\WinSDKCompiler",
    };

    public MSVCPreferenceInitializer() {
    }

    @Override
    public void initializeDefaultPreferences() {
        IPreferenceStore store = Activator.getDefault().getPreferenceStore();

        String sdkDir = StringUtils.nullToEmpty(detectSDKDir());
        store.setDefault(MSVCPreferencePage.PREFKEY_SDKDIR, sdkDir);

        String vcDir = StringUtils.nullToEmpty(detectVCDir());
        store.setDefault(MSVCPreferencePage.PREFKEY_VCDIR, vcDir);

        String vsDir = StringUtils.nullToEmpty(detectVSDir());
        store.setDefault(MSVCPreferencePage.PREFKEY_VSDIR, vsDir);
}

    /**
     * The Windows SDK installation directory (the highest version possible).
     * First looks for MSSdk environment variable then looks in system registry.
     * (OS specific format)  
     * Returns NULL if not installed.
     */
    public static String detectSDKDir() {
        // first check for environment variable
        String sdkDir = System.getenv("MSSdk");
        if (sdkDir != null)
            return sdkDir;
    
        WindowsRegistry reg = WindowsRegistry.getRegistry();
        if (reg == null)
            return null;
    
        for (int i = 0; i < SDK_VERSIONS.length; ++i) {
            sdkDir = reg.getLocalMachineValue(SDK_VERSIONS[i], "InstallationFolder");
            if (sdkDir != null)
                return sdkDir;
        }
        return null;
    }

    /**
     * The Visual C installation directory (the highest version possible)
     * First looks for VCINSTALLDIR environment variable then in the registry for
     * Visual Studio then for SDK installations and tries to find VC. (OS specific format) 
     * Returns NULL if VC is not found.
     */
    public static String detectVCDir() {
        // first check for environment variable
        String vcDir = System.getenv("VCINSTALLDIR");
        if (vcDir != null)
            return vcDir;
        
    	WindowsRegistry reg = WindowsRegistry.getRegistry();
    	if (reg == null)
    		return null;
    
    	// try to detect visual c
        for (int i = 0; i < VC_VERSIONS.length; ++i) {
            vcDir = reg.getLocalMachineValue("SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7", VC_VERSIONS[i]);
            if (vcDir != null) 
                return vcDir;
        }
        
        // Try a full SDK with compiler if no standalone visual c detected
        for (int i = 0; i < SDK_VC_VERSIONS.length; ++i) {
            String sdkVCDir = reg.getLocalMachineValue(SDK_VC_VERSIONS[i], "InstallationFolder");
            if (sdkVCDir != null) 
                return new Path(sdkVCDir).append("VC").toOSString();
        }
    	
    	return null;
    }

    /**
     * The Visual Studio installation directory (the highest version possible)
     * First checks the VSINSTALLDIR environment variable then the registry.
     * (OS specific format)
     * Returns NULL if not installed.
     */
    public static String detectVSDir() {
        // first check for environment variable
        String vsDir = System.getenv("VSINSTALLDIR");
        if (vsDir != null)
            return vsDir;
    
        WindowsRegistry reg = WindowsRegistry.getRegistry();
    	if (reg == null)
    		return null;
    
        for (int i = 0; i < VC_VERSIONS.length; ++i) {
            vsDir = reg.getLocalMachineValue("SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7", VC_VERSIONS[i]);
            if (vsDir != null) 
                return vsDir;
        }
    	
        return null;
    }
}

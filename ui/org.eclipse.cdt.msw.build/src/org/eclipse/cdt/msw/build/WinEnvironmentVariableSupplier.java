package org.eclipse.cdt.msw.build;


import java.util.HashMap;
import java.util.Map;

import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedIsToolChainSupported;
import org.eclipse.cdt.managedbuilder.core.IManagedProject;
import org.eclipse.cdt.managedbuilder.core.IToolChain;
import org.eclipse.cdt.managedbuilder.envvar.IBuildEnvironmentVariable;
import org.eclipse.cdt.managedbuilder.envvar.IConfigurationEnvironmentVariableSupplier;
import org.eclipse.cdt.managedbuilder.envvar.IEnvironmentVariableProvider;
import org.eclipse.cdt.managedbuilder.envvar.IProjectEnvironmentVariableSupplier;
import org.eclipse.cdt.utils.WindowsRegistry;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.PluginVersionIdentifier;

/**
 * Visual C environment detector. Tries to detect Visual C and (optional) MS Windows SDK
 * installation. (Returns the latest version) Also checks whether the tool-chain is installed at all.
 * Looks for environment variables VCINSTALLDIR, VSINSTALLDIR, MSSdk. If you have several versions
 * of SDK / Visual C on your machine, and need a specific one (not the latest) define these variables
 * to point to the needed installation.
 * 
 * @author DSchaefer 
 * @author rhornig
 */
public class WinEnvironmentVariableSupplier
	implements IConfigurationEnvironmentVariableSupplier, IProjectEnvironmentVariableSupplier,
	IManagedIsToolChainSupported {
    private final static String[] VC_VERSIONS = {
            "9.0","8.0","7.1","7.0",
    };
    private final static String[] SDK_VERSIONS = {
            "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.1",
            "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.0A",
            "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.0",
    };
    private final static String[] SDK_VC_VERSIONS = {
        "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.1\\WinSDKCompiler",
        "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.0A\\WinSDKCompiler",
        "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.0\\WinSDKCompiler",
};
	
	private Map<String, IBuildEnvironmentVariable> envvars;
	
	private static class WindowsBuildEnvironmentVariable implements IBuildEnvironmentVariable {
		
		private final String name;
		private final String value;
		private final int operation;
		
		public WindowsBuildEnvironmentVariable(String name, String value, int operation) {
			this.name = name;
			this.value = value;
			this.operation = operation;
		}
		
		public String getDelimiter() {
			return ";";
		}
		
		public String getName() {
			return name;
		}
		
		public String getValue() {
			return value;
		}

		public int getOperation() {
			return operation;
		}

	}

	public IBuildEnvironmentVariable getVariable(String variableName,
			IManagedProject project, IEnvironmentVariableProvider provider) {
		if (envvars == null)
			initvars();
		return envvars.get(variableName);
	}
	
	public IBuildEnvironmentVariable getVariable(String variableName,
			IConfiguration configuration, IEnvironmentVariableProvider provider) {
		if (envvars == null)
			initvars();
		return envvars.get(variableName);
	}

	public IBuildEnvironmentVariable[] getVariables(IManagedProject project,
			IEnvironmentVariableProvider provider) {
		if (envvars == null)
			initvars();
		return envvars.values().toArray(new IBuildEnvironmentVariable[envvars.size()]);
	}
	
	public IBuildEnvironmentVariable[] getVariables(
			IConfiguration configuration, IEnvironmentVariableProvider provider) {
		if (envvars == null)
			initvars();
		return envvars.values().toArray(new IBuildEnvironmentVariable[envvars.size()]);
	}
	
	private void addvar(IBuildEnvironmentVariable var) {
		envvars.put(var.getName(), var);
	}
	
    /**
     * The Windows SDK installation directory (the highest version possible).
     * First looks for MSSdk environment variable then looks in system registry.
     * (OS specific format)  
     * Returns NULL if not installed.
     */
	public static String getSDKDir() {
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
	public static String getVCDir() {
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
	public static String getVSDir() {
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
	
	private void initvars() {
		envvars = new HashMap<String, IBuildEnvironmentVariable>();
		
		String vcDir = getVCDir();
		if (vcDir == null)
			return;
		
		// The SDK Location
		String sdkDir = getSDKDir();
		
		// INCLUDE
		StringBuffer buff = new StringBuffer();

		buff.append(new Path(vcDir).append("INCLUDE").toOSString()+";");
		buff.append(new Path(vcDir).append("INCLUDE\\SYS").toOSString()+";");
		
		if (sdkDir != null) {
		    buff.append(new Path(sdkDir).append("Include").toOSString()+";");
		    buff.append(new Path(sdkDir).append("Include\\gl").toOSString()+";");
		}
		addvar(new WindowsBuildEnvironmentVariable("INCLUDE", buff.toString(), IBuildEnvironmentVariable.ENVVAR_PREPEND));

		// LIB
		buff = new StringBuffer();
		buff.append(new Path(vcDir).append("LIB").toOSString()+";");
		if (sdkDir != null)
		    buff.append(new Path(sdkDir).append("Lib").toOSString()+";");
		addvar(new WindowsBuildEnvironmentVariable("LIB", buff.toString(), IBuildEnvironmentVariable.ENVVAR_PREPEND));
		
		// PATH
		buff = new StringBuffer();
		
		String vsDir = getVSDir();
		if (vsDir != null)
			buff.append(new Path(vsDir).append("Common7\\IDE").toOSString()+";");
		
		buff.append(new Path(vcDir).append("Bin").toOSString()+";");
        if (sdkDir != null)
            buff.append(new Path(sdkDir).append("Bin").toOSString()+";");
		addvar(new WindowsBuildEnvironmentVariable("PATH", buff.toString(), IBuildEnvironmentVariable.ENVVAR_PREPEND));
	}

    public boolean isSupported(IToolChain toolChain, PluginVersionIdentifier version, String instance) {
        return getVCDir() != null;
    }
}

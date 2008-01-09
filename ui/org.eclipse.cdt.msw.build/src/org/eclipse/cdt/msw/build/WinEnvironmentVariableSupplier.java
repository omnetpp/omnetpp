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
import org.eclipse.core.runtime.PluginVersionIdentifier;

/**
 * @author DSchaefer
 * Visual C environment detector. Tries to detect Visual C and (optional) MS Windows SDK
 * installation. (Returns the latest version) Also checks whether the toolchain is installed at all.
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
     * Return NULL if not installed.
     */
	public static String getSDKDir() {
		WindowsRegistry reg = WindowsRegistry.getRegistry();
		if (reg == null)
			return null;
		
		for (int i = 0; i < SDK_VERSIONS.length; ++i) {
			String sdkDir = reg.getLocalMachineValue(SDK_VERSIONS[i], "InstallationFolder");
			if (sdkDir != null)
				return sdkDir;
		}
		
		return null;
	}
	
    /**
     * The Visual C installation directory (the highest version possible)
     * First looks in Visual Studio then in SDK and tries to find VC.  
     * Return NULL if not installed.
     */
	// TODO add VCDIR environment var detection too
	public static String getVCDir() {
		WindowsRegistry reg = WindowsRegistry.getRegistry();
		if (reg == null)
			return null;

		// try to detect visual c
        for (int i = 0; i < VC_VERSIONS.length; ++i) {
            String vcDir = reg.getLocalMachineValue("SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7", VC_VERSIONS[i]);
            if (vcDir != null) 
                return vcDir;
        }
        
        // Try a full SDK with compiler if no standalone visual c detected
        for (int i = 0; i < SDK_VC_VERSIONS.length; ++i) {
            String sdkVCDir = reg.getLocalMachineValue(SDK_VC_VERSIONS[i], "InstallationFolder");
            if (sdkVCDir != null) 
                return sdkVCDir.concat("VC\\");
        }
		
		return null;
	}
	
	/**
	 * The Visual Studio installation directory (the highest version possible)
     * Return NULL if not installed.
	 */
	public static String getVSDir() {
		WindowsRegistry reg = WindowsRegistry.getRegistry();
		if (reg == null)
			return null;

        for (int i = 0; i < VC_VERSIONS.length; ++i) {
            String vsDir = reg.getLocalMachineValue("SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7", VC_VERSIONS[i]);
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

		buff.append(vcDir).append("INCLUDE;");
		buff.append(vcDir).append("INCLUDE\\SYS;");
		
		if (sdkDir != null) {
		    buff.append(sdkDir).append("Include;");
		    buff.append(sdkDir).append("Include\\gl;");
		}
		addvar(new WindowsBuildEnvironmentVariable("INCLUDE", buff.toString(), IBuildEnvironmentVariable.ENVVAR_PREPEND));

		// LIB
		buff = new StringBuffer();
		buff.append(vcDir).append("LIB;");
		if (sdkDir != null)
		    buff.append(sdkDir).append("Lib;");
		addvar(new WindowsBuildEnvironmentVariable("LIB", buff.toString(), IBuildEnvironmentVariable.ENVVAR_PREPEND));
		
		// PATH
		buff = new StringBuffer();
		
		String vsDir = getVSDir();
		if (vsDir != null)
			buff.append(vsDir).append("Common7\\IDE;");
		
		buff.append(vcDir).append("Bin;");
        if (sdkDir != null)
            buff.append(sdkDir).append("Bin;");
		addvar(new WindowsBuildEnvironmentVariable("PATH", buff.toString(), IBuildEnvironmentVariable.ENVVAR_PREPEND));
	}

    public boolean isSupported(IToolChain toolChain, PluginVersionIdentifier version, String instance) {
        return getVCDir() != null;
    }

}

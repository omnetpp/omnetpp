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
import org.eclipse.cdt.msw.build.ui.MSVCPreferencePage;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.PluginVersionIdentifier;
import org.omnetpp.common.util.StringUtils;

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
     * Returns the SDK dir from the preferences; null if unset.
     */
	public static String getSDKDir() {
	    String sdkDir = Activator.getDefault().getPreferenceStore().getString(MSVCPreferencePage.PREFKEY_SDKDIR);
	    return StringUtils.isEmpty(sdkDir) ? null : sdkDir;
	}

    /**
	 * Returns the VC dir from the preferences; null if unset.
	 */
	public static String getVCDir() {
	    String vcDir = Activator.getDefault().getPreferenceStore().getString(MSVCPreferencePage.PREFKEY_VCDIR);
        return StringUtils.isEmpty(vcDir) ? null : vcDir;
	}

    /**
     * Returns the VS dir from the preferences; null if unset.
     */
    public static String getVSDir() {
        String vsDir = Activator.getDefault().getPreferenceStore().getString(MSVCPreferencePage.PREFKEY_VSDIR);
        return StringUtils.isEmpty(vsDir) ? null : vsDir;
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
		buff.append(new Path(vcDir).append("INCLUDE\\SYS").toOSString()+";");  //FIXME needed????
		
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

package org.omnetpp.cdt;

import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.envvar.IBuildEnvironmentVariable;
import org.eclipse.cdt.managedbuilder.envvar.IConfigurationEnvironmentVariableSupplier;
import org.eclipse.cdt.managedbuilder.envvar.IEnvironmentVariableProvider;
import org.eclipse.cdt.utils.WindowsRegistry;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;

/**
 * NOTE: This is an EXACT copy of MingwEnvironmentVariableSupplier in the CDT;
 * we'd want to subclass it but it's not exported from its plugin, so we had to
 * copy it.  --Andras
 * 
 * 
 * @author Doug Schaefer
 *
 */
public class MingwEnvironmentVariableSupplier_COPY implements
		IConfigurationEnvironmentVariableSupplier {

	private static class MingwBuildEnvironmentVariable implements IBuildEnvironmentVariable {
		private final String name;
		private final String value;
		private final int operation;
		
		public MingwBuildEnvironmentVariable(String name, String value, int operation) {
			this.name = name;
			this.value = value;
			this.operation = operation;
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
		
		public String getDelimiter() {
			return ";";
		}
	}
	
	private IBuildEnvironmentVariable path;
	
	public static IPath getBinDir() {

		IPath subPath = new Path("mingw\\bin");
		// 1. Try the mingw directory in the platform install directory
		IPath installPath = new Path(Platform.getInstallLocation().getURL().getFile());
		IPath binPath = installPath.append(subPath);
		if (binPath.toFile().isDirectory())
			return binPath;
		
		// 2. Try the directory above the install dir
		binPath = installPath.removeLastSegments(1).append(subPath);
		if (binPath.toFile().isDirectory())
			return binPath;
		
		// 3. Try looking if the mingw installer ran
		WindowsRegistry registry = WindowsRegistry.getRegistry();
		if (registry==null) return null; // probably not even windows
		
		String mingwPath = registry.getLocalMachineValue(
					"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\MinGW",
					"InstallLocation");
		if (mingwPath != null) {
			binPath = new Path(mingwPath).append("bin");
			if (binPath.toFile().isDirectory())
				return binPath;
		}
		
		// 4. Try the default MinGW install dir
		binPath = new Path("C:\\MinGW\\bin");
		if (binPath.toFile().isDirectory())
			return binPath;
		
		// No dice, return null
		return null;
	}
	
	public static IPath getMsysBinDir() {
		// Just look in the install location parent dir
		IPath installPath = new Path(Platform.getInstallLocation().getURL().getFile()).removeLastSegments(1);
		IPath msysBinPath = installPath.append("msys\\bin");
		return msysBinPath.toFile().isDirectory() ? msysBinPath : null;
	}
	
	public MingwEnvironmentVariableSupplier_COPY() {
		IPath binPath = getBinDir();
		if (binPath != null) {
			String pathStr = binPath.toOSString();
			IPath msysBinPath = getMsysBinDir();
			if (msysBinPath != null)
				pathStr += ';' + msysBinPath.toOSString();
			
			path = new MingwBuildEnvironmentVariable("PATH", pathStr, IBuildEnvironmentVariable.ENVVAR_PREPEND);
		}
	}
	
	public IBuildEnvironmentVariable getVariable(String variableName,
			IConfiguration configuration, IEnvironmentVariableProvider provider) {
		if (path != null && variableName.equals(path.getName()))
			return path;
		else
			return null;
	}

	public IBuildEnvironmentVariable[] getVariables(
			IConfiguration configuration, IEnvironmentVariableProvider provider) {
		return path != null
			? new IBuildEnvironmentVariable[] { path }
			: new IBuildEnvironmentVariable[0];
	}

}

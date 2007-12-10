package org.eclipse.cdt.msw.build;


import java.util.HashMap;
import java.util.Map;

import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredPathInfo;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredScannerInfoSerializable;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;

/**
 * @author Doug Schaefer
 *
 */
public class WinDiscoveredPathInfo implements IDiscoveredPathInfo {

	private final IPath[] paths;
	private final Map<String, String> symbols = new HashMap<String, String>();
	
	public WinDiscoveredPathInfo() {
		// Include paths
		String sdkDir = WinEnvironmentVariableSupplier.getSDKDir();
		if (sdkDir != null) {
			String vcDir = WinEnvironmentVariableSupplier.getVCDir();
			paths = new IPath[] {
				new Path(vcDir.concat("Include")),
				new Path(vcDir.concat("Include\\Sys")),
				new Path(sdkDir.concat("Include")),
				new Path(sdkDir.concat("Include\\gl"))
			};
		} else
			paths = new IPath[0];
				
		symbols.put("_M_IX86", "600");
		symbols.put("_WIN32", "1");
		symbols.put("_MSC_VER", "1400");
	}
	
	public IPath[] getIncludePaths() {
		return paths;
	}

	public IProject getProject() {
		return null;
	}

	public IDiscoveredScannerInfoSerializable getSerializable() {
		return null;
	}

	public Map<String, String> getSymbols() {
		return symbols;
	}

}

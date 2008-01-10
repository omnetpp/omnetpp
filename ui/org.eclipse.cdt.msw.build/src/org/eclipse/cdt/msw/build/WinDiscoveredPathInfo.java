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
 * Discover the include path for the VC installation and optionally for the SDK
 */
public class WinDiscoveredPathInfo implements IDiscoveredPathInfo {

	private final IPath[] paths;
	private final Map<String, String> symbols = new HashMap<String, String>();
	
	public WinDiscoveredPathInfo() {
		// Include paths
		String sdkDir = WinEnvironmentVariableSupplier.getSDKDir();
		String vcDir = WinEnvironmentVariableSupplier.getVCDir();
		if (vcDir != null ) {
		    if (sdkDir != null) {
		        paths = new IPath[] {
		                new Path(vcDir).append("INCLUDE"),
		                new Path(vcDir).append("INCLUDE\\SYS"),
		                new Path(sdkDir).append("Include"),
		                new Path(sdkDir).append("Include\\gl")
		        };
		    }
		    else {
                paths = new IPath[] {
                        new Path(vcDir).append("INCLUDE"),
                        new Path(vcDir).append("INCLUDE\\SYS"),
                };
		    }
		} 
		else
			paths = new IPath[0];
				
		// FIXME return correct version numbers
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

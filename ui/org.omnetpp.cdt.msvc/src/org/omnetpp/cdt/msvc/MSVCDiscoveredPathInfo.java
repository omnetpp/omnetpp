package org.omnetpp.cdt.msvc;


import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredPathInfo;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredScannerInfoSerializable;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.makefile.MakefileTools;

/**
 * Discover the include path for the VC installation and optionally for the SDK
 * @author Doug Schaefer, rhornig, andras
 */
public class MSVCDiscoveredPathInfo implements IDiscoveredPathInfo {

	private IPath[] paths;
	private Map<String, String> symbols = new HashMap<String, String>();
	
	public MSVCDiscoveredPathInfo(IProject project) {
		// Include paths
		String sdkDir = MSVCEnvironmentVariableSupplier.getSDKDir();
		String vcDir = MSVCEnvironmentVariableSupplier.getVCDir();
		
		List<IPath> paths = new ArrayList<IPath>(); 
		if (vcDir != null ) {
		    paths.add(new Path(vcDir).append("INCLUDE"));
		    paths.add(new Path(vcDir).append("INCLUDE\\SYS"));  //FIXME needeD?
		    if (sdkDir != null) {
		        paths.add(new Path(sdkDir).append("Include"));
		        paths.add(new Path(sdkDir).append("Include\\gl"));  //FIXME others? atl,mfc,...
		    }
		} 
				
	    // add include dirs needed for OMNeT++
        try {
            ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project);
            paths.addAll(MakefileTools.getOmnetppIncludeLocationsForProject(projectDescription));
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
        
        this.paths = paths.toArray(new IPath[]{});
		
		// add Visual C++ predefined symbols
		symbols.put("_M_IX86", "600");
		symbols.put("_WIN32", "1");

		symbols.put("_MSC_VER", "1400"); // FIXME use correct version number
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

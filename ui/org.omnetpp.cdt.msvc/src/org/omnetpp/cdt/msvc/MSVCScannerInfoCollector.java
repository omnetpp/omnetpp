/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.msvc;


import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredPathInfo;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredScannerInfoSerializable;
import org.eclipse.cdt.make.core.scannerconfig.IScannerInfoCollector3;
import org.eclipse.cdt.make.core.scannerconfig.InfoContext;
import org.eclipse.cdt.make.core.scannerconfig.ScannerInfoTypes;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.ide.OmnetppMainPlugin;

/**
 * 
 * @author Andras Varga, Doug Schaefer
 */
public class MSVCScannerInfoCollector implements IScannerInfoCollector3 {
    private InfoContext context;

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
                paths.add(new Path(vcDir).append("INCLUDE\\SYS"));  //XXX likely not needed
                if (sdkDir != null) {
                    paths.add(new Path(sdkDir).append("Include"));
                    paths.add(new Path(sdkDir).append("Include\\gl"));
                }
            }

            // add the omnetpp include directory
            paths.add(new Path(OmnetppMainPlugin.getOmnetppInclDir()));

            // add the folders inside the project and in referenced projects
            // Note: we MUST NOT compute the folders here, we must not access the CDT project configuration, see bug #299
            List<IContainer> folders = Activator.getIncludeFoldersCache().getProjectIncludeFolders(project);
            for (IContainer folder : folders)
                paths.add(folder.getLocation());
            
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

    
	@SuppressWarnings("rawtypes")
    public void contributeToScannerConfig(Object resource, Map scannerInfo) {
	}

	@SuppressWarnings("rawtypes")
    public List getCollectedScannerInfo(Object resource, ScannerInfoTypes type) {
		return null;
	}

	public IDiscoveredPathInfo createPathInfoObject() {
	    Assert.isTrue(context != null);
		return new MSVCDiscoveredPathInfo(context.getProject());
	}

	public void setInfoContext(InfoContext context) {
	    this.context = context;
	}

	public void setProject(IProject project) {
	    // CDT apparently never invokes this
	}

	public void updateScannerConfiguration(IProgressMonitor monitor) throws CoreException {
	}

}

package org.omnetpp.cdt;

import java.util.List;
import java.util.Map;

import org.eclipse.cdt.make.core.scannerconfig.IScannerInfoCollector3;
import org.eclipse.cdt.make.core.scannerconfig.InfoContext;
import org.eclipse.cdt.make.core.scannerconfig.ScannerInfoTypes;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredPathInfo;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredScannerInfoSerializable;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;

// FIXME implement it or remove totally
public class OmnetppScannerInfoCollector implements IScannerInfoCollector3 {

    public void contributeToScannerConfig(Object resource, Map scannerInfo) {
        // TODO Auto-generated method stub

    }

    public List getCollectedScannerInfo(Object resource, ScannerInfoTypes type) {
        // TODO Auto-generated method stub
        return null;
    }

    public void setInfoContext(InfoContext context) {
        // TODO Auto-generated method stub
        
    }

    public IDiscoveredPathInfo createPathInfoObject() {
        // TODO Auto-generated method stub
        return new IDiscoveredPathInfo() {

            public IPath[] getIncludePaths() {
                // TODO Auto-generated method stub
                return new Path[] { new Path("hello") };
            }

            public IProject getProject() {
                // TODO Auto-generated method stub
                return null;
            }

            public IDiscoveredScannerInfoSerializable getSerializable() {
                // TODO Auto-generated method stub
                return null;
            }

            public Map getSymbols() {
                // TODO Auto-generated method stub
                return null;
            }
            
        };
    }

    public void setProject(IProject project) {
        // TODO Auto-generated method stub
        
    }

    public void updateScannerConfiguration(IProgressMonitor monitor) throws CoreException {
        // TODO Auto-generated method stub
        
    }

}

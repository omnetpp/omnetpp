package org.eclipse.cdt.msw.build;


import java.util.List;
import java.util.Map;

import org.eclipse.cdt.make.core.scannerconfig.IScannerInfoCollector3;
import org.eclipse.cdt.make.core.scannerconfig.InfoContext;
import org.eclipse.cdt.make.core.scannerconfig.ScannerInfoTypes;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredPathInfo;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;

/**
 * @author Doug Schaefer
 *
 */
public class WinScannerInfoCollector implements IScannerInfoCollector3 {

	public void contributeToScannerConfig(Object resource, Map scannerInfo) {
	}

	public List getCollectedScannerInfo(Object resource, ScannerInfoTypes type) {
		return null;
	}

	public IDiscoveredPathInfo createPathInfoObject() {
		return new WinDiscoveredPathInfo();
	}
	
	public void setInfoContext(InfoContext context) {
	}
	
	public void setProject(IProject project) {
	}
	
	public void updateScannerConfiguration(IProgressMonitor monitor) throws CoreException {
	}
	
}

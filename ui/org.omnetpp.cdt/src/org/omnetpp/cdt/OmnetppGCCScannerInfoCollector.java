package org.omnetpp.cdt;

import java.util.LinkedHashMap;

import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredPathInfo;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IPerProjectDiscoveredPathInfo;
import org.eclipse.cdt.make.internal.core.scannerconfig2.PerProjectSICollector;
import org.eclipse.cdt.managedbuilder.scannerconfig.IManagedScannerInfoCollector;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.omnetpp.cdt.makefile.MakefileTools;

/**
 * Special InfoCollector based on the CDT's GCC per project collector, but adds
 * the omnetpp/include directory to the includes.
 *
 */
@SuppressWarnings("restriction")
public class OmnetppGCCScannerInfoCollector extends PerProjectSICollector
		implements IManagedScannerInfoCollector {

	@SuppressWarnings("unchecked")
	@Override
	public IDiscoveredPathInfo createPathInfoObject() {
		IPerProjectDiscoveredPathInfo pathInfoObject = (IPerProjectDiscoveredPathInfo)super.createPathInfoObject();
		
		// add include dirs needed for OMNeT++
		LinkedHashMap includeMap = pathInfoObject.getIncludeMap();
		try {
            for (IPath path : MakefileTools.getOmnetppIncludeDirsForProject(getContext().getProject()))
                includeMap.put(path.toOSString(), false);
            pathInfoObject.setIncludeMap(includeMap);
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
		return pathInfoObject;
	}
}

package org.omnetpp.cdt;

import java.util.LinkedHashMap;

import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredPathInfo;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IPerProjectDiscoveredPathInfo;
import org.eclipse.cdt.make.internal.core.scannerconfig2.PerProjectSICollector;
import org.eclipse.cdt.managedbuilder.scannerconfig.IManagedScannerInfoCollector;
import org.omnetpp.ide.OmnetppMainPlugin;

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
		
		// add the omnetpp include directory
		LinkedHashMap includeMap = pathInfoObject.getIncludeMap();
		includeMap.put(OmnetppMainPlugin.getOmnetppInclDir(), false);
		pathInfoObject.setIncludeMap(includeMap);
		
		return pathInfoObject;
	}

}

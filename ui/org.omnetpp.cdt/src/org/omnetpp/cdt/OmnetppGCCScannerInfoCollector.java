/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt;

import java.io.File;
import java.util.LinkedHashMap;
import java.util.List;

import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredPathInfo;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IPerProjectDiscoveredPathInfo;
import org.eclipse.cdt.make.internal.core.scannerconfig2.PerProjectSICollector;
import org.eclipse.cdt.managedbuilder.scannerconfig.IManagedScannerInfoCollector;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IProject;
import org.omnetpp.common.OmnetppDirs;

/**
 * Special InfoCollector based on the CDT's GCC per project collector, but adds
 * the omnetpp/include directory to the includes.
 */
@SuppressWarnings("restriction")
public class OmnetppGCCScannerInfoCollector extends PerProjectSICollector
        implements IManagedScannerInfoCollector {

    @Override
    public IDiscoveredPathInfo createPathInfoObject() {
        IPerProjectDiscoveredPathInfo pathInfoObject = (IPerProjectDiscoveredPathInfo)super.createPathInfoObject();

        // add include dirs needed for OMNeT++
        LinkedHashMap<String,Boolean> includeMap = pathInfoObject.getIncludeMap();
        IProject project = getContext().getProject();

        // remove the entries that point to a non-existent dir
        for (Object location : includeMap.keySet())
            if (location instanceof String && !new File((String)location).exists())
                includeMap.put((String)location, true);

        // add the omnetpp include directory
        includeMap.put(OmnetppDirs.getOmnetppInclDir(), false);

        // add the folders inside the project and in referenced projects
        // Note: we MUST NOT compute the folders here, we must not access the CDT project configuration, see bug #299
        List<IContainer> folders = Activator.getIncludeFoldersCache().getProjectDeepIncludeFolders(project);
        for (IContainer folder : folders)
            includeMap.put(folder.getLocation().toOSString(), false);

        // set it back
        pathInfoObject.setIncludeMap(includeMap);

        return pathInfoObject;
    }

}

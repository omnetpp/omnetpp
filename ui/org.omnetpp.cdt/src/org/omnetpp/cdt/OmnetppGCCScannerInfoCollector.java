package org.omnetpp.cdt;

import java.util.LinkedHashMap;

import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IDiscoveredPathInfo;
import org.eclipse.cdt.make.core.scannerconfig.IDiscoveredPathManager.IPerProjectDiscoveredPathInfo;
import org.eclipse.cdt.make.internal.core.scannerconfig2.PerProjectSICollector;
import org.eclipse.cdt.managedbuilder.scannerconfig.IManagedScannerInfoCollector;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.cdt.makefile.MakefileTools;
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

		// add project source directories as include dirs for the indexer
		try {
			for(IContainer incDir : MakefileTools.collectDirs(getContext().getProject(), ".*\\.(h|msg)")) 
				includeMap.put(incDir.getLocation().toString(), false);
		} catch (CoreException e) {
			Activator.logError(e);
		}
		
		pathInfoObject.setIncludeMap(includeMap);
		return pathInfoObject;
	}
	
//	private List<String> collectIncDirs() {
//		List<String> result = new ArrayList<String>();
//		collectDirs(getContext().getProject(), result);
//		return result; 
//	}
//
//    private void collectDirs(IProject proj, final List<String> result)  {
//        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(proj);
//        final ICSourceEntry[] srcEntries = buildInfo.getDefaultConfiguration().getSourceEntries();
//        
//        try {
//            proj.accept(new IResourceVisitor() {
//                public boolean visit(IResource resource) throws CoreException {
//                    if (MakefileTools.isGoodFolder(resource)) {
//                        if (!CDataUtil.isExcluded(resource.getProjectRelativePath(), srcEntries)
//                                && containsFileMatchingPattern((IContainer)resource))
//                            result.add(resource.getLocation().toString());
//                        
//                        return true;
//                    }
//                    return false;
//                }
//            });
//            
//            // collect directories from referenced projects too (recursively)
//            for(IProject refProj : proj.getReferencedProjects())
//                collectDirs(refProj, result);
//        }
//        catch (CoreException e) {
//        	System.out.println(e);
//        }            
//    }
//
//    private boolean containsFileMatchingPattern(IContainer container) {
//        try {
//            for (IResource member : container.members())
//                if (member.getFullPath().toPortableString().endsWith(".h"))
//                    return true;
//        }
//        catch (CoreException e) {
//            return true;
//        }
//        return false;
//    }
    
}

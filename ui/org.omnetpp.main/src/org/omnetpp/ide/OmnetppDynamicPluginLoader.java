/*--------------------------------------------------------------*
  Copyright (C) 2011 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.project.ProjectUtils;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.BundleException;
import org.osgi.framework.startlevel.FrameworkStartLevel;
import org.osgi.framework.wiring.FrameworkWiring;

/**
 * Handles the automatic installation / deinstallation of plugin jar files
 * from the project's "plugins" folder.
 * 
 *   NOTE:The p2 system automatically uninstalls all plugins during the startup
 *   that were not installed via p2. This means that each dynamic plugin is freshly installed
 *   on each IDE restart or on each project close/open meaning that persistent data
 *   for the plugin is not kept. 
 */
public class OmnetppDynamicPluginLoader implements IResourceChangeListener {

    private static final String OMNETPP_BUNDLE_MARKER = "omnetpp_bundle";
    public static final String PLUGINS_FOLDER = "plugins";
    
    protected BundleContext bundleContext;
    
    public OmnetppDynamicPluginLoader(BundleContext bundleContext) throws BundleException {
        this.bundleContext = bundleContext;
        installOmnetppDynamicBundlesInAllOpenProjects();
        startOmnetppDynamicBundles();
    }

    public void resourceChanged(IResourceChangeEvent event) {
        // uninstall the project plugins before closing the projects
        if (event.getType() == IResourceChangeEvent.PRE_CLOSE) {
            IProject project = (IProject)event.getResource();

            if (ProjectUtils.hasOmnetppNature(project)) {
                for (IResource plugin : getBundleFilesFrom(project)) {
                    try {
                        if (plugin.isAccessible())
                            uninstallBundle(plugin);
                    }
                    catch (Exception e) {
                        OmnetppMainPlugin.logError(e);
                    }
                }
            }
        }
        else if (event.getType() == IResourceChangeEvent.POST_CHANGE) {
            final boolean[] startPlugins = { false };
            try {
                event.getDelta().accept(new IResourceDeltaVisitor() {
                    public boolean visit(IResourceDelta resourceDelta) throws CoreException {
                        try {
                            IResource resource = resourceDelta.getResource();
                            // visit the workspace children
                            if (resource.getProject() == null)
                                return true;
                            
                            // ignore closed and non omnetpp projects
                            if (!resource.getProject().isOpen() || !ProjectUtils.hasOmnetppNature(resource.getProject()))
                                return false;

                            // process children if the resource is not a plugin
                            if (!isJarFileInPluginsFolder(resource))
                                return true;
                            
                            int kind = resourceDelta.getKind();
                            if (kind == IResourceDelta.ADDED && resource.isAccessible()) {
                                installBundle(resource);
                                startPlugins[0] = true;  // request to start plugins after installing after processing the delta 
                            }
                            else if (kind == IResourceDelta.REMOVED)
                                uninstallBundle(resource);
                            else if (kind == IResourceDelta.CHANGED && resource.isAccessible()) {
                                uninstallBundle(resource);
                                installBundle(resource);
                                startPlugins[0] = true;
                            }

                            return false;
                        }
                        catch (BundleException e) {
                            OmnetppMainPlugin.logError(e);
                            return false;
                        }
                        catch (IOException e) {
                            OmnetppMainPlugin.logError(e);
                            return false;
                        }
                    }
                });
            } catch (CoreException e) {
                OmnetppMainPlugin.logError(e);
            }
            
            // after installing all new plugins try to start (resolve and activate) them
            if (startPlugins[0])
                startOmnetppDynamicBundles();
        }
    }

    /**
     * Install a bundle from an omnetpp project and mark it so we will recognize it later as
     * an "omnetpp dynamic bundle"
     */
    protected void installBundle(IResource bundleResource) throws BundleException, IOException {
        // set run level 5 for the bundle
        Bundle systemBundle = bundleContext.getBundle(0);
        FrameworkStartLevel fs = (FrameworkStartLevel)systemBundle.adapt(FrameworkStartLevel.class);
        int origStartLevel = fs.getInitialBundleStartLevel();
        fs.setInitialBundleStartLevel(5); // start level for workspace installed bundles. normal IDE plugins are on level 4 

        // install the bundle
        Bundle bundle = bundleContext.installBundle(bundleResource.getLocationURI().toString());
        
        // restore run level
        fs.setInitialBundleStartLevel(origStartLevel);  
        
        // mark the bundle with a marker file in the bundle instance area 
        // so we will recognize later that it was installed from an omnetpp project
        bundle.getDataFile(OMNETPP_BUNDLE_MARKER).createNewFile();
    }

    /**
     * Uninstall a bundle and stop all bundles immediately that depend on it 
     */
    protected void uninstallBundle(IResource bundleResource) throws BundleException {
        Bundle bundle = bundleContext.installBundle(bundleResource.getLocationURI().toString());
        bundle.uninstall();
        
        // access the system bundle
        Bundle systemBundle = bundleContext.getBundle(0);
        FrameworkWiring fw = (FrameworkWiring)systemBundle.adapt(FrameworkWiring.class);
        // stop current bundle pending for uninstallation because otherwise other running bundles
        // depending on this would keep it active  
        fw.refreshBundles(Arrays.asList(new Bundle[] { bundle }));
    }

    /**
     * List dynamic omnetpp plugins that are available in this project. 
     * (closed projects or projects that has no omnetpp nature return no plugins)
     */
    protected static List<IResource> getBundleFilesFrom(IProject project) {
        List<IResource> result = new ArrayList<IResource>();
        if (project.isAccessible() && ProjectUtils.hasOmnetppNature(project)) {
        	IFolder plugins = project.getFolder(PLUGINS_FOLDER);
        	if (plugins.exists())
                try {
                    for (IResource resource : plugins.members())
                    	if (isJarFileInPluginsFolder(resource))
                    		result.add(resource);
                } catch (CoreException e) {
                    OmnetppMainPlugin.logError(e);
                }
        }

        return result;
    }
    
    private static boolean isJarFileInPluginsFolder(IResource resource) {
        // must be a file with jar extension
        if (!(resource instanceof IFile) || !"jar".equals(((IFile)resource).getFileExtension()))
            return false;
        // must be located in the <project>/plugins directory
        IFolder pluginsFolder = resource.getProject().getFolder(PLUGINS_FOLDER);
        return resource.getParent().equals(pluginsFolder);
    }

    /**
     * Tries to start all dynamic omnetpp plugins installed from the workspace.
     * Dynamic omnetpp plugins are marked with a file in the bundle's data directory.
     * This must be called after each plugin installation, so a plugin
     * that became resolvable because of the latest installations (i.e. we
     * installed a plugin on which the plugin depended on) will be started.  
     */
    public void startOmnetppDynamicBundles() {
        Bundle[] bundles = bundleContext.getBundles();
        for (Bundle bundle : bundles) {
            if (bundle.getDataFile(OMNETPP_BUNDLE_MARKER).exists())
                try {
                    bundle.start(Bundle.START_TRANSIENT);
                } catch (BundleException e) {
                    OmnetppMainPlugin.logError(e);
                }
        }
    }

    /**
     * Install all bundles from the open projects.
     */
    public void installOmnetppDynamicBundlesInAllOpenProjects() {
        for (IProject proj : ResourcesPlugin.getWorkspace().getRoot().getProjects())
            if (proj.isAccessible())
                for (IResource bundleFile : getBundleFilesFrom(proj))
                    try {
                        installBundle(bundleFile);
                    } catch (BundleException e) {
                        OmnetppMainPlugin.logError(e);
                    } catch (IOException e) {
                        OmnetppMainPlugin.logError(e);
                    }
    }
    
}

    
package org.omnetpp.ide;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.project.ProjectUtils;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.BundleException;

public class OmnetppDynamicPluginLoader implements IResourceChangeListener {
    public static final String PLUGINS_FOLDER = "plugins";
    
    protected BundleContext bundleContext;
    
    public OmnetppDynamicPluginLoader(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public void resourceChanged(IResourceChangeEvent event) {
        if (event.getType() == IResourceChangeEvent.PRE_CLOSE) {
            IProject project = (IProject)event.getResource();

            if (ProjectUtils.hasOmnetppNature(project)) {
                try {
                    for (IResource plugin : getPlugins(project)) {
                        try {
                            if (plugin.isAccessible())
                                deactivatePlugin(plugin);
                        }
                        catch (Exception e) {
                            OmnetppMainPlugin.logError(e);
                        }
                    }

                    // TODO: is this needed? see below at getInstalledBundles
                    for (Bundle bundle : getInstalledBundles(project)) {
                        try {
                            bundle.uninstall();
                        }
                        catch (Exception e) {
                            OmnetppMainPlugin.logError(e);
                        }
                    }
                }
                catch (CoreException e) {
                    OmnetppMainPlugin.logError(e);
                }
            }
        }
        else if (event.getType() == IResourceChangeEvent.POST_CHANGE) {
            IResourceDelta delta = event.getDelta();
            IResourceDelta[] projectDeltas = delta.getAffectedChildren();

            for (int i = 0; i < projectDeltas.length; i++) {
                try {
                    IResourceDelta resourceDelta = projectDeltas[i];
                    final IProject project = (IProject)resourceDelta.getResource();

                    if (project.isOpen() && ProjectUtils.hasOmnetppNature(project)) {
                        resourceDelta.accept(new IResourceDeltaVisitor() {
                            public boolean visit(IResourceDelta resourceDelta) throws CoreException {
                                try {
                                    IResource resource = resourceDelta.getResource();

                                    if (isPlugin(resource)) {
                                        int kind = resourceDelta.getKind();

                                        if (kind == IResourceDelta.ADDED && resource.isAccessible())
                                            activatePlugin(resource);
                                        else if (kind == IResourceDelta.REMOVED)
                                            deactivatePlugin(resource);
                                        else if (kind == IResourceDelta.CHANGED && resource.isAccessible()) {
                                            deactivatePlugin(resource);
                                            activatePlugin(resource);
                                        }
                                        
                                        return false;
                                    }
                                    else
                                        return true;
                                }
                                catch (BundleException e) {
                                    OmnetppMainPlugin.logError(e);
                                    return false;
                                }
                            }
                        });
                    }
                }
                catch (CoreException e) {
                    OmnetppMainPlugin.logError(e);
                }
            }
        }
    }

    protected void activatePlugin(IResource plugin) throws BundleException {
        Bundle bundle = bundleContext.installBundle(plugin.getLocationURI().toString());
        bundle.start(Bundle.START_TRANSIENT);
    }

    protected void deactivatePlugin(IResource plugin) throws BundleException {
        Bundle bundle = bundleContext.installBundle(plugin.getLocationURI().toString());
        bundle.uninstall();
    }

    protected List<IResource> getPlugins(IProject project) throws CoreException {
        List<IResource> result = new ArrayList<IResource>();
        if (project.isAccessible()) {
        	IFolder plugins = project.getFolder(PLUGINS_FOLDER);
        	if (plugins.exists())
        		for (IResource resource : plugins.members())
        			if (resource instanceof IFile && ((IFile)resource).getFileExtension().equals("jar"))
        				result.add(resource);
        }

        return result;
    }
    
    protected boolean isPlugin(IResource resource) {
        IFolder pluginFolder = resource.getProject().getFolder(PLUGINS_FOLDER);
        return resource.getParent().equals(pluginFolder);
    }

    // TODO: this is used to workaround some unreliability found in the plugin deactivation 
    protected List<Bundle> getInstalledBundles(IProject project) {
        List<Bundle> result = new ArrayList<Bundle>();
        String projectLocation = project.getLocationURI().toString();

        for (Bundle bundle : bundleContext.getBundles()) {
            if (bundle != null) {
                String location = bundle.getLocation();

                if (location != null && location.startsWith(projectLocation))
                    result.add(bundle);
            }
        }
        
        return result;
    }
}

package org.omnetpp.scave.editors.treeproviders;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.resource.LocalResourceManager;
import org.eclipse.jface.viewers.BaseLabelProvider;
import org.eclipse.jface.viewers.DecorationOverlayIcon;
import org.eclipse.jface.viewers.IDecoration;
import org.eclipse.jface.viewers.ILabelDecorator;
import org.eclipse.jface.viewers.LabelProviderChangedEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.model.InputFile;

/**
 * Decorates icons according to the markers on the associated resource.
 *
 * @author tomi
 */
public class ScaveModelLabelDecorator extends BaseLabelProvider implements ILabelDecorator, IResourceChangeListener {

	LocalResourceManager resourceManager;
	
    public ScaveModelLabelDecorator() {
    	//
    	resourceManager = new LocalResourceManager(JFaceResources.getResources(PlatformUI.getWorkbench().getDisplay()));
        // we want to listen for workspace changes
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this, IResourceChangeEvent.POST_CHANGE);
    }

    @Override
    public void dispose() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
        resourceManager.dispose();
        super.dispose();
    }
	
	public Image decorateImage(Image image, Object element) {
		if (element instanceof InputFile) {
			InputFile inputFile = (InputFile)element;
			String resourcePath = inputFile.getName();
			if (resourcePath != null && !containsWildcard(resourcePath)) {
				IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
				IResource resource = root.findMember(resourcePath);
				if (resource instanceof IFile) {
					IFile file = (IFile)resource;
					int severity = maxSeverityLevel(file);
					DecorationOverlayIcon decoratedIcon = createDecoratedIcon(image, severity);
					if (decoratedIcon != null)
						return resourceManager.createImage(decoratedIcon);
				}
			}
		}
		return null;
	}

	public String decorateText(String text, Object element) {
		return null;
	}

	public boolean isLabelProperty(Object element, String property) {
		return false;
	}
	
	private boolean containsWildcard(String resourcePath) {
		return resourcePath.indexOf('?') >= 0 || resourcePath.indexOf('*') >= 0;
	}
	
	private int maxSeverityLevel(IFile file) {
        int maxLevel = -1;
        try {
          IMarker problems[] = file.findMarkers(IMarker.PROBLEM, true, IResource.DEPTH_ZERO);
            for (IMarker marker : problems)
                maxLevel = Math.max(maxLevel, marker.getAttribute(IMarker.SEVERITY, -1));
        } catch (CoreException e) {
            // cannot compute (resource does not exist or a project is closed)
            return maxLevel;
        }
        return maxLevel;
	}
	
	private DecorationOverlayIcon createDecoratedIcon(Image base, int severity) {
		if (severity == IMarker.SEVERITY_ERROR)
			return new DecorationOverlayIcon(base, ImageFactory.getDescriptor(ImageFactory.DECORATOR_IMAGE_ERROR), IDecoration.BOTTOM_LEFT);
		else if (severity == IMarker.SEVERITY_WARNING)
			return new DecorationOverlayIcon(base, ImageFactory.getDescriptor(ImageFactory.DECORATOR_IMAGE_ERROR), IDecoration.BOTTOM_LEFT);
		else
			return null;
	}

	public void resourceChanged(IResourceChangeEvent event) {
        if (isListenerAttached()) {
        	final boolean[] someMarkerChanged = new boolean[] {false};
            try {
                event.getDelta().accept(
                        new IResourceDeltaVisitor() {
                            public boolean visit(IResourceDelta delta) {
                                // we are interested only in marker annotation changes
                                if ((delta.getFlags() & IResourceDelta.MARKERS) != 0 && 
                                		delta.getResource() instanceof IFile) {
                                	someMarkerChanged[0] = true;
                                }
                                // stop visiting after the first marker change found
                                return !someMarkerChanged[0];
                            }
                        }
                );
            } catch (CoreException e) {
                // error during visit. project is close or does not exist, no need for notification
                return;
            }

            if (someMarkerChanged[0])
            	PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable() {
					public void run() {
		            	fireLabelProviderChanged(new LabelProviderChangedEvent(ScaveModelLabelDecorator.this));
					}
            	});
        }
 	}
}

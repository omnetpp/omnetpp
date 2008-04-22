package org.omnetpp.common.decorators;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.viewers.IDecoration;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ILightweightLabelDecorator;
import org.eclipse.jface.viewers.LabelProviderChangedEvent;
import org.omnetpp.common.image.ImageFactory;

/**
 * Decorates IResources if they have error or warning markers. Attaches to the workspace and
 * listens to all changes in the resources.
 *
 * @author rhornig
 */
public class ProblemDecorator implements ILightweightLabelDecorator, IResourceChangeListener {

    private ListenerList fListeners;
	private final static int quadrant = IDecoration.BOTTOM_LEFT;
    private final static int checkDepth = IResource.DEPTH_INFINITE;

    public ProblemDecorator() {
        // we want to listen for workspace changes
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this, IResourceChangeEvent.POST_CHANGE);
    }

    /* (non-Javadoc)
     * @see org.eclipse.jface.viewers.IBaseLabelProvider#dispose()
     */
    public void dispose() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
    }

    /**
     * Calculates the max severity of the given resource
     * @param resource
     * @param depth
     * @return the max severity or -1 if no marker found on the resource
     */
    private int maxSeverityLevel(IResource resource, int depth) {
        int maxLevel = -1;
        try {
          IMarker problems[] = resource.findMarkers(IMarker.PROBLEM, true, depth);
            for (IMarker marker : problems)
                maxLevel = Math.max(maxLevel, marker.getAttribute(IMarker.SEVERITY, -1));
        } catch (CoreException e) {
            // cannot compute (resource does not exist or a project is closed)
            return maxLevel;
        }
        return maxLevel;
    }

	/* (non-Javadoc)
	 * @see org.eclipse.jface.viewers.ILightweightLabelDecorator#decorate(java.lang.Object, org.eclipse.jface.viewers.IDecoration)
     * checks the resource for warning and error markers and decorates the image
	 */
	public void decorate(Object element, IDecoration decoration) {
        if (element instanceof IResource) {
            IResource resource = (IResource)element;
            int sevLevel = maxSeverityLevel(resource, checkDepth);

            if (sevLevel == IMarker.SEVERITY_ERROR)
                decoration.addOverlay(ImageFactory.getDescriptor(ImageFactory.DECORATOR_IMAGE_ERROR), quadrant);

            if (sevLevel == IMarker.SEVERITY_WARNING)
                decoration.addOverlay(ImageFactory.getDescriptor(ImageFactory.DECORATOR_IMAGE_WARNING), quadrant);
        }
	}

	/* (non-Javadoc)
	 * @see org.eclipse.jface.viewers.IBaseLabelProvider#isLabelProperty(java.lang.Object, java.lang.String)
	 */
	public boolean isLabelProperty(Object element, String property) {
		return false;
	}

    public void addListener(ILabelProviderListener listener) {
        if (fListeners == null) {
            fListeners= new ListenerList();
        }
        fListeners.add(listener);
    }

    /* (non-Javadoc)
     * @see IBaseLabelProvider#removeListener(ILabelProviderListener)
     */
    public void removeListener(ILabelProviderListener listener) {
        if (fListeners != null) {
            fListeners.remove(listener);
        }
    }

    /* (non-Javadoc)
     * @see org.eclipse.core.resources.IResourceChangeListener#resourceChanged(org.eclipse.core.resources.IResourceChangeEvent)
     * react to any change in the workspace
     */
    public void resourceChanged(IResourceChangeEvent event) {
        if (fListeners != null && !fListeners.isEmpty()) {
            // gather all resources affected by a (marker) change
            final List<IResource> resourceList = new ArrayList<IResource>(5);
            try {
                event.getDelta().accept(
                        new IResourceDeltaVisitor() {
                            public boolean visit(IResourceDelta delta) {
                                // we are interested only in marker annotation changes
                                if ((delta.getFlags() & IResourceDelta.MARKERS) != 0) {
                                    // add the resource and its ancestors to the notification list
                                    IResource res = delta.getResource();
                                    while (res!=null && res.getType()!=IResource.ROOT) {
                                        resourceList.add(res);
                                        res = res.getParent();
                                    }
                                }
                                // System.out.println(((ResourceDelta)delta).toDebugString());
                                return true;
                            }
                        }
                );
            } catch (CoreException e) {
                // error during visit. project is close or does not exist, no need for notification
                return;
            }

            // notify the listeners about the change in the decorator
            LabelProviderChangedEvent lpevent= new LabelProviderChangedEvent(this, resourceList.toArray());
            Object[] listeners= fListeners.getListeners();
            for (int i= 0; i < listeners.length; i++) {
                ((ILabelProviderListener) listeners[i]).labelProviderChanged(lpevent);
            }
        }
    }
}
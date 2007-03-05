package org.omnetpp.common.decorators;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.IDecoration;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ILightweightLabelDecorator;
import org.omnetpp.common.image.ImageFactory;

/**
 * @author rhornig
 * Decorates IResources if they have error or warning markerts
 * 
 */
// FIXME if the markers are removed, the decorations are not removed. we should
// somehow redraw the decorations if the annotation model of the file has changed
public class ProblemDecorator implements ILightweightLabelDecorator {
    public final static String BASE_PROBLEM_MARKER_ID = "org.eclipse.core.resources.problemmarker";
    
	/** The integer value representing the placement options */
	private int quadrant = IDecoration.BOTTOM_LEFT;
    
    // TODO this should come from a configuration (compute recursive error markers)
    private int checkDepth = IResource.DEPTH_INFINITE; 
    
    private int maxSeverityLevel(IResource resource, int depth) {
        int maxLevel = -1;
        try {
          IMarker problems[] = resource.findMarkers(BASE_PROBLEM_MARKER_ID, true, depth);
            for (IMarker marker : problems)
                maxLevel = Math.max(maxLevel, marker.getAttribute(IMarker.SEVERITY, -1));
        } catch (CoreException e) {
            // cannot compute (resource does not exsist or a project is closed)
            return maxLevel;
        }
        return maxLevel;
    }
    
	public void decorate(Object element, IDecoration decoration) {
		/**
		 * Checks that the element is an IResource with the 'Read-only' attribute
		 * and adds the decorator based on the specified image description and the
		 * integer representation of the placement option.
		 */
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
	 * @see org.eclipse.jface.viewers.IBaseLabelProvider#addListener(org.eclipse.jface.viewers.ILabelProviderListener)
	 */
	public void addListener(ILabelProviderListener listener) {
	}

	/* (non-Javadoc)
	 * @see org.eclipse.jface.viewers.IBaseLabelProvider#dispose()
	 */
	public void dispose() {
	}

	/* (non-Javadoc)
	 * @see org.eclipse.jface.viewers.IBaseLabelProvider#isLabelProperty(java.lang.Object, java.lang.String)
	 */
	public boolean isLabelProperty(Object element, String property) {
		return true;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.jface.viewers.IBaseLabelProvider#removeListener(org.eclipse.jface.viewers.ILabelProviderListener)
	 */
	public void removeListener(ILabelProviderListener listener) {
	}
}
package org.omnetpp.ned.resources.decorators;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.IDecoration;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ILightweightLabelDecorator;
import org.omnetpp.ned.resources.NEDResources;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * @author rhornig
 * Decorates IResources if they have error or warning markerts
 * 
 */
// FIXME if the markers are removed, the decorations are not removed. we should
// somehow redraw the decorations if the annotation model of the file has changed
public class ProblemDecorator implements ILightweightLabelDecorator {
	/** The integer value representing the placement options */
	private int quadrant = IDecoration.BOTTOM_LEFT;
    
    private static ImageDescriptor errorImageDesc = NEDResourcesPlugin.getImageDescriptor("/icons/error.gif");
    private static ImageDescriptor warningImageDesc = NEDResourcesPlugin.getImageDescriptor("/icons/warning.gif");
    
    // TODO this should come from a configuration (compute recursive error markers)
    private int checkDepth = IResource.DEPTH_INFINITE; 
    
    private int maxSeverityLevel(IResource resource, int depth) {
        int maxLevel = -1;
        try {
            IMarker nedProblems[] = resource.findMarkers(NEDResources.NEDPROBLEM_MARKERID, true, depth);
            IMarker nedConsistencyProblems[] = resource.findMarkers(NEDResources.NEDCONSISTENCYPROBLEM_MARKERID, true, depth);
            for (IMarker marker : nedProblems)
                maxLevel = Math.max(maxLevel, marker.getAttribute(IMarker.SEVERITY, -1));
            for (IMarker marker : nedConsistencyProblems)
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
                decoration.addOverlay(errorImageDesc, quadrant);
            
            if (sevLevel == IMarker.SEVERITY_WARNING)
                decoration.addOverlay(warningImageDesc, quadrant);
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
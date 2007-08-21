package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.draw2d.ConnectionLocator;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.DragTracker;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.handles.ConnectionHandle;
import org.eclipse.gef.tools.ConnectionEndpointTracker;

/**
 * A handle used at the end of the {@link org.eclipse.draw2d.Connection}.
 * This is treated differently than the start of the Connection.
 * Redefined to provide Ned
 *
 * @author rhornig
 */
public final class NedConnectionEndHandle extends ConnectionHandle
{

    /**
     * Creates a new ConnectionEndHandle, sets its owner to <code>owner</code>,
     * and sets its locator to a {@link ConnectionLocator}.
     * @param owner the ConnectionEditPart owner
     */
    public NedConnectionEndHandle(ConnectionEditPart owner) {
    	setOwner(owner);
    	setLocator(new ConnectionLocator(getConnection(), ConnectionLocator.TARGET));
    }

    /**
     * Creates a new NedConnectionEndHandle with its owner set to <code>owner</code>.
     * If the handle is fixed, it cannot be dragged.
     * @param owner the ConnectionEditPart owner
     * @param fixed if true, handle cannot be dragged
     */
    public NedConnectionEndHandle(ConnectionEditPart owner, boolean fixed) {
    	super(fixed);
    	setOwner(owner);
    	setLocator(new ConnectionLocator(getConnection(), ConnectionLocator.TARGET));
    }

    /**
     * Creates a new NedConnectionEndHandle.
     */
    public NedConnectionEndHandle() { }

    /**
     * Creates and returns a new {@link ConnectionEndpointTracker}.
     */
    @Override
	protected DragTracker createDragTracker() {
    	if (isFixed())
    		return null;
    	ConnectionEndpointTracker tracker;
    	tracker = new NedConnectionEndpointTracker((ConnectionEditPart)getOwner());
    	tracker.setCommandName(RequestConstants.REQ_RECONNECT_TARGET);
    	tracker.setDefaultCursor(getCursor());
    	return tracker;
    }

}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.eclipse.core.internal.events.ResourceDelta;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceDelta;
import org.omnetpp.common.Debug;

/**
 * IResourcE-related utilities.
 *
 * @author andras
 */
@SuppressWarnings("restriction")
public class ResourceUtils  {

    // Utility functions for debugging
    public static void printResourceChangeEvent(IResourceChangeEvent event) {
        Debug.println("event type: "+event.getType());
        printResourceDelta(event.getDelta(), "  ");
    }

    public static void printResourceDelta(IResourceDelta delta, String indent) {
        // LEGEND: [+] added, [-] removed, [*] changed, [>] and [<] phantom added/removed;
        // then: {CONTENT, MOVED_FROM, MOVED_TO, OPEN, TYPE, SYNC, MARKERS, REPLACED, DESCRIPTION, ENCODING}
        Debug.println(indent + ((ResourceDelta)delta).toDebugString());
        for (IResourceDelta child : delta.getAffectedChildren())
            printResourceDelta(child, indent + "  ");
    }

}

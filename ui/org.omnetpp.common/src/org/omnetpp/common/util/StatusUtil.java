package org.omnetpp.common.util;

import org.eclipse.core.runtime.IStatus;

/**
 * Utility class with methods related to {@link IStatus} objects.
 *
 * @author tomi
 */
public class StatusUtil {

    /**
     * Returns the first descendant status that has {@code severity >= minSeverity},
     * or {@code null} if no such status is found.
     * <p>If {@code status} is a multi status, it recurses on their children,
     * otherwise returns the {@code status} itsef.
     */
    public static IStatus getFirstDescendantWithSeverity(IStatus status, int minSeverity) {
        if (status.isMultiStatus()) {
            for (IStatus child : status.getChildren()) {
                IStatus result = getFirstDescendantWithSeverity(child, minSeverity);
                if (result != null)
                    return result;
            }
        }
        else if (status.getSeverity() >= minSeverity)
            return status;
        return null;
    }
}

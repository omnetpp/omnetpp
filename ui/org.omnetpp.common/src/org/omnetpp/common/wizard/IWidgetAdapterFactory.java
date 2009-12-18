package org.omnetpp.common.wizard;

import org.eclipse.swt.widgets.Control;


/**
 * For extending classes to get to work with XSWTDataBinding.getValue()/
 * setValue() methods.
 *
 * @author Andras
 */
public interface IWidgetAdapterFactory {
    /**
     * Returns an adapter for the given widget.
     */
    IWidgetAdapter getAdapterFor(Control control);
}

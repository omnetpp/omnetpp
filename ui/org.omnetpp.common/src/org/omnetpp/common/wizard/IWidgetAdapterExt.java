package org.omnetpp.common.wizard;


/**
 * For extending classes to get to work with XSWTDataBinding.getValue()/
 * setValue() methods. This interface is an alternative to IWidgetAdapter.
 *
 * @author Andras
 */
public interface IWidgetAdapterExt {
    /**
     * Like the zero-arg getValue(), but also gives access to the
     * wizard context, for implementing some extended functionality.
     */
    Object getValue(CreationContext context);

    /**
     * Like the one-arg setValue(), but also gives access to the
     * wizard context, for implementing some extended functionality.
     */
    void setValue(Object value, CreationContext context);
}

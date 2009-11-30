package org.omnetpp.common.wizard;


/**
 * For extending classes to get to work with XSWTDataBinding.getValue()/
 * setValue() methods.
 *
 * @author Andras
 */
public interface IWidgetAdapter {
    /**
     * Return the widget's value.
     */
    Object getValue();

    /**
     * Set the widget's value.
     */
    void setValue(Object value);
}

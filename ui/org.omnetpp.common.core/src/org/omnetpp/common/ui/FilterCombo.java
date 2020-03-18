package org.omnetpp.common.ui;

import org.eclipse.swt.widgets.Composite;

/**
 * Combo that allows displaying a hint message when nothing is entered.
 *
 * @author Andras
 */
public class FilterCombo extends CustomCombo {
    public FilterCombo(Composite parent, int style) {
        super(parent, style);
    }

    @Override
    protected void checkSubclass() {
        // allow subclassing
    }

    @Override
    public String toString() {
        return getClass().getSimpleName() + " {" + getMessage() + "}";
    }

}

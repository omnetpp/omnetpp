package org.omnetpp.common.ui;

import org.eclipse.jface.text.IInformationControlCreator;

/**
 * Value object to be returned from IHoverInfoProvider.getHoverFor(). Describes
 * details of hover information to be shown. Contains the information control
 * creator, its input, and size constraint. The size constraint parameter is optional.
 *
 * Note: the hoverControlCreator in this class only creates the "transient"
 * information control. The "sticky" information control (that appears after
 * hitting F2) is expected be created by the transient hover control, via
 * <code>IInformationControlExtension5.getInformationPresenterControlCreator()</code>
 *
 * @author andras
 */
public class HoverInfo {
    private IInformationControlCreator hoverControlCreator;
    private Object input; // specific to the created IInformationControl
    private SizeConstraint sizeConstraint;

    public HoverInfo(IInformationControlCreator hoverControlCreator, Object input) {
        this(hoverControlCreator, input, null);
    }

    public HoverInfo(IInformationControlCreator hoverControlCreator, Object input, SizeConstraint sizeConstraint) {
        this.hoverControlCreator = hoverControlCreator;
        this.input = input;
        this.sizeConstraint = sizeConstraint;
    }

    public IInformationControlCreator getHoverControlCreator() {
        return hoverControlCreator;
    }

    public Object getInput() {
        return input;
    }

    public SizeConstraint getSizeConstraint() {
        return sizeConstraint;
    }
}
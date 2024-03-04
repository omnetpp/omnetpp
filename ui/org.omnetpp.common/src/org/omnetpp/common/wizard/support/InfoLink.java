package org.omnetpp.common.wizard.support;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.ui.HelpLink;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.IWidgetAdapter;

/**
 * A control for which displays a text with embedded link(s), and clicking
 * on a link will display a hover text in a window. The hover text can be given
 * the with setHoverText method (i.e. the hoverText XSWT attribute), or bound
 * to a template variable (using the x:id XSWT attribute).
 *
 * @author Andras
 */
public class InfoLink extends HelpLink implements IWidgetAdapter {

    public InfoLink(Composite parent, int style) {
        super(parent, style);
    }

    /**
     * Adapter interface.
     */
    public Object getValue() {
        return StringUtils.defaultString(getHoverText());
    }

    /**
     * Adapter interface.
     */
    public void setValue(Object value) {
        setHoverText(value.toString());
    }

}

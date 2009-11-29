package org.omnetpp.common.wizard.support;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Link;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.IWidgetAdapter;

/**
 * A control for displaying the name of a resource as a link. When clicked, it shows
 * the resource (opens Project Explorer and focuses it to the resource).
 *
 * @author Andras
 */
public class InfoLink extends Composite implements IWidgetAdapter {
    private Link link;
    private String hoverText;
    private HoverSupport hoverSupport;
    private SizeConstraint sizeConstraint = new SizeConstraint();

    public InfoLink(Composite parent, int style) {
        super(parent, style);
        setLayout(new FillLayout());

        link = new Link(this, SWT.NONE);

        hoverSupport = new HoverSupport();
        hoverSupport.adapt(link, new IHoverTextProvider() {
            public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
                return getHoverText(x, y, outSizeConstraint);
            }
        });

        link.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                hoverSupport.makeHoverSticky(link);
            }
        });
    }

    protected String getHoverText(int x, int y, SizeConstraint outSizeConstraint) {
        if (StringUtils.isEmpty(hoverText))
            return null;
        outSizeConstraint.minimumWidth = sizeConstraint.minimumWidth;
        outSizeConstraint.minimumHeight = sizeConstraint.minimumHeight;
        outSizeConstraint.preferredWidth = sizeConstraint.preferredWidth;
        outSizeConstraint.preferredHeight = sizeConstraint.preferredHeight;
        return HoverSupport.addHTMLStyleSheet(hoverText);
    }

    public Link getLinkControl() {
        return link;
    }

    public void setText(String text) {
        link.setText(text);
    }
    
    public String getText() {
        return link.getText();
    }

    public void setHoverText(String hoverText) {
        this.hoverText = hoverText;
    }
    
    public String getHoverText() {
        return hoverText;
    }

    public int getHoverMinimumWidth() {
        return sizeConstraint.minimumHeight;
    }

    public void setHoverMinimumWidth(int minimumWidth) {
        sizeConstraint.minimumWidth = minimumWidth;
    }

    public int getHoverMinimumHeight() {
        return sizeConstraint.minimumHeight;
    }

    public void setHoverMinimumHeight(int minimumHeight) {
        sizeConstraint.minimumHeight = minimumHeight;
    }

    public int getHoverPreferredWidth() {
        return sizeConstraint.preferredWidth;
    }

    public void setHoverPreferredWidth(int preferredWidth) {
        sizeConstraint.preferredWidth = preferredWidth;
    }

    public int getHoverPreferredHeight() {
        return sizeConstraint.preferredHeight;
    }

    public void setHoverPreferredHeight(int preferredHeight) {
        sizeConstraint.preferredHeight = preferredHeight;
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

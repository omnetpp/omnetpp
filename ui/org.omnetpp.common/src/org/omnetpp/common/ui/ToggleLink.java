package org.omnetpp.common.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.TypedListener;


/**
 * A link that shows/hides other controls. Link text alternates between 
 * "More >>" or "<< Less" (can be overridden). The controlled Controls 
 * need to be under GridLayout.
 * 
 * @author Andras
 */
public class ToggleLink extends Composite {
    private Link link;
    private boolean isOpen = false;
    private String moreText = "<A>More >></A>";
    private String lessText = "<A><< Less</A>";
    private Control[] controls = new Control[0];

    public ToggleLink(Composite parent, int style) {
        super(parent, style);
        link = new Link(this, SWT.NONE);
        this.setLayout(new FillLayout());

        link.addSelectionListener(new SelectionListener() {
            public void widgetDefaultSelected(SelectionEvent e) {
                widgetSelected(e);
            }
            public void widgetSelected(SelectionEvent e) {
                toggle();
            }
        });
    }

    public void setExpanded(boolean open) {
        if (isOpen != open) {
            isOpen = open;
            refresh();
        }
    }

    public boolean isExpanded() {
        return isOpen;
    }
    
    public void toggle() {
        setExpanded(!isExpanded());
    }

    public String getMoreText() {
        return moreText;
    }

    public void setMoreText(String moreText) {
        this.moreText = moreText;
        link.setText(isOpen ? lessText : moreText);
    }

    public String getLessText() {
        return lessText;
    }

    public void setLessText(String lessText) {
        this.lessText = lessText;
        link.setText(isOpen ? lessText : moreText);
    }

    public Control[] getControls() {
        return controls;
    }

    public void setControls(Control[] controls) {
        this.controls = controls;
        refresh();
    }

    protected void refresh() {
        for (Control c : controls) {
            ((GridData)c.getLayoutData()).exclude = !isOpen;
            c.setVisible(isOpen);
        }
        link.setText(isOpen ? lessText : moreText);
        getParent().layout();
    }
    
    public void addSelectionListener (SelectionListener listener) {
    	link.addSelectionListener(listener);
    }
    
    public void removeSelectionListener (SelectionListener listener) {
    	link.removeSelectionListener(listener);
    }
    
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.text.AbstractInformationControl;
import org.eclipse.jface.text.AbstractReusableInformationControlCreator;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.IInformationControlExtension2;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.swt.custom.StyledText;
import org.omnetpp.common.util.HTMLUtils;

/**
 * This class uses a StyledText widget to present information to the user.
 * It supports a subset of HTML 3.2 as provided by HTMLUtils.
 *
 * @author levy
 */
public class StyledTextInformationControl extends AbstractInformationControl implements IInformationControlExtension2 {
    private StyledText styledText;

    public StyledTextInformationControl(Shell parentShell, boolean isResizable) {
        super(parentShell, isResizable);
        create();
    }

    public StyledTextInformationControl(Shell parentShell, String statusFieldText) {
        super(parentShell, statusFieldText);
        create();
    }

    @Override
    public Point computeSizeHint() {
        Point size = styledText.computeSize(-1, -1);
        int maxLineWidth = Math.min(size.x, 800);
        // KLUGE: added some space for the status line
        size = styledText.computeSize(maxLineWidth, -1);
        int affordanceTextHeight = JFaceResources.getDialogFont().getFontData()[0].getHeight() * Display.getCurrent().getDPI().y / 72;
        int extraHeight = 10;  // borders, etc.
        return new Point(size.x, size.y + affordanceTextHeight + extraHeight);
    }

    @Override
    public boolean hasContents() {
        return styledText.getCharCount() != 0;
    }

    @Override
    protected void createContent(Composite parent) {
        styledText = new StyledText(parent, SWT.V_SCROLL | SWT.H_SCROLL | SWT.WRAP | SWT.READ_ONLY);
        styledText.setMargins(8, 2, 8, 2);
        styledText.setFont(HTMLUtils.getInitialFont());
        styledText.setBackground(styledText.getParent().getBackground());
        styledText.setForeground(styledText.getParent().getForeground());
    }

    @Override
    public void setInformation(String information) {
        styledText.setText("");
        HTMLUtils.htmlToStyledText(information, styledText);
    }

    @Override
    public void setInput(Object input) {
        styledText.setText("");
        if (input instanceof String)
            HTMLUtils.htmlToStyledText((String)input, styledText);
        else {
            HtmlInput htmlInput = (HtmlInput)input;
            HTMLUtils.htmlToStyledText(htmlInput.getContent(), styledText, htmlInput.getImageMap());
        }
    }

    /**
     * Utility method: returns a creator that creates a (transient) StyledTextInformationControl.
     */
    public static IInformationControlCreator getCreator() {
        return new AbstractReusableInformationControlCreator() {
            @Override
            public IInformationControl doCreateInformationControl(Shell shell) {
                return new StyledTextInformationControl(shell, HoverSupport.AFFORDANCE);
            }
        };
    }

    /**
     * Utility method: returns a creator that creates a (sticky) StyledTextInformationControl.
     */
    public static IInformationControlCreator getStickyCreator() {
        return new AbstractReusableInformationControlCreator() {
            @Override
            public IInformationControl doCreateInformationControl(Shell shell) {
                return new StyledTextInformationControl(shell, true);
            }
        };
    }

    /**
     * Implements IInformationControlExtension5.getInformationPresenterControlCreator().
     */
    @Override
    public IInformationControlCreator getInformationPresenterControlCreator() {
        return getStickyCreator();
    }
}
/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.color.ColorFactory;


/**
 * A checkbox button that cycles among three states: cleared, checked, grayed.
 * Note that these states are internally represented with two booleans:
 * isGrayed and isSelected. The isGrayed==true, isSelected==false combination
 * is explicitly disallowed (it is rendered as a clear checkbox), so
 * setGrayed(true) implies setSelected(true).
 *
 * Also: The grayed state is not always rendered differently from checked
 * (e.g. in some KDE themes). To make them surely distinguishable for the user,
 * for grayed we also set the foreground color to gray.
 *
 * @author Andras
 */
public class TristateCheckButton extends Button {
    private static Color GREY = ColorFactory.GREY50;

    /**
     * Constructor.
     */
    public TristateCheckButton(Composite parent, int style) {
        super(parent, style | SWT.CHECK);

        // make the button cycle through the cleared -> checked -> grayed states
        addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                if (!getGrayed() && !getSelection())
                    setGrayed(true);
                else if (getGrayed() && !getSelection())
                    setGrayed(false);
            }
        });
    }

    @Override
    protected void checkSubclass () {
        // allow subclassing
    }

    /**
     * Set the button's grayed state. NOTE: when called with true,
     * this also selects the checkbox (as we disallow grayed+unselected
     * as invalid state).
     */
    @Override
    public void setGrayed(boolean grayed) {
        super.setGrayed(grayed);
        if (grayed && !getSelection()) {
            setSelection(true);
            Assert.isTrue(getGrayed());
        }
        setForeground(getGrayed() ? GREY : null);
    }
}



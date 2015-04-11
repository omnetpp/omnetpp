/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.access;

import org.eclipse.swt.widgets.Label;

public class LabelAccess extends ControlAccess
{
    public LabelAccess(Label control) {
        super(control);
    }

    @Override
    public Label getControl() {
        return (Label)widget;
    }
}

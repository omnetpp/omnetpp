/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.access;

import org.eclipse.swt.widgets.Canvas;

public class CanvasAccess extends CompositeAccess
{
    public CanvasAccess(Canvas canvas) {
        super(canvas);
    }

    @Override
    public Canvas getControl() {
        return (Canvas)widget;
    }

}

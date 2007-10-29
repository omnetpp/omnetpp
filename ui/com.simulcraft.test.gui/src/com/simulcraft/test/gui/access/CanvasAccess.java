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

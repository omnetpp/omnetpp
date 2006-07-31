package org.omnetpp.experimental.animation.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.experimental.animation.live.LiveAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;


public class LiveAnimationEditor extends ReplayAnimationEditor {
	@Override
	protected void createAnimationController(Composite parent) {
		AnimationCanvas canvas = new AnimationCanvas(parent, SWT.NONE);
		canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		animationController = new LiveAnimationController(canvas);
		animationController.addAnimationListener(this);
		animationController.init();
	}
}
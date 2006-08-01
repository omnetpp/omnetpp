package org.omnetpp.experimental.animation.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.CoolItem;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.experimental.animation.AnimationPlugin;
import org.omnetpp.experimental.animation.controller.ILiveAnimationListener;
import org.omnetpp.experimental.animation.live.LiveAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public class LiveAnimationEditor extends ReplayAnimationEditor implements ILiveAnimationListener {
	
	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);
		
		ToolBar toolBar = new ToolBar(coolBar, SWT.NONE);

		ToolItem toolItem;

		toolItem = new ToolItem(toolBar, SWT.PUSH);
		toolItem.setToolTipText("Restart");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor("icons/restart.gif").createImage());
		toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().restartSimulation();
			}
		});

		toolItem = new ToolItem(toolBar, SWT.PUSH);
		toolItem.setToolTipText("Step");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor("icons/dooneevent.gif").createImage());
		toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().simulateStep();
			}
		});
		
		toolItem = new ToolItem(toolBar, SWT.PUSH);
		toolItem.setToolTipText("Run");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor("icons/run.gif").createImage());
		toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().simulateRun();
			}
		});
		
		toolItem = new ToolItem(toolBar, SWT.PUSH);
		toolItem.setToolTipText("Fast");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor("icons/fast.gif").createImage());
		toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().simulateFast();
			}
		});

		toolItem = new ToolItem(toolBar, SWT.PUSH);
		toolItem.setToolTipText("Express");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor("icons/express.gif").createImage());
		toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().simulateExpress();
			}
		});

		toolItem = new ToolItem(toolBar, SWT.PUSH);
		toolItem.setToolTipText("Stop");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor("icons/stopsim.gif").createImage());
		toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().simulateStop();
			}
		});

		toolItem = new ToolItem(toolBar, SWT.PUSH);
		toolItem.setToolTipText("Finish");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor("icons/finish.gif").createImage());
		toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().finishSimulation();
			}
		});
		
		toolBar.setSize(toolBar.computeSize(SWT.DEFAULT, SWT.DEFAULT));

		CoolItem coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(toolBar);
	}
	
	@Override
	protected void createAnimationController(Composite parent) {
		AnimationCanvas canvas = new AnimationCanvas(parent, SWT.NONE);
		canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		animationController = new LiveAnimationController(canvas);
		animationController.addAnimationListener(this);
		animationController.init();
	}

	protected LiveAnimationController getLiveAnimationController() {
		return ((LiveAnimationController)animationController);
	}

	public void livePositionChanged(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		// TODO:
	}
}
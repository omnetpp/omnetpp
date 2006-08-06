package org.omnetpp.experimental.animation.editors;

import java.text.NumberFormat;
import java.text.ParseException;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.CoolBar;
import org.eclipse.swt.widgets.CoolItem;
import org.eclipse.swt.widgets.Scale;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.experimental.animation.AnimationPlugin;
import org.omnetpp.experimental.animation.controller.IReplayAnimationListener;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public class ReplayAnimationEditor extends EditorPart implements IReplayAnimationListener {
	private static final String ICONS_REWIND_GIF = "icons/rewind.gif";
	private static final String ICONS_GOTOEND_GIF = "icons/gotoend.gif";
	private static final String ICONS_PLAY_GIF = "icons/play.gif";
	private static final String ICONS_BACKPLAY_GIF = "icons/backplay.gif";
	private static final String ICONS_STEP_GIF = "icons/step.gif";
	private static final String ICONS_BACKSTEP_GIF = "icons/backstep.gif";
	private static final String ICONS_STOP_GIF = "icons/stop.gif";
	
	protected ReplayAnimationController animationController;

	protected CoolBar coolBar;
	protected ToolBar replayToolBar;
	protected ToolItem replayStopToolItem;
	protected Text replayEventNumberWidget;
	protected Text replaySimulationTimeWidget;
	protected Text replayAnimationNumberWidget;
	protected Text replayAnimationTimeWidget;
	
	protected NumberFormat numberFormat;
	private ToolItem replayBeginToolItem;
	private ToolItem replayBackToolItem;
	private ToolItem replayBackstepToolItem;
	private ToolItem replayStepToolItem;
	private ToolItem replayPlayToolItem;
	private ToolItem replayEndToolItem;
	protected static final int COOLBAR_HEIGHT = 25;
	
	public ReplayAnimationEditor() {
	}

	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
	    if (!(input instanceof IFileEditorInput))
	    	throw new PartInitException("Invalid input: this editor only works with workspace files");
		setSite(site);
		setInput(input);
		
		setPartName(input.getName());
		numberFormat = NumberFormat.getNumberInstance();
		numberFormat.setMaximumFractionDigits(Integer.MAX_VALUE);
	}

	@Override
	public void createPartControl(Composite parent) {
		parent.setLayout(new GridLayout(1, false));
		
		createCoolbar(parent);
		
		createNavigationToolbar();
		createTimeGauges();
		createSpeedSlider();

		createAnimationController(parent);
	}

	protected void createCoolbar(Composite parent) {
		coolBar = new CoolBar(parent, SWT.NONE);
		coolBar.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
	}

	protected void createNavigationToolbar() {
		// navigation tool bar
		replayToolBar = new ToolBar(coolBar, SWT.NONE);
	
	    replayBeginToolItem = new ToolItem(replayToolBar, SWT.PUSH);
	    replayBeginToolItem.setToolTipText("Begin");
	    replayBeginToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_REWIND_GIF).createImage());
	    replayBeginToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.gotoAnimationBegin();
			}
	    });
	
	    replayBackToolItem = new ToolItem(replayToolBar, SWT.PUSH);
	    replayBackToolItem.setToolTipText("Back");
	    replayBackToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_BACKPLAY_GIF).createImage());
	    replayBackToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.runAnimationBack();
			}
	    });
	
	    replayBackstepToolItem = new ToolItem(replayToolBar, SWT.PUSH);
	    replayBackstepToolItem.setToolTipText("Backstep");
	    replayBackstepToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_BACKSTEP_GIF).createImage());
	    replayBackstepToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.stepAnimationBack();
			}
	    });
	    
	    replayStopToolItem = new ToolItem(replayToolBar, SWT.PUSH);
	    replayStopToolItem.setEnabled(false);
	    replayStopToolItem.setToolTipText("Stop");
	    replayStopToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_STOP_GIF).createImage());
	    replayStopToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.stopAnimation();
			}
	    });
	
	    replayStepToolItem = new ToolItem(replayToolBar, SWT.PUSH);
	    replayStepToolItem.setToolTipText("Step");
	    replayStepToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_STEP_GIF).createImage());
	    replayStepToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.stepAnimation();
			}
	    });
	
	    replayPlayToolItem = new ToolItem(replayToolBar, SWT.PUSH);
	    replayPlayToolItem.setToolTipText("Play");
	    replayPlayToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_PLAY_GIF).createImage());
	    replayPlayToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.runAnimation();
			}
	    });
	
	    replayEndToolItem = new ToolItem(replayToolBar, SWT.PUSH);
	    replayEndToolItem.setToolTipText("End");
	    replayEndToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_GOTOEND_GIF).createImage());
	    replayEndToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.gotoAnimationEnd();
			}
	    });
	    // navigation buttons
		CoolItem coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(replayToolBar);
		//toolBar.setSize(toolBar.computeSize(SWT.DEFAULT, SWT.DEFAULT));
		coolItem.setSize(new Point(180, COOLBAR_HEIGHT));
	}

	protected void createTimeGauges() {
		CoolItem coolItem;
		// animation mode selector
/*
		Combo animationMode = new Combo(coolBar, SWT.NONE);
		for (ReplayAnimationController.AnimationMode t : ReplayAnimationController.AnimationMode.values())
			animationMode.add(t.name());
		animationMode.setVisibleItemCount(ReplayAnimationController.AnimationMode.values().length);
		coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(animationMode);
		coolItem.setSize(new Point(100, coolBarHeight));
*/
		// event number
		Composite labels = new Composite(coolBar, SWT.NONE);
		labels.setLayout(new RowLayout(SWT.HORIZONTAL));
		replayEventNumberWidget = new Text(labels, SWT.BORDER);
		replayEventNumberWidget.setToolTipText("event number");
		replayEventNumberWidget.setLayoutData(new RowData(100, COOLBAR_HEIGHT - 10));
		replayEventNumberWidget.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				long eventNumber = -1;
				try {
					eventNumber = numberFormat.parse(((Text)e.widget).getText()).longValue();
				}
				catch (ParseException e1) {
					// void
				}
				if (eventNumber != -1 && eventNumber != animationController.getEventNumber())
					animationController.gotoEventNumber(eventNumber);
			}
		});
		replayAnimationNumberWidget = new Text(labels, SWT.BORDER);
		replayAnimationNumberWidget.setToolTipText("\"animation number\"");
		replayAnimationNumberWidget.setLayoutData(new RowData(100, COOLBAR_HEIGHT - 10));
		replayAnimationNumberWidget.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				long animationNumber = -1;
				try {
					animationNumber = numberFormat.parse(((Text)e.widget).getText()).longValue();
				}
				catch (ParseException e1) {
					// void
				}

				if (animationNumber != -1 && animationNumber != animationController.getAnimationNumber())
					animationController.gotoAnimationNumber(animationNumber);
			}
		});
		replaySimulationTimeWidget = new Text(labels, SWT.BORDER);
		replaySimulationTimeWidget.setToolTipText("simulation time");
		replaySimulationTimeWidget.setLayoutData(new RowData(100, COOLBAR_HEIGHT - 10));
		replaySimulationTimeWidget.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				double simulationTime = -1;
				try {
					simulationTime = numberFormat.parse(((Text)e.widget).getText()).doubleValue();
				}
				catch (ParseException e1) {
					// void
				}

				if (simulationTime != -1 && simulationTime != animationController.getSimulationTime())
					animationController.gotoSimulationTime(simulationTime);
			}
		});
		replayAnimationTimeWidget = new Text(labels, SWT.BORDER);
		replayAnimationTimeWidget.setToolTipText("\"animation time\"");
		replayAnimationTimeWidget.setLayoutData(new RowData(100, COOLBAR_HEIGHT - 10));
		replayAnimationTimeWidget.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				double animationTime = -1;
				try {
					animationTime = numberFormat.parse(((Text)e.widget).getText()).doubleValue();
				}
				catch (ParseException e1) {
					// void
				}

				if (animationTime != -1 && animationTime != animationController.getAnimationTime())
					animationController.gotoAnimationTime(animationTime);
			}
		});
	    coolItem = new CoolItem(coolBar, SWT.NONE);
	    coolItem.setControl(labels);
		coolItem.setSize(new Point(480, COOLBAR_HEIGHT));

/*
   		animationMode.select(animationController.getAnimationMode().ordinal());
   		animationMode.addSelectionListener(new SelectionAdapter () {
   			public void widgetSelected(SelectionEvent e) {
   				animationController.setAnimationMode(ReplayAnimationController.AnimationMode.values()[((Combo)e.getSource()).getSelectionIndex()]);
   			}
   		});
*/
	}

	protected void createSpeedSlider() {
		CoolItem coolItem;
		// speed slider
	    Scale scale = new Scale(coolBar, SWT.HORIZONTAL);
	    scale.setMinimum(0);
	    scale.setMaximum(100);
	    scale.setSelection((scale.getMaximum() - scale.getMinimum()) / 2); 
	    scale.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				Scale scale = (Scale)e.widget;
				int value = scale.getSelection() - (scale.getMaximum() - scale.getMinimum()) / 2;
				animationController.setRealTimeToAnimationTimeScale(Math.pow(1.1, value));
			}	    	
	    });
	    coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(scale);
		coolItem.setSize(new Point(200, COOLBAR_HEIGHT)); //XXX height has no effect
	}
	
	protected void createAnimationController(Composite parent) {
		AnimationCanvas canvas = new AnimationCanvas(parent, SWT.DOUBLE_BUFFERED);
		canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		animationController = new ReplayAnimationController(canvas, ((IFileEditorInput)getEditorInput()).getFile());
		animationController.addAnimationListener(this);
		animationController.restart();
	}

	protected void setReplayToolbarEnabled(boolean enable) {
		replayToolBar.setEnabled(enable);		
	}

	@Override
	public void setFocus() {
	}
	
	@Override
	public void dispose() {
		animationController.stopAnimation();
		animationController.shutdown();
		super.dispose();
	}

	@Override
	public boolean isDirty() {
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		return false;
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
	}

	@Override
	public void doSaveAs() {
	}

	public void controllerStateChanged() {
		if (!replayToolBar.isDisposed()) {
			if (animationController.isRunning())
				replayStopToolItem.setEnabled(true);
			else
				replayStopToolItem.setEnabled(false);
			}
	}

	public void replayPositionChanged(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		if (!replayToolBar.isDisposed()) {
			if (animationController.isAnimationPositionValid() && animationController.isAtAnimationBegin()) {
				replayBeginToolItem.setEnabled(false);
				replayBackToolItem.setEnabled(false);
				replayBackstepToolItem.setEnabled(false);
			}
			else {
				replayBeginToolItem.setEnabled(true);
				replayBackToolItem.setEnabled(true);
				replayBackstepToolItem.setEnabled(true);
			}
	
			if (animationController.isAnimationPositionValid() && animationController.isAtAnimationEnd()) {
				replayStepToolItem.setEnabled(false);
				replayPlayToolItem.setEnabled(false);
				replayEndToolItem.setEnabled(false);
			}
			else {
				replayStepToolItem.setEnabled(true);
				replayPlayToolItem.setEnabled(true);
				replayEndToolItem.setEnabled(true);
			}
		}

		valueChanged(replayEventNumberWidget, eventNumber);
		valueChanged(replaySimulationTimeWidget, simulationTime);
		valueChanged(replayAnimationNumberWidget, animationNumber);
		valueChanged(replayAnimationTimeWidget, animationTime);
	}

	protected void valueChanged(Text widget, Number newValue) {
		if (!widget.isDisposed()) {
			Number oldValue = null;
			
			try {
				oldValue = numberFormat.parse(widget.getText());
			}
			catch (ParseException e) {
				// void
			}
			
			if (oldValue == null || newValue.doubleValue() != oldValue.doubleValue())
				widget.setText(numberFormat.format(newValue));
		}
	}
}
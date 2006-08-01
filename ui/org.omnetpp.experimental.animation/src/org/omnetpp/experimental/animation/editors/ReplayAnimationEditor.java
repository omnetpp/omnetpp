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
	protected Text replayEventNumberWidget;
	protected Text replaySimulationTimeWidget;
	protected Text replayAnimationNumberWidget;
	protected Text replayAnimationTimeWidget;
	
	protected NumberFormat numberFormat;
	
	public ReplayAnimationEditor() {
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
	}

	@Override
	public void doSaveAs() {
	}

	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		setSite(site);
		setInput(input);
		
		setPartName(input.getName());
		numberFormat = NumberFormat.getNumberInstance();
		numberFormat.setMaximumFractionDigits(Integer.MAX_VALUE);
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
	public void createPartControl(Composite parent) {
		parent.setLayout(new GridLayout());
		
		// user interface controls
		int coolBarHeight = 25;
		coolBar = new CoolBar(parent, SWT.NONE);
		coolBar.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
		
		// navigation tool bar
		ToolBar toolBar = new ToolBar(coolBar, SWT.NONE);

	    ToolItem toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setToolTipText("Begin");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_REWIND_GIF).createImage());
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.gotoBegin();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setToolTipText("Back");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_BACKPLAY_GIF).createImage());
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animateBack();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setToolTipText("Backstep");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_BACKSTEP_GIF).createImage());
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animateBackStep();
			}
	    });
	    
	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setToolTipText("Stop");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_STOP_GIF).createImage());
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animateStop();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setToolTipText("Step");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_STEP_GIF).createImage());
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animateStep();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setToolTipText("Play");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_PLAY_GIF).createImage());
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animatePlay();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setToolTipText("End");
	    toolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_GOTOEND_GIF).createImage());
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.gotoEnd();
			}
	    });
	    
	    // navigation buttons
		CoolItem coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(toolBar);
		//toolBar.setSize(toolBar.computeSize(SWT.DEFAULT, SWT.DEFAULT));
		coolItem.setSize(new Point(180, coolBarHeight));

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
		// simulation time label
		Composite labels = new Composite(coolBar, SWT.NONE);
		labels.setLayout(new RowLayout(SWT.HORIZONTAL));
		replayEventNumberWidget = new Text(labels, SWT.BORDER);
		replayEventNumberWidget.setLayoutData(new RowData(100, coolBarHeight - 10));
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
		replayAnimationNumberWidget.setLayoutData(new RowData(100, coolBarHeight - 10));
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
		replaySimulationTimeWidget.setLayoutData(new RowData(100, coolBarHeight - 10));
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
		replayAnimationTimeWidget.setLayoutData(new RowData(100, coolBarHeight - 10));
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
		coolItem.setSize(new Point(480, coolBarHeight));

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
		coolItem.setSize(new Point(200, coolBarHeight)); //XXX height has no effect

	    createAnimationController(parent);
/*
		animationMode.select(animationController.getAnimationMode().ordinal());
		animationMode.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				animationController.setAnimationMode(ReplayAnimationController.AnimationMode.values()[((Combo)e.getSource()).getSelectionIndex()]);
			}
		});
*/	}

	protected void createAnimationController(Composite parent) {
		AnimationCanvas canvas = new AnimationCanvas(parent, SWT.NONE);
		canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		animationController = new ReplayAnimationController(canvas, ((IFileEditorInput)getEditorInput()).getFile());
		animationController.addAnimationListener(this);
		animationController.init();
	}

	@Override
	public void setFocus() {
	}
	
	@Override
	public void dispose() {
		animationController.animateStop();
		animationController.shutdown();
		super.dispose();
	}

	public void replayPositionChanged(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		valueChanged(replayEventNumberWidget, eventNumber);
		valueChanged(replaySimulationTimeWidget, simulationTime);
		valueChanged(replayAnimationNumberWidget, animationNumber);
		valueChanged(replayAnimationTimeWidget, animationTime);
	}

	public void valueChanged(Text widget, Number newValue) {
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
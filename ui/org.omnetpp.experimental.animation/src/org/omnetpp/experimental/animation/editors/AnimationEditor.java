package org.omnetpp.experimental.animation.editors;

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
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.CoolBar;
import org.eclipse.swt.widgets.CoolItem;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.experimental.animation.controller.IAnimationController;
import org.omnetpp.experimental.animation.controller.IAnimationListener;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public class AnimationEditor extends EditorPart implements IAnimationListener {
	private IAnimationController animationController;

	private Text replayEventNumberWidget;

	private Text replaySimulationTimeWidget;

	private Text replayAnimationNumberWidget;

	private Text replayAnimationTimeWidget;
	
	public AnimationEditor() {
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
		CoolBar coolBar = new CoolBar(parent, SWT.NONE);
		coolBar.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
		
		// navigation tool bar
		ToolBar toolBar = new ToolBar(coolBar, SWT.NONE);

	    ToolItem toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setText("Begin");
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.gotoBegin();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setText("Back");
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animateBack();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setText("Stop");
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animateStop();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setText("Step");
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animateStep();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setText("Play");
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animatePlay();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setText("Fast");
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animateFast();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setText("Express");
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.animateExpress();
			}
	    });

	    toolItem = new ToolItem(toolBar, SWT.PUSH);
	    toolItem.setText("End");
	    toolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.gotoEnd();
			}
	    });
	    
	    // navigation buttons
		CoolItem coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(toolBar);
		coolItem.setSize(new Point(300, coolBarHeight));

		// animation mode selector
		Combo animationMode = new Combo(coolBar, SWT.NONE);
		for (ReplayAnimationController.AnimationMode t : ReplayAnimationController.AnimationMode.values())
			animationMode.add(t.name());
		animationMode.setVisibleItemCount(ReplayAnimationController.AnimationMode.values().length);
		coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(animationMode);
		coolItem.setSize(new Point(100, coolBarHeight));

		// simulation time label
		Composite labels = new Composite(coolBar, SWT.NONE);
		labels.setLayout(new RowLayout(SWT.HORIZONTAL));
		replayEventNumberWidget = new Text(labels, SWT.BORDER);
		replayEventNumberWidget.setLayoutData(new RowData(100, coolBarHeight - 10));
		replayEventNumberWidget.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				long eventNumber = Long.parseLong(((Text)e.widget).getText());
				if (eventNumber != animationController.getEventNumber())
					animationController.gotoEventNumber(eventNumber);
			}
		});
		replayAnimationNumberWidget = new Text(labels, SWT.BORDER);
		replayAnimationNumberWidget.setLayoutData(new RowData(100, coolBarHeight - 10));
		replayAnimationNumberWidget.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				long animationNumber = Long.parseLong(((Text)e.widget).getText());
				if (animationNumber != animationController.getAnimationNumber())
					animationController.gotoAnimationNumber(animationNumber);
			}
		});
		replaySimulationTimeWidget = new Text(labels, SWT.BORDER);
		replaySimulationTimeWidget.setLayoutData(new RowData(100, coolBarHeight - 10));
		replaySimulationTimeWidget.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				double simulationTime = Double.parseDouble(((Text)e.widget).getText());
				if (simulationTime != animationController.getSimulationTime())
					animationController.gotoSimulationTime(simulationTime);
			}
		});
		replayAnimationTimeWidget = new Text(labels, SWT.BORDER);
		replayAnimationTimeWidget.setLayoutData(new RowData(100, coolBarHeight - 10));
		replayAnimationTimeWidget.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				double animationTime = Double.parseDouble(((Text)e.widget).getText());
				if (animationTime != animationController.getAnimationTime())
					animationController.gotoAnimationTime(animationTime);
			}
		});
	    coolItem = new CoolItem(coolBar, SWT.NONE);
	    coolItem.setControl(labels);
		coolItem.setSize(new Point(480, coolBarHeight));

		// speed slider
	    Slider slider = new Slider(coolBar, SWT.HORIZONTAL);
	    slider.setMinimum(0);
	    slider.setMaximum(100);
	    slider.setThumb(1);
	    slider.setSelection((slider.getMaximum() - slider.getMinimum()) / 2); 
	    slider.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				Slider slider = (Slider)e.widget;
				int scale = ((Slider)e.widget).getSelection() - (slider.getMaximum() - slider.getMinimum()) / 2;
				animationController.setRealTimeToAnimationTimeScale(Math.pow(1.1, scale));
			}	    	
	    });
	    coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(slider);

	    AnimationCanvas canvas = new AnimationCanvas(parent, SWT.NONE);
		canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		animationController = new ReplayAnimationController(canvas, ((IFileEditorInput)getEditorInput()).getFile());
//		animationController = new LiveAnimationController(canvas);
		animationController.addAnimationListener(this);

		animationMode.select(animationController.getAnimationMode().ordinal());
		animationMode.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				animationController.setAnimationMode(ReplayAnimationController.AnimationMode.values()[((Combo)e.getSource()).getSelectionIndex()]);
			}
		});
	}

	@Override
	public void setFocus() {
	}
	
	@Override
	public void dispose() {
		animationController.animateStop();
		super.dispose();
	}

	public void replayEventNumberChanged(long eventNumber) {
		long oldEventNumber = -1;
		
		try {
			oldEventNumber = Long.parseLong(replayEventNumberWidget.getText());
		}
		catch (NumberFormatException e) {
			// void
		}
		
		if (oldEventNumber != eventNumber)
			replayEventNumberWidget.setText(String.valueOf(eventNumber));
	}

	public void replaySimulationTimeChanged(double simulationTime) {
		double oldSimulationTime = -1;
		
		try {
			oldSimulationTime = Double.parseDouble(replaySimulationTimeWidget.getText());
		}
		catch (NumberFormatException e) {
			// void
		}
		
		if (oldSimulationTime != simulationTime)
			replaySimulationTimeWidget.setText(String.valueOf(simulationTime));
	}

	public void replayAnimationNumberChanged(long animationNumber) {
		long oldAnimationNumber = -1; 
		
		try {
			oldAnimationNumber = Long.parseLong(replayAnimationNumberWidget.getText());
		}
		catch (NumberFormatException e) {
			// void
		}
		
		if (oldAnimationNumber != animationNumber)
			replayAnimationNumberWidget.setText(String.valueOf(animationNumber));
	}

	public void replayAnimationTimeChanged(double animationTime) {
		double oldAnimationTime = -1;
		
		try {
			oldAnimationTime = Double.parseDouble(replayAnimationTimeWidget.getText());
		}
		catch (NumberFormatException e) {
			// void
		}
		
		if (oldAnimationTime != animationTime)
			replayAnimationTimeWidget.setText(String.valueOf(animationTime));
	}

	public void liveEventNumberChanged(long eventNumber) {
	}

	public void liveSimulationTimeChanged(double simulationTime) {
	}

	public void liveAnimationNumberChanged(long animationNumber) {
	}

	public void liveAnimationTimeChanged(double animationTime) {
	}
}

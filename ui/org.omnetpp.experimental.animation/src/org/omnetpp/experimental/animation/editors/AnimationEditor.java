package org.omnetpp.experimental.animation.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
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
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.experimental.animation.controller.IAnimationListener;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public class AnimationEditor extends EditorPart implements IAnimationListener {
	private ReplayAnimationController animationController;

	private Label replaySimulationTimeLabel;

	private Label replayEventNumberLabel;
	
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
		coolItem.setSize(new Point(300, 25));

		// animation mode selector
		Combo animationMode = new Combo(coolBar, SWT.NONE);
		for (ReplayAnimationController.AnimationMode t : ReplayAnimationController.AnimationMode.values())
			animationMode.add(t.name());
		animationMode.setVisibleItemCount(ReplayAnimationController.AnimationMode.values().length);
		coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(animationMode);
		coolItem.setSize(new Point(100, 25));

		// simulation time label
		Composite labels = new Composite(coolBar, SWT.NONE);
		labels.setLayout(new RowLayout(SWT.HORIZONTAL));
		replaySimulationTimeLabel = new Label(labels, SWT.NONE);
		replaySimulationTimeLabel.setLayoutData(new RowData(100, 25));
		replayEventNumberLabel = new Label(labels, SWT.NONE);
		replayEventNumberLabel.setLayoutData(new RowData(100, 25));
	    coolItem = new CoolItem(coolBar, SWT.NONE);
	    coolItem.setControl(labels);
		coolItem.setSize(new Point(230, 25));

		// speed slider
	    Slider slider = new Slider(coolBar, SWT.HORIZONTAL);
	    slider.setMinimum(0);
	    slider.setMaximum(100);
	    slider.setThumb(1);
	    slider.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				int scale = ((Slider)e.widget).getSelection();
				animationController.setRealTimeToAnimationTimeScale(Math.pow(0.9, scale));
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
		replayEventNumberLabel.setText(String.valueOf(eventNumber));
	}

	public void replaySimulationTimeChanged(double simulationTime) {
		replaySimulationTimeLabel.setText(String.valueOf(simulationTime));
	}

	public void liveEventNumberChanged(long eventNumber) {
	}

	public void liveSimulationTimeChanged(double simulationTime) {
	}
}

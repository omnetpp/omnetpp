package org.omnetpp.experimental.animation.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.CoolBar;
import org.eclipse.swt.widgets.CoolItem;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public class AnimationEditor extends EditorPart {
	private ReplayAnimationController animationController;
	
	public AnimationEditor() {
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void doSaveAs() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		setSite(site);
		setInput(input);
		
		setPartName(input.getName());
	}

	@Override
	public boolean isDirty() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void createPartControl(Composite parent) {
		parent.setLayout(new GridLayout());

		CoolBar coolBar = new CoolBar(parent, SWT.NONE);
		coolBar.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
		
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
	    
		CoolItem coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(toolBar);
		coolItem.setSize(coolItem.computeSize(toolBar.getSize().x, toolBar.getSize().y));
		coolItem.setSize(new Point(300, 25));
		coolItem.setMinimumSize(toolBar.getSize());

	    Slider slider = new Slider(coolBar, SWT.HORIZONTAL);
	    slider.setMinimum(0);
	    slider.setMaximum(10);
	    slider.setThumb(1);
	    slider.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				animationController.setSpeed(((Slider)e.widget).getSelection());
			}	    	
	    });
	    coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(slider);
		coolItem.setSize(coolItem.computeSize(slider.getSize().x, slider.getSize().y));
		coolItem.setMinimumSize(slider.getSize());

	    AnimationCanvas canvas = new AnimationCanvas(parent, SWT.NONE);
		canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		animationController = new ReplayAnimationController(canvas);
	}

	@Override
	public void setFocus() {
		// TODO Auto-generated method stub
		
	}
	
	@Override
	public void dispose() {
		animationController.animateStop();
		super.dispose();
	}
}

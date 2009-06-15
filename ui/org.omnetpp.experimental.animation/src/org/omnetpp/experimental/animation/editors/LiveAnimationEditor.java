/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.editors;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.CoolItem;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.IFileEditorInput;
import org.omnetpp.experimental.animation.AnimationPlugin;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.controller.ILiveAnimationListener;
import org.omnetpp.experimental.animation.live.LiveAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;
import org.omnetpp.runtime.nativelibs.simkernel.Simkernel;
import org.omnetpp.runtime.nativelibs.simkernel.cEnvir;
import org.omnetpp.runtime.nativelibs.simkernel.cStaticFlag;

public class LiveAnimationEditor extends ReplayAnimationEditor implements ILiveAnimationListener {
	private static final String ICONS_FINISH_GIF = "icons/finish.gif";
	private static final String ICONS_STOPSIM_GIF = "icons/stopsim.gif";
	private static final String ICONS_EXPRESS_GIF = "icons/express.gif";
	private static final String ICONS_FAST_GIF = "icons/fast.gif";
	private static final String ICONS_RUN_GIF = "icons/run.gif";
	private static final String ICONS_DOONEEVENT_GIF = "icons/dooneevent.gif";
	private static final String ICONS_RESTART_GIF = "icons/restart.gif";

	protected cStaticFlag staticFlag;
	protected ToolBar liveToolBar;
	protected ToolItem liveStopToolItem; 
	protected ToolItem liveRestartToolItem;
	protected ToolItem liveStepToolItem;
	protected ToolItem liveRunToolItem;
	protected ToolItem liveFastToolItem;
	protected ToolItem liveExpressToolItem;
	protected ToolItem liveFinishToolItem; 
	
	@Override
	public void dispose() {
		super.dispose();

		// see matching code in createPartControl()
		AnimationPlugin.getDefault().setCurrentLiveAnimation(null);
		
		// release simkernel
		shutdownSimulation();
	}

	@Override
	public void createPartControl(Composite parent) {
		this.parent = parent;

		// make sure there's only one simulation running at a time
		if (AnimationPlugin.getDefault().getCurrentLiveAnimation()!=null) {
			MessageDialog.openError(null, "Error", "There can only be one simulation running at a time.");
		    getSite().getPage().closeEditor(this, false);
		    return;
		}
		AnimationPlugin.getDefault().setCurrentLiveAnimation(this);

		// initialize simulation
		initSimulation();

		// now create controls.
		// note: cannot simply call super.createPartControl(parent) because
		// we want toolbars in different order (could use coolBar.setItemLayout() though)
		this.parent = parent;
		parent.setLayout(new FormLayout());
		parent.setBackground(new Color(null, 228, 228, 228));
		
		createCoolbar();
		createSimulationToolbar();
		createNavigationToolbar();
		createTimeGauges();
		createSpeedSlider();

		createAnimationController();
		
		coolBar.setWrapIndices(new int[] {2,3});
	}

	private void initSimulation() {
		// read the ".launch" file and extract inifile name from it
	    IFile launchFile = ((IFileEditorInput)getEditorInput()).getFile();
		String inifilePathName = null;
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(launchFile.getContents()));
			String line = reader.readLine();
			String[] tokens = line.split(" ");
			if (tokens.length>=2 && tokens[0].equals("inifile"))
				inifilePathName = tokens[1];
			else
				throw new RuntimeException(launchFile+": invalid syntax"); //XXX better error handling
		} catch (CoreException e) {
			e.printStackTrace(); //XXX better error handling
			return; //XXX close editor etc
		} catch (IOException e) {
			e.printStackTrace(); //XXX better error handling
			return; //XXX close editor etc
		}

		// change into the ini file's directory
		IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
		IPath inifilePath = new Path(inifilePathName);
		if (!inifilePath.isAbsolute())
			inifilePath = launchFile.getParent().getFullPath().append(inifilePath);
		IResource inifile = workspaceRoot.findMember(inifilePath);
		if (!(inifile instanceof IFile))
			throw new RuntimeException("File "+inifilePath+" does not exist"); //XXX better error handling
		String inifileDir = inifile.getRawLocation().removeLastSegments(1).toOSString();

		Simkernel.changeToDir(inifileDir);

		staticFlag = new cStaticFlag(); // needed by sim kernel
		Simkernel.evSetup(inifile.getName()); // ev.setup(argc, argv);

		//TEST CODE:
		//Javaenv jenv = Simkernel.getJavaenv();
		//jenv.setJCallback(null, new DummyEnvirCallback());

		//cEnvir ev = Simkernel.getEV();
		//cSimulation simulation = Simkernel.getSimulation();

		//jenv.newRun(1);
		//for (int i=0; i<10; i++)
		//	jenv.doOneStep();
	}

	private void shutdownSimulation() {
		cEnvir ev = Simkernel.getEV();
		ev.shutdown();
		staticFlag.delete();
		staticFlag = null;
	}

	private void createSimulationToolbar() {
		liveToolBar = new ToolBar(coolBar, SWT.NONE);

		liveRestartToolItem = new ToolItem(liveToolBar, SWT.PUSH);
		liveRestartToolItem.setToolTipText("Restart");
		liveRestartToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_RESTART_GIF).createImage());
		liveRestartToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().restartSimulation();
				
				liveStopToolItem.setEnabled(false);

				liveRestartToolItem.setEnabled(true);
				liveStepToolItem.setEnabled(true);
				liveRunToolItem.setEnabled(true);
				liveFastToolItem.setEnabled(true);
				liveExpressToolItem.setEnabled(true);
				liveFinishToolItem.setEnabled(true); 
			}
		});

		liveStepToolItem = new ToolItem(liveToolBar, SWT.PUSH);
		liveStepToolItem.setToolTipText("Step");
	    liveStepToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_DOONEEVENT_GIF).createImage());
		liveStepToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().stepSimulation();
			}
		});
		
		liveRunToolItem = new ToolItem(liveToolBar, SWT.PUSH);
		liveRunToolItem.setToolTipText("Run");
	    liveRunToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_RUN_GIF).createImage());
		liveRunToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().runSimulation();
			}
		});
		
		liveFastToolItem = new ToolItem(liveToolBar, SWT.PUSH);
		liveFastToolItem.setToolTipText("Fast");
	    liveFastToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_FAST_GIF).createImage());
		liveFastToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().runSimulationFast();
			}
		});

		liveExpressToolItem = new ToolItem(liveToolBar, SWT.PUSH);
		liveExpressToolItem.setToolTipText("Express");
	    liveExpressToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_EXPRESS_GIF).createImage());
		liveExpressToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().runSimulationExpress();
			}
		});

		liveStopToolItem = new ToolItem(liveToolBar, SWT.PUSH);
		liveStopToolItem.setEnabled(false);
		liveStopToolItem.setToolTipText("Stop");
		liveStopToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_STOPSIM_GIF).createImage());
		liveStopToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().stopSimulation();
			}
		});

		liveFinishToolItem = new ToolItem(liveToolBar, SWT.PUSH);
		liveFinishToolItem.setToolTipText("Finish");
	    liveFinishToolItem.setImage(AnimationPlugin.getImageDescriptor(ICONS_FINISH_GIF).createImage());
		liveFinishToolItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				getLiveAnimationController().finishSimulation();
			}
		});
		
		//toolBar.setSize(toolBar.computeSize(SWT.DEFAULT, SWT.DEFAULT)); XXX does not work

		CoolItem coolItem = new CoolItem(coolBar, SWT.NONE);
		coolItem.setControl(liveToolBar);

		coolItem.setSize(new Point(270, COOLBAR_HEIGHT));
	}
	
	@Override
	protected void createAnimationController() {
		AnimationCanvas canvas = createAnimationCanvas();
		
		animationController = new LiveAnimationController(canvas);
		animationController.addAnimationListener(this);
		animationController.restart();
	}

	protected LiveAnimationController getLiveAnimationController() {
		return ((LiveAnimationController)animationController);
	}

	@Override
	public void controllerStateChanged() {		
		super.controllerStateChanged();

		if (!liveToolBar.isDisposed()) {
			if (getLiveAnimationController().isRunning() && getLiveAnimationController().isRunningLive())
				liveStopToolItem.setEnabled(true);
			else
				liveStopToolItem.setEnabled(false);
			
			if (getLiveAnimationController().isSimulationFinished()) {
				liveRestartToolItem.setEnabled(true);
	
				liveStopToolItem.setEnabled(false);
				liveStepToolItem.setEnabled(false);
				liveRunToolItem.setEnabled(false);
				liveFastToolItem.setEnabled(false);
				liveExpressToolItem.setEnabled(false);
				liveFinishToolItem.setEnabled(false); 
			}
		}
	}

	public void livePositionChanged(AnimationPosition animationPosition) {
		// TODO:
	}
}
/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.editors;

import java.text.NumberFormat;
import java.text.ParseException;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
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
import org.omnetpp.common.canvas.BorderedComposite;
import org.omnetpp.experimental.animation.AnimationPlugin;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.controller.IReplayAnimationListener;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public class ReplayAnimationEditor extends EditorPart implements IReplayAnimationListener {
	protected static final String ICONS_REWIND_GIF = "icons/rewind.gif";
	protected static final String ICONS_GOTOEND_GIF = "icons/gotoend.gif";
	protected static final String ICONS_PLAY_GIF = "icons/play.gif";
	protected static final String ICONS_BACKPLAY_GIF = "icons/backplay.gif";
	protected static final String ICONS_STEP_GIF = "icons/step.gif";
	protected static final String ICONS_BACKSTEP_GIF = "icons/backstep.gif";
	protected static final String ICONS_STOP_GIF = "icons/stop.gif";

	protected ReplayAnimationController animationController;

	protected Composite parent;
	protected CoolBar coolBar;
	protected ToolBar replayToolBar;
	protected ToolItem replayStopToolItem;
	protected Text replayEventNumberWidget;
	protected Text replaySimulationTimeWidget;
	protected Text replayAnimationNumberWidget;
	protected Text replayAnimationTimeWidget;

	protected NumberFormat numberFormat;
	protected ToolItem replayBeginToolItem;
	protected ToolItem replayBackToolItem;
	protected ToolItem replayBackstepToolItem;
	protected ToolItem replayStepToolItem;
	protected ToolItem replayPlayToolItem;
	protected ToolItem replayEndToolItem;
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
		parent.setLayout(new FillLayout());
		this.parent = new Composite(parent, SWT.V_SCROLL | SWT.H_SCROLL);
		this.parent.setLayout(new FormLayout());
		this.parent.setBackground(new Color(null, 228, 228, 228));
		createCoolbar();

		createNavigationToolbar();
		createTimeGauges();
		createSpeedSlider();

		createAnimationController();

		coolBar.setWrapIndices(new int[] {2});
	}

	protected void createCoolbar() {
		BorderedComposite borderedComposite = new BorderedComposite(parent, SWT.NONE);
		FormData formData = new FormData();
		formData.left = new FormAttachment(0, 0);
		formData.top = new FormAttachment(0, 0);
		formData.right = new FormAttachment(100, 0);
		borderedComposite.setLayoutData(formData);
		FormLayoutMouseListener listener = new FormLayoutMouseListener(false);
		borderedComposite.addMouseListener(listener);
		borderedComposite.addMouseMoveListener(listener);
		borderedComposite.addMouseTrackListener(listener);
		coolBar = new CoolBar(borderedComposite, SWT.NONE);
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

	protected void createAnimationController() {
		AnimationCanvas canvas = createAnimationCanvas();

		animationController = new ReplayAnimationController(canvas, ((IFileEditorInput)getEditorInput()).getFile());
		animationController.addAnimationListener(this);
		animationController.restart();
	}

	public AnimationCanvas createAnimationCanvas() {
		BorderedComposite borderedComposite = new BorderedComposite(parent, SWT.NONE);
		FormData formData = new FormData();
		formData.left = new FormAttachment(0, 0);
		formData.top = new FormAttachment(coolBar.getParent(), 0);
		formData.right = new FormAttachment(100, 0);
		formData.bottom = new FormAttachment(100, 0);
		borderedComposite.setLayoutData(formData);
		FormLayoutMouseListener listener = new FormLayoutMouseListener(true);
		borderedComposite.addMouseListener(listener);
		borderedComposite.addMouseMoveListener(listener);
		borderedComposite.addMouseTrackListener(listener);

		return new AnimationCanvas(borderedComposite, SWT.DOUBLE_BUFFERED);
	}

	public class FormLayoutMouseListener implements MouseListener, MouseMoveListener, MouseTrackListener {
		protected boolean allowHorizontalResize;

		protected boolean allowVerticalResize;

		protected BorderedComposite dragControl;

		protected Point dragStart;

		protected Point dragStartControlSize;

		protected Point dragStartControlLocation;

		protected boolean dragLeft;

		protected boolean dragRight;

		protected boolean dragTop;

		protected boolean dragBottom;

		protected boolean dragMove;

		public FormLayoutMouseListener(boolean allowResize) {
			this(allowResize, allowResize);
		}

		public FormLayoutMouseListener(boolean allowHorizontalResize, boolean allowVerticalResize) {
			this.allowHorizontalResize = allowHorizontalResize;
			this.allowVerticalResize = allowVerticalResize;
		}

		public void mouseDoubleClick(MouseEvent e) {
			extractDragControlInformation(e);
	
			if ((e.stateMask & SWT.CONTROL) != 0) {
				if (dragControl.getChildren()[0] instanceof AnimationCanvas) {
					AnimationCanvas animationCanvas = (AnimationCanvas)dragControl.getChildren()[0];
					Point size = animationCanvas.getPreferredSize();
					FormData formData = (FormData)dragControl.getLayoutData();
					formData.right = new FormAttachment(0, dragControl.getLocation().x + size.x + dragControl.getBorderSize() * 2);
					formData.bottom = new FormAttachment(0, dragControl.getLocation().y + size.y + dragControl.getBorderSize() * 2);
					parent.layout();
				}
			}
			else {
				Point delta = new Point(0, 0);
				DockingLimits dockingLimits = new DockingLimits(10000, 0, 0, 10000);
				calculateDragMode(e);
				boolean oldDragLeft = dragLeft;
				boolean oldDragRight = dragRight;
				boolean oldDragTop = dragTop;
				boolean oldDragBottom = dragBottom;

				dragRight = dragTop = dragBottom = false;
				dragLeft = oldDragLeft & allowHorizontalResize;
				moveOrResizeDraggedControl(delta, dockingLimits);
				parent.layout();
		
				calculateDragMode(e);
				dragLeft = dragTop = dragBottom = false;
				dragRight = oldDragRight & allowHorizontalResize;
				moveOrResizeDraggedControl(delta, dockingLimits);
				parent.layout();
		
				calculateDragMode(e);
				dragLeft = dragRight = dragBottom = false;
				dragTop = oldDragTop & allowVerticalResize;
				moveOrResizeDraggedControl(delta, dockingLimits);
				parent.layout();
		
				calculateDragMode(e);
				dragLeft = dragRight = dragTop = false;
				dragBottom = oldDragBottom & allowVerticalResize;
				moveOrResizeDraggedControl(delta, dockingLimits);
				parent.layout();
			}

			dragControl = null;
		}

		public void mouseDown(MouseEvent e) {
			extractDragControlInformation(e);
			dragControl.moveAbove(null);
			dragStart = dragControl.toDisplay(e.x, e.y);
	
			calculateDragMode(e);
		}

		public void mouseUp(MouseEvent e) {
			dragControl = null;
		}

		public void mouseMove(MouseEvent e) {
			if (dragControl != null) {
				Control control = (Control)e.widget;
				Point p = control.toDisplay(e.x, e.y);
				Point delta = new Point(p.x - dragStart.x, p.y - dragStart.y);

				moveOrResizeDraggedControl(delta, new DockingLimits((e.stateMask & SWT.CONTROL) == 0 ? 10 : 0));

				parent.layout();
			}
			else
				updateCursor(e);
		}

		public void mouseEnter(MouseEvent e) {
			updateCursor(e);
		}

		public void mouseExit(MouseEvent e) {
			Control control = (Control)e.widget;
			control.setCursor(null);
		}

		public void mouseHover(MouseEvent e) {
		}

		protected class DockingLimits {
			public int maximumSmallerValueDecrease;
	
			public int maximumSmallerValueIncrease;
	
			public int maximumBiggerValueDecrease;

			public int maximumBiggerValueIncrease;

			public DockingLimits(int maximumValueChange) {
				this(maximumValueChange, maximumValueChange, maximumValueChange, maximumValueChange);
			}

			public DockingLimits(int maximumSmallerValueDecrease, int maximumSmallerValueIncrease, int maximumBiggerValueDecrease, int maximumBiggerValueIncrease) {
				this.maximumSmallerValueDecrease = maximumSmallerValueDecrease;
				this.maximumSmallerValueIncrease = maximumSmallerValueIncrease;
				this.maximumBiggerValueDecrease = maximumBiggerValueDecrease;
				this.maximumBiggerValueIncrease = maximumBiggerValueIncrease;
			}
		}

		protected void extractDragControlInformation(MouseEvent e) {
			dragControl = (BorderedComposite)e.widget;
			dragStartControlSize = dragControl.getSize();
			dragStartControlLocation = dragControl.getLocation();
		}

		protected void moveOrResizeDraggedControl(Point delta, DockingLimits dockingLimits)
		{
			int left = dragStartControlLocation.x + delta.x;
			int right = dragStartControlLocation.x + dragStartControlSize.x + delta.x;
			int top = dragStartControlLocation.y + delta.y;
			int bottom = dragStartControlLocation.y + dragStartControlSize.y + delta.y;
			FormAttachment[] horizontalFormAttachments = getDockingAttachments(true, left, right, dockingLimits);
			FormAttachment[] verticalFormAttachments = getDockingAttachments(false, top, bottom, dockingLimits);
			FormData formData = (FormData)dragControl.getLayoutData();
			if (dragLeft)
				formData.left = horizontalFormAttachments != null ? horizontalFormAttachments[0] : new FormAttachment(0, left);
			if (dragRight)
				formData.right = horizontalFormAttachments != null ? horizontalFormAttachments[1] : new FormAttachment(0, right);
			if (dragTop)
				formData.top = verticalFormAttachments != null ? verticalFormAttachments[0] : new FormAttachment(0, top);
			if (dragBottom)
				formData.bottom = verticalFormAttachments != null ? verticalFormAttachments[1] : new FormAttachment(0, bottom);
		}

		protected void updateCursor(MouseEvent e) {
			Control control = (Control)e.widget;

			calculateDragMode(e);
	
			int cursorType;
			if (dragMove)
				cursorType = SWT.CURSOR_SIZEALL;
			else if (dragLeft && !dragRight && !dragTop && !dragBottom)
				cursorType = SWT.CURSOR_SIZEW;
			else if (!dragLeft && dragRight && !dragTop && !dragBottom)
				cursorType = SWT.CURSOR_SIZEE;
			else if (!dragLeft && !dragRight && dragTop && !dragBottom)
				cursorType = SWT.CURSOR_SIZEN;
			else if (!dragLeft && !dragRight && !dragTop && dragBottom)
				cursorType = SWT.CURSOR_SIZES;
			else if (dragLeft && !dragRight && dragTop && !dragBottom)
				cursorType = SWT.CURSOR_SIZENW;
			else if (!dragLeft && dragRight && dragTop && !dragBottom)
				cursorType = SWT.CURSOR_SIZENE;
			else if (!dragLeft && dragRight && !dragTop && dragBottom)
				cursorType = SWT.CURSOR_SIZESE;
			else if (dragLeft && !dragRight && !dragTop && dragBottom)
				cursorType = SWT.CURSOR_SIZESW;
			else
				cursorType = SWT.CURSOR_ARROW;

			control.setCursor(new Cursor(null, cursorType));
		}

		protected void calculateDragMode(MouseEvent e) {
			Control control = ((Control)e.widget);
			Point size = control.getSize();
			int dragMinHandleSize = 20;
			int dragHandleWidth = Math.min(dragMinHandleSize, size.x / 6);
			int dragHandleHeight = Math.min(dragMinHandleSize, size.y / 6);
			int dragX = getDragValue(e.x, size.x, dragHandleWidth);
			int dragY = getDragValue(e.y, size.y, dragHandleHeight);

			dragLeft = dragX == 0 || dragX == 3 || dragY == 3;
			dragRight = dragX == 2 || dragX == 3 || dragY == 3;

			if (!allowHorizontalResize)
				dragLeft = dragRight = dragLeft || dragRight;

			dragTop = dragY == 0 || dragY == 3 || dragX == 3;
			dragBottom = dragY == 2 || dragY == 3 || dragX == 3;

			if (!allowVerticalResize)
				dragTop = dragBottom = dragTop || dragBottom;

			dragMove = dragLeft && dragRight && dragTop & dragBottom;
		}

		protected int getDragValue(int p, int controlSize, int dragHandleSize) {
			if (0 <= p && p <= dragHandleSize)
				return 0;
			else if (controlSize / 2 - dragHandleSize <= p && p <= controlSize / 2 + dragHandleSize)
				return 1;
			else if (controlSize - dragHandleSize <= p && p <= controlSize)
				return 2;
			else
				return 3;
		}

		protected FormAttachment[] getDockingAttachments(boolean horizontal, int smallerValue, int biggerValue, DockingLimits dockingLimits) {
			FormAttachment[] formAttachments = new FormAttachment[] {null, null};
			Rectangle parentRectangle = parent.getClientArea();
			int minValue = 0;
			int maxValue = horizontal ? parentRectangle.width : parentRectangle.height;
			int bestDockingDistance = Integer.MAX_VALUE;
			int size = biggerValue - smallerValue;
			boolean dragSmaller = horizontal ? dragLeft : dragTop;
			boolean dragBigger = horizontal ? dragRight : dragBottom;

			// docking to parent
			bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, null, dragSmaller, smallerValue, minValue, true, 0, size);
			bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, null, dragBigger, biggerValue, maxValue, false, 100, size);

			// docking to siblings
			for (Control childControl : parent.getChildren()) {
				if (childControl == dragControl)
					continue;

				Point childLocation = childControl.getLocation();
				Point childSize = childControl.getSize();
				int childSmallerValue = horizontal ? childLocation.x : childLocation.y;
				int childBiggerValue = horizontal ? childLocation.x + childSize.x : childLocation.y + childSize.y;

				bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, childControl, dragSmaller, smallerValue, childSmallerValue, true, 0, size);
				bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, childControl, dragSmaller, smallerValue, childBiggerValue, true, 0, size);
				bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, childControl, dragBigger, biggerValue, childSmallerValue, false, 0, size);
				bestDockingDistance = setDockingFormAttachments(formAttachments, bestDockingDistance, dockingLimits, childControl, dragBigger, biggerValue, childBiggerValue, false, 0, size);
			}
	
			if (bestDockingDistance != Integer.MAX_VALUE)
				return formAttachments;
			else
				return null;
		}

		protected int setDockingFormAttachments(FormAttachment[] formAttachments,
											  int bestDistance,
											  DockingLimits dockingLimits,
											  Control dockingControl,
											  boolean dragFlag,
											  int currentValue,
											  int dockingValue,
											  boolean smallerValue,
											  int parentPercent,
											  int size) {
			int currentDockingDistance = Math.abs(currentValue - dockingValue);
			int maximumValueIncrease = smallerValue ? dockingLimits.maximumSmallerValueIncrease : dockingLimits.maximumBiggerValueIncrease;
			int maximumValueDecrease = smallerValue ? dockingLimits.maximumSmallerValueDecrease : dockingLimits.maximumBiggerValueDecrease;

			if (dragFlag &&
				currentDockingDistance < bestDistance &&
				dockingValue - maximumValueIncrease <= currentValue &&
				currentValue <= dockingValue + maximumValueDecrease)
			{
/* TODO: if you ever want to attach to siblings then use this and break dependeny cycles
				if (dockingControl != null) {
					formAttachments[0] = new FormAttachment(dockingControl, smallerValue ? 0 : -size);
					formAttachments[1] = new FormAttachment(dockingControl, smallerValue ? size : 0);
				}
*/
				formAttachments[0] = new FormAttachment(parentPercent, (parentPercent == 0 ? dockingValue : 0) + (smallerValue ? 0 : -size));
				formAttachments[1] = new FormAttachment(parentPercent, (parentPercent == 0 ? dockingValue : 0) + (smallerValue ? size : 0));
				
				return currentDockingDistance;
			}
			else
				return bestDistance;
		}
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

	public void replayPositionChanged(AnimationPosition animationPosition) {
		if (!replayToolBar.isDisposed()) {
			if (animationController.isCurrentAnimationPositionValid() && animationController.isAtAnimationBegin()) {
				replayBeginToolItem.setEnabled(false);
				replayBackToolItem.setEnabled(false);
				replayBackstepToolItem.setEnabled(false);
			}
			else {
				replayBeginToolItem.setEnabled(true);
				replayBackToolItem.setEnabled(true);
				replayBackstepToolItem.setEnabled(true);
			}

			if (animationController.isCurrentAnimationPositionValid() && animationController.isAtAnimationEnd()) {
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

		valueChanged(replayEventNumberWidget, animationPosition.getEventNumber());
		valueChanged(replaySimulationTimeWidget, animationPosition.getSimulationTime());
		valueChanged(replayAnimationNumberWidget, animationPosition.getAnimationNumber());
		valueChanged(replayAnimationTimeWidget, animationPosition.getAnimationTime());
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
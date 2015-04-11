/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.editors;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IViewSite;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.contexts.IContextService;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.providers.IAnimationPrimitiveProvider;
import org.omnetpp.animation.widgets.AnimationCanvas;
import org.omnetpp.common.eventlog.EventLogView;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.eventlog.engine.IEventLog;

/**
 * View for animation files.
 *
 * @author levy
 */
public abstract class AnimationView extends EventLogView implements IAnimationCanvasProvider {
    private AnimationCanvas animationCanvas;
    private AnimationContributor animationContributor;
    private AnimationController animationController;
    private IAnimationPrimitiveProvider animationPrimitiveProvider;
	private ISelectionListener selectionListener;
	private IPartListener partListener;

	public AnimationView() {
	}

    public AnimationCanvas getAnimationCanvas() {
        return animationCanvas;
    }

    public AnimationController getAnimationController() {
        return animationController;
    }

    public AnimationContributor getAnimationContributor() {
        return animationContributor;
    }

	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);
        IViewSite viewSite = (IViewSite)getSite();
        viewSite.setSelectionProvider(animationCanvas);
        IContextService contextService = (IContextService)viewSite.getService(IContextService.class);
        contextService.activateContext("org.omnetpp.context.AnimationCanvas");
        // contribute to toolbar
        animationContributor = new AnimationContributor(animationCanvas);
        animationCanvas.setAnimationContributor(animationContributor);
        animationContributor.contributeToToolBar(viewSite.getActionBars().getToolBarManager(), true);
        // follow selection
        selectionListener = new ISelectionListener() {
            public void selectionChanged(IWorkbenchPart part, ISelection selection) {
                if (followSelection && part != AnimationView.this && selection instanceof IEventLogSelection)
                    setSelection(selection);
            }
        };
        viewSite.getPage().addSelectionListener(selectionListener);
		// follow active editor changes
		partListener = new IPartListener() {
			public void partActivated(IWorkbenchPart part) {
			}

			public void partBroughtToTop(IWorkbenchPart part) {
                if (followSelection && part instanceof IEditorPart && !animationCanvas.isDisposed())
                    setSelectionFromActiveEditor();
			}

			public void partClosed(IWorkbenchPart part) {
                if (followSelection && part instanceof IEditorPart && !animationCanvas.isDisposed())
                    setSelectionFromActiveEditor();
			}

			public void partDeactivated(IWorkbenchPart part) {
			}

			public void partOpened(IWorkbenchPart part) {
                if (followSelection && part instanceof IEditorPart && !animationCanvas.isDisposed())
                    setSelectionFromActiveEditor();
			}
		};
		viewSite.getPage().addPartListener(partListener);
		// bootstrap with current selection
		selectionListener.selectionChanged(null, getActiveEditorSelection());
	}

	@Override
	public void dispose() {
		IViewSite viewSite = (IViewSite)getSite();
		if (selectionListener != null)
			viewSite.getPage().removeSelectionListener(selectionListener);
		if (partListener != null)
			viewSite.getPage().removePartListener(partListener);
        animationController.stopAnimation();
		super.dispose();
	}

	@Override
	public void setFocus() {
	    animationCanvas.setFocus();
	}

	@Override
	protected Control createViewControl(Composite parent) {
	    animationCanvas = createAnimationCanvas(parent);
        animationPrimitiveProvider = createAnimationPrimitiveProvider();
        animationController = createAnimationController();
        animationCanvas.setAnimationController(animationController);
        animationCanvas.setWorkbenchPart(this);
        animationPrimitiveProvider.setAnimationController(animationController);
        animationController.clearInternalState();
        animationController.gotoInitialAnimationPosition();
	    return animationCanvas;
	}

    protected AnimationCanvas createAnimationCanvas(Composite parent) {
        return new AnimationCanvas(parent, SWT.NONE);
    }

    protected AnimationController createAnimationController() {
        return new AnimationController(null, animationPrimitiveProvider);
    }

    protected abstract IAnimationPrimitiveProvider createAnimationPrimitiveProvider();

	@Override
    public void setSelection(ISelection selection) {
        if (selection instanceof IEventLogSelection) {
            hideMessage();
            animationCanvas.setSelection(selection);
        }
        else {
            animationCanvas.setInput(null);
            showMessage("No event log available");
        }
    }

    @Override
    public IEventLog getEventLog() {
        return animationCanvas.getEventLog();
    }
}

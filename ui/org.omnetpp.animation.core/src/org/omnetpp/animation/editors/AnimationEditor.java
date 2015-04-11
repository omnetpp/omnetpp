/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.editors;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.contexts.IContextService;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.IShowInSource;
import org.eclipse.ui.part.IShowInTargetList;
import org.eclipse.ui.part.ShowInContext;
import org.omnetpp.animation.AnimationCorePlugin;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.controller.IAnimationCoordinateSystem;
import org.omnetpp.animation.providers.IAnimationPrimitiveProvider;
import org.omnetpp.animation.widgets.AnimationCanvas;
import org.omnetpp.animation.widgets.AnimationTimeline;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.eventlog.EventLogEditor;
import org.omnetpp.common.eventlog.IEventLogSelection;

/**
 * Editor for animation files.
 *
 * @author levy
 */
public abstract class AnimationEditor extends EventLogEditor implements IAnimationCanvasProvider, INavigationLocationProvider, IShowInSource, IShowInTargetList
{
    private AnimationCanvas animationCanvas;
    private AnimationTimeline animationTimeline;
    private AnimationController animationController;
    private IAnimationPrimitiveProvider animationPrimitiveProvider;
    private ISelectionListener selectionListener;
    private ResourceChangeListener resourceChangeListener = new ResourceChangeListener();

    public AnimationCanvas getAnimationCanvas() {
        return animationCanvas;
    }

    public AnimationTimeline getAnimationTimeline() {
        return animationTimeline;
    }

    public AnimationController getAnimationController() {
        return animationController;
    }

    public IAnimationPrimitiveProvider getAnimationPrimitiveProvider() {
        return animationPrimitiveProvider;
    }

	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        super.init(site, input);
        ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener);
        IContextService contextService = (IContextService)site.getService(IContextService.class);
        contextService.activateContext("org.omnetpp.context.AnimationCanvas");

        // try to open the log view after all events are processed
        Display.getDefault().asyncExec(new Runnable() {
            @Override
            public void run() {
                try {
                    // Eclipse feature: during startup, showView() throws "Abnormal Workbench Condition" because perspective is null
                    if (getSite().getPage().getPerspective() != null)
                        getSite().getPage().showView("org.omnetpp.eventlogtable.editors.EventLogTableView");
                }
                catch (PartInitException e) {
                    AnimationCorePlugin.getDefault().logException(e);
                }
            }
        });

	    if (!(input instanceof IFileEditorInput))
	    	throw new PartInitException("Invalid input: this editor only works with workspace files");
	}

    @Override
    public void dispose() {
        if (resourceChangeListener != null)
            ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);
        if (selectionListener != null)
            getSite().getPage().removeSelectionListener(selectionListener);
        animationController.stopAnimation();
        super.dispose();
    }

	@Override
	public void createPartControl(Composite parent) {
	    GridLayout gridLayout = new GridLayout(1, false);
	    gridLayout.verticalSpacing = gridLayout.horizontalSpacing = 0;
	    gridLayout.marginWidth = gridLayout.marginHeight = 0;
        parent.setLayout(gridLayout);

        animationPrimitiveProvider = createAnimationPrimitiveProvider();
	    animationController = createAnimationController();
        animationTimeline = createAnimationTimeline(parent);
        animationTimeline.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        animationTimeline.setAnimationController(animationController);
        animationCanvas = createAnimationCanvas(parent);
        animationCanvas.setInput(eventLogInput);
        animationCanvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        animationCanvas.setWorkbenchPart(this);
        animationCanvas.setAnimationController(animationController);
        animationController.setAnimationCanvas(animationCanvas);
        animationPrimitiveProvider.setAnimationController(animationController);

        animationController.clearInternalState();
        animationController.gotoInitialAnimationPosition();

		getSite().setSelectionProvider(animationCanvas);
        addLocationProviderPaintListener(animationCanvas);

        // follow selection
        selectionListener = new ISelectionListener() {
            public void selectionChanged(IWorkbenchPart part, ISelection selection) {
                if (followSelection && part != AnimationEditor.this && selection instanceof IEventLogSelection) {
                    IEventLogSelection eventLogSelection = (IEventLogSelection)selection;

                    if (eventLogSelection.getEventLogInput() == animationCanvas.getInput()) {
                        animationCanvas.setSelection(selection);
                        markLocation();
                    }
                }
            }
        };
        getSite().getPage().addSelectionListener(selectionListener);
	}

    protected AnimationCanvas createAnimationCanvas(Composite parent) {
        return new AnimationCanvas(parent, SWT.DOUBLE_BUFFERED);
    }

    protected AnimationTimeline createAnimationTimeline(Composite parent) {
        return new AnimationTimeline(parent, SWT.NONE);
    }

    protected AnimationController createAnimationController() {
        return new AnimationController(createAnimationCoordinateSystem(), animationPrimitiveProvider);
    }

    protected abstract IAnimationCoordinateSystem createAnimationCoordinateSystem();

    protected abstract IAnimationPrimitiveProvider createAnimationPrimitiveProvider();

	@Override
	public void setFocus() {
        animationCanvas.setFocus();
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
	    throw new RuntimeException();
	}

	@Override
	public void doSaveAs() {
        throw new RuntimeException();
	}

    public class AnimationLocation implements INavigationLocation {
        private AnimationPosition animationPosition;

        public AnimationLocation(AnimationPosition animationPosition) {
            this.animationPosition = animationPosition;
        }

        public void dispose() {
            // void
        }

        public Object getInput() {
            return AnimationEditor.this.getEditorInput();
        }

        public String getText() {
            return AnimationEditor.this.getPartName() + ": " + animationPosition;
        }

        public boolean mergeInto(INavigationLocation currentLocation) {
            return equals(currentLocation);
        }

        public void releaseState() {
            // void
        }

        public void restoreLocation() {
            animationController.gotoAnimationPosition(animationPosition);
        }

        public void restoreState(IMemento memento) {
            String value = memento.getString("AnimationPosition");
            if (value != null)
                animationPosition = AnimationPosition.parse(value);
        }

        public void saveState(IMemento memento) {
            memento.putString("AnimationPosition", animationPosition.unparse());
        }

        public void setInput(Object input) {
            AnimationEditor.this.setInput((IFileEditorInput)input);
        }

        public void update() {
            // void
        }
    }

    @Override
    protected boolean canCreateNavigationLocation() {
        return !eventLogInput.isCanceled() && super.canCreateNavigationLocation();
    }

    public INavigationLocation createEmptyNavigationLocation() {
        return new AnimationLocation(new AnimationPosition());
    }

    public INavigationLocation createNavigationLocation() {
        if (!canCreateNavigationLocation())
            return null;
        else
            return new AnimationLocation(animationCanvas.getAnimationController().getCurrentAnimationPosition());
    }

    private class ResourceChangeListener implements IResourceChangeListener, IResourceDeltaVisitor {
        public void resourceChanged(IResourceChangeEvent event) {
            try {
                // close editor on project close
                if (event.getType() == IResourceChangeEvent.PRE_CLOSE) {
                    final IEditorPart thisEditor = AnimationEditor.this;
                    final IResource resource = event.getResource();
                    Display.getDefault().asyncExec(new Runnable(){
                        public void run(){
                            if (((FileEditorInput)thisEditor.getEditorInput()).getFile().getProject().equals(resource)) {
                                thisEditor.getSite().getPage().closeEditor(thisEditor, true);
                            }
                        }
                    });
                }
                IResourceDelta delta = event.getDelta();
                if (delta != null)
                    delta.accept(this);
            }
            catch (CoreException e) {
                throw new RuntimeException(e);
            }
        }

        public boolean visit(IResourceDelta delta) {
            if (delta != null && delta.getResource() != null && delta.getResource().equals(eventLogInput.getFile())) {
                Display.getDefault().asyncExec(new Runnable() {
                    public void run() {
                        if (!animationCanvas.isDisposed())
                            animationCanvas.redraw();
                    }
                });
            }
            return true;
        }
    }

    public ShowInContext getShowInContext() {
        return new ShowInContext(getEditorInput(), getSite().getSelectionProvider().getSelection());
    }

    public String[] getShowInTargetIds() {
        // contents of the "Show In..." context menu
        return new String[] {
                IConstants.SEQUENCECHART_VIEW_ID,
                IConstants.EVENTLOG_VIEW_ID
                };
    }
}
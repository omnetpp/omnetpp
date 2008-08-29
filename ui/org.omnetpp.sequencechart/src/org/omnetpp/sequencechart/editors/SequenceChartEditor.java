package org.omnetpp.sequencechart.editors;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
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
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.IShowInSource;
import org.eclipse.ui.part.IShowInTargetList;
import org.eclipse.ui.part.ShowInContext;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.eventlog.EventLogEditor;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.sequencechart.SequenceChartPlugin;
import org.omnetpp.sequencechart.widgets.SequenceChart;

/**
 * Sequence chart display tool. (It is not actually an editor; it is only named so
 * because it extends EditorPart).
 * 
 * @author levy
 */
public class SequenceChartEditor
    extends EventLogEditor 
    implements ISequenceChartProvider, INavigationLocationProvider, IGotoMarker, IShowInSource, IShowInTargetList
{
	private ResourceChangeListener resourceChangeListener = new ResourceChangeListener();

	private SequenceChart sequenceChart;

	private ISelectionListener selectionListener;
	
	public SequenceChart getSequenceChart() {
		return sequenceChart;
	}
	
	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		super.init(site, input);
		ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener);

		IContextService contextService = (IContextService)site.getService(IContextService.class);
        contextService.activateContext("org.omnetpp.context.SequenceChart");

		// try to open the log view
		try {
			// Eclipse feature: during startup, showView() throws "Abnormal Workbench Condition" because perspective is null
			if (site.getPage().getPerspective() != null)
				site.getPage().showView("org.omnetpp.eventlogtable.editors.EventLogTableView");
		}
		catch (PartInitException e) {
			SequenceChartPlugin.getDefault().logException(e);					
		}
	}

	@Override
	public void dispose() {
		if (resourceChangeListener != null)
			ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);
		
		if (selectionListener != null)
			getSite().getPage().removeSelectionListener(selectionListener);

		super.dispose();
	}

	@Override
	public void createPartControl(Composite parent) {
		sequenceChart = new SequenceChart(parent, SWT.DOUBLE_BUFFERED);
		sequenceChart.setFollowSelection(false);
		sequenceChart.setInput(eventLogInput);
		sequenceChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		sequenceChart.setSequenceChartContributor(SequenceChartContributor.getDefault());

		getSite().setSelectionProvider(sequenceChart);
		addLocationProviderPaintListener(sequenceChart);

		// follow selection
		selectionListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != SequenceChartEditor.this && selection instanceof IEventLogSelection) {
					IEventLogSelection eventLogSelection = (IEventLogSelection)selection;
					
					if (eventLogSelection.getEventLogInput() == sequenceChart.getInput()) {
						sequenceChart.setSelection(selection);
						markLocation();
					}
				}
			}
		};
		getSite().getPage().addSelectionListener(selectionListener);
	}

	@Override
	public void setFocus() {
		sequenceChart.setFocus();
	}
	
	public class SequenceChartLocation implements INavigationLocation {
		// TODO: ambiguous when restored
		private org.omnetpp.common.engine.BigDecimal startSimulationTime;

		private org.omnetpp.common.engine.BigDecimal endSimulationTime;
		
		public SequenceChartLocation(org.omnetpp.common.engine.BigDecimal startSimulationTime, org.omnetpp.common.engine.BigDecimal endSimulationTime) {
			this.startSimulationTime = startSimulationTime;
			this.endSimulationTime = endSimulationTime;
		}

		public void dispose() {
			// void
		}

		public Object getInput() {
			return SequenceChartEditor.this.getEditorInput();
		}

		public String getText() {
			return SequenceChartEditor.this.getPartName() + ": " + startSimulationTime + "s - " + endSimulationTime + "s";
		}

		public boolean mergeInto(INavigationLocation currentLocation) {
            return equals(currentLocation);
		}

		public void releaseState() {
			// void
		}

		public void restoreLocation() {
			sequenceChart.zoomToSimulationTimeRange(startSimulationTime, endSimulationTime);
		}

		public void restoreState(IMemento memento) {
		    String value = memento.getString("StartSimulationTime");
            if (value != null)
                startSimulationTime = org.omnetpp.common.engine.BigDecimal.parse(value);

            value = memento.getString("EndSimulationTime");
            if (value != null)
                endSimulationTime = org.omnetpp.common.engine.BigDecimal.parse(value);
		}

		public void saveState(IMemento memento) {
            memento.putString("StartSimulationTime", startSimulationTime.toString());
            memento.putString("EndSimulationTime", endSimulationTime.toString());
		}

		public void setInput(Object input) {
			SequenceChartEditor.this.setInput((IFileEditorInput)input);
		}

		public void update() {
			// void
		}

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + getOuterType().hashCode();
            result = prime * result + ((endSimulationTime == null) ? 0 : endSimulationTime.hashCode());
            result = prime * result + ((startSimulationTime == null) ? 0 : startSimulationTime.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            SequenceChartLocation other = (SequenceChartLocation) obj;
            if (!getOuterType().equals(other.getOuterType()))
                return false;
            if (endSimulationTime == null) {
                if (other.endSimulationTime != null)
                    return false;
            }
            else if (!endSimulationTime.equals(other.endSimulationTime))
                return false;
            if (startSimulationTime == null) {
                if (other.startSimulationTime != null)
                    return false;
            }
            else if (!startSimulationTime.equals(other.startSimulationTime))
                return false;
            return true;
        }

        private SequenceChartEditor getOuterType() {
            return SequenceChartEditor.this;
        }
	}

	public INavigationLocation createEmptyNavigationLocation() {
		return new SequenceChartLocation(org.omnetpp.common.engine.BigDecimal.getZero(), org.omnetpp.common.engine.BigDecimal.getNaN());
	}

	@Override
	protected boolean canCreateNavigationLocation() {
		return !eventLogInput.isCanceled() && super.canCreateNavigationLocation();
	}

	public INavigationLocation createNavigationLocation() {
		if (!canCreateNavigationLocation())
			return null;
		else
			return new SequenceChartLocation(sequenceChart.getViewportLeftSimulationTime(), sequenceChart.getViewportRightSimulationTime());
	}

    public void gotoMarker(IMarker marker)
    {
		int eventNumber = marker.getAttribute("EventNumber", -1);
		sequenceChart.gotoElement(eventLogInput.getEventLog().getEventForEventNumber(eventNumber));
    }

	private class ResourceChangeListener implements IResourceChangeListener, IResourceDeltaVisitor {
		public void resourceChanged(IResourceChangeEvent event) {
            try {
                // close editor on project close
                if (event.getType() == IResourceChangeEvent.PRE_CLOSE) {
                    final IEditorPart thisEditor = SequenceChartEditor.this;
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
                        if (!sequenceChart.isDisposed())
                            sequenceChart.clearCanvasCacheAndRedraw();
					}
            	});
            }

            return true;
        }	
	}

    /* (non-Javadoc)
     * Method declared on IShowInSource
     */
    public ShowInContext getShowInContext() {
        return new ShowInContext(getEditorInput(), getSite().getSelectionProvider().getSelection());
    }

    /* (non-Javadoc)
     * Method declared on IShowInTargetList
     */
    public String[] getShowInTargetIds() {
        // contents of the "Show In..." context menu
        return new String[] {
                IConstants.EVENTLOG_VIEW_ID, 
                //TODO IConstants.MODULEHIERARCHY_VIEW_ID,
                };
    }
}

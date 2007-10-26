package org.omnetpp.sequencechart.editors;

import org.eclipse.core.resources.IMarker;
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
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.ide.IGotoMarker;
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
public class SequenceChartEditor extends EventLogEditor implements INavigationLocationProvider, IGotoMarker {
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

		// try to open the log view
		try {
			// Eclipse feature: during startup, showView() throws "Abnormal Workbench Condition" because perspective is null
			if (getSite().getPage().getPerspective() != null)
				getSite().getPage().showView("org.omnetpp.eventlogtable.editors.EventLogTableView");
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
		private double startSimulationTime;

		private double endSimulationTime;
		
		public SequenceChartLocation(double startSimulationTime, double endSimulationTime) {
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
			return false;
		}

		public void releaseState() {
			// void
		}

		public void restoreLocation() {
			sequenceChart.zoomToSimulationTimeRange(startSimulationTime, endSimulationTime);
		}

		public void restoreState(IMemento memento) {
			// TODO: implement
		}

		public void saveState(IMemento memento) {
			// TODO: implement
		}

		public void setInput(Object input) {
			SequenceChartEditor.this.setInput((IFileEditorInput)input);
		}

		public void update() {
			// void
		}

		@Override
		public int hashCode() {
			final int PRIME = 31;
			int result = 1;
			long temp;
			temp = Double.doubleToLongBits(endSimulationTime);
			result = PRIME * result + (int) (temp ^ (temp >>> 32));
			temp = Double.doubleToLongBits(startSimulationTime);
			result = PRIME * result + (int) (temp ^ (temp >>> 32));
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
			final SequenceChartLocation other = (SequenceChartLocation) obj;
			if (Double.doubleToLongBits(endSimulationTime) != Double.doubleToLongBits(other.endSimulationTime))
				return false;
			if (Double.doubleToLongBits(startSimulationTime) != Double.doubleToLongBits(other.startSimulationTime))
				return false;
			return true;
		}
	}

	public INavigationLocation createEmptyNavigationLocation() {
		return new SequenceChartLocation(0, Double.NaN);
	}

	@Override
	protected boolean canCreateNavigationLocation() {
		return !eventLogInput.isCanceled() && super.canCreateNavigationLocation();
	}

	public INavigationLocation createNavigationLocation() {
		if (canCreateNavigationLocation())
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
}

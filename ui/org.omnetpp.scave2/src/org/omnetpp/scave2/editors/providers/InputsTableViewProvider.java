package org.omnetpp.scave2.editors.providers;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerFilter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.ui.FilterPanel;

public abstract class InputsTableViewProvider {

	protected ScaveEditor editor;
	
	public InputsTableViewProvider(ScaveEditor editor) {
		this.editor = editor;
	}
	
	public abstract IStructuredContentProvider getContentProvider();
	public abstract ITableLabelProvider getLabelProvider();
	
	public ViewerFilter getFilter()  { return new Filter(); }
	
	public void configureFilterPanel(final FilterPanel filterPanel) {
		final Filter filter = new Filter();
		
		filterPanel.getTableViewer().setContentProvider(getContentProvider());
		filterPanel.getTableViewer().setLabelProvider(getLabelProvider());
		filterPanel.getTableViewer().addFilter(filter);
		
		filterPanel.getFilterButton().addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				filter.setFileAndRunFilter(filterPanel.getRunNameCombo().getText());
				filter.setModuleFilter(filterPanel.getModuleNameCombo().getText());
				filter.setNameFilter(filterPanel.getNameCombo().getText());
				filterPanel.getTableViewer().refresh();
			}
		});
	}
	
	private static final Long[] EMPTY_ARRAY = new Long[0];
	
	abstract class ContentProvider implements IStructuredContentProvider, INotifyChangedListener {
		
		protected Inputs inputs;
		protected IDList idlist;
		protected Viewer viewer;
		
		public Object[] getElements(Object inputElement) {
			if (inputElement instanceof Inputs) {
				if (inputElement != inputs)
					rebuildIDList((Inputs)inputElement);
				if (idlist != null) {
					// The filter is not called for SWT.VIRTUAL tables (bug?),
					// so filtering the content here.
					TableViewer tableViewer = (TableViewer)viewer;
					if ((tableViewer.getTable().getStyle() & SWT.VIRTUAL) != 0) {
						ViewerFilter[] filters = ((TableViewer)viewer).getFilters();
						if (filters != null && filters.length > 0 && filters[0] instanceof Filter) {
							Filter filter = (Filter)filters[0];
							return filter.filter(viewer, inputs, idlist.toArray());
						}
					}
					
					return idlist.toArray();
				}
			}
			return EMPTY_ARRAY;
		}
		
		protected abstract void buildIDList();

		public void dispose() {
		}

		public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
			this.viewer = viewer;
			if (oldInput instanceof Inputs)
				removeListener((Inputs)oldInput);
			rebuildIDList(newInput);
			if (newInput instanceof Inputs) {
				addListener((Inputs)newInput);
			}
		}
		
		public void notifyChanged(Notification notification) {
			if (notification.isTouch())
				return;
			rebuildIDList(inputs);
			if (viewer != null && viewer.getControl() != null && !viewer.getControl().isDisposed())
				viewer.refresh();
		}
		
		public void addListener(Inputs inputs) {
			if (editor.getAdapterFactory() instanceof IChangeNotifier) {
				IChangeNotifier notifier = (IChangeNotifier)editor.getAdapterFactory();
				notifier.addListener(this);
			}
		}
		
		public void removeListener(Inputs inputs) {
			if (editor.getAdapterFactory() instanceof IChangeNotifier) {
				IChangeNotifier notifier = (IChangeNotifier)editor.getAdapterFactory();
				notifier.removeListener(this);
			}
		}
		
		private void rebuildIDList(Object input) {
			if (input instanceof Inputs) {
				inputs = (Inputs)input;
				idlist = null;
				buildIDList();
			}
		}
	}
	
	class LabelProvider implements ITableLabelProvider {

		public Image getColumnImage(Object element, int columnIndex) {
			return null;
		}

		public String getColumnText(Object element, int columnIndex) {
			return null;
		}

		public void addListener(ILabelProviderListener listener) {
		}

		public void dispose() {
		}

		public boolean isLabelProperty(Object element, String property) {
			return false;
		}

		public void removeListener(ILabelProviderListener listener) {
		}
	}
	
	public class Filter extends ViewerFilter {
		private String fileAndRunFilter = "";
		private String moduleFilter = "";
		private String nameFilter = "";
		
		public Filter() {
		}

		public void setFileAndRunFilter(String fileAndRunFilter) {
			this.fileAndRunFilter = fileAndRunFilter;
		}

		public void setModuleFilter(String moduleFilter) {
			this.moduleFilter = moduleFilter;
		}

		public void setNameFilter(String nameFilter) {
			this.nameFilter = nameFilter;
		}

		@Override
		public Object[] filter(Viewer viewer, Object parent, Object[] elements) {
			IDList idlist = new IDList();
			for (Object element : elements)
				if (element instanceof Long)
					idlist.add((Long)element);
			
			ResultFileManager manager = editor.getResultFileManager();
			idlist = manager.getFilteredList(idlist, fileAndRunFilter, moduleFilter, nameFilter);
			return idlist.toArray();
		}


		@Override
		public boolean select(Viewer viewer, Object parentElement, Object element) {
			// not called
			return false;
		}
	}
}

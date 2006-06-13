package org.omnetpp.scave2.editors.providers;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerFilter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.ui.FilterPanel;

/**
 * This class parametrizes the table views of the input data.
 * 
 * It contains base class for the content providers, which must
 * be derived by subclasses.
 * 
 * @author Tomi
 */
public abstract class InputsTableViewProvider {

	protected ScaveEditor editor;
	
	public InputsTableViewProvider(ScaveEditor editor) {
		this.editor = editor;
	}
	
	/* Methods to be implemented by concrete subclasses. */
	protected abstract ContentProvider getContentProvider();
	protected abstract ITableLabelProvider getLabelProvider();
	
	public void configureFilterPanel(final FilterPanel filterPanel) {
		final Filter filter = new Filter();
		final ContentProvider contentProvider = getContentProvider();
		
		filterPanel.getTableViewer().setContentProvider(contentProvider);
		filterPanel.getTableViewer().setLabelProvider(getLabelProvider());
		filterPanel.getTableViewer().addFilter(filter);
		
		// when the table content changes, update the filter hints in combos
		contentProvider.addContentChangeListener(new IContentChangeListener() {
			public void contentChanged(IDList idlist) {
				ResultFileManager manager = editor.getResultFileManager();
				filterPanel.getModuleNameCombo().setItems(manager.getModuleFilterHints(idlist).toArray());
				filterPanel.getRunNameCombo().setItems(manager.getRunNameFilterHints(idlist).toArray());
				filterPanel.getNameCombo().setItems(manager.getNameFilterHints(idlist).toArray());
			}
		});
		
		// when the filter button pressed, update the filter of the tableviewer
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
	
	/**
	 * Table content provider serves as a base class.
	 * Its viewer is refreshed when the model changes.
	 */
	abstract class ContentProvider implements IStructuredContentProvider, INotifyChangedListener {
		
		protected Object lastInputElement;
		protected IDList idlistCached;
		protected Viewer viewer;
		private ListenerList listeners = new ListenerList();
		
		public Object[] getElements(Object inputElement) {
			if (inputElement instanceof EObject) {

				if (inputElement != lastInputElement || idlistCached == null) {
					lastInputElement = inputElement;
					idlistCached = buildIDList(inputElement);
					fireContentChange();
				}

				if (idlistCached != null) {
					// The filter is not called for SWT.VIRTUAL tables (bug?),
					// so filtering the content here.
					TableViewer tableViewer = (TableViewer)viewer;
					if ((tableViewer.getTable().getStyle() & SWT.VIRTUAL) != 0) {
						ViewerFilter[] filters = ((TableViewer)viewer).getFilters();
						if (filters != null && filters.length > 0 && filters[0] instanceof Filter) {
							Filter filter = (Filter)filters[0];
							return filter.filter(viewer, lastInputElement, idlistCached.toArray());
						}
					}
					
					return idlistCached.toArray();
				}
			}
			return EMPTY_ARRAY;
		}
		
		protected abstract IDList buildIDList(Object inputElement);

		public void dispose() {
			viewer = null;
		}

		/* Notification from the viewer that its input has changed. */
		public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
			this.viewer = viewer;
			if (oldInput instanceof EObject &&
				editor.getAdapterFactory() instanceof IChangeNotifier) {
				IChangeNotifier notifier = (IChangeNotifier)editor.getAdapterFactory();
				notifier.removeListener(this);
			}
			if (newInput instanceof EObject &&
				editor.getAdapterFactory() instanceof IChangeNotifier) {
				IChangeNotifier notifier = (IChangeNotifier)editor.getAdapterFactory();
				notifier.addListener(this);
			}
		}
		
		/* Notification about EMF model changes. */
		public void notifyChanged(Notification notification) {
			if (notification.isTouch())
				return;
			idlistCached = null;
			if (viewer != null)
				viewer.refresh();
		}
		
		public void addContentChangeListener(IContentChangeListener listener) {
			listeners.add(listener);
		}
		
		public void removeContentChangeListener(IContentChangeListener listener) {
			listeners.remove(listener);
		}
		
		private void fireContentChange() {
			Object[] listenersCopy = new Object[listeners.getListeners().length];
			System.arraycopy(listeners.getListeners(), 0, listenersCopy, 0, listenersCopy.length);
			
			for (Object listener : listenersCopy)
				((IContentChangeListener)listener).contentChanged(idlistCached);
		}
	}
	
	/**
	 *  Notification interface about changes of the content of the table.
	 */
	interface IContentChangeListener {
		void contentChanged(IDList idlist);
	}
	
	/**
	 * Filter of the scalars/vectors table. 
	 */
	public class Filter extends ViewerFilter {
		private String fileAndRunFilter = "";
		private String moduleFilter = "";
		private String nameFilter = "";
		
		public Filter() {
		}

		public void setFileAndRunFilter(String fileAndRunFilter) {
			this.fileAndRunFilter = fileAndRunFilter == null ? "" : fileAndRunFilter;
		}

		public void setModuleFilter(String moduleFilter) {
			this.moduleFilter = moduleFilter == null ? "" : moduleFilter;
		}

		public void setNameFilter(String nameFilter) {
			this.nameFilter = nameFilter == null ? "" : nameFilter;
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

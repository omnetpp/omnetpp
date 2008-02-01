/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.editors;


import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.EventObject;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.emf.common.command.BasicCommandStack;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.command.CommandStackListener;
import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.ui.MarkerHelper;
import org.eclipse.emf.common.ui.editor.ProblemEditorPart;
import org.eclipse.emf.common.util.BasicDiagnostic;
import org.eclipse.emf.common.util.Diagnostic;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EValidator;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.util.EContentAdapter;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.emf.edit.domain.IEditingDomainProvider;
import org.eclipse.emf.edit.provider.AdapterFactoryItemDelegator;
import org.eclipse.emf.edit.provider.ComposedAdapterFactory;
import org.eclipse.emf.edit.provider.ReflectiveItemProviderAdapterFactory;
import org.eclipse.emf.edit.provider.resource.ResourceItemProviderAdapterFactory;
import org.eclipse.emf.edit.ui.action.EditingDomainActionBarContributor;
//import org.eclipse.emf.edit.ui.celleditor.AdapterFactoryTreeEditor;
import org.eclipse.emf.edit.ui.dnd.EditingDomainViewerDropAdapter;
import org.eclipse.emf.edit.ui.dnd.LocalTransfer;
import org.eclipse.emf.edit.ui.dnd.ViewerDragAdapter;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryContentProvider;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.emf.edit.ui.util.EditUIMarkerHelper;
import org.eclipse.emf.edit.ui.view.ExtendedPropertySheetPage;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.viewers.DecoratingLabelProvider;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.StructuredViewer;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.actions.WorkspaceModifyOperation;
import org.eclipse.ui.dialogs.SaveAsDialog;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.views.contentoutline.ContentOutline;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.eclipse.ui.views.properties.PropertySheet;
import org.eclipse.ui.views.properties.PropertySheetPage;
import org.omnetpp.common.ui.MultiPageEditorPartExt;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.editors.treeproviders.InputsViewLabelProvider;
import org.omnetpp.scave.editors.treeproviders.ScaveModelLabelDecorator;
import org.omnetpp.scave.editors.treeproviders.ScaveModelLabelProvider;
import org.omnetpp.scave.model.provider.ScaveEditPlugin;
import org.omnetpp.scave.model2.provider.ScaveModelItemProviderAdapterFactory;


/**
 * This is an abstract base class for EMF-based editors. Compare
 * with EMF-generated multi-page editors.
 */
//FIXME if Datasets is empty, context menu offers "New Input File" if Inputs is selected in the content outline
public abstract class AbstractEMFModelEditor extends MultiPageEditorPartExt
	implements IEditingDomainProvider, ISelectionProvider, IMenuListener, IGotoMarker {

	/**
	 * This keeps track of the editing domain that is used to track all changes to the model.
	 */
	protected AdapterFactoryEditingDomain editingDomain;

	/**
	 * This is the one adapter factory used for providing views of the model.
	 */
	protected ComposedAdapterFactory adapterFactory;

	/**
	 * This is the content outline page.
	 */
	protected IContentOutlinePage contentOutlinePage;

	/**
	 * This is a kludge...
	 */
	protected IStatusLineManager contentOutlineStatusLineManager;

	/**
	 * This is the content outline page's viewer.
	 */
	protected TreeViewer contentOutlineViewer;

	/**
	 * This is the property sheet page.
	 */
	protected PropertySheetPage propertySheetPage;

	/**
	 * The selection change listener added to all viewers 
	 */
	ISelectionChangedListener selectionChangedListener = new ISelectionChangedListener() {
		public void selectionChanged(SelectionChangedEvent selectionChangedEvent) {
			handleSelectionChange(selectionChangedEvent.getSelection());
		}
	};
	
	/**
	 * This keeps track of all the {@link org.eclipse.jface.viewers.ISelectionChangedListener}s that are listening to this editor.
	 * We need this because we implement ISelectionProvider which includes having to manage a listener list.
	 */
	protected Collection<ISelectionChangedListener> selectionChangedListeners = new ArrayList<ISelectionChangedListener>();

	/**
	 * This keeps track of the selection of the editor as a whole.
	 */
	protected ISelection editorSelection = StructuredSelection.EMPTY;

	/**
	 * The MarkerHelper is responsible for creating workspace resource markers presented
	 * in Eclipse's Problems View.
	 */
	protected MarkerHelper markerHelper = new EditUIMarkerHelper();

	/**
	 * This listens for when the outline becomes active
	 */
	protected IPartListener partListener =
		new IPartListener() {
			public void partActivated(IWorkbenchPart p) {
				if (p instanceof ContentOutline) {
					if (((ContentOutline)p).getCurrentPage() == contentOutlinePage) {
						getActionBarContributor().setActiveEditor(AbstractEMFModelEditor.this);
					}
				}
				else if (p instanceof PropertySheet) {
					if (((PropertySheet)p).getCurrentPage() == propertySheetPage) {
						getActionBarContributor().setActiveEditor(AbstractEMFModelEditor.this);
						handleActivate();
					}
				}
				else if (p == AbstractEMFModelEditor.this) {
					handleActivate();
				}
			}
			public void partBroughtToTop(IWorkbenchPart p) {
			}
			public void partClosed(IWorkbenchPart p) {
			}
			public void partDeactivated(IWorkbenchPart p) {
			}
			public void partOpened(IWorkbenchPart p) {
			}
		};

	/**
	 * Resources that have been removed since last activation.
	 */
	protected Collection<Resource> removedResources = new ArrayList<Resource>();

	/**
	 * Resources that have been changed since last activation.
	 */
	protected Collection<Resource> changedResources = new ArrayList<Resource>();

	/**
	 * Resources that have been saved.
	 */
	protected Collection<Resource> savedResources = new ArrayList<Resource>();

	/**
	 * Map to store the diagnostic associated with a resource.
	 */
	protected Map<Resource, Diagnostic> resourceToDiagnosticMap = new LinkedHashMap<Resource, Diagnostic>();

	/**
	 * Controls whether the problem indication should be updated.
	 */
	protected boolean updateProblemIndication = true;

	/**
	 * Adapter used to update the problem indication when resources are demanded loaded.
	 */
	protected EContentAdapter problemIndicationAdapter = 
		new EContentAdapter() {
			public void notifyChanged(Notification notification) {
				if (notification.getNotifier() instanceof Resource) {
					switch (notification.getFeatureID(Resource.class)) {
						case Resource.RESOURCE__IS_LOADED:
						case Resource.RESOURCE__ERRORS:
						case Resource.RESOURCE__WARNINGS: {
							Resource resource = (Resource)notification.getNotifier();
							Diagnostic diagnostic = analyzeResourceProblems((Resource)notification.getNotifier(), null);
							if (diagnostic.getSeverity() != Diagnostic.OK) {
								resourceToDiagnosticMap.put(resource, diagnostic);
							}
							else {
								resourceToDiagnosticMap.remove(resource);
							}

							if (updateProblemIndication) {
								getSite().getShell().getDisplay().asyncExec
									(new Runnable() {
										 public void run() {
											 updateProblemIndication();
										 }
									 });
							}
						}
					}
				}
				else {
					super.notifyChanged(notification);
				}
			}

			protected void setTarget(Resource target) {
				basicSetTarget(target);
			}

			protected void unsetTarget(Resource target) {
				basicUnsetTarget(target);
			}
		};

	/**
	 * This listens for workspace changes.
	 */
	protected IResourceChangeListener resourceChangeListener =
		new IResourceChangeListener() {
			public void resourceChanged(IResourceChangeEvent event) {
				// Only listening to these.
				// if (event.getType() == IResourceDelta.POST_CHANGE)
				{
					IResourceDelta delta = event.getDelta();
					try {
						class ResourceDeltaVisitor implements IResourceDeltaVisitor {
							protected ResourceSet resourceSet = editingDomain.getResourceSet();
							protected Collection<Resource> changedResources = new ArrayList<Resource>();
							protected Collection<Resource> removedResources = new ArrayList<Resource>();

							public boolean visit(IResourceDelta delta) {
								if (delta.getFlags() != IResourceDelta.MARKERS &&
								    delta.getResource().getType() == IResource.FILE) {
									if ((delta.getKind() & (IResourceDelta.CHANGED | IResourceDelta.REMOVED)) != 0) {
										Resource resource = resourceSet.getResource(URI.createURI(delta.getFullPath().toString()), false);
										if (resource != null) {
											if ((delta.getKind() & IResourceDelta.REMOVED) != 0) {
												removedResources.add(resource);
											}
											else if (!savedResources.remove(resource)) {
												changedResources.add(resource);
											}
										}
									}
								}

								return true;
							}

							public Collection<Resource> getChangedResources() {
								return changedResources;
							}

							public Collection<Resource> getRemovedResources() {
								return removedResources;
							}
						}

						ResourceDeltaVisitor visitor = new ResourceDeltaVisitor();
						delta.accept(visitor);

						if (!visitor.getRemovedResources().isEmpty()) {
							removedResources.addAll(visitor.getRemovedResources());
							if (!isDirty()) {
								getSite().getShell().getDisplay().asyncExec
									(new Runnable() {
										 public void run() {
											 getSite().getPage().closeEditor(AbstractEMFModelEditor.this, false);
											 AbstractEMFModelEditor.this.dispose();
										 }
									 });
							}
						}

						if (!visitor.getChangedResources().isEmpty()) {
							changedResources.addAll(visitor.getChangedResources());
							if (getSite().getPage().getActiveEditor() == AbstractEMFModelEditor.this) {
								getSite().getShell().getDisplay().asyncExec
									(new Runnable() {
										 public void run() {
											 handleActivate();
										 }
									 });
							}
						}
					}
					catch (CoreException exception) {
						ScaveEditPlugin.INSTANCE.log(exception);
					}
				}
			}
		};

	/**
	 * Handles activation of the editor or it's associated views.
	 */
	protected void handleActivate() {
		// Recompute the read only state.
		//
		if (editingDomain.getResourceToReadOnlyMap() != null) {
		  editingDomain.getResourceToReadOnlyMap().clear();

		  // Refresh any actions that may become enabled or disabled.
		  //
		  setSelection(getSelection());
		}

		if (!removedResources.isEmpty()) {
			if (handleDirtyConflict()) {
				getSite().getPage().closeEditor(AbstractEMFModelEditor.this, false);
				AbstractEMFModelEditor.this.dispose();
			}
			else {
				removedResources.clear();
				changedResources.clear();
				savedResources.clear();
			}
		}
		else if (!changedResources.isEmpty()) {
			changedResources.removeAll(savedResources);
			handleChangedResources();
			changedResources.clear();
			savedResources.clear();
		}
	}

	/**
	 * Handles what to do with changed resources on activation.
	 */
	protected void handleChangedResources() {
		if (!changedResources.isEmpty() && (!isDirty() || handleDirtyConflict())) {
			editingDomain.getCommandStack().flush();

			updateProblemIndication = false;
			for (Iterator<Resource> i = changedResources.iterator(); i.hasNext(); ) {
				Resource resource = i.next();
				if (resource.isLoaded()) {
					resource.unload();
					try {
						resource.load(Collections.EMPTY_MAP);
					}
					catch (IOException exception) {
						if (!resourceToDiagnosticMap.containsKey(resource)) {
							resourceToDiagnosticMap.put(resource, analyzeResourceProblems(resource, exception));
						}
					}
				}
			}
			updateProblemIndication = true;
			updateProblemIndication();
		}
	}
  
	/**
	 * Updates the problems indication with the information described in the specified diagnostic.
	 */
	protected void updateProblemIndication() {
		if (updateProblemIndication) {
			BasicDiagnostic diagnostic =
				new BasicDiagnostic
					(Diagnostic.OK,
					 "org.omnetpp.scave.model",
					 0,
					 null,
					 new Object [] { editingDomain.getResourceSet() });
			for (Iterator<Diagnostic> i = resourceToDiagnosticMap.values().iterator(); i.hasNext(); ) {
				Diagnostic childDiagnostic = i.next();
				if (childDiagnostic.getSeverity() != Diagnostic.OK) {
					diagnostic.add(childDiagnostic);
				}
			}

			int lastEditorPage = getPageCount() - 1;
			if (lastEditorPage >= 0 && getEditor(lastEditorPage) instanceof ProblemEditorPart) {
				((ProblemEditorPart)getEditor(lastEditorPage)).setDiagnostic(diagnostic);
				if (diagnostic.getSeverity() != Diagnostic.OK) {
					setActivePage(lastEditorPage);
				}
			}
			else if (diagnostic.getSeverity() != Diagnostic.OK) {
				ProblemEditorPart problemEditorPart = new ProblemEditorPart();
				problemEditorPart.setDiagnostic(diagnostic);
				problemEditorPart.setMarkerHelper(markerHelper);
				try {
					addPage(getPageCount(), problemEditorPart, getEditorInput());
					lastEditorPage++;
					setPageText(lastEditorPage, problemEditorPart.getPartName());
					setActivePage(lastEditorPage);
				}
				catch (PartInitException exception) {
					ScaveEditPlugin.INSTANCE.log(exception);
				}
			}

			if (markerHelper.hasMarkers(editingDomain.getResourceSet())) {
				markerHelper.deleteMarkers(editingDomain.getResourceSet());
				if (diagnostic.getSeverity() != Diagnostic.OK) {
					try {
						markerHelper.createMarkers(diagnostic);
					}
					catch (CoreException exception) {
						ScaveEditPlugin.INSTANCE.log(exception);
					}
				}
			}
		}
	}

	/**
	 * Shows a dialog that asks if conflicting changes should be discarded.
	 */
	protected boolean handleDirtyConflict() {
		return
			MessageDialog.openQuestion
				(getSite().getShell(),
				 getString("_UI_FileConflict_label"),
				 getString("_WARN_FileConflict"));
	}

	/**
	 * This creates a model editor.
	 */
	public AbstractEMFModelEditor() {
		super();

		// Create an adapter factory that yields item providers.
		//
		List<AdapterFactory> factories = new ArrayList<AdapterFactory>();
		factories.add(new ResourceItemProviderAdapterFactory());
		factories.add(new ScaveModelItemProviderAdapterFactory());
		factories.add(new ReflectiveItemProviderAdapterFactory());

		adapterFactory = new ComposedAdapterFactory(factories);

		// Create the command stack that will notify this editor as commands are executed.
		//
		BasicCommandStack commandStack = new BasicCommandStack();

		// Add a listener to set the most recent command's affected objects to be the selection of the viewer with focus.
		//
		commandStack.addCommandStackListener
			(new CommandStackListener() {
				 public void commandStackChanged(final EventObject event) {
					 getContainer().getDisplay().asyncExec
						 (new Runnable() {
							  public void run() {
								  firePropertyChange(IEditorPart.PROP_DIRTY);

								  // Try to select the affected objects.
								  //
								  Command mostRecentCommand = ((CommandStack)event.getSource()).getMostRecentCommand();
								  if (mostRecentCommand != null) {
									  setSelectionToViewer(mostRecentCommand.getAffectedObjects());
								  }
								  if (propertySheetPage != null && !propertySheetPage.getControl().isDisposed()) {
									  propertySheetPage.refresh();
								  }
							  }
						  });
				 }
			 });

		// Create the editing domain with a special command stack.
		//
		editingDomain = new AdapterFactoryEditingDomain(adapterFactory, commandStack, new HashMap<Resource,Boolean>());
	}

	/**
	 * This is here for the listener to be able to call it.
	 */
	protected void firePropertyChange(int action) {
		super.firePropertyChange(action);
	}

	/**
	 * This sets the selection into whichever viewer is active.
	 */
	public void setSelectionToViewer(Collection<?> collection) {
		handleSelectionChange(new StructuredSelection(collection.toArray()));
	}

	/**
	 * Utility function to update selection in a viewer without generating 
	 * further notifications.
	 */
	public void setViewerSelectionNoNotify(Viewer target, ISelection selection) {
		if (target!=null) {
			target.removeSelectionChangedListener(selectionChangedListener);
			target.setSelection(selection,true);
			target.addSelectionChangedListener(selectionChangedListener);
		}
	}

	boolean selectionChangeInProgress = false; // to prevent recursive notifications
	
	/**
	 * Propagates the selection everywhere. Override if you have more widgets to update!
	 */
	protected void handleSelectionChange(ISelection selection) {
		//FIXME merge this method into fireSelectionChangedEvent()!!! that's where the guard should be as well!
		boolean selectionReallyChanged = selection!=editorSelection && !selection.equals(editorSelection);
		if (!selectionChangeInProgress || selectionReallyChanged) {
			try {
				selectionChangeInProgress = true; // "finally" ensures this gets reset in any case
				editorSelection = selection;
				 // FIXME notifying the view about IDListSelections would remove the selection from the editor!
				if (!(selection instanceof IDListSelection)) {
					//setViewerSelectionNoNotify(contentOutlineViewer, selection);
					updateStatusLineManager(contentOutlineStatusLineManager, selection);
				}
				updateStatusLineManager(getActionBars().getStatusLineManager(), selection);
				fireSelectionChangedEvent(selection);
			} finally {
				selectionChangeInProgress = false;
			}
		}
	}

	/**
	 * Notify listeners on {@link org.eclipse.jface.viewers.ISelectionProvider} about a selection change.
	 */
	protected void fireSelectionChangedEvent(ISelection selection) {
		for (ISelectionChangedListener listener : selectionChangedListeners)
			listener.selectionChanged(new SelectionChangedEvent(this, selection));
	}
	
	/**
	 * This returns the editing domain as required by the {@link IEditingDomainProvider} interface.
	 * This is important for implementing the static methods of {@link AdapterFactoryEditingDomain}
	 * and for supporting {@link org.eclipse.emf.edit.ui.action.CommandAction}.
	 */
	public EditingDomain getEditingDomain() {
		return editingDomain;
	}


	/**
	 * This creates a context menu for the viewer and adds a listener as well registering the menu for extension.
	 */
	protected void createContextMenuFor(StructuredViewer viewer) {
		MenuManager contextMenu = new MenuManager("#PopUp");
		contextMenu.add(new Separator("additions"));
		contextMenu.setRemoveAllWhenShown(true);
		contextMenu.addMenuListener(this);
		Menu menu = contextMenu.createContextMenu(viewer.getControl());
		viewer.getControl().setMenu(menu);

		// Note: don't register the context menu, otherwise "Run As", "Debug As", "Team", and other irrelevant menu items appear...
		//getSite().registerContextMenu(contextMenu, viewer);
	}

	/**
	 * Sets up the viewer so that it functions as drag source and drop target for model elements
	 */
	protected void setupDragAndDropSupportFor(StructuredViewer viewer) {
		int dndOperations = DND.DROP_COPY | DND.DROP_MOVE | DND.DROP_LINK;
		Transfer[] transfers = new Transfer[] { LocalTransfer.getInstance() };
		viewer.addDragSupport(dndOperations, transfers, new ViewerDragAdapter(viewer));
		viewer.addDropSupport(dndOperations, transfers, new EditingDomainViewerDropAdapter(editingDomain, viewer));
	}

	/**
	 * This is the method called to load a resource into the editing 
	 * domain's resource set based on the editor's input.
	 */
	public void createModel() {
		// Assumes that the input is a file object.
		//
		IFileEditorInput modelFile = (IFileEditorInput)getEditorInput();
		URI resourceURI = URI.createPlatformResourceURI(modelFile.getFile().getFullPath().toString(), true);;
		Exception exception = null;
		Resource resource = null;
		try {
			// Load the resource through the editing domain.
			//
			resource = editingDomain.getResourceSet().getResource(resourceURI, true);
		}
		catch (Exception e) {
			exception = e;
			resource = editingDomain.getResourceSet().getResource(resourceURI, false);
		}

		Diagnostic diagnostic = analyzeResourceProblems(resource, exception);
		if (diagnostic.getSeverity() != Diagnostic.OK) {
			resourceToDiagnosticMap.put(resource,  diagnostic); //FIXME throw exception?
		}
		editingDomain.getResourceSet().eAdapters().add(problemIndicationAdapter);
	}

	/**
	 * Returns a dignostic describing the errors and warnings listed in the resource
	 * and the specified exception (if any).
	 */
	public Diagnostic analyzeResourceProblems(Resource resource, Exception exception) {
		if (!resource.getErrors().isEmpty() || !resource.getWarnings().isEmpty()) {
			BasicDiagnostic basicDiagnostic =
				new BasicDiagnostic
					(Diagnostic.ERROR,
					 "org.omnetpp.scave.model",
					 0,
					 getString("_UI_CreateModelError_message", resource.getURI()),
					 new Object [] { exception == null ? (Object)resource : exception });
			basicDiagnostic.merge(EcoreUtil.computeDiagnostic(resource, true));
			return basicDiagnostic;
		}
		else if (exception != null) {
			return
				new BasicDiagnostic
					(Diagnostic.ERROR,
					 "org.omnetpp.scave.model",
					 0,
					 getString("_UI_CreateModelError_message", resource.getURI()),
					 new Object[] { exception });
		}
		else {
			return Diagnostic.OK_INSTANCE;
		}
	}


	/**
	 * This is the method used by the framework to install your own controls.
	 */
	public void createPages() {
		// Creates the model from the editor input
		createModel();
		// do the real job
        doCreatePages();
        // refresh 
		updateProblemIndication();
	}

	/**
	 * createPages() delegates real work here. 
	 * @author Andras
	 */
	protected abstract void doCreatePages();

	/**
	 * Utility class to add content and label providers, context menu etc to a TreeViewer
	 * that is used to edit the model.
	 */
	public void configureTreeViewer(TreeViewer modelViewer) {
		ILabelProvider labelProvider =
			new DecoratingLabelProvider(
				new ScaveModelLabelProvider(new AdapterFactoryLabelProvider(adapterFactory)),
				new ScaveModelLabelDecorator());
		modelViewer.setContentProvider(new AdapterFactoryContentProvider(adapterFactory));
		modelViewer.setLabelProvider(labelProvider);
		modelViewer.setAutoExpandLevel(TreeViewer.ALL_LEVELS);
		// new AdapterFactoryTreeEditor(modelViewer.getTree(), adapterFactory); //XXX this appears to be something about in-place editing - do we need it?

		modelViewer.addSelectionChangedListener(selectionChangedListener);

		createContextMenuFor(modelViewer);
		setupDragAndDropSupportFor(modelViewer);
		
		// on double-click, open (the dataset or chart), or bring up the Properties dialog
		modelViewer.getTree().addSelectionListener(new SelectionAdapter() {
			public void widgetDefaultSelected(SelectionEvent e) {
				ScaveEditorContributor contributor = ScaveEditorContributor.getDefault();
				if (contributor != null) {
					if (contributor.getOpenAction().isEnabled())
						contributor.getOpenAction().run();
					else if (contributor.getEditAction().isEnabled())
						contributor.getEditAction().run();
				}
			}
		});
		
	}	

	/**
	 * This is how the framework determines which interfaces we implement.
	 */
	@SuppressWarnings("unchecked")
	public Object getAdapter(Class key) {
		if (key.equals(IContentOutlinePage.class)) {
			return showOutlineView() ? getContentOutlinePage() : null;
		}
		else if (key.equals(IPropertySheetPage.class)) {
			return getPropertySheetPage();
		}
		else if (key.equals(IGotoMarker.class)) {
			return this;
		}
		else {
			return super.getAdapter(key);
		}
	}
	
	// The content outline is just a tree.
	//
	class MyContentOutlinePage extends ContentOutlinePage {
		public void createControl(Composite parent) {
			super.createControl(parent);
			contentOutlineViewer = getTreeViewer();
			contentOutlineViewer.addSelectionChangedListener(this);

			// Set up the tree viewer.
			//
			contentOutlineViewer.setContentProvider(new AdapterFactoryContentProvider(adapterFactory));
			//contentOutlineViewer.setLabelProvider(new AdapterFactoryLabelProvider(adapterFactory));
			contentOutlineViewer.setLabelProvider(
					new ScaveModelLabelProvider(new AdapterFactoryLabelProvider(adapterFactory)));
			initializeContentOutlineViewer(contentOutlineViewer); // should call setInput()
			contentOutlineViewer.expandToLevel(3);
			
			// Make sure our popups work.
			//
			createContextMenuFor(contentOutlineViewer);

			if (!editingDomain.getResourceSet().getResources().isEmpty()) {
			  // Select the root object in the view.
			  //
			  ArrayList<Object> selection = new ArrayList<Object>();
			  selection.add(editingDomain.getResourceSet().getResources().get(0));
			  contentOutlineViewer.setSelection(new StructuredSelection(selection), true);
			}
		}

		public void makeContributions(IMenuManager menuManager, IToolBarManager toolBarManager, IStatusLineManager statusLineManager) {
			super.makeContributions(menuManager, toolBarManager, statusLineManager);
			contentOutlineStatusLineManager = statusLineManager;
		}

		public void setActionBars(IActionBars actionBars) {
			super.setActionBars(actionBars);
			getActionBarContributor().shareGlobalActions(this, actionBars);
		}
	}

	/**
	 * This accesses a cached version of the content outliner.
	 */
	public IContentOutlinePage getContentOutlinePage() {
		if (contentOutlinePage == null) {
			contentOutlinePage = new MyContentOutlinePage();
			// Listen to selection so that we can handle it is a special way.
			//
			contentOutlinePage.addSelectionChangedListener(selectionChangedListener);
		}

		return contentOutlinePage;
	}

	/**
	 * Should call contentOutlineViewer.setInput().
	 */
	protected abstract void initializeContentOutlineViewer(Viewer contentOutlineViewer);

	/**
	 * This accesses a cached version of the property sheet.
	 */
	public IPropertySheetPage getPropertySheetPage() {
		if (propertySheetPage == null) {
			propertySheetPage =
				new ExtendedPropertySheetPage(editingDomain) {
					public void setSelectionToViewer(List<?> selection) {
						AbstractEMFModelEditor.this.setSelectionToViewer(selection);
						AbstractEMFModelEditor.this.setFocus();
					}

					public void setActionBars(IActionBars actionBars) {
						super.setActionBars(actionBars);
						getActionBarContributor().shareGlobalActions(this, actionBars);
					}
				};
		}

		return propertySheetPage;
	}

	/**
	 * This is for implementing {@link IEditorPart} and simply tests the command stack.
	 */
	public boolean isDirty() {
		return ((BasicCommandStack)editingDomain.getCommandStack()).isSaveNeeded();
	}

	/**
	 * This is for implementing {@link IEditorPart} and simply saves the model file.
	 */
	public void doSave(IProgressMonitor progressMonitor) {
		// Do the work within an operation because this is a long running activity that modifies the workbench.
		//
		WorkspaceModifyOperation operation =
			new WorkspaceModifyOperation() {
				// This is the method that gets invoked when the operation runs.
				//
				public void execute(IProgressMonitor monitor) {
					// Save the resources to the file system.
					//
					boolean first = true;
					for (Iterator<Resource> i = editingDomain.getResourceSet().getResources().iterator(); i.hasNext(); ) {
						Resource resource = i.next();
						if (!isSaveable(resource))
							continue;
						if ((first || !resource.getContents().isEmpty() || isPersisted(resource)) && !editingDomain.isReadOnly(resource)) {
							try {
								savedResources.add(resource);
								resource.save(Collections.EMPTY_MAP);
							}
							catch (Exception exception) {
								resourceToDiagnosticMap.put(resource, analyzeResourceProblems(resource, exception));
							}
							first = false;
						}
					}
				}
			};

		updateProblemIndication = false;
		try {
			// This runs the options, and shows progress.
			//
			new ProgressMonitorDialog(getSite().getShell()).run(true, false, operation);

			// Refresh the necessary state.
			//
			((BasicCommandStack)editingDomain.getCommandStack()).saveIsDone();
			firePropertyChange(IEditorPart.PROP_DIRTY);
		}
		catch (Exception exception) {
			// Something went wrong that shouldn't.
			//
			ScaveEditPlugin.INSTANCE.log(exception);
		}
		updateProblemIndication = true;
		updateProblemIndication();
	}
	
	protected boolean isSaveable(Resource resource) {
		return true;
	}

	/**
	 * This returns wether something has been persisted to the URI of the specified resource.
	 * The implementation uses the URI converter from the editor's resource set to try to open an input stream. 
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected boolean isPersisted(Resource resource) {
		boolean result = false;
		try {
			InputStream stream = editingDomain.getResourceSet().getURIConverter().createInputStream(resource.getURI());
			if (stream != null) {
				result = true;
				stream.close();
			}
		}
		catch (IOException e) {
		}
		return result;
	}

	/**
	 * This always returns true because it is not currently supported.
	 */
	public boolean isSaveAsAllowed() {
		return true;
	}

	/**
	 * "Save As" also changes the editor's input.
	 */
	public void doSaveAs() {
		SaveAsDialog saveAsDialog= new SaveAsDialog(getSite().getShell());
		saveAsDialog.open();
		IPath path= saveAsDialog.getResult();
		if (path != null) {
			IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(path);
			if (file != null) {
				doSaveAs(URI.createPlatformResourceURI(file.getFullPath().toString(), true), new FileEditorInput(file));
			}
		}
	}

	/**
	 * Perform "Save As"
	 */
	protected void doSaveAs(URI uri, IEditorInput editorInput) {
		((Resource)editingDomain.getResourceSet().getResources().get(0)).setURI(uri);
		setInputWithNotify(editorInput);
		setPartName(editorInput.getName());
		IProgressMonitor progressMonitor =
			getActionBars().getStatusLineManager() != null ?
				getActionBars().getStatusLineManager().getProgressMonitor() :
				new NullProgressMonitor();
		doSave(progressMonitor);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void gotoMarker(IMarker marker) {
		try {
			if (marker.getType().equals(EValidator.MARKER)) {
				String uriAttribute = marker.getAttribute(EValidator.URI_ATTRIBUTE, null);
				if (uriAttribute != null) {
					URI uri = URI.createURI(uriAttribute);
					EObject eObject = editingDomain.getResourceSet().getEObject(uri, true);
					if (eObject != null) {
					  setSelectionToViewer(Collections.singleton(editingDomain.getWrapper(eObject)));
					}
				}
			}
		}
		catch (CoreException exception) {
			ScaveEditPlugin.INSTANCE.log(exception);
		}
	}

	/**
	 * This is called during startup.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void init(IEditorSite site, IEditorInput editorInput)
		throws PartInitException {
		setSite(site);
		setInputWithNotify(editorInput);
		setPartName(editorInput.getName());
		site.setSelectionProvider(this);
		site.getPage().addPartListener(partListener);
		ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener, IResourceChangeEvent.POST_CHANGE);
	}

	/**
	 * This implements {@link org.eclipse.jface.viewers.ISelectionProvider}.
	 */
	public void addSelectionChangedListener(ISelectionChangedListener listener) {
		selectionChangedListeners.add(listener);
	}

	/**
	 * This implements {@link org.eclipse.jface.viewers.ISelectionProvider}.
	 */
	public void removeSelectionChangedListener(ISelectionChangedListener listener) {
		selectionChangedListeners.remove(listener);
	}

	/**
	 * This implements {@link org.eclipse.jface.viewers.ISelectionProvider} to return this editor's overall selection.
	 */
	public ISelection getSelection() {
		return editorSelection;
	}

	/**
	 * This implements {@link org.eclipse.jface.viewers.ISelectionProvider} to set this editor's overall selection.
	 * Calling this will result in notifing the listeners.
	 */
	public void setSelection(ISelection selection) {
		handleSelectionChange(selection);
	}

	/**
	 * Utility method to update "Selected X objects" text on the status bar.
	 */
	protected void updateStatusLineManager(IStatusLineManager statusLineManager, ISelection selection) {
		if (statusLineManager != null) {
			if (selection instanceof IDListSelection) {
				IDListSelection idlistSelection = (IDListSelection)selection;
				int scalars = idlistSelection.getScalarsCount();
				int vectors = idlistSelection.getVectorsCount();
				int histograms = idlistSelection.getHistogramsCount();
				if (scalars + vectors + histograms == 0)
					statusLineManager.setMessage(getString("_UI_NoObjectSelected"));
				else {
					List<String> strings = new ArrayList<String>(3);
					if (scalars > 0) strings.add(StringUtils.formatCounted(scalars, "scalar"));
					if (vectors > 0) strings.add(StringUtils.formatCounted(vectors, "vector"));
					if (histograms > 0) strings.add(StringUtils.formatCounted(histograms, "histogram"));
					String message = "Selected " + StringUtils.join(strings, ", ", " and ");
					statusLineManager.setMessage(message);
				}
			}
			else if (selection instanceof IStructuredSelection) {
				Collection<?> collection = ((IStructuredSelection)selection).toList();
				if (collection.size()==0) {
						statusLineManager.setMessage(getString("_UI_NoObjectSelected"));
				}
				else if (collection.size()==1) {
					Object object = collection.iterator().next();
					 // XXX unify label providers
					String text = new InputsViewLabelProvider().getText(object);
					if (text == null)
						text = new AdapterFactoryItemDelegator(adapterFactory).getText(object);
					statusLineManager.setMessage(getString("_UI_SingleObjectSelected", text));
				}
				else {
						statusLineManager.setMessage(getString("_UI_MultiObjectSelected", Integer.toString(collection.size())));
				}
			}
			else {
				statusLineManager.setMessage("");
			}
		}
	}

	/**
	 * This looks up a string in the plugin's plugin.properties file.
	 */
	private static String getString(String key) {
		return ScaveEditPlugin.INSTANCE.getString(key);
	}

	/**
	 * This looks up a string in plugin.properties, making a substitution.
	 */
	private static String getString(String key, Object s1) {
		return ScaveEditPlugin.INSTANCE.getString(key, new Object [] { s1 });
	}

	/**
	 * This implements {@link org.eclipse.jface.action.IMenuListener} to help fill the context menus with contributions from the Edit menu.
	 */
	public void menuAboutToShow(IMenuManager menuManager) {
		((IMenuListener)getEditorSite().getActionBarContributor()).menuAboutToShow(menuManager);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EditingDomainActionBarContributor getActionBarContributor() {
		if (getEditorSite() != null)
			return (EditingDomainActionBarContributor)getEditorSite().getActionBarContributor();
		else
			return null;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public IActionBars getActionBars() {
		return getActionBarContributor().getActionBars();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public AdapterFactory getAdapterFactory() {
		return adapterFactory;
	}

	/**
	 * Dispose widget.
	 */
	public void dispose() {
		ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);

		if (getSite() != null && getSite().getPage() != null)
			getSite().getPage().removePartListener(partListener);

		adapterFactory.dispose();

		if (getActionBarContributor() != null && getActionBarContributor().getActiveEditor() == this) {
			getActionBarContributor().setActiveEditor(null);
		}

		if (propertySheetPage != null) {
			propertySheetPage.dispose();
		}

		if (contentOutlinePage != null) {
			contentOutlinePage.dispose();
		}

		super.dispose();
	}

	/**
	 * Returns whether the outline view should be presented to the user (YES).
	 */
	protected boolean showOutlineView() {
		return true;
	}
}

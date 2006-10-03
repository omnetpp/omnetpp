package org.omnetpp.scave2.editors.ui;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave2.actions.EditAction;
import org.omnetpp.scave2.actions.IScaveAction;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Common functionality of Scave multi-page editor pages.
 * @author andras
 */
public class ScaveEditorPage extends ScrolledForm {

	protected ScaveEditor scaveEditor = null;  // backreference to the containing editor
	private String pageTitle = "untitled";
	
	public ScaveEditorPage(Composite parent, int style, ScaveEditor scaveEditor) {
		super(parent, style);
		this.scaveEditor = scaveEditor;
	}

	public void setFormTitle(String title) {
		setFont(new Font(null, "Arial", 12, SWT.BOLD));
		setForeground(new Color(null, 0, 128, 255));
		setText(title);
	}
	
	/**
	 * Return the text that should appear on the page's tab.
	 */
	public String getPageTitle() {
		return pageTitle;
	}
	
	/**
	 * Sets the text that should appear on the page's tab.
	 */
	public void setPageTitle(String title) {
		pageTitle = title;
		scaveEditor.setPageTitle(this, title);
	}
	
	/**
	 * Updates the page title, etc.
	 * Called when the model changed.
	 */
	public void updatePage(Notification notification) {
	}
	
	/**
	 * Sets up the given control so that when a file is drag-dropped into it,
	 * it will be added to Inputs unless it's already in there.
	 */
	public void setupResultFileDropTarget(Control dropTargetControl) {
		// set up DropTarget, and add FileTransfer to it;
		// issue: the EMF editor already adds a drop target to the TreeViewer
		DropTarget dropTarget = null;
		Object o = dropTargetControl.getData("DropTarget");
		if (o instanceof DropTarget) {
			dropTarget = (DropTarget) o; // this is a dirty hack, but there seems to be no public API for getting an existing drop target
			// add FileTransfer to the transfer array if not already in there
			Transfer[] transfer = dropTarget.getTransfer();
			Transfer[] transfer2 = new Transfer[transfer.length+1];
			System.arraycopy(transfer, 0, transfer2, 0, transfer.length);
			transfer2[transfer2.length-1] = FileTransfer.getInstance();
			dropTarget.setTransfer(transfer2);
		}
		else {
			dropTarget = new DropTarget(dropTargetControl, DND.DROP_DEFAULT | DND.DROP_COPY);
			dropTarget.setTransfer(new Transfer[] {FileTransfer.getInstance()});
		}

		// add our listener to handle file transfer to the DropTarget
		dropTarget.addDropListener(new DropTargetAdapter() {
			public void dragEnter(DropTargetEvent event) {
				if (event.detail == DND.DROP_DEFAULT)
					event.detail = DND.DROP_COPY;
			}
			public void dragOperationChanged(DropTargetEvent event) {
				if (event.detail == DND.DROP_DEFAULT)
					event.detail = DND.DROP_COPY;
			}
			public void drop(DropTargetEvent event) {
				if (event.data instanceof String[]) {
					String [] fileNames = (String[])event.data;
					for (int i=0; i<fileNames.length; i++)
						addDroppedFileToInputs(fileNames[i]);
				}
			}
		});
	}

	/**
	 * Adds the given file to Inputs unless it's already in there.
	 * The file name should be an OS path (D:\... or /home/you/...),
	 * not a workspace path! 
	 * 
	 * XXX Limitation: currently the file must be available via the 
	 * workspace as well. This may get improved in the future. 
	 */
	private void addDroppedFileToInputs(String fileName) {
		// convert OS path to workspace path
		IFile iFile = ScaveEditor.findFileInWorkspace(fileName);
		if (iFile==null) {
			System.out.println("path not in workspace: "+fileName); //XXX error dialog?
			return;
		}
		scaveEditor.addWorkspaceFileToInputs(iFile);
	}

	/**
	 * Connects the button with an action, so that the action is executed 
	 * when the button is pressed, and the button is enabled/disabled when 
	 * the action becomes enabled/disabled.
	 * 
	 * Note: ActionContributionItem is not good here because:
	 *  (1) it wants to create the button itself, and thus not compatible with FormToolkit
	 *  (2) the button it creates has wrong background color, and there's no way to access the button to fix it
	 *  (3) it will make the button listen to global selection changes, and cannot be tied to a viewer 
	 */
	private static void doConfigureButton(final Button button, final IScaveAction action) {
		button.setText(action.getText());
		button.setToolTipText(action.getToolTipText());
		if (action.getImageDescriptor() != null)
			button.setImage(action.getImageDescriptor().createImage());
		
		button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				action.run();
			}
		});
		final IPropertyChangeListener propertyChangeListener = new IPropertyChangeListener() {
			public void propertyChange(PropertyChangeEvent event) {
				if (event.getProperty().equals(IAction.ENABLED)) {
					if (!button.isDisposed())
						button.setEnabled(action.isEnabled());
				}
			}
		};
		action.addPropertyChangeListener(propertyChangeListener);
		button.addDisposeListener(new DisposeListener() {
			public void widgetDisposed(DisposeEvent e) {
				action.removePropertyChangeListener(propertyChangeListener);
			}
		});
	}

	/* 
	 * Connects the button with an action, so that the action is executed 
	 * when the button is pressed, and the button is enabled/disabled when 
	 * the action becomes enabled/disabled. 
	 * 
	 * The action will be enabled/disabled based on the selection service's
	 * selection.
	 */
	public static void configureGlobalButton(IWorkbenchWindow workbenchWindow, final Button button, final IScaveAction action) {
		doConfigureButton(button, action);
		workbenchWindow.getSelectionService().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				action.selectionChanged(selection);
			}
		});
	}
	
	/* 
	 * Connects the button with an action, so that the action is executed 
	 * when the button is pressed, and the button is enabled/disabled when 
	 * the action becomes enabled/disabled. 
	 * 
	 * The action will be enabled/disabled based on a viewer's selection.
	 */
	public static void configureViewerButton(final Button button, final Viewer viewer, final IScaveAction action) {
		doConfigureButton(button, action);
		action.setViewer(viewer);
		viewer.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				action.selectionChanged(event.getSelection());
			}
		});
	}
	
	/**
	 * Like <code>configureViewerButton</code>, but action will also run
	 * on double-clicking in the viewer.
	 */
	public static void configureViewerDefaultButton(final Button button, final TreeViewer viewer, final IScaveAction action) {
		configureViewerButton(button, viewer, action);
		configureViewerDefaultAction(viewer, action);
	}
	
	/**
	 * Connects the double-click in the viewer with the action.
	 */
	public static void configureViewerDefaultAction(final TreeViewer viewer, final IScaveAction action) {
		action.setViewer(viewer);
		viewer.getTree().addSelectionListener(new SelectionAdapter() {
			public void widgetDefaultSelected(SelectionEvent e) {
				action.run();
			}
		});
	}
	
	/**
	 * Adds event adapter to the chart view so that clicking on the view
	 * will select the chart object in the model.
	 * Adds the "Edit..." item to the context menu of the view.
	 */
	public void configureChartView(final Control view, final Chart chart) {
		// mouse click on the view selects the chart object in the model
		view.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				scaveEditor.setSelection(new StructuredSelection(chart));
			}
		});
		
		// add Edit action to the context menu of the view
		if (view.getMenu() == null)
			view.setMenu(new Menu(view));
		final EditAction editAction = new EditAction();
		MenuItem item = new MenuItem(view.getMenu(), SWT.PUSH);
		item.setText(editAction.getText());
		item.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				scaveEditor.setSelection(new StructuredSelection(chart));
				editAction.run();
			}
		});
	}
}

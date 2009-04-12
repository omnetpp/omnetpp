/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.runtimeenv.views;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.common.util.ActionExt;
import org.omnetpp.runtimeenv.Activator;

/**
 * Abstract base class for views that normally show information that
 * belongs to the active workbenchpart's selection, but can be pinned to 
 * a given editor or view. Subclasses are expected to implement the doBuildContent() 
 * method, which will be invoked whenever the selection changes.
 *
 * @author Andras
 */
public abstract class PinnableView2 extends ViewWithMessagePart {
	private boolean isPinned = false;
	private IWorkbenchPart associatedPart = null; // where the selection we display comes from
	private IAction pinAction;
	
	private ISelectionListener selectionChangedListener;
	private IPartListener partListener;

	public PinnableView2() {
	}

	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);
		hookListeners();
		rebuildContent();
	}

	@Override
	public void dispose() {
		unhookListeners();
		super.dispose();
	}

	protected void hookListeners() {
		// Listen on selection changes
		selectionChangedListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != PinnableView2.this) // ignore our own selection changes
					// Debug.println("*** AbstractModule selection changed from: "+part);
					workbenchSelectionChanged();
			}
		};
		getSite().getPage().addPostSelectionListener(selectionChangedListener);
        getSite().getPage().addSelectionListener(selectionChangedListener);

		// Listens to workbench changes, and invokes activePartChanged() whenever the
		// active editor changes. Listening on workbench changes is needed because
		// editors don't always send selection changes when they get opened or closed.
		//
		// NOTE: the view only gets workbench events while it is visible. So we also
		// need to update our contents on getting activated.
		//
		associatedPart = getActivePart()==this ? null : getActivePart();
		partListener = new IPartListener() {
			public void partActivated(IWorkbenchPart part) {
				if (part == PinnableView2.this) {
					viewActivated();
				}
				else if (!isPinned) {
					associatedPart = part;
					activePartChanged();
				}
			}

			public void partBroughtToTop(IWorkbenchPart part) {
			}

			public void partClosed(IWorkbenchPart part) {
				if (part == associatedPart) {
					associatedPart = null;
					if (isPinned)
						unpin(); // unpin on closing of the editor we're pinned to
					activePartChanged();
				}
			}

			public void partDeactivated(IWorkbenchPart part) {
			}

			public void partOpened(IWorkbenchPart part) {
			}
		};
		getSite().getPage().addPartListener(partListener);
	}

	protected void unhookListeners() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
        if (selectionChangedListener != null)
            getSite().getPage().removeSelectionListener(selectionChangedListener);
		if (partListener != null)
			getSite().getPage().removePartListener(partListener);
	}

	public void workbenchSelectionChanged() {
		rebuildContent();
	}

	protected void viewActivated() {
		//Debug.println("*** VIEW ACTIVATED");
		rebuildContent();
	}

	protected void activePartChanged() {
		//Debug.println("*** ACTIVE EDITOR CHANGED");
		rebuildContent();
	}

	protected IAction getOrCreatePinAction() {
		if (pinAction == null) {
			pinAction = new ActionExt("Pin", IAction.AS_CHECK_BOX, Activator.getImageDescriptor("icons/elcl16/pin.gif")) {
				@Override
				public void run() {
					if (isChecked())
						pin();
					else
						unpin();
				}
			};
		} 
		return pinAction;
	}	

	/**
	 * Returns the associated editor or view (the selection of which we 
	 * display), or null if there is none
	 */
	protected IWorkbenchPart getAssociatedPart() {
		return associatedPart;
	}

	/**
	 * Returns the selection of the associated part (the selection of which 
	 * we display), or null if there is none
	 */
	protected ISelection getAssociatedPartSelection() {
		if (associatedPart == null)
			return null;
		ISelectionProvider selectionProvider = associatedPart.getSite().getSelectionProvider();
		return selectionProvider == null ? null : selectionProvider.getSelection();
	}
	
	/**
	 * Pin the view to the current editor's current selection.
	 */
	public void pin() {
		isPinned = true;
		pinAction.setChecked(true);
        rebuildContent();  // to update label: "Pinned to: ..."
	}

	/**
	 * Unpin the view: let it follow the active editor selection.
	 */
	public void unpin() {
		isPinned = false;
		pinAction.setChecked(false);
        rebuildContent();  // to update label: "Pinned to: ..."
	}

	/**
	 * Rebuild the content according to the selection.
	 */
	abstract protected void rebuildContent();
}

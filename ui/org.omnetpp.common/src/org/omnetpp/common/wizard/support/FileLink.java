package org.omnetpp.common.wizard.support;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Link;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.wizard.IWidgetAdapter;

/**
 * A control for displaying the name of a resource as a link. When clicked, it shows 
 * the resource (opens Project Explorer and focuses it to the resource).
 * 
 * @author Andras
 */
public class FileLink extends Composite implements IWidgetAdapter {
	private Link link;
	private IResource resource;

	public FileLink(Composite parent, int style) {
		super(parent, style);
		setLayout(new FillLayout());
		
		link = new Link(this, SWT.NONE);
		link.setText("n/a");
		
		link.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				showResource();
			}
		});
	}

	protected void showResource() {
	    // show resource in Project Explorer (set its selection)
        IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
        if (workbenchPage != null) {  // note: may be null during platform startup...
            try {
                IViewPart view = workbenchPage.showView(IConstants.PROJECT_EXPLORER_ID);
                view.getSite().getSelectionProvider().setSelection(new StructuredSelection(resource));
            }
            catch (Exception e) {
                CommonPlugin.logError("FileLink: could not show resource in Project Explorer", e);
            }
        }
	}
	
	public Link getLinkControl() {
		return link;
	}

	public IResource getResource() {
        return resource;
    }
	
	public void setResource(IResource resource) {
        this.resource = resource;
        link.setText(resource==null ? "n/a" : "<a>" + resource.getFullPath().toString() + "</a>");
    }
	
	/**
	 * Adapter interface.
	 */
	public Object getValue() {
		IResource resource = getResource();
        return resource==null ? "" : resource.getFullPath().toString();
	}

	/**
	 * Adapter interface.
	 */
	public void setValue(Object value) {
	    IResource resource;
	    if (value instanceof IResource)
	        resource = (IResource)value;
	    else {
	        Path path = new Path(value.toString());
            IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
            resource = root.findMember(path);
            if (resource == null)
                resource = (path.segmentCount()==1) ? root.getProject(path.segment(0)) : root.getFile(path);
	    }

		setResource(resource);
	}

}

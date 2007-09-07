package org.omnetpp.ide.properties;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * This property page is shown for OMNeT++ Projects (projects with the
 * omnetpp nature), and lets the user edit the contents of the ".nedfolders" 
 * file.
 *
 * @author Andras
 */
public class NEDSourceFoldersPropertyPage extends PropertyPage {
	private static final String NEDFOLDERS_FILENAME = ".nedfolders";

	private CheckboxTreeViewer treeViewer;

	/**
	 * Constructor.
	 */
	public NEDSourceFoldersPropertyPage() {
		super();
	}

	/**
	 * @see PreferencePage#createContents(Composite)
	 */
	protected Control createContents(Composite parent) {
		Composite composite = new Composite(parent, SWT.NONE);
		composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		composite.setLayout(new GridLayout(2, false));

		final IProject project = (IProject) getElement();

		Label label = new Label(composite, SWT.NONE);
		label.setText("NED source folders in project " + project.getName() + ":");
		label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1));
		
		treeViewer = new CheckboxTreeViewer(composite, SWT.BORDER);
		treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
		((GridData)treeViewer.getTree().getLayoutData()).heightHint = 300;
		
		treeViewer.setLabelProvider(new WorkbenchLabelProvider());
		
		treeViewer.setContentProvider(new WorkbenchContentProvider() {
	        @Override
	        public Object[] getChildren(Object element) {
	            List<Object> filteredChildren = new ArrayList<Object>();
	            for (Object child : super.getChildren(element)) {
	                if (child==project || child instanceof IFolder)
	                    filteredChildren.add(child);
	            }
	            return filteredChildren.toArray();
	        }
	    });
	
		treeViewer.addCheckStateListener(new ICheckStateListener() {
			public void checkStateChanged(CheckStateChangedEvent event) {
				if (event.getChecked()) {
			        // when a folder is checked, uncheck its subtree and all parents up to the root  
					IResource e = (IResource)event.getElement();
					treeViewer.setSubtreeChecked(e, false);
					treeViewer.setChecked(e, true);
				    for (IResource current=e.getParent(); current!=null; current=current.getParent())
				    	treeViewer.setChecked(current, false);
				}
			}
		});

		treeViewer.setInput(project.getParent());

		loadNedPathFile();
		
		return composite;
	}

	protected void performDefaults() {
	}
	
	public boolean performOk() {
		saveNedPathFile();
		return true;
	}

	private void loadNedPathFile() {
		try {
			IProject project = (IProject) getElement();
			IFile nedpathFile = project.getFile(NEDFOLDERS_FILENAME);
			String contents = "";
			if (nedpathFile.exists())
				contents = FileUtils.readTextFile(nedpathFile.getContents());

			for (String line : StringUtils.splitToLines(contents)) {
				if (!StringUtils.isBlank(line)) {
					IFolder folder = project.getFolder(line.trim());
					treeViewer.setChecked(folder, true);
				    for (IResource current=folder.getParent(); current!=null; current=current.getParent())
				    	treeViewer.setExpandedState(current, true);
				}
			}
			if (treeViewer.getCheckedElements().length == 0)
				treeViewer.setChecked(project, true);
		} 
		catch (IOException e) {
			errorDialog("Cannot read NED Source Paths: ", e);
		} catch (CoreException e) {
			errorDialog("Cannot read NED Source Paths: ", e);
		}
		
	}

	private void saveNedPathFile() {
		try {
			// assemble file content to save
			String content = "";
			for (Object element : treeViewer.getCheckedElements())
				content += getProjectRelativePathOf((IContainer)element) + "\n";
			
			// save it
			IProject project = (IProject) getElement();
			IFile nedpathFile = project.getFile(NEDFOLDERS_FILENAME);
			if (!nedpathFile.exists())
				nedpathFile.create(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
			else
				nedpathFile.setContents(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
		} 
		catch (CoreException e) {
			errorDialog("Cannot store NED Source Folder list: ", e);
		}
	} 

	private void errorDialog(String message, Throwable e) {
		IStatus status = new Status(IMarker.SEVERITY_ERROR, "org.omnetpp.main", e.getMessage(), e);
		ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", message, status);
	}

	private String getProjectRelativePathOf(IContainer container) {
		Assert.isTrue(container != null);
		IProject project = (IProject) getElement();
		if (container == project)
			return "";
		else
			return StringUtils.removeStart(container.getFullPath().toString(), project.getFullPath().toString()+"/") + "/";
	}
}
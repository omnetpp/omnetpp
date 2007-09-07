package org.omnetpp.ide.properties;

import java.io.ByteArrayInputStream;
import java.io.IOException;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.List;
import org.eclipse.ui.dialogs.ContainerSelectionDialog;
import org.eclipse.ui.dialogs.ISelectionValidator;
import org.eclipse.ui.dialogs.PropertyPage;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * This property page is shown for OMNeT++ Projects (projects with the
 * omnetpp nature), and lets the user edit the contents of the ".nedpath" 
 * file.
 *
 * @author Andras
 */
public class NEDSourceFoldersPropertyPage extends PropertyPage {
	private static final String NEDPATH_FILENAME = ".nedpath";

	private List list;
	private Button add;
	private Button remove;

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
		
		list = new List(composite, SWT.BORDER);
		list.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 1, 2));
		((GridData)list.getLayoutData()).heightHint = 150;

		add = new Button(composite, SWT.NONE);
		add.setText("Add");
		add.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		add.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				ContainerSelectionDialog dialog = new ContainerSelectionDialog(
						Display.getCurrent().getActiveShell(), 
						project, false, "Select a folder as NED Source Folder:");
				dialog.setValidator(new ISelectionValidator() {
					public String isValid(Object selection) {
						IContainer container = getContainer((IPath)selection);
						return validate(container);
					}
				});
				if (dialog.open() == Window.OK) {
					Object result = dialog.getResult()[0];
					IContainer container = getContainer((IPath)result);
					String name = getProjectRelativePathOf(container);
					list.add(name);
				}
			}
		});

		remove = new Button(composite, SWT.NONE);
		remove.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		remove.setText("Remove");
		remove.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				list.remove(list.getSelectionIndices());
			}
		});

		loadNedPathFile();
		
		return composite;
	}

	protected void performDefaults() {
		list.removeAll();
	}
	
	public boolean performOk() {
		saveNedPathFile();
		return true;
	}

	private void loadNedPathFile() {
		String contents = "";
		try {
			IProject project = (IProject) getElement();
			IFile nedpathFile = project.getFile(NEDPATH_FILENAME);
			if (nedpathFile.exists())
				contents = FileUtils.readTextFile(nedpathFile.getContents());
		} 
		catch (IOException e) {
			errorDialog("Cannot read NED Source Paths: ", e);
		} catch (CoreException e) {
			errorDialog("Cannot read NED Source Paths: ", e);
		}
		
		for (String line : StringUtils.splitToLines(contents))
			if (!StringUtils.isBlank(line))
				list.add(line.trim());
	}

	private void saveNedPathFile() {
		try {
			IProject project = (IProject) getElement();
			IFile nedpathFile = project.getFile(NEDPATH_FILENAME);
			String contents = StringUtils.join(list.getItems(), "\n") + "\n";
			if (!nedpathFile.exists())
				nedpathFile.create(new ByteArrayInputStream(contents.getBytes()), IFile.FORCE, null);
			else
				nedpathFile.setContents(new ByteArrayInputStream(contents.getBytes()), IFile.FORCE, null);
		} 
		catch (CoreException e) {
			errorDialog("Cannot store NED Source Paths: ", e);
		}
	} 

	private void errorDialog(String message, Throwable e) {
		IStatus status = new Status(IMarker.SEVERITY_ERROR, "", e.getMessage(), e); //XXX pluginID
		ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", message, status);
	}

	private String validate(IContainer container) {
		// validate if it's good for us
		IProject project = (IProject) getElement();
		if (container.getProject() != project)
			return "Selection must be within project \"" + project.getName() + "\"";
		
		// overlap check.
		// NOTE: this relies on trailing and leading "/"'s being exactly as they are now!
		String containerName = getProjectRelativePathOf(container);
		for (String listItem : list.getItems())
			if (containerName.startsWith(listItem) || (listItem).startsWith(containerName))
				return "Selection overlaps with folder \"" + listItem + "\""; 

		return null;
	}

	private String getProjectRelativePathOf(IContainer container) {
		// IMPORTANT: if you change this, overlap checks (in validate()) MUST be tested again!
		Assert.isTrue(container != null);
		IProject project = (IProject) getElement();
		if (container == project)
			return ".";
		else
			return StringUtils.removeStart(container.getFullPath().toString(), project.getFullPath().toString()+"/") + "/";
	}

	private IContainer getContainer(IPath path) {
		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		if (path.segmentCount() == 0)
			return root;
		else if (path.segmentCount() == 1)
			return root.getProject(path.segment(0));
		else
			return root.getFolder(path);
	}

}
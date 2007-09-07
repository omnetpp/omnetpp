package org.omnetpp.ned.core.properties;

import java.io.ByteArrayInputStream;
import java.io.IOException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
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
import org.eclipse.ui.dialogs.PropertyPage;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

public class NEDSourceFoldersPropertyPage extends PropertyPage {

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

		Label label = new Label(composite, SWT.NONE);
		label.setText("NED source folders in project " + getElement().toString() + ":");
		label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1));
		
		list = new List(composite, SWT.BORDER);
		list.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 2));

		add = new Button(composite, SWT.NONE);
		add.setText("Add");
		add.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		add.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				IProject project = (IProject) getElement();
				ContainerSelectionDialog dialog = new ContainerSelectionDialog(
						Display.getCurrent().getActiveShell(), 
						project, false, "Select a folder as NED Source Folder:");
				if (dialog.open() == Window.OK) {
					list.add(dialog.getResult()[0].toString());
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
			IFile nedpathFile = project.getFile(".nedpath");
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
			IFile nedpathFile = project.getFile(".nedpath");
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

}
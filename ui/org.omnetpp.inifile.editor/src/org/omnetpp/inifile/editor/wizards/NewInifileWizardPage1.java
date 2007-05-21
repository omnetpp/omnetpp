package org.omnetpp.inifile.editor.wizards;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.Arrays;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.eclipse.ui.ide.IDE;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.ned.core.NEDResourcesPlugin;

/**
 * The "New" wizard page allows setting the container for the new file as well
 * as the file name. The page will only accept file name without the extension
 * OR with the extension that matches the expected one (ini).
 */
public class NewInifileWizardPage1 extends WizardNewFileCreationPage {
	private IWorkbench workbench;
	private Combo networkCombo;

	public NewInifileWizardPage1(IWorkbench aWorkbench, IStructuredSelection selection) {
		super("page1", selection);
		setTitle("Create an ini file");
		setDescription("This wizard allows you to create a new OMNeT++/OMNEST simulation configuration file.");
		setImageDescriptor(InifileEditorPlugin.getImageDescriptor("icons/full/wizban/newinifile.png"));
		workbench = aWorkbench;

		setFileExtension("ini");
		setFileName("omnetpp.ini");  // append "1", "2" etc if not unique in that folder
		//XXX set initial folder to either selection, or folder of input of current editor
		//setContainerFullPath(path)
	}

	@Override
	public void createControl(Composite parent) {
		super.createControl(parent);

		Composite composite = (Composite) getControl();

		// sample section generation group
		Group group = new Group(composite, SWT.NONE);
		group.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL | GridData.HORIZONTAL_ALIGN_FILL));
		group.setLayout(new GridLayout(2, false));
		group.setText("Content");

		// network name field
		createLabel(group, "NED Network:", parent.getFont());
		networkCombo = new Combo(group, SWT.BORDER);
		networkCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

		//XXX here!

		// fill network combo
		Set<String> networkNameSet = NEDResourcesPlugin.getNEDResources().getNetworkNames();
		String[] networkNames = networkNameSet.toArray(new String[]{});
		Arrays.sort(networkNames);
		networkCombo.setItems(networkNames);
		networkCombo.setVisibleItemCount(Math.min(20, networkCombo.getItemCount()));
		//XXX set combo to a NED network in the current directory

		new Label(composite, SWT.NONE);

		setPageComplete(validatePage());
	}

	protected static Label createLabel(Composite parent, String text, Font font) {
		Label label = new Label(parent, SWT.WRAP);
		label.setText(text);
		label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
		label.setFont(font);
		return label;
	}

	@Override
	protected InputStream getInitialContents() {
		String networkName = networkCombo.getText().trim();

		String contents = 
			"[General]\n" +
			"network = "+networkName+"\n";

		return new ByteArrayInputStream(contents.getBytes());
	}

	public boolean finish() {
		IFile newFile = createNewFile();
		if (newFile == null)
			return false; // creation was unsuccessful

		// Since the file resource was created fine, open it for editing
		try {
			IWorkbenchWindow dwindow = workbench.getActiveWorkbenchWindow();
			IWorkbenchPage page = dwindow.getActivePage();
			if (page != null)
				IDE.openEditor(page, newFile, true);
		} catch (PartInitException e) {
			InifileEditorPlugin.logError(e);
			return false;
		}
		return true;
	}

}

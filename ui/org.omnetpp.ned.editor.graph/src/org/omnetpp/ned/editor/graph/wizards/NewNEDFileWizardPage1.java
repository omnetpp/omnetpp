package org.omnetpp.ned.editor.graph.wizards;

import java.io.ByteArrayInputStream;
import java.io.InputStream;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.eclipse.ui.ide.IDE;

public class NewNEDFileWizardPage1 extends WizardNewFileCreationPage {

	private IWorkbench workbench;

	private static int exampleCount = 1;

	//FIXME FIXME FIXME factor out UI controls to another page!!!!! --Andras
	private Button model1 = null;
	private Button model2 = null;
	private int modelSelected = 1;

	public NewNEDFileWizardPage1(IWorkbench aWorkbench,
			IStructuredSelection selection) {
		super("page1", selection); //$NON-NLS-1$
		this.setTitle("Create a NED file");
		this.setDescription("This wizard allows you to create a new network description");
		this.setImageDescriptor(ImageDescriptor.createFromFile(getClass(),
				"icons/logicbanner.gif")); //$NON-NLS-1$
		this.workbench = aWorkbench;
	}

	@Override
    public void createControl(Composite parent) {
		super.createControl(parent);
		this.setFileName("new" + exampleCount + ".ned"); //$NON-NLS-2$//$NON-NLS-1$

		Composite composite = (Composite) getControl();

		// sample section generation group
		Group group = new Group(composite, SWT.NONE);
		group.setLayout(new GridLayout());
		group.setText("Content");
		group.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL
				| GridData.HORIZONTAL_ALIGN_FILL));

		SelectionListener listener = new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (e.getSource() == model1) {
					modelSelected = 1;
					setFileName("untitled" + exampleCount + ".ned"); //$NON-NLS-2$//$NON-NLS-1$
				} else {
					modelSelected = 2;
					setFileName("new" + exampleCount + ".ned"); //$NON-NLS-2$//$NON-NLS-1$
				}
			}
		};

		// sample section generation checkboxes
		model1 = new Button(group, SWT.RADIO);
		model1.setText("Empty file");
		model1.addSelectionListener(listener);
		model1.setSelection(true);

		model2 = new Button(group, SWT.RADIO);
		model2.setText("Example");
		model2.addSelectionListener(listener);

		new Label(composite, SWT.NONE);

		setPageComplete(validatePage());
	}

	@Override
    protected InputStream getInitialContents() {
		String contents = "//\n// NED file\n//\n\n"; //FIXME
		return new ByteArrayInputStream(contents.getBytes());
	}

	public boolean finish() {
		IFile newFile = createNewFile();
		if (newFile == null)
			return false; // ie.- creation was unsuccessful

		// Since the file resource was created fine, open it for editing
		// iff requested by the user
		try {
			IWorkbenchWindow dwindow = workbench.getActiveWorkbenchWindow();
			IWorkbenchPage page = dwindow.getActivePage();
			if (page != null)
				IDE.openEditor(page, newFile, true);
		} catch (org.eclipse.ui.PartInitException e) {
			e.printStackTrace();
			return false;
		}
		exampleCount++;
		return true;
	}

	/**
	 * Empty method
	 */
	public void widgetDefaultSelected(SelectionEvent e) {
	}
}

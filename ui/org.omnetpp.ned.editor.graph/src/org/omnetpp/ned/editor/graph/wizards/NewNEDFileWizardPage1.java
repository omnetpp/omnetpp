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

    private static String[] NEDFILE_TEMPLATES = {
        "//\n// NED file\n//\n\n",
        "//\n// TODO Place comment here\n//\n\nsimple #NAME# {\n  parameters:\n  gates:\n}\n",
        "//\n// TODO Place comment here\n//\n\nmodule #NAME# {\n  parameters:\n  gates:\n  submodules:\n connections:\n}\n",
        "//\n// TODO Place comment here\n//\n\nnetwork #NAME# {\n  parameters:\n  submodules:\n connections:\n}\n"
    };
    
	private IWorkbench workbench;
	private static int exampleCount = 1;

	private Button emptyButton = null;
    private Button simpleButton = null;
    private Button compoundButton = null;
    private Button networkButton = null;
	private int modelSelected = 0;

	public NewNEDFileWizardPage1(IWorkbench aWorkbench,
			IStructuredSelection selection) {
		super("page1", selection); //$NON-NLS-1$
		this.setTitle("Create a NED file");
		this.setDescription("This wizard allows you to create a new network description file");
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
				if (e.getSource() == emptyButton) {
					modelSelected = 0;
				} else if (e.getSource() == simpleButton) {
					modelSelected = 1;
				} else if (e.getSource() == compoundButton) {
                    modelSelected = 2;
                } else if (e.getSource() == networkButton) {
                    modelSelected = 3;
                }
			}
		};

		// sample section generation checkboxes
		emptyButton = new Button(group, SWT.RADIO);
		emptyButton.setText("Empty Network Definition (NED) file");
		emptyButton.addSelectionListener(listener);
		emptyButton.setSelection(true);

		simpleButton = new Button(group, SWT.RADIO);
		simpleButton.setText("A new Simple Module");
		simpleButton.addSelectionListener(listener);

        compoundButton = new Button(group, SWT.RADIO);
        compoundButton.setText("A new Compound Module");
        compoundButton.addSelectionListener(listener);

        networkButton = new Button(group, SWT.RADIO);
        networkButton.setText("A new toplevel Network");
        networkButton.addSelectionListener(listener);

        new Label(composite, SWT.NONE);

		setPageComplete(validatePage());
	}

	@Override
    protected InputStream getInitialContents() {
        String name = getFileName();
        if (name == null || "".equals(name))
            return null;
        
        name = name.substring(0, name.lastIndexOf('.'));
        
		String contents = NEDFILE_TEMPLATES[modelSelected].replaceAll("#NAME#", name);
		return new ByteArrayInputStream(contents.getBytes());
	}
    
	public boolean finish() {
        // add an extension if missing
        String name = getFileName();
        if (name.lastIndexOf('.') < 0) 
            setFileName(name+".ned");

        IFile newFile = createNewFile();
		if (newFile == null)
			return false; // ie.- creation was unsuccessful

		// Since the file resource was created fine, open it for editing
		// if requested by the user
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

}

package org.omnetpp.ned.editor.graph.wizards;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.ObjectOutputStream;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
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
import org.omnetpp.ned2.model.NEDElementFactoryEx;
import org.omnetpp.ned2.model.NedFileNodeEx;
import org.omnetpp.ned2.model.pojo.NEDElementFactory;
import org.omnetpp.ned2.model.pojo.NEDElementTags;

public class ModuleCreationWizardPage1 extends WizardNewFileCreationPage
		implements SelectionListener {

	private IWorkbench workbench;

	private static int exampleCount = 1;

	private Button model1 = null;

	private Button model2 = null;

	private int modelSelected = 1;

	public ModuleCreationWizardPage1(IWorkbench aWorkbench,
			IStructuredSelection selection) {
		super("sampleLogicPage1", selection); //$NON-NLS-1$
		this.setTitle("Create a NED file");
		this.setDescription("This wizard allows you to create a new netwrok topology file");
		this.setImageDescriptor(ImageDescriptor.createFromFile(getClass(),
				"icons/logicbanner.gif")); //$NON-NLS-1$
		this.workbench = aWorkbench;
	}

	@Override
    public void createControl(Composite parent) {
		super.createControl(parent);
		this.setFileName("emptyModel" + exampleCount + ".logic"); //$NON-NLS-2$//$NON-NLS-1$

		Composite composite = (Composite) getControl();

		// sample section generation group
		Group group = new Group(composite, SWT.NONE);
		group.setLayout(new GridLayout());
		group.setText("Group 1");
		group.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL
				| GridData.HORIZONTAL_ALIGN_FILL));

		// sample section generation checkboxes
		model1 = new Button(group, SWT.RADIO);
		model1.setText("Empty model");
		model1.addSelectionListener(this);
		model1.setSelection(true);

		model2 = new Button(group, SWT.RADIO);
		model2.setText("Example model");
		model2.addSelectionListener(this);

		new Label(composite, SWT.NONE);

		setPageComplete(validatePage());
	}

	@Override
    protected InputStream getInitialContents() {
		NedFileNodeEx ld = (NedFileNodeEx)NEDElementFactory.getInstance().createNodeWithTag(NEDElementTags.NED_NED_FILE);
		ByteArrayInputStream bais = null;
		try {
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			ObjectOutputStream oos = new ObjectOutputStream(baos);
			oos.writeObject(ld);
			oos.flush();
			oos.close();
			baos.close();
			bais = new ByteArrayInputStream(baos.toByteArray());
			bais.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return bais;
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
	 * @see org.eclipse.swt.events.SelectionListener#widgetSelected(SelectionEvent)
	 */
	public void widgetSelected(SelectionEvent e) {
		if (e.getSource() == model1) {
			modelSelected = 1;
			setFileName("untitled" + exampleCount + ".gned"); //$NON-NLS-2$//$NON-NLS-1$
		} else {
			modelSelected = 2;
			setFileName("complex" + exampleCount + ".gned"); //$NON-NLS-2$//$NON-NLS-1$
		}
	}

	/**
	 * Empty method
	 */
	public void widgetDefaultSelected(SelectionEvent e) {
	}

}

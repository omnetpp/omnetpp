package org.omnetpp.inifile.editor.wizards;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
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
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;

/**
 * The "New" wizard page allows setting the container for the new file as well
 * as the file name. The page will only accept file name without the extension
 * OR with the extension that matches the expected one (ini).
 */
public class NewInifileWizardPage1 extends WizardNewFileCreationPage {
	private IWorkbench workbench;
	private Combo networkCombo;

	// dialog state
    //private enum KeyType { PARAM_ONLY, MODULE_AND_PARAM, ANYNETWORK_FULLPATH, FULLPATH };
    //private KeyType keyType;

	public NewInifileWizardPage1(IWorkbench aWorkbench, IStructuredSelection selection) {
		super("page1", selection);
		setTitle("Create an ini file");
		setDescription("This wizard allows you to create a new OMNeT++ simulation configuration file.");
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
		networkCombo.addFocusListener(new FocusListener() {
            public void focusGained(FocusEvent e) {
                IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(getContainerFullPath());
                if (resource == null || !(resource instanceof IContainer)) {
                    networkCombo.removeAll();
                }
                else {
                    // fill network combo
                    IContainer container = (IContainer)resource;
                    IProject project = container.getProject();
                    NEDResources nedResources = NEDResourcesPlugin.getNEDResources();

                    // collect networks: separately those in the local package, and others
                    List<String> networkNames = new ArrayList<String>(); 
                    List<String> networkQNames = new ArrayList<String>(); 
                    String iniFilePackage = nedResources.getExpectedPackageFor(container.getFile(new Path("anything.ini")));
                    if (StringUtils.isNotEmpty(iniFilePackage)) {
                        for (String networkQName : nedResources.getNetworkQNames(project)) {
                            INEDTypeInfo network = nedResources.getToplevelNedType(networkQName, project);
                            if ((iniFilePackage+".").equals(network.getNamePrefix()))
                                networkNames.add(network.getName());
                            if (!network.getNamePrefix().equals(""))
                                networkQNames.add(network.getFullyQualifiedName());
                        }
                    }
                    Collections.sort(networkNames);
                    Collections.sort(networkQNames);
                    networkNames.addAll(networkQNames);
                    
                    networkCombo.setItems(networkNames.toArray(new String[]{}));
                    networkCombo.setVisibleItemCount(Math.min(20, networkCombo.getItemCount()));
                }
            }

            public void focusLost(FocusEvent e) {
            }
		});

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

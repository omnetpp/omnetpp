package org.omnetpp.msg.editor.wizard;

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
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.eclipse.ui.ide.IDE;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.msg.editor.MsgEditorPlugin;

/**
 * Wizard for creating a msg file
 *
 * @author andras
 */
public class NewMsgFileWizardPage1 extends WizardNewFileCreationPage {

    private static final String[] MSGFILE_TEMPLATES = {
        "//\n" +
        "// TODO comment\n" +
        "//\n\n",
        
        "//\n" +
        "// TODO comment\n" +
        "//\n" +
        "message #NAME# {\n" +
        "    @omitGetVerb(true);\n" +
        "    int someField;\n" +
        "    string anotherField;\n" +
        "    double arrayField1[];\n" +
        "    double arrayField2[10];\n" +
        "}\n",

        "cplusplus {{\n" + 
        "#include \"SomeMessage_m.h\"\n" + 
        "}}\n" +
        "message SomeMessage;\n" +
        "\n" + 
        "//\n" +
        "// TODO comment\n" +
        "//\n" +
        "message #NAME# extends SomeMessage {\n" +
        "    @omitGetVerb(true);\n" +
        "    int someField;\n" +
        "}\n",
        
        "cplusplus {{\n" + 
        "#include \"SomeType.h\"\n" + 
        "}}\n" + 
        "class noncobject SomeType;\n" + 
        "\n" + 
        "cplusplus {{\n" + 
        "#include <map>\n" + 
        "typedef OtherType std::map<int,int>;\n" + 
        "}}\n" + 
        "class noncobject OtherType;\n" + 
        "\n" + 
        "//\n" + 
        "// TODO comment\n" + 
        "//\n" + 
        "message #NAME# {\n" + 
        "    @omitGetVerb(true);\n" + 
        "    SomeType field1;\n" + 
        "    OtherType field2;\n" + 
        "}\n"
    };
    
	private IWorkbench workbench;
	private static int exampleCount = 1;

	private Button emptyButton = null;
    private Button simpleButton = null;
    private Button subclassedButton = null;
    private Button withCppButton = null;
	private int modelSelected = 0;

	public NewMsgFileWizardPage1(IWorkbench aWorkbench, IStructuredSelection selection) {
		super("page1", selection);
		setTitle("Create a msg file");
		setDescription("This wizard allows you to create a new OMNeT++ message definition file");
		setImageDescriptor(ImageDescriptor.createFromFile(getClass(),"/icons/newmsgfile_wiz.png"));

		setFileExtension("msg");
		setFileName("untitled" + exampleCount + ".msg"); 
		workbench = aWorkbench;
	}

	@Override
    public void createControl(Composite parent) {
		super.createControl(parent);

		Composite composite = (Composite) getControl();

		// sample section generation group
		Group group = new Group(composite, SWT.NONE);
		group.setLayout(new GridLayout());
		group.setText("Content");
		group.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL | GridData.HORIZONTAL_ALIGN_FILL));

		SelectionListener listener = new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				if (e.getSource() == emptyButton)
					modelSelected = 0;
				else if (e.getSource() == simpleButton)
					modelSelected = 1;
				else if (e.getSource() == subclassedButton)
				    modelSelected = 2;
				else if (e.getSource() == withCppButton)
                    modelSelected = 3;
			}
		};

		// sample section generation checkboxes
		emptyButton = new Button(group, SWT.RADIO);
		emptyButton.setText("Empty file");
		emptyButton.addSelectionListener(listener);
		emptyButton.setSelection(true);

		simpleButton = new Button(group, SWT.RADIO);
		simpleButton.setText("A basic example message class");
		simpleButton.addSelectionListener(listener);

		subclassedButton = new Button(group, SWT.RADIO);
		subclassedButton.setText("An example message that subclasses from a message defined in another .msg file");
		subclassedButton.addSelectionListener(listener);

        withCppButton = new Button(group, SWT.RADIO);
        withCppButton.setText("An example message class that uses data types defined in C++");
        withCppButton.addSelectionListener(listener);

		setPageComplete(validatePage());
	}

	@Override
    protected InputStream getInitialContents() {
        String name = getFileName();
        if (name == null || "".equals(name))
            return null;
        
        // make a valid identifier
        name = name.substring(0, name.lastIndexOf('.'));
        name = StringUtils.capitalize(StringUtils.makeValidIdentifier(name));

        // substitute name into the template
        String contents = MSGFILE_TEMPLATES[modelSelected].replaceAll("#NAME#", name);
		return new ByteArrayInputStream(contents.getBytes());
	}
    
	public boolean finish() {
        IFile newFile = createNewFile();
		if (newFile == null)
			return false; // creation was unsuccessful

		// Since the file resource was created fine, open it for editing
		// if requested by the user
		try {
			IWorkbenchWindow dwindow = workbench.getActiveWorkbenchWindow();
			IWorkbenchPage page = dwindow.getActivePage();
			if (page != null)
				IDE.openEditor(page, newFile, true);
		} 
		catch (org.eclipse.ui.PartInitException e) {
			MsgEditorPlugin.logError(e);
			return false;
		}
		exampleCount++;
		return true;
	}

}

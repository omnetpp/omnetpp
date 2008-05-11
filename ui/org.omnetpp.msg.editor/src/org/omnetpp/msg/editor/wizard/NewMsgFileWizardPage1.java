package org.omnetpp.msg.editor.wizard;

import java.io.ByteArrayInputStream;
import java.io.InputStream;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
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
        "}\n",

        "//\n" +
        "// TODO comment\n" +
        "//\n" +
        "message #NAME# {\n" +
        "    @omitGetVerb(true);\n" +
        "    @customize(true);  // see the generated C++ header for more info\n" +
        "    int someField;\n" +
        "    abstract int anotherField;\n" +
        "}\n",
        
    };
    
	private IWorkbench workbench;
	private static int exampleCount = 1;

	private int templateSelected = 0;

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

		Button emptyButton = createRadioButton(group, "Empty file", 0);
		createRadioButton(group, "A basic example message class", 1);
		createRadioButton(group, "An example message that subclasses from a message defined in another .msg file", 2);
		createRadioButton(group, "An example message class that uses data types defined in C++", 3);
		createRadioButton(group, "An example message class that can be customized in C++", 4);

        emptyButton.setSelection(true);

		setPageComplete(validatePage());
	}

	protected Button createRadioButton(Composite parent, String text, final int templateToSelect) {
        Button button = new Button(parent, SWT.RADIO);
		button.setText(text);
		button.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                    templateSelected = templateToSelect;
            }});
		return button;
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
        String contents = MSGFILE_TEMPLATES[templateSelected].replaceAll("#NAME#", name);
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

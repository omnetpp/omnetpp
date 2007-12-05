package org.omnetpp.ide.properties;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.QualifiedName;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.dialogs.ResourceListSelectionDialog;

/**
 * This is a property page displayed in project properties to set the project specific parameters for documentation generation.
 * The property values are stored on the project as persistent properties.
 * The parameters are the following project relative paths:
 *  - doxygen configuration file
 *  - generated doxygen documentation
 *  - generated neddoc documentation
 * 
 * @author levy
 */
public class DocumentationGeneratorPropertyPage 
    extends PropertyPage
{
    private static final String DEFAULT_DOXY_PATH = "/Documentation/doxy";

    private static final String DEFAULT_DOXY_CONFIG_FILE_PATH = "/doxy.cfg";

    private static final String DEFAULT_NEDDOC_PATH = "/Documentation/neddoc";

    public static QualifiedName DOXY_PATH_QNAME = new QualifiedName("DocumentationGenerator", "DoxyPath");

    public static QualifiedName DOXY_CONFIG_FILE_PATH_QNAME = new QualifiedName("DocumentationGenerator", "DoxyConfigFilePath");
    
    public static QualifiedName NEDDOC_PATH_QNAME = new QualifiedName("DocumentationGenerator", "NeddocPath");

	private Text doxyPath;

	private Text doxyConfigFilePath;
    
	private Text neddocPath;

    public DocumentationGeneratorPropertyPage() {
		super();
	}

    @Override
	protected Control createContents(Composite parent) {
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1, false));
        
        Group group = new Group(composite, SWT.NONE);
        group.setText("Project relative paths");
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        group.setLayout(new GridLayout(3, false));

        doxyPath = addLabelledText(group, "Generated Doxygen documentation:");
        doxyConfigFilePath = addLabelledText(group, "Doxygen configuration file:");
        neddocPath = addLabelledText(group, "Generated NED documentation:");
        
        try {
            IProject project = getProject();
            setText(doxyPath, getDoxyPath(project));
            setText(doxyConfigFilePath, getDoxyConfigFilePath(project));
            setText(neddocPath, getNeddocPath(project));
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
        
        return composite;
	}

    private void setText(Text text, String value) {
        if (value != null)
            text.setText(value);
    }

    private Text addLabelledText(Composite composite, String labelText) {
        Label label = new Label(composite, SWT.NONE);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 1, 1));
        label.setText(labelText);
        
        final Text text = new Text(composite, SWT.BORDER);
        text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));
        
        Button button = new Button(composite, SWT.PUSH);
        button.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 1, 1));
        button.setText("Browse...");
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                ResourceListSelectionDialog dialog = new ResourceListSelectionDialog(getShell(), getProject(), IResource.FOLDER + IResource.FILE);

                if (dialog.open() == Window.OK)
                    text.setText(dialog.getResult()[0].toString().substring(1));
            }
        });
        
        return text;
    }

    @Override
	protected void performDefaults() {
        doxyPath.setText(DEFAULT_DOXY_PATH);
        doxyConfigFilePath.setText(DEFAULT_DOXY_CONFIG_FILE_PATH);
        neddocPath.setText(DEFAULT_NEDDOC_PATH);
	}

	@Override
	public boolean performOk() {
        try {
            IProject project = getProject();

            project.setPersistentProperty(DOXY_PATH_QNAME, doxyPath.getText());
            project.setPersistentProperty(DOXY_CONFIG_FILE_PATH_QNAME, doxyConfigFilePath.getText());
            project.setPersistentProperty(NEDDOC_PATH_QNAME, neddocPath.getText());

            return true;
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
	}

    private IProject getProject() {
        return (IProject)getElement().getAdapter(IProject.class);
    }
    
    public static String getDoxyPath(IProject project) throws CoreException {
        String value = project.getPersistentProperty(DOXY_PATH_QNAME);
        
        if (value == null)
            return DEFAULT_DOXY_PATH;
        else
            return value;
    }

    public static String getDoxyConfigFilePath(IProject project) throws CoreException {
        String value = project.getPersistentProperty(DOXY_CONFIG_FILE_PATH_QNAME);
        
        if (value == null)
            return DEFAULT_DOXY_CONFIG_FILE_PATH;
        else
            return value;
    }

    public static String getNeddocPath(IProject project) throws CoreException {
        String value = project.getPersistentProperty(NEDDOC_PATH_QNAME);
        
        if (value == null)
            return DEFAULT_NEDDOC_PATH;
        else
            return value;
    }
}
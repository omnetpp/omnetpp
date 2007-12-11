package org.omnetpp.cdt.ui;

import java.io.IOException;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.PropertyPage;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecUtils;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.common.color.ColorFactory;

/**
 * This property page is shown for folders in an OMNeT++ CDT Project, and lets the user 
 * edit the contents of the ".oppbuildspec" file.
 *
 * @author Andras
 */
//TODO: "Show Dependencies" (of a folder)
public class OmnetppCDTFolderPropertyPage extends PropertyPage {
    protected MakemakeOptionsPanel contents;
    
    // state
    protected BuildSpecification buildSpec;

	/**
	 * Constructor.
	 */
	public OmnetppCDTFolderPropertyPage() {
		super();
	}

	/**
	 * @see PreferencePage#createContents(Composite)
	 */
	protected Control createContents(Composite parent) {
	    Group group = new Group(parent, SWT.NONE);
	    group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
	    group.setLayout(new GridLayout(1,false));
        final Button enableMakefileCheckbox = createCheckbox(group, "Generate Makefile automatically");
        enableMakefileCheckbox.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        
        Text message = new Text(group, SWT.MULTI);
        message.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        message.setEditable(false);
        message.setText("WARNING: Makefiles are not in use because this project is configured for CDT-managed Makefiles...........");
        
	    contents = new MakemakeOptionsPanel(parent, SWT.NONE, null); //XXX
	    contents.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

	    enableMakefileCheckbox.addSelectionListener(new SelectionAdapter() {
	        @Override
	        public void widgetSelected(SelectionEvent e) {
	            boolean enabled = enableMakefileCheckbox.getSelection();
	            contents.setVisible(enabled);
	        } 
	    });
	    
		loadBuildSpecFile();
		
		return contents;
	}

	protected Button createCheckbox(Composite parent, String text) {
	    Button button = new Button(parent, SWT.CHECK);
	    button.setText(text);
	    return button;
	}

	public boolean performOk() {
		saveBuildSpecFile();
		return true;
	}

	protected void loadBuildSpecFile() {
		try {
		    IProject project = (IProject) getElement().getAdapter(IProject.class);
		    buildSpec = BuildSpecUtils.readBuildSpecFile(project);
		    if (buildSpec == null)
		        buildSpec = new BuildSpecification();
		    //FIXME TODO: treeViewer.refresh();
		} 
		catch (IOException e) {
			errorDialog("Cannot read build specification: ", e);
		} catch (CoreException e) {
			errorDialog("Cannot read build specification: ", e);
		}
		
	}

	protected void saveBuildSpecFile() {
		try {
			IProject project = (IProject)getElement().getAdapter(IProject.class);
            BuildSpecUtils.saveBuildSpecFile(project, buildSpec);
		} 
		catch (CoreException e) {
			errorDialog("Cannot store build specification: ", e);
		}
	} 

	protected void errorDialog(String message, Throwable e) {
		IStatus status = new Status(IMarker.SEVERITY_ERROR, Activator.PLUGIN_ID, e.getMessage(), e);
		ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", message, status);
	}
}


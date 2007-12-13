package org.omnetpp.cdt.ui;

import java.io.IOException;

import org.eclipse.core.resources.IContainer;
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
import org.omnetpp.cdt.makefile.MakemakeOptions;

/**
 * This property page is shown for folders in an OMNeT++ CDT Project, and lets the user 
 * edit the contents of the ".oppbuildspec" file.
 *
 * @author Andras
 */
public class OmnetppCDTFolderPropertyPage extends PropertyPage {
    protected MakemakeOptionsPanel contents;
    
    // state
    protected BuildSpecification buildSpec;

    private Button enableMakefileCheckbox;

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
        enableMakefileCheckbox = createCheckbox(group, "Generate Makefile automatically");
        enableMakefileCheckbox.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        
        Text message = new Text(group, SWT.MULTI);
        message.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        message.setEditable(false);
        message.setText(getInformationalMessage());
        
	    contents = new MakemakeOptionsPanel(parent, SWT.NONE); 
	    contents.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

	    enableMakefileCheckbox.addSelectionListener(new SelectionAdapter() {
	        @Override
	        public void widgetSelected(SelectionEvent e) {
	            boolean enabled = enableMakefileCheckbox.getSelection();
	            contents.setVisible(enabled);
	        } 
	    });
        
		loadBuildSpecFile();
		MakemakeOptions folderOptions = buildSpec.getFolderOptions(getResource());
		enableMakefileCheckbox.setSelection(folderOptions != null);
        contents.populate(folderOptions != null ? folderOptions : new MakemakeOptions());
		
        updateDialogState();

        return contents;
	}

    protected void updateDialogState() {
        contents.setVisible(enableMakefileCheckbox.getSelection());
    }

    protected String getInformationalMessage() {
        //XXX return some message to be displayed to the user.
        // display info if: 
        //  - this directory is under a "deep" makefile
        //  - this directory is excluded
        //  - this directory is not under a source folder
        // display warning if:
        //  - managed make is turned on (CDT's "Generate Makefiles automatically")
        //  - makefile consistency error (i.e. a subdir doesn't contain a makefile,
        //    or makefile generation is turned on on a directory under a "deep" makefile dir)
        //  - something else is wrong?
        return "WARNING: Makefiles are not in use because this project is configured for CDT-managed Makefiles...........";
    }

    protected Button createCheckbox(Composite parent, String text) {
	    Button button = new Button(parent, SWT.CHECK);
	    button.setText(text);
	    return button;
	}

	public boolean performOk() {
        if (enableMakefileCheckbox.getSelection() == true)
            buildSpec.setFolderOptions(getResource(), contents.getResult());
        else
            buildSpec.setFolderOptions(getResource(), null);
		saveBuildSpecFile();
		return true;
	}

	protected void loadBuildSpecFile() {
		try {
            IProject project = getResource().getProject();
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

    /**
     * The resource whose properties we are editing.
     */
	protected IContainer getResource() {
        IContainer container = (IContainer) getElement().getAdapter(IContainer.class);
        return container;
    }

	protected void saveBuildSpecFile() {
		try {
			IProject project = getResource().getProject();
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


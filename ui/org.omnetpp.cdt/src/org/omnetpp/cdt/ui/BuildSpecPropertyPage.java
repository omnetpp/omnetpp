package org.omnetpp.cdt.ui;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecUtils;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.cdt.makefile.BuildSpecification.FolderType;
import org.omnetpp.common.util.StringUtils;

/**
 * FIXME
 * This property page is shown for OMNeT++ Projects (projects with the
 * omnetpp nature), and lets the user edit the contents of the ".nedfolders" 
 * file.
 *
 * @author Andras
 */
public class BuildSpecPropertyPage extends PropertyPage {
    // widgets
	private TreeViewer treeViewer;
    private Button generatedMakefileButton;
    private Button customMakefileButton;
    private Button excludeButton;
    private Button optionsButton;
    private Button removeOptionsButton;
    
    // state
    private BuildSpecification buildSpec;

	/**
	 * Constructor.
	 */
	public BuildSpecPropertyPage() {
		super();
	}

	/**
	 * @see PreferencePage#createContents(Composite)
	 */
	protected Control createContents(Composite parent) {
		Composite composite = new Composite(parent, SWT.NONE);
		composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		composite.setLayout(new GridLayout(2, false));

		final IProject project = (IProject) getElement();

        //createWrapLabel(composite, "Some explanation....", 2, 300);

		// create treeviewer and label above it
        Label label = new Label(composite, SWT.NONE);
        label.setText("&Build specification for project '" + project.getName() + "':");
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1));
        
		treeViewer = new TreeViewer(composite, SWT.BORDER | SWT.MULTI);
		treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
		((GridData)treeViewer.getTree().getLayoutData()).heightHint = 300;

		// create buttons
		Composite buttons = new Composite(composite, SWT.NONE);
        buttons.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));
        buttons.setLayout(new GridLayout(1, true));

        generatedMakefileButton = createButton(buttons, "Generated Makefile");
        customMakefileButton = createButton(buttons, "Custom Makefile");
        excludeButton = createButton(buttons, "Exclude From Build");
        new Label(buttons, SWT.PUSH);
        optionsButton = createButton(buttons, "Options...");
        removeOptionsButton = createButton(buttons, "Remove Options");

        // configure buttons
        generatedMakefileButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                setFolderType(FolderType.GENERATED_MAKEFILE);
            }});
        customMakefileButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                setFolderType(FolderType.CUSTOM_MAKEFILE);
            }});
        excludeButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                setFolderType(FolderType.EXCLUDED_FROM_BUILD);
            }});
        optionsButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                editFolderOptions();
            }});
        removeOptionsButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                removeFolderOptions();
            }});
        
        // configure treeviewer
        treeViewer.setContentProvider(new WorkbenchContentProvider() {
	        @Override
	        public Object[] getChildren(Object element) {
	            List<Object> filteredChildren = new ArrayList<Object>();
	            for (Object child : super.getChildren(element)) {
	                if (child==project || child instanceof IFolder)
	                    filteredChildren.add(child);
	            }
	            return filteredChildren.toArray();
	        }
	    });

		treeViewer.setLabelProvider(new WorkbenchLabelProvider() {
            @Override
            protected String decorateText(String input, Object element) {
                IContainer folder = (IContainer) element;
                String additionalText = "";
                switch (buildSpec.getFolderType(folder)) {
                    case CUSTOM_MAKEFILE: additionalText = "custom"; break;
                    case EXCLUDED_FROM_BUILD: additionalText = "exclude"; break;
                    case GENERATED_MAKEFILE: additionalText = "makemake"; break;
                }
                MakemakeOptions makemakeOptions = buildSpec.getMakemakeOptions(folder);
                if (makemakeOptions != null)
                    additionalText += ": " + StringUtils.join(makemakeOptions.toArgs(), " ");
                
                if (additionalText.length() > 0)
                    return super.decorateText(input, element) + " -- " + additionalText;
                else
                    return super.decorateText(input, element);
            }
		}); 
	        
		treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                updateButtonStates();
            }
		});
		
		loadBuildSpecFile();

		treeViewer.setInput(project.getParent());
		treeViewer.expandToLevel(2);
		
		return composite;
	}

    private Button createButton(Composite buttons, String label) {
        Button button = new Button(buttons, SWT.PUSH);
        button.setLayoutData(new GridData(SWT.FILL,SWT.FILL,false,false));
        button.setText(label);
        return button;
    }

	public static Label createWrapLabel(Composite parent, String text, int hspan, int wrapwidth) {
	    // code from SWTFactory (debug.internal.ui)
	    Label l = new Label(parent, SWT.NONE | SWT.WRAP);
	    l.setFont(parent.getFont());
	    l.setText(text);
	    GridData gd = new GridData(GridData.FILL_HORIZONTAL);
	    gd.horizontalSpan = hspan;
	    gd.widthHint = wrapwidth;
	    l.setLayoutData(gd);
	    return l;
	}

	protected void updateButtonStates() {
	    IStructuredSelection sel = (IStructuredSelection)treeViewer.getSelection();
	    
	    int generatedMakefileCount = 0;
	    for (Object o : sel.toArray())
	        if (buildSpec.getFolderType((IContainer)o) == FolderType.GENERATED_MAKEFILE)
	            generatedMakefileCount++;
	    
	    generatedMakefileButton.setEnabled(!sel.isEmpty());
	    customMakefileButton.setEnabled(!sel.isEmpty());
	    excludeButton.setEnabled(!sel.isEmpty());
	    optionsButton.setEnabled(generatedMakefileCount > 0);
	    removeOptionsButton.setEnabled(generatedMakefileCount > 0);
	}

    protected void setFolderType(FolderType folderType) {
        IStructuredSelection sel = (IStructuredSelection)treeViewer.getSelection();
        for (Object o : sel.toArray())
            buildSpec.setFolderInfo((IContainer)o, folderType, null); //FIXME
        treeViewer.refresh();
        updateButtonStates();
    }

    protected void editFolderOptions() {
//        IStructuredSelection sel = (IStructuredSelection)treeViewer.getSelection();
//        for (Object o : sel.toArray())
//            buildSpec.setFolderInfo((IContainer)o, folderType, null); //FIXME
        treeViewer.refresh();
        updateButtonStates();
    }

    protected void removeFolderOptions() {
        IStructuredSelection sel = (IStructuredSelection)treeViewer.getSelection();
        for (Object o : sel.toArray())
            buildSpec.setFolderOptions((IContainer)o, null);
        treeViewer.refresh();
        updateButtonStates();
    }

	protected void performDefaults() {
	    buildSpec = new BuildSpecification();
	    treeViewer.refresh();
	}
	
	public boolean performOk() {
		saveBuildSpecFile();
		return true;
	}

	private void loadBuildSpecFile() {
		try {
		    IProject project = (IProject) getElement();
		    buildSpec = BuildSpecUtils.readBuildSpecFile(project);
		    treeViewer.refresh();
		} 
		catch (IOException e) {
			errorDialog("Cannot read build specification: ", e);
		} catch (CoreException e) {
			errorDialog("Cannot read build specification: ", e);
		}
		
	}

	private void saveBuildSpecFile() {
		try {
			IProject project = (IProject)getElement();
            BuildSpecUtils.saveBuildSpecFile(project, buildSpec);
		} 
		catch (CoreException e) {
			errorDialog("Cannot store build specification: ", e);
		}
	} 

	private void errorDialog(String message, Throwable e) {
		IStatus status = new Status(IMarker.SEVERITY_ERROR, Activator.PLUGIN_ID, e.getMessage(), e);
		ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", message, status);
	}
}
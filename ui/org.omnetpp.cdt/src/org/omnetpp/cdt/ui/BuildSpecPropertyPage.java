package org.omnetpp.cdt.ui;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecUtils;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.cdt.makefile.MakefileTools;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.cdt.makefile.BuildSpecification.FolderType;

/**
 * This property page is shown for OMNeT++ CDT Projects, and lets the user 
 * edit the contents of the ".oppbuildspec" file.
 *
 * @author Andras
 */
//TODO: "Show Dependencies" (of a folder)
//TODO: out-of-directory build
public class BuildSpecPropertyPage extends PropertyPage {
    private static Image IMAGE_FOLDER_EXCLUDED = Activator.getCachedImage("icons/full/obj16/folder_excl.gif");
    private static Image IMAGE_FOLDER_CUSTOM = Activator.getCachedImage("icons/full/obj16/folder_custom.gif");
    private static Image IMAGE_FOLDER_EXE = Activator.getCachedImage("icons/full/obj16/folder_exe2.gif");
    private static Image IMAGE_FOLDER_DLL = Activator.getCachedImage("icons/full/obj16/folder_dll.gif");
    private static Image IMAGE_FOLDER_LIB = Activator.getCachedImage("icons/full/obj16/folder_lib.gif");
    private static Image IMAGE_FOLDER_NOLINK = Activator.getCachedImage("icons/full/obj16/folder_nolink.gif");
    
    // widgets
	private TreeViewer treeViewer;
    private Button generatedMakefileButton;
    private Button customMakefileButton;
    private Button excludeButton;
    private Button defaultButton;
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

        createWrapLabel(composite, 
                "Makefiles will be automatically generated in folders marked so. " +
        		"Makefiles can be configured via options for opp_makemake, and " +
        		"via \"makefrag\" files whose contents will be inserted into the " +
        		"generated Makefile in the same folder. " +
        		"Cross-folder include paths need not be specified, they will be discovered " +
        		"and added automatically.", 2, 300);

		// create treeviewer and label above it
        Label label = new Label(composite, SWT.NONE);
        label.setText("&Build specification for project '" + project.getName() + "':");
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1));
        
		treeViewer = new TreeViewer(composite, SWT.BORDER | SWT.MULTI);
		treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

		// create buttons
		Composite buttons = new Composite(composite, SWT.NONE);
        buttons.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));
        buttons.setLayout(new GridLayout(1, true));

        generatedMakefileButton = createButton(buttons, "Generated Makefile");
        customMakefileButton = createButton(buttons, "Custom Makefile");
        excludeButton = createButton(buttons, "Exclude From Build");
        defaultButton = createButton(buttons, "Default");
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
        defaultButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                setFolderType(null);
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
	            for (Object child : super.getChildren(element))
	                if (child instanceof IProject ? child==project : MakefileTools.isGoodFolder((IResource)child))
	                    filteredChildren.add(child);
	            return filteredChildren.toArray();
	        }
	    });

		treeViewer.setLabelProvider(new LabelProvider() {
		    
            @Override
            public Image getImage(Object element) {
                IContainer folder = (IContainer) element;
                switch (buildSpec.getFolderType(folder)) {
                    case CUSTOM_MAKEFILE: return IMAGE_FOLDER_CUSTOM;
                    case EXCLUDED_FROM_BUILD: return IMAGE_FOLDER_EXCLUDED;
                    case GENERATED_MAKEFILE: 
                        MakemakeOptions makemakeOptions = buildSpec.getFolderOptions(folder);
                        if (makemakeOptions == null) makemakeOptions = new MakemakeOptions(); //FIXME getFolderOptions should never return null??
                        switch (makemakeOptions.type) {
                            case EXE: return IMAGE_FOLDER_EXE;
                            case SO: return IMAGE_FOLDER_DLL;
                            //case LIB: return IMAGE_FOLDER_LIB; //XXX no such type! 
                            case NOLINK: return IMAGE_FOLDER_NOLINK;
                        }
                }
                return null;
            }

            @Override
            public String getText(Object element) {
                IContainer folder = (IContainer) element;
                String additionalText = "";

                boolean isInherited = buildSpec.isFolderTypeInherited(folder);
                switch (buildSpec.getFolderType(folder)) {
                    case CUSTOM_MAKEFILE: additionalText = isInherited ? "(custom)" : "CUSTOM"; break;
                    case EXCLUDED_FROM_BUILD: additionalText = isInherited ? "(exclude)" : "EXCLUDE"; break;
                    case GENERATED_MAKEFILE: additionalText = isInherited ? "(makemake)" : "MAKEMAKE"; break;
                }
                MakemakeOptions makemakeOptions = buildSpec.getFolderOptions(folder);
                if (buildSpec.isMakemakeFolder(folder) && makemakeOptions != null) {
                    String args = makemakeOptions.toString();
                    additionalText += ": " + (buildSpec.isFolderOptionsInherited(folder) ? "("+args+")" : args);
                }
                
                if (additionalText.length() > 0)
                    return folder.getName() + " -- " + additionalText;
                else
                    return folder.getName();
            }
		}); 
	        
		treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                updateButtonStates();
            }
		});
		
		loadBuildSpecFile();

		treeViewer.setInput(project.getParent());

		// expand so that all "interesting" points are visible, but open at least the project node
		treeViewer.expandToLevel(2);
		for (IContainer folder : buildSpec.getFolders())
	        treeViewer.expandToLevel(folder, 0);
		
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
	    defaultButton.setEnabled(!sel.isEmpty());
	    optionsButton.setEnabled(generatedMakefileCount > 0);
	    removeOptionsButton.setEnabled(generatedMakefileCount > 0);
	}

    protected void setFolderType(FolderType folderType) {
        IStructuredSelection sel = (IStructuredSelection)treeViewer.getSelection();
        for (Object o : sel.toArray()) {
            buildSpec.setFolderType((IContainer)o, folderType);
            if (folderType == FolderType.EXCLUDED_FROM_BUILD)
                setFolderTypeInSubtree((IContainer)o, null); // if EXCLUDED, all folders below must be also EXCLUDED
            else
                ; //FIXME ensure it has no EXCLUDED parent! if so, don't let the user change the type 
        }
        treeViewer.refresh();
        updateButtonStates();
    }

    protected void setFolderTypeInSubtree(IContainer parentFolder, FolderType folderType) {
        for (IContainer folder : buildSpec.getFolders())
            if (containsFolder(parentFolder, folder))
                buildSpec.setFolderType(folder, folderType);
    }

    private boolean containsFolder(IContainer parentFolder, IContainer folder) {
        boolean isChild = false;
        for (IContainer cur = folder.getParent(); cur != null; cur = cur.getParent())
            if (cur.equals(parentFolder))
                {isChild = true; break;}
        return isChild;
    }

    @SuppressWarnings("unchecked")
    protected void editFolderOptions() {
        IStructuredSelection sel = (IStructuredSelection)treeViewer.getSelection();

        // dialog default value: take it from first suitable folder 
        MakemakeOptions options = null;
        for (IContainer folder : (List<IContainer>)sel.toList())
            if (buildSpec.getFolderType(folder) == FolderType.GENERATED_MAKEFILE && buildSpec.getFolderOptions(folder) != null && !buildSpec.isFolderOptionsInherited(folder))
                options = buildSpec.getFolderOptions(folder);
        String value = options == null ? "" : options.toString();

        // open dialog  
        //TODO something more sophisticated...
        InputDialog dialog = new InputDialog(getShell(), "Folder Build Options", "Command-line options for opp_makemake:", value, new IInputValidator() {
            public String isValid(String newText) {
                try {
                    new MakemakeOptions(newText);
                } catch (Exception e) {
                    return e.getMessage();
                }
                return null;
            }
        });
        if (dialog.open() == Window.OK) {
            String args = dialog.getValue();
            for (IContainer folder : (List<IContainer>)sel.toList())
                if (buildSpec.getFolderType(folder) == FolderType.GENERATED_MAKEFILE)
                    buildSpec.setFolderOptions(folder, new MakemakeOptions(args));
            
            treeViewer.refresh();
            updateButtonStates();
        }
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
		    if (buildSpec == null)
		        buildSpec = new BuildSpecification();
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
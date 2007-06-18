package org.omnetpp.launch.tabs;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.internal.ui.SWTFactory;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;

import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPlugin;

/**
 * A launch configuration tab that displays and edits omnetpp project
 */
public class OmnetppMainTab extends SimulationTab {

	// Project UI widgets
	protected Text fProjText;

	// Main class UI widgets
	protected Text fProgText;

    /**
     * Content provider displaying possible projects in the workspace
     * @author rhornig
     */
    protected class ProjectWorkbenchContentProvider extends WorkbenchContentProvider {
        @Override
        public Object[] getChildren(Object element) {
            List<Object> filteredChildren = new ArrayList<Object>();
            for(Object child : super.getChildren(element)) {
                if (child instanceof IProject && ((IProject)child).isAccessible())
                    filteredChildren.add(child);
            }
            return filteredChildren.toArray();
        }
    };

    /**
     * A workbench content provider that returns only executable files
     * @author rhornig
     */
    protected class ExecutableWorkbenchContentProvider extends WorkbenchContentProvider {
        @Override
        public Object[] getChildren(Object element) {
            List<Object> filteredChildren = new ArrayList<Object>();
            for(Object child : super.getChildren(element)) {
                if (child instanceof IFile &&
                        (((IFile)child).getResourceAttributes().isExecutable() ||
                                "exe".equals(((IFile)child).getFileExtension()) && SWT.getPlatform().equals("win32"))
                        || getChildren(child).length > 0)
                        filteredChildren.add(child);
            }
            return filteredChildren.toArray();
        }
    };
	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#createControl(org.eclipse.swt.widgets.Composite)
	 */
	@Override
    public void createControl(Composite parent) {
        Composite comp = SWTFactory.createComposite(parent, 1, 1, GridData.FILL_HORIZONTAL);
        createProjectGroup(comp, 1);
        createExeFileGroup(comp, 1);
        createLibraryGroup(comp, 1);
        createIniGroup(comp, 1);
        createConfigGroup(comp, 1);
        createUIGroup(comp, 1);
        createAdditionalGroup(comp, 1);
        WorkingDirectoryBlock wb = new WorkingDirectoryBlock();
        wb.createControl(comp);
        setControl(comp);
    }

	protected void createProjectGroup(Composite parent, int colSpan) {
		Composite projComp = SWTFactory.createComposite(parent, 3,colSpan,GridData.FILL_HORIZONTAL);
		GridLayout ld = (GridLayout)projComp.getLayout();
		ld.marginHeight = 1;

		SWTFactory.createLabel(projComp, "Simulation Project:",1);

		fProjText = SWTFactory.createSingleText(projComp, 1);
		fProjText.addModifyListener(this);

		Button fProjButton = SWTFactory.createPushButton(projComp, "Browse...", null);
		fProjButton.addSelectionListener(new SelectionAdapter() {
			@Override
            public void widgetSelected(SelectionEvent evt) {
				handleProjectButtonSelected();
				updateLaunchConfigurationDialog();
			}
		});
	}

	protected void createExeFileGroup(Composite parent, int colSpan) {
		Composite mainComp =  SWTFactory.createComposite(parent, 3,colSpan,GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)mainComp.getLayout();
        ld.marginHeight = 1;

		SWTFactory.createLabel(mainComp, "Simulation Program:",1);

		fProgText = SWTFactory.createSingleText(mainComp, 1);
		fProgText.addModifyListener(this);

		Button fBrowseForBinaryButton = SWTFactory.createPushButton(mainComp, "Browse...", null); //$NON-NLS-1$
		fBrowseForBinaryButton.addSelectionListener(new SelectionAdapter() {
			@Override
            public void widgetSelected(SelectionEvent evt) {
				handleBinaryBrowseButtonSelected();
				updateLaunchConfigurationDialog();
			}
		});
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#initializeFrom(org.eclipse.debug.core.ILaunchConfiguration)
	 */
	@Override
    public void initializeFrom(ILaunchConfiguration config) {
	    super.initializeFrom(config);
        try {
            fProjText.setText(config.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, EMPTY_STRING));
            fProgText.setText(config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, EMPTY_STRING));
        } catch (CoreException ce) {
            LaunchPlugin.logError(ce);
        }
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#performApply(org.eclipse.debug.core.ILaunchConfigurationWorkingCopy)
	 */
	@Override
    public void performApply(ILaunchConfigurationWorkingCopy config) {
	    super.performApply(config);
        config.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, fProjText.getText());
        config.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, fProgText.getText());
	}

	/**
	 * Show a dialog that lets the user select a project. This in turn provides context for the main
	 * type, allowing the user to key a main type name, or constraining the search for main types to
	 * the specified project.
	 */
	protected void handleBinaryBrowseButtonSelected() {
        ElementTreeSelectionDialog dialog
            = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(),
                                                         new ExecutableWorkbenchContentProvider());
        dialog.setAllowMultiple(false);
        dialog.setTitle("Select Executable File");
        dialog.setMessage("Select the executable file that should be started.\n");
        dialog.setInput(getProject());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID && dialog.getFirstResult() instanceof IFile) {
            String exefile = ((IFile)dialog.getFirstResult()).getProjectRelativePath().toString();
            fProgText.setText(exefile);
        }
	}

	/**
	 * Show a dialog that lets the user select a project. This in turn provides context for the main
	 * type, allowing the user to key a main type name, or constraining the search for main types to
	 * the specified project.
	 */
	protected void handleProjectButtonSelected() {
        ElementTreeSelectionDialog dialog = new ElementTreeSelectionDialog(getShell(),
                new WorkbenchLabelProvider(),
                new ProjectWorkbenchContentProvider());
        dialog.setAllowMultiple(false);
        dialog.setTitle("Select a Project");
        dialog.setMessage("Select an open project where the exectuable file is located\n");
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID && dialog.getFirstResult() instanceof IProject) {
            String projectname = ((IProject) dialog.getFirstResult()).getFullPath().toString();
            fProjText.setText(projectname);
        }
	}

	/*
     * (non-Javadoc)
     *
     * @see org.eclipse.debug.ui.ILaunchConfigurationTab#isValid(org.eclipse.debug.core.ILaunchConfiguration)
     */
	@Override
    public boolean isValid(ILaunchConfiguration config) {

	    if (!super.isValid(config))
	        return false;

		String name = fProjText.getText().trim();
		if (name.length() == 0) {
			setErrorMessage("Project is not specified");
			return false;
		}
		if (!ResourcesPlugin.getWorkspace().getRoot().getProject(name).exists()) {
			setErrorMessage("Project does not exist");
			return false;
		}
		IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(name);
		if (!project.isOpen()) {
			setErrorMessage("Project must be opened");
			return false;
		}

		name = fProgText.getText().trim();
		if (name.length() == 0) {
			setErrorMessage("Simulation program not specified");
			return false;
		}
		if (name.equals(".") || name.equals("..")) {
			setErrorMessage("Simulation program does not exist");
			return false;
		}
		IPath exePath = new Path(name);
		if (!exePath.isAbsolute()) {
			if (!project.getFile(name).exists()) {
				setErrorMessage("Simulation program does not exist");
				return false;
			}
			exePath = project.getFile(name).getLocation();
		} else {
			if (!exePath.toFile().exists()) {
				setErrorMessage("Simulation program does not exist");
				return false;
			}
		}

		return true;
	}

    @Override
    public String getName() {
        return "Main";
    }

    @Override
    public String getId() {
        return "org.omnetpp.launch.mainTab";
    }
}

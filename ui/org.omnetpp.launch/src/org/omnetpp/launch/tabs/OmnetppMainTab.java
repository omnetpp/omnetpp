package org.omnetpp.launch.tabs;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
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
public class OmnetppMainTab extends OmnetppLaunchTab {

	// UI widgets
	protected Text progText;

    private final WorkingDirectoryBlock workingDirBlock = new WorkingDirectoryBlock(this);
    private final SimulationTab simulationBlock = new SimulationTab(this, false);

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


    public OmnetppMainTab() {
        super();
    }

    public OmnetppMainTab(OmnetppLaunchTab embeddingTab) {
        super(embeddingTab);
    }

    public void createControl(Composite parent) {
        Composite comp = SWTFactory.createComposite(parent, 1, 1, GridData.FILL_HORIZONTAL);
        createExeFileGroup(comp, 1);
        simulationBlock.createControl(comp);
        GridLayout ld = (GridLayout)((Composite)simulationBlock.getControl()).getLayout();
        ld.marginWidth = ld.marginHeight = 0;

        workingDirBlock.createControl(comp);
        setControl(comp);
    }

	protected void createExeFileGroup(Composite parent, int colSpan) {
		Composite mainComp =  SWTFactory.createComposite(parent, 3,colSpan,GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)mainComp.getLayout();
        ld.marginHeight = 1;

		SWTFactory.createLabel(mainComp, "Simulation Program:",1);

		progText = SWTFactory.createSingleText(mainComp, 1);
		progText.addModifyListener(this);

		Button fBrowseForBinaryButton = SWTFactory.createPushButton(mainComp, "Browse...", null); //$NON-NLS-1$
		fBrowseForBinaryButton.addSelectionListener(new SelectionAdapter() {
			@Override
            public void widgetSelected(SelectionEvent evt) {
				handleBinaryBrowseButtonSelected();
				updateLaunchConfigurationDialog();
			}
		});
	}

	@Override
    public void initializeFrom(ILaunchConfiguration config) {
	    super.initializeFrom(config);
        try {
            progText.setText(config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, EMPTY_STRING));
        } catch (CoreException ce) {
            LaunchPlugin.logError(ce);
        }
        simulationBlock.initializeFrom(config);
        workingDirBlock.initializeFrom(config);
	}

    public void performApply(ILaunchConfigurationWorkingCopy config) {
        config.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, progText.getText());
        simulationBlock.performApply(config);
        workingDirBlock.performApply(config);
	}

    public void setDefaults(ILaunchConfigurationWorkingCopy configuration) {
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, (String)null);
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, (String)null);
        simulationBlock.setDefaults(configuration);
        workingDirBlock.setDefaults(configuration);
    }

   	/*
     * (non-Javadoc)
     *
     * @see org.eclipse.debug.ui.ILaunchConfigurationTab#isValid(org.eclipse.debug.core.ILaunchConfiguration)
     */
	@Override
    public boolean isValid(ILaunchConfiguration config) {
	    if (!workingDirBlock.isValid(config))
	        return false;

	    String name = progText.getText().trim();
	    if (name.length() == 0) {
	        setErrorMessage("Simulation program not specified");
	        return false;
	    }
	    IFile exefile = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(name));
	    if (exefile == null || !exefile.isAccessible()) {
	        setErrorMessage("Simulation program does not exist or not accessible in workspace");
	        return false;
	    }

	    if (!simulationBlock.isValid(config))
            return false;

		return super.isValid(config);
	}

    public String getName() {
        return "Main";
    }

    @Override
    public String getId() {
        return IOmnetppLaunchConstants.OMNETPP_LAUNCH_ID+".mainTab";
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
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID && dialog.getFirstResult() instanceof IFile) {
            String exefile = ((IFile)dialog.getFirstResult()).getFullPath().toString();
            progText.setText(exefile);
        }
    }
}

package org.omnetpp.launch.tabs;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.internal.ui.SWTFactory;
import org.eclipse.debug.ui.AbstractLaunchConfigurationTab;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;

import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPlugin;

/**
 * A launch configuration tab that displays and edits omnetpp project
 *
 * @author rhornig
 */
public class OmnetppMainTab extends AbstractLaunchConfigurationTab 
    implements ModifyListener, IChangeListener {

	// UI widgets
	protected Text fProgText;
	protected Button fShowDebugViewButton;

    private final WorkingDirectoryBlock workingDirBlock = new WorkingDirectoryBlock(this);
    private final SimulationTab simulationBlock = new SimulationTab(this, false);
    private boolean updateInProgress = false;


    public OmnetppMainTab() {
        super();
    }

    public void createControl(Composite parent) {
        Composite comp = SWTFactory.createComposite(parent, 1, 1, GridData.FILL_HORIZONTAL);
        workingDirBlock.createControl(comp);

        createExeFileGroup(comp, 1);
        
        simulationBlock.createControl(comp);
        GridLayout ld = (GridLayout)((Composite)simulationBlock.getControl()).getLayout();
        ld.marginWidth = ld.marginHeight = 0;

        // additional options
        createOptionsGroup(comp, 1);
        setControl(comp);
    }

	protected void createExeFileGroup(Composite parent, int colSpan) {
		Composite mainComp =  SWTFactory.createComposite(parent, 3,colSpan,GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)mainComp.getLayout();
        ld.marginHeight = 1;

		SWTFactory.createLabel(mainComp, "Simulation Program:",1);

		fProgText = SWTFactory.createSingleText(mainComp, 1);
		fProgText.addModifyListener(this);

		Button fBrowseForBinaryButton = SWTFactory.createPushButton(mainComp, "Browse...", null);
		fBrowseForBinaryButton.addSelectionListener(new SelectionAdapter() {
			@Override
            public void widgetSelected(SelectionEvent evt) {
				handleBinaryBrowseButtonSelected();
				updateLaunchConfigurationDialog();
			}
		});
	}

    protected void createOptionsGroup(Composite parent, int colSpan) {
        Composite mainComp =  SWTFactory.createComposite(parent, 3,colSpan,GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)mainComp.getLayout();
        ld.marginHeight = 1;

        fShowDebugViewButton = SWTFactory.createCheckButton(mainComp, "Show Debug View on Launch", null, false, 3);
        fShowDebugViewButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                updateLaunchConfigurationDialog();
            }
        });
    }

    public void initializeFrom(ILaunchConfiguration config) {
        try {
            fProgText.setText(config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, ""));
            fShowDebugViewButton.setSelection(config.getAttribute(IOmnetppLaunchConstants.ATTR_SHOWDEBUGVIEW, false));
        } catch (CoreException ce) {
            LaunchPlugin.logError(ce);
        }
        simulationBlock.initializeFrom(config);
        workingDirBlock.setExecutableLocation(fProgText.getText().trim());
        workingDirBlock.initializeFrom(config);
	}

    public void performApply(ILaunchConfigurationWorkingCopy config) {
        config.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, fProgText.getText());
        config.setAttribute(IOmnetppLaunchConstants.ATTR_SHOWDEBUGVIEW, fShowDebugViewButton.getSelection());
        simulationBlock.performApply(config);
        workingDirBlock.performApply(config);
	}

    public void setDefaults(ILaunchConfigurationWorkingCopy configuration) {
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, (String)null);
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, (String)null);
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_SHOWDEBUGVIEW, false);
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
	    setErrorMessage(null);
	    if (!workingDirBlock.isValid(config)) {
	        setErrorMessage(workingDirBlock.getErrorMessage());
	        return false;
	    }

	    String name = fProgText.getText().trim();
	    if (name.length() == 0) {
	        setErrorMessage("Simulation program not specified");
	        return false;
	    }
	    IFile exefile = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(name));
	    if (exefile == null || !exefile.isAccessible()) {
	        setErrorMessage("Simulation program does not exist or not accessible in workspace");
	        return false;
	    }

	    if (!simulationBlock.isValid(config)) {
            setErrorMessage(simulationBlock.getErrorMessage());
            return false;
	    }

        return super.isValid(config);
	}

    @Override
    public Image getImage() {
        return LaunchPlugin.getImage("/icons/full/ctool16/omnetsim.gif");
    }

    public String getName() {
        return "Main";
    }

    /**
     * Show a dialog that lets the user select a project. This in turn provides context for the main
     * type, allowing the user to key a main type name, or constraining the search for main types to
     * the specified project.
     */
    protected void handleBinaryBrowseButtonSelected() {
        ElementTreeSelectionDialog dialog
            = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(),
                                                         new OmnetppLaunchUtils.ExecutableWorkbenchContentProvider());
        dialog.setAllowMultiple(false);
        dialog.setTitle("Select Executable File");
        dialog.setMessage("Select the executable file that should be started.\n");
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID && dialog.getFirstResult() instanceof IFile) {
            String exefile = ((IFile)dialog.getFirstResult()).getFullPath().toString();
            fProgText.setText(exefile);
        }
    }

    public void modifyText(ModifyEvent e) {
        if (e.getSource() == fProgText) {
            workingDirBlock.setExecutableLocation(fProgText.getText().trim());
            simulationBlock.updateNedPathText();
            simulationBlock.updateConfigCombo();
        }
        widgetChanged();
    }
        
    public void widgetChanged() {
        if (!updateInProgress) {
            updateInProgress = true;
            simulationBlock.updateConfigCombo();
            updateLaunchConfigurationDialog();
            updateInProgress = false;
        }
    }
}

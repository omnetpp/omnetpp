package org.omnetpp.cdt.ui;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.dialogs.PropertyPage;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.common.color.ColorFactory;

/**
 * This property page is shown for folders in an OMNeT++ CDT Project, and lets the user 
 * edit the contents of the ".oppbuildspec" file.
 *
 * @author Andras
 */
public class FolderMakemakePropertyPage extends PropertyPage {
    public static final String MAKEFRAG_FILENAME = "makefrag";
    public static final String MAKEFRAGVC_FILENAME = "makefrag.vc";

    // state
    protected BuildSpecification buildSpec;

    // controls
    protected Label errorMessageLabel;
    protected MakemakeOptionsPanel optionsPanel;

    /**
     * Constructor.
     */
    public FolderMakemakePropertyPage() {
        super();
    }

    /**
     * @see PreferencePage#createContents(Composite)
     */
    protected Control createContents(Composite parent) {
        final Composite composite = new Composite(parent, SWT.NONE);
        GridData data = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
        composite.setLayoutData(data);
        GridLayout layout = new GridLayout(1,false);
        layout.marginWidth = layout.marginHeight = 0;
        composite.setLayout(layout);
        
        errorMessageLabel = new Label(composite, SWT.WRAP);
        errorMessageLabel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));
        errorMessageLabel.setForeground(ColorFactory.RED2);
        //errorMessageLabel.setBackground(errorMessageLabel.getDisplay().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND));

        // make the error text label wrap properly; see https://bugs.eclipse.org/bugs/show_bug.cgi?id=9866
        composite.addControlListener(new ControlAdapter(){
            public void controlResized(ControlEvent e){
                GridData data = (GridData)errorMessageLabel.getLayoutData();
                GridLayout layout = (GridLayout)composite.getLayout();
                data.widthHint = composite.getClientArea().width - 2*layout.marginWidth;
                composite.layout(true);
            }
        });

        loadBuildSpecFile();

        IContainer folder = getFolder();
        boolean isMakemakeFolder = buildSpec.getMakemakeFolders().contains(folder);
        if (isMakemakeFolder) {
            createLabel(composite, "Configure makefile generation here. Settings apply to all build configurations.");

            optionsPanel = new MakemakeOptionsPanel(composite, SWT.NONE); 
            optionsPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
            optionsPanel.setOwnerPage(this);
            
            optionsPanel.populate(folder, buildSpec);
        }
        else {
            createLabel(composite, "Makefile generation is not enabled for this folder. You can enable it in the Project Properties dialog.");
        }        
        
        return composite; //???
    }

    protected Label createLabel(Composite composite, String text) {
        Label label = new Label(composite, SWT.NONE);
        label.setText(text);
        label.setLayoutData(new GridData());
        return label;
    }

    @Override
    public void setVisible(boolean visible) {
        super.setVisible(visible);
        updatePageState();
    }

    protected void updatePageState() {
        String message = ProjectMakemakePropertyPage.getDiagnosticMessage(getFolder(), buildSpec);
        errorMessageLabel.setText(message==null ? "" : message);
    }

    /**
     * The resource for which the Properties dialog was brought up.
     */
    protected IContainer getFolder() {
        return (IContainer) getElement().getAdapter(IContainer.class);
    }

    @Override
    public boolean performOk() {
        // note: performApply() delegates here too
        if (optionsPanel != null) {
            IContainer folder = getFolder();
            
            buildSpec.setMakemakeOptions(folder, optionsPanel.getResult());
            saveBuildSpecFile();
            
            try {
                optionsPanel.saveMakefragFiles();
            }
            catch (CoreException e) {
                errorDialog(e.getMessage(), e);
                return false;
            }
        }
        return true;
    }

    protected void loadBuildSpecFile() {
        IProject project = getFolder().getProject();
        try {
            buildSpec = BuildSpecification.readBuildSpecFile(project);
        } 
        catch (CoreException e) {
            errorDialog("Cannot read build specification, reverting page content to the default settings.", e);
        }

        if (buildSpec == null)
            buildSpec = BuildSpecification.createBlank(project);
    }

    protected void saveBuildSpecFile() {
        try {
            buildSpec.save();
        } 
        catch (CoreException e) {
            errorDialog("Cannot store build specification", e);
        }
    } 

    protected void errorDialog(String message, Throwable e) {
        Activator.logError(message, e);
        IStatus status = new Status(IMarker.SEVERITY_ERROR, Activator.PLUGIN_ID, e.getMessage(), e);
        ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", message, status);
    }
}


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
import org.omnetpp.cdt.tmp.MakemakeOptionsDialog;

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
	    contents = new MakemakeOptionsPanel(parent, SWT.NONE, null); //XXX
	    contents.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		loadBuildSpecFile();
		
		return contents;
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


package org.omnetpp.cdt.wizard;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Properties;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.DateTime;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.Scale;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.Spinner;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.omnetpp.cdt.Activator;
import org.omnetpp.common.project.ProjectUtils;

import com.swtworkbench.community.xswt.XSWT;
import com.swtworkbench.community.xswt.XSWTException;

import freemarker.template.Configuration;

/**
 * Project template loaded from a workspace project.
 * @author Andras
 */
public class WorkspaceBasedProjectTemplate extends ProjectTemplate {
	public static final String TEMPLATE_PROPERTIES_FILENAME = "template.properties";
	public static final Image MISSING_IMAGE = ImageDescriptor.getMissingImageDescriptor().createImage();
	
	// property names:
	public static final String NAME = "name"; // template display name
	public static final String DESCRIPTION = "description"; // template description
	public static final String CATEGORY = "category"; // template category (parent tree node)
	public static final String IMAGE = "image"; // template icon name
	public static final String DEPENDENT = "dependent"; // boolean: if true, make project as dependent on this one
	private static final String NONTEMPLATES = "nontemplates"; // list of non-template files: won't get copied over
	private static final String OPTIONALFILES = "optionalfiles"; // list of files to be suppressed if they'd be blank
	
	protected IFolder templateFolder;
	protected Properties properties = new Properties();
	protected Set<IResource> nontemplateResources = new HashSet<IResource>();
	protected Set<IFile> suppressIfBlankFiles = new HashSet<IFile>();
	protected boolean markAsDependentProject;

	public WorkspaceBasedProjectTemplate(IFolder folder) throws CoreException {
		super();
		this.templateFolder = folder;

		try {
			InputStream is = folder.getFile(TEMPLATE_PROPERTIES_FILENAME).getContents();
			properties.load(is);
			is.close();
		} catch (IOException e) {
			throw Activator.wrapIntoCoreException(e);
		}
		
		setName(StringUtils.defaultIfEmpty(properties.getProperty(NAME), folder.getName()));
		setDescription(StringUtils.defaultIfEmpty(properties.getProperty(DESCRIPTION), "Template loaded from " + folder.getFullPath()));
		setCategory(StringUtils.defaultIfEmpty(properties.getProperty(CATEGORY), folder.getProject().getName()));

		// load image
		String imageFileName = properties.getProperty(IMAGE);
		if (imageFileName != null) {
			IFile file = templateFolder.getFile(new Path(imageFileName));
			nontemplateResources.add(file); // do not copy image file to dest project
			IPath locPath = file.getLocation();
			String loc = locPath==null ? "<unknown>" : locPath.toOSString();
			ImageRegistry imageRegistry = Activator.getDefault().getImageRegistry();
			Image image = imageRegistry.get(loc);
			if (image==null) {
				try {
					image = new Image(Display.getDefault(), loc);
					imageRegistry.put(loc, image);
				} catch (SWTException e) {
					Activator.logError("Error loading image for project template in "+templateFolder.getFullPath(), e);
					image = MISSING_IMAGE;
				}
			}
			setImage(image);
		}
		
		// other options
		markAsDependentProject = StringUtils.defaultIfEmpty(properties.getProperty(DEPENDENT), "true").equals("true");

		nontemplateResources.add(folder.getFile(TEMPLATE_PROPERTIES_FILENAME));
		for (String item : StringUtils.defaultString(properties.getProperty(NONTEMPLATES)).split(" +"))
			nontemplateResources.add(folder.getFile(new Path(item)));

		for (String item : StringUtils.defaultString(properties.getProperty(OPTIONALFILES)).split(" +"))
			suppressIfBlankFiles.add(folder.getFile(new Path(item)));
		
		// TODO Properties (or rather, resolved/edited properties) should be available as template vars too
		// TODO probably template var names and property names should be the same (see "templateName" vs "name")
	}

	/**
	 * XSWT-based wizard page.
	 * @author Andras
	 */
	class XSWTWizardPage extends WizardPage {
		protected IFile xswtFile;
		protected Map<String,Control> widgetMap;
		
		public XSWTWizardPage(String name, IFile xswtFile) {
			super(name);
			this.xswtFile = xswtFile;
		}
		
		@SuppressWarnings("unchecked")
		public void createControl(Composite parent) {
			Composite composite = null;
	    	try {
	    		composite = new Composite(parent, SWT.NONE);
	    		composite.setLayout(new GridLayout());
	    		widgetMap = (Map<String,Control>) XSWT.create(composite, xswtFile.getContents()); 
	    		setControl(composite);
	    		
	    		//TODO fill up controls with values from the property file!!! 
	    		
	    	} catch (XSWTException e) {
	    		displayError(parent, composite, e); 
	    	} catch (CoreException e) {
	    		displayError(parent, composite, e); 
			}
		}
		
		public void displayError(final Composite parent, Composite composite, Throwable e) {
			String msg = "Error loading form from "+xswtFile.getFullPath();
			Activator.logError(msg, e);
			composite.dispose();
			
			// create error text widget
    		Text errorText = new Text(parent, SWT.MULTI|SWT.READ_ONLY|SWT.WRAP) {
    			@Override
    			public Point computeSize(int whint, int hhint, boolean changed) {
    				// Prevent text from blowing up the window horizontally.
    				// This solution looks a bit harsh, but others like setting
    				// GridData.widthHint don't work.
    				Point size = super.computeSize(whint, hhint, changed);
    				size.x = parent.getSize().x;
					return size;
    			}
    			@Override
    			protected void checkSubclass() {
    			}
    		};
    		
			errorText.setText(msg + ":\n\n" + StringUtils.defaultIfEmpty(e.getMessage(), e.getClass().getSimpleName()));
			setControl(errorText);
		}
		
		/** The widgets that have id attributes in the form. May return null (if the page was never shown). */
		public Map<String, Control> getWidgetMap() {
			return widgetMap;
		}
	};

	public IWizardPage[] createCustomPages() throws CoreException {
    	// collect page IDs from property file ("page.1", "page.2" etc keys)
    	int[] pageIDs = new int[0];
    	for (Object key : properties.keySet())
    		if (((String)key).matches("page\\.[0-9]+\\.file"))
    			pageIDs = ArrayUtils.add(pageIDs, Integer.parseInt(((String)key).replaceFirst("^page\\.([0-9]+)\\.file$", "$1")));
    	Arrays.sort(pageIDs);
    	
    	// create pages
    	IWizardPage[] result = new IWizardPage[pageIDs.length];
		for (int i=0; i<pageIDs.length; i++) {
			// create page
			int pageID = pageIDs[i];
			String xswtFileName = properties.getProperty("page."+pageID+".file");
			IFile xswtFile = templateFolder.getFile(new Path(xswtFileName));
			if (!xswtFile.exists())
				throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Template file not found: "+xswtFile.getFullPath()));
			result[i] = new XSWTWizardPage(getName()+"#"+pageID, xswtFile);

			// set title and description
			String title = StringUtils.defaultIfEmpty(
					properties.getProperty("page."+pageID+".title"),
					getName());
			String description = StringUtils.defaultIfEmpty(
					properties.getProperty("page."+pageID+".description"),
					"Page "+(i+1)+" of "+pageIDs.length);
			result[i].setTitle(title);
			result[i].setDescription(description);
			
			// do not copy forms into the new project
			nontemplateResources.add(xswtFile);
		}
		
		return result;
    }

	public Map<String, Object> extractVariablesFromPages(IWizardPage[] customPages) {
    	// extract data from the XSWT forms
    	Map<String, Object> result = new HashMap<String, Object>();
		for (IWizardPage page : customPages) {
			Map<String,Control> widgetMap = ((XSWTWizardPage)page).getWidgetMap();
			if (widgetMap != null) {
				for (String widgetName : widgetMap.keySet()) {
					Control control = widgetMap.get(widgetName);
					Object value = getValueFromControl(control);
					result.put(widgetName, value);
				}
			}
		}
		return result;
	}

	protected Object getValueFromControl(Control control) {
		if (control instanceof Button)
			return ((Button) control).getSelection(); // -> Boolean
		if (control instanceof CCombo)
			return ((CCombo) control).getText();
		if (control instanceof Combo)
			return ((Combo) control).getText();
		if (control instanceof DateTime) {
			DateTime d = (DateTime) control;
			return d.getYear()+"-"+d.getMonth()+"-"+d.getDay()+" "+d.getHours()+":"+d.getMinutes()+":"+d.getSeconds();
		}
		if (control instanceof Label)
			return ((Label) control).getText(); // not very useful
		if (control instanceof List)
			return ((List) control).getSelection(); // -> String[] 
		if (control instanceof Link)
			return ((Link) control).getText(); // not very useful 
		if (control instanceof Scale)
			return ((Scale) control).getSelection(); // -> Integer 
		if (control instanceof Slider)
			return ((Slider) control).getSelection(); // -> Integer 
		if (control instanceof Spinner)
			return ((Spinner) control).getSelection(); // -> Integer 
		if (control instanceof StyledText)
			return ((StyledText) control).getText(); 
		if (control instanceof Text)
			return ((Text) control).getText();
		if (control instanceof Table) {
			Table t = (Table)control;
			int rows = t.getItemCount();
			int cols = t.getColumnCount();
			if (cols == 1) {
				String[] data = new String[rows];
				TableItem[] items = t.getItems();
				for (int i=0; i<rows; i++)
					data[i] = items[i].getText();
				return data; // -> String[]
			}
			else {
				String[][] data = new String[rows][cols];
				TableItem[] items = t.getItems();
				for (int i=0; i<rows; i++)
					for (int j=0; j<cols; j++)
						data[i][j] = items[i].getText(j);
				return data;  // -> String[][]
			}
		}
		if (control instanceof Tree) {
			Tree t = (Tree)control;
			int cols = t.getColumnCount();
			if (cols == 1) {
				return null; //TODO
			}
			else {
				return null;  //TODO
			}
		}
		//TODO implement some adapterproviderfactoryvisitorproxy so that custom widgets can be supported
		return null;
	}

	@Override
	protected void doConfigure() throws CoreException {
		IProject project = getProject();
		substituteNestedVariables();
		if (markAsDependentProject)
			ProjectUtils.addReferencedProject(project, templateFolder.getProject(), getProgressMonitor());
		copy(templateFolder, project);
	}

	protected void copy(IFolder parent, IContainer dest) throws CoreException {
		for (IResource resource : parent.members()) {
			if (!nontemplateResources.contains(resource)) {
				if (resource instanceof IFile) {
					// copy w/ template substitution
					IFile file = (IFile)resource;
					IFile destFile = dest.getFile(new Path(file.getName()));
					createFileFromWorkspaceResource(destFile, file, suppressIfBlankFiles.contains(file));
				}
				else if (resource instanceof IFolder) {
					// create
					IFolder folder = (IFolder)resource;
					IFolder newFolder = dest.getFolder(new Path(folder.getName()));
					if (!newFolder.exists())
						newFolder.create(true, true, getProgressMonitor());
					copy(folder, newFolder);
				}
			}
		}
	}

    protected void createFileFromWorkspaceResource(IFile file, IFile templateFile, boolean suppressIfBlank) throws CoreException {
        Configuration cfg = new Configuration();
        cfg.setTemplateLoader(new WorkspaceTemplateLoader(templateFolder));
		String templateName = templateFile.getFullPath().removeFirstSegments(templateFolder.getFullPath().segmentCount()).toString();
        createFile(file, cfg, templateName, suppressIfBlank);
    }

}

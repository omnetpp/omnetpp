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
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.cdt.Activator;
import org.omnetpp.common.json.ExceptionErrorListener;
import org.omnetpp.common.json.JSONValidatingReader;
import org.omnetpp.common.project.ProjectUtils;

import com.swtworkbench.community.xswt.XSWT;

import freemarker.template.Configuration;

/**
 * Project template loaded from a workspace project.
 * @author Andras
 */
//FIXME: freemarker logging currently goes to stdout
public class WorkspaceBasedProjectTemplate extends ProjectTemplate {
	public static final String TEMPLATE_PROPERTIES_FILENAME = "template.properties";
	public static final Image MISSING_IMAGE = ImageDescriptor.getMissingImageDescriptor().createImage();
	
	// property names:
	public static final String PROP_TEMPLATENAME = "templateName"; // template display name
	public static final String PROP_TEMPLATEDESCRIPTION = "templateDescription"; // template description
	public static final String PROP_TEMPLATECATEGORY = "templateCategory"; // template category (parent tree node)
	public static final String PROP_TEMPLATEIMAGE = "templateImage"; // template icon name
	public static final String PROP_ADDPROJECTREFERENCE = "addProjectReference"; // boolean: if true, make project as dependent on this one
	public static final String PROP_IGNORABLERESOURCES = "ignorableResources"; // list of non-template files: won't get copied over
	public static final String PROP_OPTIONALFILES = "optionalFiles"; // list of files to be suppressed if they'd be blank
	public static final String PROP_SOURCEFOLDERS = "sourceFolders"; // source folders to be created and configured
	public static final String PROP_NEDSOURCEFOLDERS = "nedSourceFolders"; // NED source folders to be created and configured
	public static final String PROP_MAKEMAKEOPTIONS = "makemakeOptions"; // makemake options, as "folder1:options1,folder2:options2,..."

	protected IFolder templateFolder;
	protected Properties properties = new Properties();
	protected Set<IResource> ignorableResources = new HashSet<IResource>();
	protected Set<IFile> optionalFiles = new HashSet<IFile>(); // files not to be generated if they'd be blank

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
		
		setName(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATENAME), folder.getName()));
		setDescription(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATEDESCRIPTION), "Template loaded from " + folder.getFullPath()));
		setCategory(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATECATEGORY), folder.getProject().getName()));

		// load image
		String imageFileName = properties.getProperty(PROP_TEMPLATEIMAGE);
		if (imageFileName != null) {
			IFile file = templateFolder.getFile(new Path(imageFileName));
			ignorableResources.add(file); // do not copy image file to dest project
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

		// the following options may not be modified via the wizard, so they are initialized here
		ignorableResources.add(folder.getFile(TEMPLATE_PROPERTIES_FILENAME));
		for (String item : SWTDataUtil.toStringArray(StringUtils.defaultString(properties.getProperty(PROP_IGNORABLERESOURCES))," +"))
			ignorableResources.add(folder.getFile(new Path(item)));

		for (String item : SWTDataUtil.toStringArray(StringUtils.defaultString(properties.getProperty(PROP_OPTIONALFILES))," +"))
			optionalFiles.add(folder.getFile(new Path(item)));
	}

	@Override
	public CreationContext createContext(IProject project) {
		CreationContext context = super.createContext(project);

		// default values of recognized options (will be overwritten from property file)
		context.getVariables().put(PROP_ADDPROJECTREFERENCE, "true");
		context.getVariables().put(PROP_IGNORABLERESOURCES, "");
		context.getVariables().put(PROP_OPTIONALFILES, "");
		context.getVariables().put(PROP_SOURCEFOLDERS, "");
		context.getVariables().put(PROP_NEDSOURCEFOLDERS, "");
		context.getVariables().put(PROP_MAKEMAKEOPTIONS, "");
		
		// add property file entries as template variables
		for (Object key : properties.keySet()) {
			Object value = properties.get(key);
			Assert.isTrue(key instanceof String && value instanceof String);
				context.getVariables().put((String)key, parseJSON((String)value));
		}
		
		// add more predefined variables (these ones cannot be overwritten from the property file, would make no sense)
		context.getVariables().put("templateFolderName", templateFolder.getName());
		context.getVariables().put("templateFolderPath", templateFolder.getFullPath().toString());
		context.getVariables().put("templateProject", templateFolder.getProject().getName());

		return context;
	}
	
	/**
	 * XSWT-based wizard page.
	 * @author Andras
	 */
	class XSWTWizardPage extends WizardPage implements ICustomWizardPage {
		protected IFile xswtFile;
		protected String condition; // FreeMarker expr, use as isEnabled() condition
		protected Map<String,Control> widgetMap;
		protected CreationContext context; // to transfer context from populatePage() to createControl()
		
		public XSWTWizardPage(String name, IFile xswtFile, String condition) {
			super(name);
			this.xswtFile = xswtFile;
			this.condition = condition;
		}
		
		@SuppressWarnings("unchecked")
		public void createControl(Composite parent) {
			Composite composite = null;
	    	try {
	    		// instantiate XSWT form
	    		composite = new Composite(parent, SWT.NONE);
	    		composite.setLayout(new GridLayout());
	    		widgetMap = (Map<String,Control>) XSWT.create(composite, xswtFile.getContents()); 
	    		setControl(composite);
	    		
	    		if (context != null) {
	    			// do deferred populatePage() call
	    			populatePage(context);
	    			context = null; // to be safe
	    		}
	    		
	    	} catch (Exception e) {
	    		widgetMap = new HashMap<String, Control>(); // fake it
	    		displayError(parent, composite, e); 
			}
		}
		
		public void displayError(final Composite parent, Composite composite, Exception e) {
			String msg = "Error creating form from "+xswtFile.getFullPath();
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
    		
			errorText.setText(msg + ":\n\n" + e.getClass().getSimpleName()+": "+e.getMessage());
			setControl(errorText);
		}
		
		public void populatePage(CreationContext context) {
			if (widgetMap == null) {
				// call too early -- defer it to createControl()
				this.context = context; 
			}
			else {
				// fill up controls with values from the context
				for (String key : widgetMap.keySet()) {
					Control control = widgetMap.get(key);
					Object value = context.getVariables().get(key);
					if (value != null) {
						try {
							SWTDataUtil.writeValueIntoControl(control, value);
						}
						catch (Exception e) {
							// NumberFormatException, ParseException (for date/time), something like that
							String message = "Cannot put value '"+value+"' into "+control.getClass().getSimpleName()+" control "+key;
							MessageDialog.openError(getShell(), "Error", "Wizard page: "+message);
							Activator.logError(message, e);
						}
					}
				}
			}
		}

		public void extractPageContent(CreationContext context) {
			// extract data from the XSWT form
			Assert.isNotNull(widgetMap);
			for (String widgetName : widgetMap.keySet()) {
				Control control = widgetMap.get(widgetName);
				Object value = SWTDataUtil.getValueFromControl(control);
				context.getVariables().put(widgetName, value);
			}
		}

		public boolean isEnabled(CreationContext context) {
			if (StringUtils.isBlank(condition))
				return true;
			
			// evaluate as FreeMarker expression
			try {
				String macro = "${(" + condition + ")?string}";
				String result = evaluate(macro, context);
				return result.equals("true");
			} 
			catch (Exception e) {
				String message = "Cannot evaluate condition '"+condition+"' for page "+getName();
				MessageDialog.openError(getShell(), "Error", "Wizard: "+message+": " + e.getMessage());
				Activator.logError(message, e);
				return true;
			}
		}

		@Override
		public IWizardPage getPreviousPage() {
			if (getWizard() != null)
				getWizard().getPreviousPage(this); // as required by ICustomWizardPage
			return super.getPreviousPage();
		}
	};

	public ICustomWizardPage[] createCustomPages() throws CoreException {
    	// collect page IDs from property file ("page.1", "page.2" etc keys)
    	int[] pageIDs = new int[0];
    	for (Object key : properties.keySet())
    		if (((String)key).matches("page\\.[0-9]+\\.file"))
    			pageIDs = ArrayUtils.add(pageIDs, Integer.parseInt(((String)key).replaceFirst("^page\\.([0-9]+)\\.file$", "$1")));
    	Arrays.sort(pageIDs);
    	
    	// create pages
    	ICustomWizardPage[] result = new ICustomWizardPage[pageIDs.length];
		for (int i=0; i<pageIDs.length; i++) {
			// create page
			int pageID = pageIDs[i];
			String xswtFileName = properties.getProperty("page."+pageID+".file");
			String condition = properties.getProperty("page."+pageID+".condition");
			IFile xswtFile = templateFolder.getFile(new Path(xswtFileName));
			if (!xswtFile.exists())
				throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Template file not found: "+xswtFile.getFullPath()));
			result[i] = new XSWTWizardPage(getName()+"#"+pageID, xswtFile, condition);

			// set title and description
			String title = StringUtils.defaultIfEmpty(properties.getProperty("page."+pageID+".title"), getName());
			String description = StringUtils.defaultIfEmpty(properties.getProperty("page."+pageID+".description"),"Select options below"); // "1 of 5" not good as default, because of conditional pages
			result[i].setTitle(title);
			result[i].setDescription(description);
			
			// do not copy forms into the new project
			ignorableResources.add(xswtFile);
		}
		
		return result;
    }

	/**
	 * Parse text as JSON. Returns Boolean, String, Number, List or Map. If the
	 * text does not look like JSON, treats it as an unquoted literal string, and
	 * returns it as String.
	 *  
	 * @throws IllegalArgumentException on JSON parse error.
	 */
	public static Object parseJSON(String text) {
		String numberRegex = "\\s*[+-]?[0-9.]+([eE][+-]?[0-9]+)?\\s*"; // sort of
		if (text.equals("true") || text.equals("false") || text.matches(numberRegex) || 
				text.trim().startsWith("[") || text.trim().startsWith("{")) {
			// looks like JSON -- parse as such
			JSONValidatingReader reader = new JSONValidatingReader(new ExceptionErrorListener());
			return reader.read(text); // throws IllegalArgumentException on parse errors
		} 
		else {
			// apparently not JSON -- take it as a literal string with missing quotes
			return text.trim();
		}
	}
	
	@Override
	protected void doConfigure(CreationContext context) throws CoreException {
		substituteNestedVariables(context);

		if (SWTDataUtil.toBoolean(context.getVariables().get(PROP_ADDPROJECTREFERENCE)))
			ProjectUtils.addReferencedProject(context.getProject(), templateFolder.getProject(), context.getProgressMonitor());
		
		String[] srcFolders = SWTDataUtil.toStringArray(context.getVariables().get(PROP_SOURCEFOLDERS), " +");
		if (srcFolders.length > 0)
			createAndSetSourceFolders(srcFolders, context);

		String[] nedSrcFolders = SWTDataUtil.toStringArray(context.getVariables().get(PROP_NEDSOURCEFOLDERS), " +");
		if (nedSrcFolders.length > 0)
			createAndSetNedSourceFolders(nedSrcFolders, context);

		Map<String,String> makemakeOptions = SWTDataUtil.toStringMap(context.getVariables().get(PROP_MAKEMAKEOPTIONS));
		if (!makemakeOptions.isEmpty())
			createBuildSpec(makemakeOptions, context);

	    // copy over files and folders, with template substitution
		copy(templateFolder, context.getProject(), context);
	}
	
	protected void copy(IFolder folder, IContainer destFolder, CreationContext context) throws CoreException {
		for (IResource resource : folder.members()) {
			if (!ignorableResources.contains(resource)) {
				if (resource instanceof IFile) {
					IFile file = (IFile)resource;
					IPath relativePath = file.getFullPath().removeFirstSegments(templateFolder.getFullPath().segmentCount());
					if (file.getFileExtension().equals("ftl")) {
						// copy it with template substitution
						IFile newFile = destFolder.getFile(relativePath.removeFileExtension());
						createFileFromWorkspaceResource(newFile, relativePath.toString(), optionalFiles.contains(file), context);
					}
					else {
						// copy it verbatim
						IFile newFile = destFolder.getFile(relativePath);
						if (newFile.exists())
							newFile.delete(true, context.getProgressMonitor());
						file.copy(newFile.getFullPath(), true, context.getProgressMonitor());
					}
				}
				else if (resource instanceof IFolder) {
					// create
					IFolder subfolder = (IFolder)resource;
					IFolder newSubfolder = destFolder.getFolder(new Path(subfolder.getName()));
					if (!newSubfolder.exists())
						newSubfolder.create(true, true, context.getProgressMonitor());
					copy(subfolder, newSubfolder, context);
				}
			}
		}
	}

    protected void createFileFromWorkspaceResource(IFile file, String templateName, boolean suppressIfBlank, CreationContext context) throws CoreException {
        Configuration cfg = new Configuration();
        cfg.setTemplateLoader(new WorkspaceTemplateLoader(templateFolder));
		createFile(file, cfg, templateName, suppressIfBlank, context);
    }

}

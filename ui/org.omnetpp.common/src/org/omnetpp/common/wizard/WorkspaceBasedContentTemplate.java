package org.omnetpp.common.wizard;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Properties;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.json.ExceptionErrorListener;
import org.omnetpp.common.json.JSONValidatingReader;


import freemarker.template.Configuration;

/**
 * Project template loaded from a workspace project.
 * @author Andras
 */
//FIXME: freemarker logging currently goes to stdout
//XXX removed properties: PROP_SOURCEFOLDERS = "addProjectReference", "sourceFolders", "nedSourceFolders", "makemakeOptions"
public class WorkspaceBasedContentTemplate extends ContentTemplate {
	public static final String TEMPLATE_PROPERTIES_FILENAME = "template.properties";
	public static final Image MISSING_IMAGE = ImageDescriptor.getMissingImageDescriptor().createImage();
	
	// property names:
	public static final String PROP_TEMPLATENAME = "templateName"; // template display name
	public static final String PROP_TEMPLATEDESCRIPTION = "templateDescription"; // template description
	public static final String PROP_TEMPLATECATEGORY = "templateCategory"; // template category (parent tree node)
	public static final String PROP_TEMPLATEIMAGE = "templateImage"; // template icon name
	public static final String PROP_IGNORERESOURCES = "ignoreResources"; // list of non-template files: won't get copied over
	public static final String PROP_OPTIONALFILES = "optionalFiles"; // list of files to be suppressed if they'd be blank

	protected IFolder templateFolder;
	protected Properties properties = new Properties();
	protected Set<IResource> ignoreResources = new HashSet<IResource>();
	protected Set<IFile> optionalFiles = new HashSet<IFile>(); // files not to be generated if they'd be blank

	public WorkspaceBasedContentTemplate(IFolder folder) throws CoreException {
		super();
		this.templateFolder = folder;

		try {
			InputStream is = folder.getFile(TEMPLATE_PROPERTIES_FILENAME).getContents();
			properties.load(is);
			is.close();
		} catch (IOException e) {
			throw CommonPlugin.wrapIntoCoreException(e);
		}
		
		setName(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATENAME), folder.getName()));
		setDescription(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATEDESCRIPTION), "Template loaded from " + folder.getFullPath()));
		setCategory(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATECATEGORY), folder.getProject().getName()));

		// load image
		String imageFileName = properties.getProperty(PROP_TEMPLATEIMAGE);
		if (imageFileName != null) {
			IFile file = templateFolder.getFile(new Path(imageFileName));
			ignoreResources.add(file); // do not copy image file to dest project
			IPath locPath = file.getLocation();
			String loc = locPath==null ? "<unknown>" : locPath.toOSString();
			ImageRegistry imageRegistry = CommonPlugin.getDefault().getImageRegistry();
			Image image = imageRegistry.get(loc);
			if (image==null) {
				try {
					image = new Image(Display.getDefault(), loc);
					imageRegistry.put(loc, image);
				} catch (SWTException e) {
					CommonPlugin.logError("Error loading image for project template in "+templateFolder.getFullPath(), e);
					image = MISSING_IMAGE;
				}
			}
			setImage(image);
		}

		// the following options may not be modified via the wizard, so they are initialized here
		ignoreResources.add(folder.getFile(TEMPLATE_PROPERTIES_FILENAME));
		for (String item : SWTDataUtil.toStringArray(StringUtils.defaultString(properties.getProperty(PROP_IGNORERESOURCES))," *, *"))
			ignoreResources.add(folder.getFile(new Path(item)));

		for (String item : SWTDataUtil.toStringArray(StringUtils.defaultString(properties.getProperty(PROP_OPTIONALFILES))," *, *"))
			optionalFiles.add(folder.getFile(new Path(item)));
	}

	@Override
	public CreationContext createContext(IContainer folder) {
		CreationContext context = super.createContext(folder);

		// default values of recognized options (will be overwritten from property file)
		context.getVariables().put(PROP_IGNORERESOURCES, "");
		context.getVariables().put(PROP_OPTIONALFILES, "");
		
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
	 * Overridden so that we can load JAR files from the template folder.
	 */
	@Override
	protected ClassLoader createClassLoader() {
	    List<URL> urls = new ArrayList<URL>();
	    try {
	        for (IResource resource : templateFolder.members())
	            if (resource instanceof IFile && resource.getFileExtension().equals("jar"))
	                urls.add(new URL("file", "", resource.getLocation().toPortableString()));
	    } 
	    catch (Exception e) {
	        CommonPlugin.logError("Error assembling classpath for loading jars from the workspace", e);
	    }
	    return new URLClassLoader(urls.toArray(new URL[]{}), getClass().getClassLoader());
	}
	
	public ICustomWizardPage[] createCustomPages() throws CoreException {
    	// collect page IDs from property file ("page.1", "page.2" etc keys)
    	int[] pageIDs = new int[0];
    	for (Object key : properties.keySet())
    		if (((String)key).matches("page\\.[0-9]+\\.file"))
    			pageIDs = ArrayUtils.add(pageIDs, Integer.parseInt(((String)key).replaceFirst("^page\\.([0-9]+)\\.file$", "$1")));
    	Arrays.sort(pageIDs);
    	
    	// create pages
    	ICustomWizardPage[] result = new ICustomWizardPage[pageIDs.length];
		for (int i = 0; i < pageIDs.length; i++) {
			// create page
			int pageID = pageIDs[i];
			String xswtFileName = properties.getProperty("page."+pageID+".file");
			String condition = properties.getProperty("page."+pageID+".condition");
			IFile xswtFile = templateFolder.getFile(new Path(xswtFileName));
			if (!xswtFile.exists())
				throw new CoreException(new Status(IStatus.ERROR, CommonPlugin.PLUGIN_ID, "Template file not found: "+xswtFile.getFullPath()));
			try {
                result[i] = new XSWTWizardPage(this, getName()+"#"+pageID, xswtFile, condition);
            }
            catch (IOException e) {
                throw new CoreException(new Status(IStatus.ERROR, CommonPlugin.PLUGIN_ID, "Error loading template file "+xswtFile.getFullPath()));
            }

			// set title and description
			String title = StringUtils.defaultIfEmpty(properties.getProperty("page."+pageID+".title"), getName());
			String description = StringUtils.defaultIfEmpty(properties.getProperty("page."+pageID+".description"),"Select options below"); // "1 of 5" not good as default, because of conditional pages
			result[i].setTitle(title);
			result[i].setDescription(description);
			
			// do not copy forms into the new project
			ignoreResources.add(xswtFile);
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
	protected void doPerformFinish(CreationContext context) throws CoreException {
		substituteNestedVariables(context);

		// copy over files and folders, with template substitution
		copy(templateFolder, context.getFolder(), context); 
	}
	
	protected void copy(IFolder folder, IContainer destFolder, CreationContext context) throws CoreException {
		for (IResource resource : folder.members()) {
			if (!ignoreResources.contains(resource)) {
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

package org.omnetpp.common.wizard;

import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
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
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Plugin;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.json.ExceptionErrorListener;
import org.omnetpp.common.json.JSONValidatingReader;
import org.omnetpp.common.util.FileUtils;

import freemarker.cache.MultiTemplateLoader;
import freemarker.cache.TemplateLoader;
import freemarker.cache.URLTemplateLoader;
import freemarker.template.Configuration;

/**
 * Project template loaded from a workspace project or a resource bundle.
 * @author Andras
 */
public class FileBasedContentTemplate extends ContentTemplate {
	public static final String TEMPLATE_PROPERTIES_FILENAME = "template.properties";
	public static final String FILELIST_FILENAME = "filelist.txt"; // for URL loading
	public static final Image MISSING_IMAGE = ImageDescriptor.getMissingImageDescriptor().createImage();
	
	// property names:
	public static final String PROP_TEMPLATENAME = "templateName"; // template display name
	public static final String PROP_SUPPORTEDWIZARDTYPES = "supportedWizardTypes"; // list of items: "project", "simulation", "nedfile", "inifile", "network", etc
	public static final String PROP_TEMPLATEDESCRIPTION = "templateDescription"; // template description
	public static final String PROP_TEMPLATECATEGORY = "templateCategory"; // template category (parent tree node)
	public static final String PROP_TEMPLATEIMAGE = "templateImage"; // template icon name
	public static final String PROP_IGNORERESOURCES = "ignoreResources"; // list of files NOT top copy into dest folder; basic glob patterns accepted
	public static final String PROP_VERBATIMFILES = "verbatimFiles"; // list of files to copy verbatim, even if they would be ignored otherwise; basic glob patterns accepted

	// template is either given with an IFolder or with an URL
	protected IFolder templateFolder;
	protected URL templateUrl;
	protected String[] fileList;
	protected Properties properties = new Properties();
	protected Set<String> supportedWizardTypes = new HashSet<String>();
	protected List<String> ignoreResourcePatterns = new ArrayList<String>();
	protected List<String> verbatimFilePatterns = new ArrayList<String>();
    private boolean imageAlreadyLoaded = false;

    /**
     * Currently only tests whether the given folder contains a template.properties file.
     */
    public static boolean looksLikeTemplateFolder(IFolder folder) {
        return folder.getFile(TEMPLATE_PROPERTIES_FILENAME).exists();
    }

    /**
     * Loads the template from the given subdirectory of the given plugin's folder.
     * The template loaded this way will be treated as a template loaded from an URL
     * (see constructor taking an URL).
     */
    public FileBasedContentTemplate(Plugin plugin, String templateFolder) throws CoreException {
        this(plugin.getBundle().getResource(templateFolder));
        
        // generate a better default description
        setDescription(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATEDESCRIPTION), "Template loaded from plugin " + plugin.getBundle().getSymbolicName()));
    }

    /**
     * Loads the template from the given URL. The URL should point to the folder 
     * which contains the template.properties file. The folder should also contain a
     * filelist.txt, containing the list of the files in the template folder, one per line.
     * This is needed to overcome the Java limitation that contents of a resource bundle 
     * (or URL) cannot be enumerated.  
     */
    public FileBasedContentTemplate(URL templateUrl) throws CoreException {
        super();
        this.templateUrl = templateUrl;

        properties = loadPropertiesFrom(openFile(TEMPLATE_PROPERTIES_FILENAME));

        // note: image will be loaded lazily, in getImage()
        setName(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATENAME), StringUtils.substringAfterLast(templateUrl.getPath(), "/")));
        setDescription(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATEDESCRIPTION), "Template loaded from " + templateUrl.toString()));
        setCategory(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATECATEGORY), null));

        // other initializations
        init();
    }

    /**
     * Loads the template from the given folder. This is a relatively cheap operation
     * (only the template.properties file is read), so it is OK for the wizard to 
     * instantiate WorkspaceBasedContentTemplate for each candidate folder just to determine
     * whether it should be offered to the user.
     */
	public FileBasedContentTemplate(IFolder folder) throws CoreException {
		super();
		this.templateFolder = folder;

        properties = loadPropertiesFrom(openFile(TEMPLATE_PROPERTIES_FILENAME));

		// note: image will be loaded lazily, in getImage()
		setName(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATENAME), folder.getName()));
		setDescription(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATEDESCRIPTION), "Template loaded from " + folder.getFullPath()));
		setCategory(StringUtils.defaultIfEmpty(properties.getProperty(PROP_TEMPLATECATEGORY), folder.getProject().getName()));

		init();
	}

    protected void init() {
        ignoreResourcePatterns.add("**/*.xswt");
		ignoreResourcePatterns.add("**/*.fti");  // note: "*.ftl" is NOT to be added! (or they'd be skipped altogether)
		ignoreResourcePatterns.add("**/*.jar");
		ignoreResourcePatterns.add(TEMPLATE_PROPERTIES_FILENAME);
		ignoreResourcePatterns.add(FILELIST_FILENAME);

		// the following options may not be modified via the wizard, so they are initialized here
        String[] labels = SWTDataUtil.toStringArray(StringUtils.defaultString(properties.getProperty(PROP_SUPPORTEDWIZARDTYPES))," *, *");
        supportedWizardTypes.addAll(Arrays.asList(labels));
		
		for (String item : SWTDataUtil.toStringArray(StringUtils.defaultString(properties.getProperty(PROP_IGNORERESOURCES))," *, *"))
		    ignoreResourcePatterns.add(item);

		for (String item : SWTDataUtil.toStringArray(StringUtils.defaultString(properties.getProperty(PROP_VERBATIMFILES))," *, *"))
		    verbatimFilePatterns.add(item);
    }

    /**
     * Returns the workspace folder from which the template was loaded. Returns null
     * if the template was loaded from an URL. 
     */
    public IFolder getTemplateFolder() {
        return templateFolder;
    }

    /**
     * Returns the URL folder from which the template was loaded. Returns null
     * if the template was loaded from a workspace folder. 
     */
    public URL getTemplateUrl() {
        return templateUrl;
    }
    
    /**
     * Returns the values in the supportedWizardTypes property file entry.
     */
    public Set<String> getSupportedWizardTypes() {
        return supportedWizardTypes;
    }

    /**
     * Overridden to provide lazy loading.
     */
    @Override
	public Image getImage() {
	    if (!imageAlreadyLoaded) {
	        imageAlreadyLoaded = true;
	        String imageFileName = properties.getProperty(PROP_TEMPLATEIMAGE);
	        if (imageFileName != null) {
	            ignoreResourcePatterns.add(imageFileName);
	            try {
	                ImageRegistry imageRegistry = CommonPlugin.getDefault().getImageRegistry();
	                String key = asURL(imageFileName).toString();
	                Image image = imageRegistry.get(key);
	                if (image == null)
	                    imageRegistry.put(key, image = new Image(Display.getDefault(), openFile(imageFileName)));
	                setImage(image);
	            } 
	            catch (Exception e) {
	                CommonPlugin.logError("Error loading image for content template " + getName(), e);
	                setImage(MISSING_IMAGE);
	            }
	        }
	    }
        return super.getImage();
	}

    private static Properties loadPropertiesFrom(InputStream is) throws CoreException {
        try {
            Properties result = new Properties();
			result.load(is);
			is.close();
			return result;
		} 
        catch (IOException e) {
		    try { is.close(); } catch (IOException e2) { }
			throw CommonPlugin.wrapIntoCoreException(e);
		}
    }

    /**
     * Overridden to add new variables into the context.
     */
	@Override
	public CreationContext createContext(IContainer folder) {
		CreationContext context = super.createContext(folder);

		// default values for recognized options (will be overwritten from property file)
		context.getVariables().put(PROP_IGNORERESOURCES, "");
		
		// add property file entries as template variables
		for (Object key : properties.keySet()) {
			Object value = properties.get(key);
			Assert.isTrue(key instanceof String && value instanceof String);
				context.getVariables().put((String)key, parseJSON((String)value));
		}
		
		// add more predefined variables (these ones cannot be overwritten from the property file, would make no sense)
		if (templateUrl != null) {
		    context.getVariables().put("templateURL", templateUrl);
		}
		if (templateFolder != null) {
		    context.getVariables().put("templateFolderName", templateFolder.getName());
		    context.getVariables().put("templateFolderPath", templateFolder.getFullPath().toString());
		    context.getVariables().put("templateProject", templateFolder.getProject().getName());
		}
		return context;
	}
	
	/**
	 * Overridden so that we can load JAR files from the template folder
	 */
	//TODO the project's "plugins" folder
	@Override
	protected ClassLoader createClassLoader() {
	    try {
	        List<URL> urls = new ArrayList<URL>();
	        for (String fileName : getFileList())
                if (fileName.endsWith(".jar"))
                    urls.add(asURL(fileName));
	        return new URLClassLoader(urls.toArray(new URL[]{}), getClass().getClassLoader());
	    } 
	    catch (Exception e) {
	        CommonPlugin.logError("Error assembling classpath for loading jars from the workspace", e);
	        return getClass().getClassLoader();
	    }
	}
	
    static class URLTemplateLoader2 extends URLTemplateLoader {
	    private URL baseUrl;

	    public URLTemplateLoader2(URL baseUrl) {
	        this.baseUrl = baseUrl;
	    }

	    public Object findTemplateSource(String name) throws IOException {
	        // WORKAROUND: When Freemarker tries the file with local suffix 
	        // ("en_US" etc), the file won't exist, and this method throws a 
	        // FileNotFoundException from the URLTemplateSource constructor.
	        // That's exactly what the method documentation says NOT to do,
	        // as it aborts template processing. Solution: We probe whether 
	        // the file exists, and only proceed to the original implementation
	        // if it does.
	        try {
	            URL url = getURL(name);
	            InputStream stream = url.openStream();
	            try { stream.close(); } catch (IOException e) { }
	        } catch (IOException e) {
	            return null;
	        }
	        return super.findTemplateSource(name);
	    }
	    
	    @Override
	    protected URL getURL(String name) {
	        try {
                return new URL(baseUrl.toString() + "/" + name);
            }
            catch (MalformedURLException e) {
                throw new IllegalArgumentException("Illegal template name: " + name, e);
            }
	    }
	}
	
	@Override
	protected Configuration createFreemarkerConfiguration() {
	    // add workspace template loader
	    Configuration cfg = super.createFreemarkerConfiguration();
	    cfg.setTemplateLoader(new MultiTemplateLoader( new TemplateLoader[] { 
	            cfg.getTemplateLoader(), 
	            templateUrl!=null ? new URLTemplateLoader2(templateUrl) : new WorkspaceTemplateLoader(templateFolder)
	    }));
	    return cfg;
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
			try {
                result[i] = new XSWTWizardPage(this, getName()+"#"+pageID, openFile(xswtFileName), xswtFileName, condition);
            }
            catch (IOException e) {
                throw new CoreException(new Status(IStatus.ERROR, CommonPlugin.PLUGIN_ID, "Error loading template file "+xswtFileName, e));
            }

			// set title and description
			String title = StringUtils.defaultIfEmpty(properties.getProperty("page."+pageID+".title"), getName());
			String description = StringUtils.defaultIfEmpty(properties.getProperty("page."+pageID+".description"),"Select options below"); // "1 of 5" not good as default, because of conditional pages
			result[i].setTitle(title);
			result[i].setDescription(description);
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
	
	public void performFinish(CreationContext context) throws CoreException {
		substituteNestedVariables(context);

		// copy over files and folders, with template substitution
		copyFiles(context);
	}
	
	/**
	 * Instantiate the template given with an URL: copy files and folders given 
	 * in file list to the destination folder specified in the context.
	 */
	protected void copyFiles(CreationContext context) throws CoreException {
	    String[] fileList = getFileList();
	    for (String fileName : fileList) {
	        if (fileName.endsWith("/")) {
	            createFolder(fileName, context);
	        }
	        else {
	            boolean isFtlFile = new Path(fileName).getFileExtension().equals("ftl");
	            InputStream inputStream = openFile(fileName);
	            if (matchesAny(fileName.toString(), verbatimFilePatterns) || (!isFtlFile && !matchesAny(fileName.toString(), ignoreResourcePatterns)))
	                createVerbatimFile(fileName, inputStream, context);
	            else if (isFtlFile && !matchesAny(fileName.toString(), ignoreResourcePatterns))
	                createTemplateFile(fileName.replaceFirst("\\.ftl$", ""), getFreemarkerConfiguration(), fileName.toString(), context);
	        }
	    }
	}

	/**
	 * Exists to abstract out the difference between workspace loading (IFile) 
	 * and URL (or resource bundle) based loading. 
	 */
    protected URL asURL(String fileName) throws CoreException {
        try {
            if (templateUrl != null)
                return new URL(templateUrl.toString() + "/" + fileName);
            else
                return new URL("file", "", templateFolder.getFile(new Path(fileName)).getLocation().toPortableString());
        }
        catch (MalformedURLException e) {
            throw CommonPlugin.wrapIntoCoreException("Cannot make URL for file " + fileName, e);
        }
    }

    /**
     * Exists to abstract out the difference between workspace loading (IFile) 
     * and URL (or resource bundle) based loading. 
     */
    protected InputStream openFile(String fileName) throws CoreException {
        if (templateUrl != null) {
            try {
                return new URL(templateUrl.toString() + "/" + fileName).openStream();
            }
            catch (IOException e) {
                throw CommonPlugin.wrapIntoCoreException("Cannot read file " + fileName, e);
            }
        }
        else { 
            return templateFolder.getFile(new Path(fileName)).getContents();
        }
    }
    
    /**
     * Exists to abstract out the difference between workspace loading (IFile) 
     * and URL (or resource bundle) based loading. 
     */
    protected String[] getFileList() throws CoreException {
        if (fileList == null) {
            if (templateUrl != null) {
                try {
                    String filelistTxt = FileUtils.readTextFile(openFile(FILELIST_FILENAME));
                    fileList = filelistTxt.trim().split("\\s*\n\\s*");
                } catch (IOException e) {
                    throw CommonPlugin.wrapIntoCoreException("Cannot read filelist.txt", e);
                }
            }
            else {
                List<String> list = new ArrayList<String>();
                collectFiles(templateFolder, templateFolder, list);
                fileList = list.toArray(new String[]{});
            }
        }
        return fileList;
    }

    private void collectFiles(IContainer folder, IContainer baseFolder, List<String> result) throws CoreException {
        int segmentCount = baseFolder.getFullPath().segmentCount();
        for (IResource resource : folder.members()) {
            String relativePath = resource.getFullPath().removeFirstSegments(segmentCount).toString();
            if (resource instanceof IFile)
                result.add(relativePath);
            else {
                result.add(relativePath + "/");
                collectFiles((IContainer)resource, baseFolder, result);
            }
        }
    }
    
    /**
     * Returns whether the given file name matches any of the given glob patterns.
     * Only a limited subset of glob patterns is recognized: '*' and '?' only, 
     * no curly braces or square brackets. Extra: "**" is recognized.
     * Note: "*.txt" means text files in the ROOT folder. To mean "*.txt in any 
     * folder", specify "** / *.txt" (without the spaces).
     */
    protected static boolean matchesAny(String path, List<String> basicGlobPatterns) {
        path = path.replace('\\', '/'); // simplify matching
        for (String pattern : basicGlobPatterns) {
            String regex = "^" + pattern.replace(".", "\\.").replace("?", ".").replace("**/", "(.&/)?").replace("**", ".&").replace("*", "[^/]*").replace(".&", ".*") + "$";
            if (path.matches(regex))
                return true;
        }
        return false;
    }
}

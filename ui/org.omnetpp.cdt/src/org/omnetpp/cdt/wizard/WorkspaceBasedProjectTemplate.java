package org.omnetpp.cdt.wizard;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
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
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
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
	public static final String NAME = "name";
	public static final String DESCRIPTION = "description";
	public static final String CATEGORY = "category";
	public static final String IMAGE = "image";
	public static final String DEPENDENT = "dependent";
	
	protected IFolder templateFolder;
	protected Properties properties = new Properties();
	protected Set<IResource> nontemplateResources = new HashSet<IResource>();
	protected boolean markAsDependentProject;

	public WorkspaceBasedProjectTemplate(IFolder folder) throws CoreException {
		super(null, null, null, null);
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
		//TODO: setImage(properties.getProperty("image"), ...);

		markAsDependentProject = StringUtils.defaultIfEmpty(properties.getProperty(DEPENDENT), "true").equals("true");
		
		nontemplateResources.add(folder.getFile(TEMPLATE_PROPERTIES_FILENAME));
		
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
	    	try {
	    		Composite composite = new Composite(parent, SWT.NONE);
	    		composite.setLayout(new GridLayout());
	    		widgetMap = (Map<String,Control>) XSWT.create(composite, xswtFile.getContents()); 
	    		setControl(composite);
	    	} catch (XSWTException e) {
	    	   e.printStackTrace(); //XXX 
	    	} catch (CoreException e) {
	    		e.printStackTrace(); //XXX 
			}
		}
		
		public Map<String, Control> getWidgetMap() {
			return widgetMap;
		}
	};
	
    public IWizardPage[] createCustomPages() {
    	// collect page IDs from property file ("page.1", "page.2" etc keys)
    	int[] pageIDs = new int[0];
    	for (Object key : properties.keySet())
    		if (((String)key).matches("page\\.[0-9]+"))
    			pageIDs = ArrayUtils.add(pageIDs, Integer.parseInt(((String)key).replaceFirst("^.*\\.", "")));
    	Arrays.sort(pageIDs);
    	
    	// create pages
    	IWizardPage[] result = new IWizardPage[pageIDs.length];
		for (int i=0; i<pageIDs.length; i++) {
			int pageID = pageIDs[i];
			String xswtFileName = properties.getProperty("page."+pageID);
			IFile xswtFile = templateFolder.getFile(new Path(xswtFileName)); //FIXME error if not exists
			result[i] = new XSWTWizardPage(getName()+"#"+pageID, xswtFile);
		}
		//FIXME form files must not be copied into the project!
		
		return result;
    }

    @Override
	protected void extractVariablesFromPages(IWizardPage[] customPages) {
    	// extract data from the XSWT forms, and set them as template variables
		for (IWizardPage page : customPages) {
			Map<String,Control> widgetMap = ((XSWTWizardPage)page).getWidgetMap();
			for (String widgetName : widgetMap.keySet()) {
				Control control = widgetMap.get(widgetName);
				String value = control.toString(); //FIXME this is somewhat more complicated
				setVariable(widgetName, value);
			}
		}
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
					//TODO files and dirs to ignore; suppressIfBlank setting 
					IFile file = (IFile)resource;
					IFile destFile = dest.getFile(new Path(file.getName()));
					createFileFromWorkspaceResource(destFile, file, true);
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

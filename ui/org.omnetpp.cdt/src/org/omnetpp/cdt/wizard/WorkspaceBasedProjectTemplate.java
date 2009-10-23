package org.omnetpp.cdt.wizard;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.cdt.Activator;

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
	
	protected IFolder folder;
	protected Properties properties = new Properties();

	public WorkspaceBasedProjectTemplate(IFolder folder) throws CoreException {
		super(null, null, null, null);
		this.folder = folder;

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
		
	}

	@Override
	protected void doConfigure() throws CoreException {
		// TODO Auto-generated method stub
		
	}

}

package org.omnetpp.common.velocity;

import java.io.InputStream;

import org.apache.commons.collections.ExtendedProperties;
import org.apache.commons.lang.StringUtils;
import org.apache.velocity.exception.ResourceNotFoundException;
import org.apache.velocity.runtime.resource.Resource;
import org.apache.velocity.runtime.resource.loader.ResourceLoader;
import org.apache.velocity.util.ExceptionUtils;

/**
 * ClasspathResourceLoader is a simple loader that will load templates from the
 * classpath.
 * 
 * The similarly named class in Velocity is not usable, because it cannot load
 * anything from other plug-ins (except those in this plug-ins dependency list),
 * due to the way Equinox isolates plug-ins from each other. The only solution
 * is to pass in the proper ClassLoader as a parameter.
 * 
 * @author andras
 */
public class ClasspathResourceLoader extends ResourceLoader {
	protected ClassLoader resourceRoot;
	protected String[] resourcePath;

	public ClasspathResourceLoader(ClassLoader resourceRoot, String[] resourcePath) {
		this.resourceRoot = resourceRoot;
		this.resourcePath = resourcePath;
	}

	public void init(ExtendedProperties configuration) {
		if (log.isTraceEnabled())
			log.trace("ClasspathResourceLoader : initialization complete.");
	}

	public InputStream getResourceStream(String name) throws ResourceNotFoundException {
		InputStream result = null;

		if (StringUtils.isEmpty(name))
			throw new ResourceNotFoundException("No template name provided");

		while (name.startsWith("/"))
            name = name.substring(1);

		try {
			for (String prefix : resourcePath) {
				result = resourceRoot.getResourceAsStream(prefix + "/" + name);
				if (result != null) 
					break;
			}
		} 
		catch (Exception fnfe) {
			throw (ResourceNotFoundException) ExceptionUtils.createWithCause(ResourceNotFoundException.class, 
					"problem with template: " + name, fnfe);
		}

		if (result == null)
			throw new ResourceNotFoundException("ClasspathResourceLoader Error: cannot find resource " + name);

		return result;
	}

	public boolean isSourceModified(Resource resource) {
		return false;
	}

	public long getLastModified(Resource resource) {
		return 0;
	}
}

package org.omnetpp.common.velocity;

import java.util.List;
import java.util.logging.Logger;

import org.apache.velocity.app.VelocityEngine;
import org.apache.velocity.runtime.RuntimeConstants;
import org.eclipse.core.resources.IContainer;
import org.omnetpp.common.util.ReflectionUtils;

/**
 * Velocity-related utility functions.
 * 
 * @author Andras
 */
public class VelocityUtil {

	/**
	 * Returns a configured Velocity engine instance. Templates will be loaded
	 * from the class path of the given ClassLoader (via its getResourceAsStream() 
	 * method), after prefixing the template name with one of the resourcePath
	 * strings. The verbose parameter selects whether to log on the Eclipse log
	 * or not.
	 */
	@SuppressWarnings("unchecked")
	public static VelocityEngine createEngine(ClassLoader resourceRoot, String[] resourcePath,  boolean verbose) throws Exception {
        //FIXME handle 'verbose'! implement LogChute!
		//using CommonPlugin.getDefault().getLog().log(status)...

		final String LOGGER_NAME = "velogger";
		Logger log = Logger.getLogger(LOGGER_NAME);
		VelocityEngine ve = new VelocityEngine();
		ve.setProperty(RuntimeConstants.RUNTIME_LOG_LOGSYSTEM_CLASS, "org.apache.velocity.runtime.log.Log4JLogChute" );
		ve.setProperty("runtime.log.logsystem.log4j.logger", LOGGER_NAME);
		ve.init();

		// add our own resourceLoader. Since there are no getters at all, we need to resort to reflection...
		Object runtimeInstance = ReflectionUtils.getFieldValue(ve, "ri");
		Object resourceManagerImpl = ReflectionUtils.getFieldValue(runtimeInstance, "resourceManager");
		List resourceLoaders = (List)ReflectionUtils.getFieldValue(resourceManagerImpl, "resourceLoaders");
		resourceLoaders.add(new ClasspathResourceLoader(resourceRoot, resourcePath));
		return ve;
	}

	/**
	 * Returns a configured Velocity engine instance. Templates will be loaded
	 * from the given resource paths. The verbose parameter selects whether to log 
	 * on the Eclipse log or not.
	 */
	public static VelocityEngine createEngine(IContainer[] resourcePath, boolean verbose) {
		return null;
	}
}

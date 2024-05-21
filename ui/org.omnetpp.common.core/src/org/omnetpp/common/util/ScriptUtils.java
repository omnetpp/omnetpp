package org.omnetpp.common.util;

import java.io.IOException;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.omnetpp.common.CommonCorePlugin;

import bsh.EvalError;
import bsh.Interpreter;

/**
 * Common helper functions for executing scripts
 *
 * @author rhornig
 */
public class ScriptUtils {
	
	/**
	 * Evaluates a file containing bean shell script and returns the evaluation result
	 * 
	 * @param fileName - fully qualified file name
	 * @return script evaluation result
	 * @throws CoreException - if file is unreadable or script evaluation fails
	 */
	public static Object sourceBeanScript(String fileName) throws CoreException {
		
		Interpreter i = new Interpreter();
		try {
            return i.source(fileName);
		} catch (IOException | EvalError e) {
            IStatus error = new Status(IStatus.ERROR, CommonCorePlugin.PLUGIN_ID, 0, "Script evaluation error.", e);
	        throw new CoreException(error);
        }
	}

}

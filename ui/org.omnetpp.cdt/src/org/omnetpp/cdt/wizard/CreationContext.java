package org.omnetpp.cdt.wizard;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.IProgressMonitor;

/**
 * Stores the project handle, template variables etc during the lifetime
 * of the New Project wizard.
 * 
 * @author Andras
 */
public class CreationContext {
	private IProject project;
	private Map<String,Object> variables = new HashMap<String, Object>();
	private IProgressMonitor progressMonitor;
	
	public CreationContext(IProject project) {
		this.project = project;
	}

	public IProject getProject() {
		return project;
	}
	
	public Map<String,Object> getVariables() {
		return variables;
	}

	public void setProgressMonitor(IProgressMonitor progressMonitor) {
		this.progressMonitor = progressMonitor;
	}
	
	public IProgressMonitor getProgressMonitor() {
		return progressMonitor;
	}
}

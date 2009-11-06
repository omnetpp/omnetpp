package org.omnetpp.common.wizard;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.IProgressMonitor;

/**
 * Stores the resource and the template variables during the lifetime of the wizard. 
 * 
 * The resource may be folder or project to be created, or an existing
 * folder or project in which the wizard will create files/folders.
 * 
 * @author Andras
 */
public class CreationContext {
	private final IContainer folder; // MUST REMAIN UNCHANGED after the constructor call
	private Map<String,Object> variables = new HashMap<String, Object>();
	private IProgressMonitor progressMonitor;
	
	public CreationContext(IContainer folder) {
		this.folder = folder;
	}

	public IContainer getFolder() { // note: intentionally no setter defined!
		return folder;
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

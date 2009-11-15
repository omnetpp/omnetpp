package org.omnetpp.common.wizard;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.wizard.IWizard;

/**
 * Stores the resource and the template variables during the lifetime of the wizard. 
 * 
 * The resource may be folder or project to be created, or an existing
 * folder or project in which the wizard will create files/folders.
 * 
 * @author Andras
 */
public class CreationContext {
    private final IWizard wizard;  // immutable after ctor call
	private final IContainer folder; // MUST REMAIN UNCHANGED after the constructor call
	private Map<String,Object> variables = new HashMap<String, Object>();
	private IProgressMonitor progressMonitor;
	
    /**
     * Constructor. Accepts the folder or project to be created, or an existing
     * folder or project in which the wizard will create files/folders.
     */
	public CreationContext(IContainer folder, IWizard wizard) {
		this.folder = folder;
		this.wizard = wizard;
	}

	/**
	 * Returns the folder or project to be created, or an existing
	 * folder or project in which the wizard will create files/folders.
	 * 
	 * For health and safety of the wizards, this field is immutable 
	 * (can only be set in the constructor).
	 */
	public IContainer getFolder() { // note: intentionally no setter defined!
	    return folder;
	}
	
	/**
	 * Returns the owner wizard.
	 */
	public IWizard getWizard() {
		return wizard;
	}
	
	/**
	 * Stores the template variables to be used during template instantiation.
	 * Read/write access.
	 */
	public Map<String,Object> getVariables() {
		return variables;
	}

    /**
     * Convenience function, equivalent to getVariables().put(name, value).
     */
	public void setVariable(String name, Object value) {
	    getVariables().put(name, value);
	}

    /**
     * Convenience function, equivalent to getVariables().put(name, value) unless 
     * there is already a variable with the given name; in that case it does nothing.
     */
	public void setVariableIfMissing(String name, Object value) {
	    if (!getVariables().containsKey(name))
	        getVariables().put(name, value);
	}

	/**
	 * Convenience function, equivalent to getVariables().get(name).
	 */
	public Object getVariable(String name) {
	    return getVariables().get(name);
	}

	/**
	 * Sets the progress monitor; may be null.
	 */
	public void setProgressMonitor(IProgressMonitor progressMonitor) {
		this.progressMonitor = progressMonitor;
	}
	
    /**
     * Progress monitor; may be null.
     */
	public IProgressMonitor getProgressMonitor() {
		return progressMonitor;
	}
}

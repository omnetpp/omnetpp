package org.omnetpp.cdt.wizard;

import java.util.Map;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.IProgressMonitor;

public class CreationContext {
	public IProject project = null;
	public Map<String,Object> variables = null;
	public IProgressMonitor progressMonitor = null;
}

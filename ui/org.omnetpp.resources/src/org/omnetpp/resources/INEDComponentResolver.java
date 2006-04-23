package org.omnetpp.resources;

import java.util.Collection;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned2.model.NEDElement;

public interface INEDComponentResolver {

	/**
	 * Returns NED files in the workspace.
	 */
	public abstract Set<IFile> getNEDFiles();

	/**
	 * Returns parsed contents of a NED file. Returns null if file was not parseable.
	 */
	public abstract NEDElement getNEDFileContents(IFile file);

	/**
	 * Returns a component declated at the given file/line. The line number should 
	 * point into the declaration of the component. Returns null if no such component 
	 * was found.
	 */
	public abstract INEDComponent getComponentAt(IFile file, int lineNumber);
	
	/**
	 * Returns all components in the NED files.
	 */
	public abstract Collection<INEDComponent> getAllComponents();

	/**
	 * Returns all simple and compound modules in the NED files.
	 */
	public abstract Collection<INEDComponent> getModules();

	/**
	 * Returns all channels in the NED files.
	 */
	public abstract Collection<INEDComponent> getChannels();

	/**
	 * Returns all module interfaces in the NED files.
	 */
	public abstract Collection<INEDComponent> getModuleInterfaces();

	/**
	 * Returns all channel interfaces in the NED files.
	 */
	public abstract Collection<INEDComponent> getChannelInterfaces();

	/**
	 * Returns all module names in the NED files.
	 */
	public abstract Set<String> getModuleNames();

	/**
	 * Returns all channel names in the NED files.
	 */
	public abstract Set<String> getChannelNames();

	/**
	 * Returns all module interface names in the NED files.
	 */
	public abstract Set<String> getModuleInterfaceNames();

	/**
	 * Returns all channel interface names in the NED files.
	 */
	public abstract Set<String> getChannelInterfaceNames();

	/**
	 * Returns a component by name.
	 */
	public abstract INEDComponent getComponent(String name);

}